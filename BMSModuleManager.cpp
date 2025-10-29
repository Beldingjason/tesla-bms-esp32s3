#include "config.h"
#include "BMSModuleManager.h"
#include "BMSUtil.h"
#include "Logger.h"
#include "pin_config.h"
#include "constants.h"

BMSModuleManager::BMSModuleManager()
    : watchdogCallback_(nullptr),
      Pstring(1),
      batteryID(0),
      numFoundModules(0),
      isFaulted_(false),
      lowestPackVoltHistory_(1000.0f),
      highestPackVoltHistory_(0.0f),
      lowestPackTempHistory_(TEMP_INIT_FOR_MIN_TRACKING),
      highestPackTempHistory_(TEMP_INIT_FOR_MAX_TRACKING)
{
    telemetry_ = PackTelemetry{};
    telemetry_.lowestPackVolt = lowestPackVoltHistory_;
    telemetry_.highestPackVolt = highestPackVoltHistory_;
    telemetry_.lowestPackTemp = lowestPackTempHistory_;
    telemetry_.highestPackTemp = highestPackTempHistory_;
    for (int i = 0; i <= MAX_MODULE_ADDR; i++) {
        modules[i].setExists(false);
        modules[i].setAddress(i);
    }
}

void BMSModuleManager::balanceCells()
{
    uint8_t payload[4];
    uint8_t buff[30];
    uint8_t balance = 0;//bit 0 - 5 are to activate cell balancing 1-6

    for (int address = 1; address <= MAX_MODULE_ADDR; address++)
    {
      applyWatchdogReset();
      if (modules[address].isExisting())
      {
        float moduleLow = modules[address].getLowCellV();
        float moduleHigh = modules[address].getHighCellV();
        if (moduleHigh < BMS_BALANCE_VOLTAGE_MIN) {
            continue;
        }
        if (moduleLow <= 0.0f) {
            continue;
        }
        if ((moduleHigh - moduleLow) < BMS_BALANCE_VOLTAGE_DELTA) {
            continue;
        }

        balance = 0;
        for (int i = 0; i < 6; i++)
        {
            float cellVoltage = modules[address].getCellVoltage(i);
            if (cellVoltage > (moduleLow + BMS_BALANCE_VOLTAGE_DELTA))
            {
                balance |= static_cast<uint8_t>(1U << i);
            }
        }

        if (balance != 0) //only send balance command when needed
        {
            payload[0] = address << 1;
            payload[1] = REG_BAL_TIME;
            payload[2] = BMS_BALANCE_TIME_LIMIT_SEC; //60 second balance limit, if not triggered to balance it will stop after 5 seconds
            BMSUtil::sendData(payload, 3, true);
            delay(BMS_COMMAND_DELAY_MS);
            if (BMSUtil::getReply(buff, 30) < 3) {
                Logger::warn("Module %i balance time write did not acknowledge", address);
                continue;
            }

            payload[0] = address << 1;
            payload[1] = REG_BAL_CTRL;
            payload[2] = balance; //write balance state to register
            BMSUtil::sendData(payload, 3, true);
            delay(BMS_COMMAND_DELAY_MS);
            if (BMSUtil::getReply(buff, 30) < 3) {
                Logger::warn("Module %i balance control write did not acknowledge", address);
                continue;
            }

            if (Logger::isDebug()) //read registers back out to check if everthing is good
            {
                delay(BMS_READ_DELAY_MS);
                payload[0] = address << 1;
                payload[1] = REG_BAL_TIME;
                payload[2] = 1; //
                BMSUtil::sendData(payload, 3, false);
                delay(BMS_COMMAND_DELAY_MS);
                BMSUtil::getReply(buff, 30);

                payload[0] = address << 1;
                payload[1] = REG_BAL_CTRL;
                payload[2] = 1; //
                BMSUtil::sendData(payload, 3, false);
                delay(BMS_COMMAND_DELAY_MS);
                BMSUtil::getReply(buff, 30);
            }
        }
      }
    }
}

/*
 * Try to set up any unitialized boards. Send a command to address 0 and see if there is a response. If there is then there is
 * still at least one unitialized board. Go ahead and give it the first ID not registered as already taken.
 * If we send a command to address 0 and no one responds then every board is inialized and this routine stops.
 * Don't run this routine until after the boards have already been enumerated.\
 * Note: The 0x80 conversion it is looking might in theory block the message from being forwarded so it might be required
 * To do all of this differently. Try with multiple boards. The alternative method would be to try to set the next unused
 * address and see if any boards respond back saying that they set the address. 
 */
void BMSModuleManager::setupBoards()
{
    uint8_t payload[3];
    uint8_t buff[10];
    int retLen;

    payload[0] = 0;
    payload[1] = 0;
    payload[2] = 1;
    
    while (1 == 1)
    {
        applyWatchdogReset();
        payload[0] = 0;
        payload[1] = 0;
        payload[2] = 1;
        retLen = BMSUtil::sendDataWithReply(payload, 3, false, buff, 4);
        if (retLen == 4)
        {
            if (buff[0] == 0x80 && buff[1] == 0 && buff[2] == 1)
            {
                Logger::debug("00 found");
                //look for a free address to use
                for (int y = 1; y < MAX_BMS_MODULES; y++) 
                {
                    applyWatchdogReset();
                    if (!modules[y].isExisting())
                    {
                        payload[0] = 0;
                        payload[1] = REG_ADDR_CTRL;
                        payload[2] = y | 0x80;
                        BMSUtil::sendData(payload, 3, true);
                        delay(3);
                        if (BMSUtil::getReply(buff, 10) > 2)
                        {
                            if (buff[0] == (0x81) && buff[1] == REG_ADDR_CTRL && buff[2] == (y + 0x80)) 
                            {
                                modules[y].setExists(true);
                                numFoundModules++;
                                Logger::debug("Address assigned");
                            }
                        }
                        break; //quit the for loop
                    }
                }
            }
            else break; //nobody responded properly to the zero address so our work here is done.
        }
        else break;
    }
}

/*
 * Iterate through all 62 possible board addresses (1-62) to see if they respond
 */
void BMSModuleManager::findBoards()
{
    uint8_t payload[3];
    uint8_t buff[8];

    numFoundModules = 0;
    payload[0] = 0;
    payload[1] = 0; //read registers starting at 0
    payload[2] = 1; //read one byte
    for (int x = 1; x <= MAX_MODULE_ADDR; x++)
    {
        applyWatchdogReset();
        modules[x].setExists(false);
        payload[0] = x << 1;
        BMSUtil::sendData(payload, 3, false);
        delay(20);
        if (BMSUtil::getReply(buff, 8) > 4)
        {
            if (buff[0] == (x << 1) && buff[1] == 0 && buff[2] == 1 && buff[4] > 0) {
                modules[x].setExists(true);
                numFoundModules++;
                Logger::debug("Found module with address: %X", x); 
            }
        }
        delay(5);
    }
}


/*
 * Force all modules to reset back to address 0 then set them all up in order so that the first module
 * in line from the master board is 1, the second one 2, and so on.
*/
void BMSModuleManager::renumberBoardIDs()
{
    uint8_t payload[3];
    uint8_t buff[8];
    int attempts = 1;

    for (int y = 1; y < MAX_BMS_MODULES; y++) 
    {
        applyWatchdogReset();
        modules[y].setExists(false);  
        numFoundModules = 0;
    }    
    
    while (attempts < 3)
    {
        applyWatchdogReset();
        payload[0] = 0x3F << 1; //broadcast the reset command
        payload[1] = 0x3C;//reset
        payload[2] = 0xA5;//data to cause a reset
        BMSUtil::sendData(payload, 3, true);
        delay(100);
        BMSUtil::getReply(buff, 8);
        if (buff[0] == 0x7F && buff[1] == 0x3C && buff[2] == 0xA5 && buff[3] == 0x57) break;
        attempts++;
    }
    
    setupBoards();
}

/*
After a RESET boards have their faults written due to the hard restart or first time power up, this clears thier faults
*/
void BMSModuleManager::clearFaults()
{
    uint8_t payload[3];
    uint8_t buff[8];
    payload[0] = 0x7F; //broadcast
    payload[1] = REG_ALERT_STATUS;//Alert Status
    payload[2] = 0xFF;//data to cause a reset
    BMSUtil::sendDataWithReply(payload, 3, true, buff, 4);
    
    payload[0] = 0x7F; //broadcast
    payload[2] = 0x00;//data to clear
    BMSUtil::sendDataWithReply(payload, 3, true, buff, 4);
  
    payload[0] = 0x7F; //broadcast
    payload[1] = REG_FAULT_STATUS;//Fault Status
    payload[2] = 0xFF;//data to cause a reset
    BMSUtil::sendDataWithReply(payload, 3, true, buff, 4);
    
    payload[0] = 0x7F; //broadcast
    payload[2] = 0x00;//data to clear
    BMSUtil::sendDataWithReply(payload, 3, true, buff, 4);
  
    isFaulted_ = false;
}

/*
Puts all boards on the bus into a Sleep state, very good to use when the vehicle is a rest state. 
Pulling the boards out of sleep only to check voltage decay and temperature when the contactors are open.
*/

void BMSModuleManager::sleepBoards()
{
    uint8_t payload[3];
    uint8_t buff[8];
    applyWatchdogReset();
    payload[0] = 0x7F; //broadcast
    payload[1] = REG_IO_CTRL;//IO ctrl start
    payload[2] = 0x04;//write sleep bit
    BMSUtil::sendData(payload, 3, true);
    delay(BMS_COMMAND_DELAY_MS);
    BMSUtil::getReply(buff, 8);
}

/*
Wakes all the boards up and clears thier SLEEP state bit in the Alert Status Registery
*/

void BMSModuleManager::wakeBoards()
{
    uint8_t payload[3];
    uint8_t buff[8];
    applyWatchdogReset();
    payload[0] = 0x7F; //broadcast
    payload[1] = REG_IO_CTRL;//IO ctrl start
    payload[2] = 0x00;//write sleep bit
    BMSUtil::sendData(payload, 3, true);
    delay(BMS_COMMAND_DELAY_MS);
    BMSUtil::getReply(buff, 8);
  
    payload[0] = 0x7F; //broadcast
    payload[1] = REG_ALERT_STATUS;//Fault Status
    payload[2] = 0x04;//data to cause a reset
    BMSUtil::sendData(payload, 3, true);
    delay(BMS_COMMAND_DELAY_MS);
    BMSUtil::getReply(buff, 8);
    payload[0] = 0x7F; //broadcast
    payload[2] = 0x00;//data to clear
    BMSUtil::sendData(payload, 3, true);
    delay(BMS_COMMAND_DELAY_MS);
    BMSUtil::getReply(buff, 8);
}

bool BMSModuleManager::collectTelemetry()
{
    applyWatchdogReset();
    PackTelemetry newTelemetry;
    newTelemetry.lowestPackVolt = lowestPackVoltHistory_;
    newTelemetry.highestPackVolt = highestPackVoltHistory_;
    newTelemetry.lowestPackTemp = lowestPackTempHistory_;
    newTelemetry.highestPackTemp = highestPackTempHistory_;

    float packVoltageSum = 0.0f;
    float lowCell = 1000.0f;
    float highCell = -1000.0f;
    bool anyData = false;
    int modulesRead = 0;
    int modulesFailed = 0;

    for (int x = 1; x <= MAX_MODULE_ADDR; x++)
    {
        ModuleTelemetry moduleTelemetry;
        moduleTelemetry.present = modules[x].isExisting();

        if (moduleTelemetry.present)
        {
            applyWatchdogReset();
            Logger::debug("");
            Logger::debug("Module %i exists. Reading voltage and temperature values", x);
            moduleTelemetry.telemetryValid = updateModuleTelemetry(x, moduleTelemetry);
            if (moduleTelemetry.telemetryValid)
            {
                Logger::debug("Module voltage: %f", moduleTelemetry.moduleVoltage);
                Logger::debug("Lowest Cell V: %f     Highest Cell V: %f", moduleTelemetry.lowCell, moduleTelemetry.highCell);

                packVoltageSum += moduleTelemetry.moduleVoltage;
                if (moduleTelemetry.lowCell < lowCell) lowCell = moduleTelemetry.lowCell;
                if (moduleTelemetry.highCell > highCell) highCell = moduleTelemetry.highCell;

                if (moduleTelemetry.lowTemp < lowestPackTempHistory_) lowestPackTempHistory_ = moduleTelemetry.lowTemp;
                if (moduleTelemetry.highTemp > highestPackTempHistory_) highestPackTempHistory_ = moduleTelemetry.highTemp;

                anyData = true;
                modulesRead++;
                commStats_[x].successCount++;
            }
            else
            {
                Logger::warn("Failed to read telemetry from module %i", x);
                modulesFailed++;
                commStats_[x].failureCount++;
            }
        }

        newTelemetry.modules[x] = moduleTelemetry;
    }

    // Log aggregate statistics
    if (modulesFailed > 0) {
        Logger::warn("Telemetry collection: %d modules read successfully, %d modules failed",
                     modulesRead, modulesFailed);
    } else if (modulesRead > 0) {
        Logger::debug("Telemetry collection: %d modules read successfully", modulesRead);
    }

    if (anyData && Pstring > 0)
    {
        newTelemetry.packVoltage = packVoltageSum / static_cast<float>(Pstring);
        newTelemetry.lowestCell = lowCell;
        newTelemetry.highestCell = highCell;
        newTelemetry.cellDelta = highCell - lowCell;

        if (newTelemetry.packVoltage > highestPackVoltHistory_) highestPackVoltHistory_ = newTelemetry.packVoltage;
        if (newTelemetry.packVoltage < lowestPackVoltHistory_) lowestPackVoltHistory_ = newTelemetry.packVoltage;
        newTelemetry.lowestPackVolt = lowestPackVoltHistory_;
        newTelemetry.highestPackVolt = highestPackVoltHistory_;

        newTelemetry.lowestPackTemp = lowestPackTempHistory_;
        newTelemetry.highestPackTemp = highestPackTempHistory_;
        newTelemetry.hasData = true;
    }
    else
    {
        newTelemetry.hasData = false;
        newTelemetry.packVoltage = 0.0f;
        newTelemetry.lowestCell = 0.0f;
        newTelemetry.highestCell = 0.0f;
        newTelemetry.cellDelta = 0.0f;
    }

    newTelemetry.faultPinActive = (digitalRead(PIN_BMS_FAULT) == LOW);
    if (newTelemetry.faultPinActive && !isFaulted_) {
        Logger::error("One or more BMS modules have entered the fault state!");
    } else if (!newTelemetry.faultPinActive && isFaulted_) {
        Logger::info("All modules have exited a faulted state");
    }
    isFaulted_ = newTelemetry.faultPinActive;

    telemetry_ = newTelemetry;
    return telemetry_.hasData;
}

const BMSModuleManager::PackTelemetry &BMSModuleManager::getTelemetry() const
{
    return telemetry_;
}

void BMSModuleManager::setWatchdogCallback(WatchdogCallback callback)
{
    watchdogCallback_ = callback;
}

bool BMSModuleManager::updateModuleTelemetry(int moduleIndex, ModuleTelemetry &outTelemetry)
{
    if (!modules[moduleIndex].readModuleValues())
    {
        outTelemetry.telemetryValid = false;
        return false;
    }

    outTelemetry.telemetryValid = true;
    outTelemetry.moduleVoltage = modules[moduleIndex].getModuleVoltage();
    outTelemetry.lowCell = modules[moduleIndex].getLowCellV();
    outTelemetry.highCell = modules[moduleIndex].getHighCellV();
    outTelemetry.cellDelta = outTelemetry.highCell - outTelemetry.lowCell;
    outTelemetry.lowTemp = modules[moduleIndex].getLowTemp();
    outTelemetry.highTemp = modules[moduleIndex].getHighTemp();
    return true;
}

void BMSModuleManager::applyWatchdogReset() const
{
    if (watchdogCallback_) {
        watchdogCallback_();
    }
}

float BMSModuleManager::getLowCellVolt()
{
    return telemetry_.hasData ? telemetry_.lowestCell : 0.0f;
}

float BMSModuleManager::getHighCellVolt()
{
    return telemetry_.hasData ? telemetry_.highestCell : 0.0f;
}

float BMSModuleManager::getPackVoltage()
{
    return telemetry_.packVoltage;
}

float BMSModuleManager::getLowVoltage()
{
    return telemetry_.lowestPackVolt;
}

float BMSModuleManager::getHighVoltage()
{
    return telemetry_.highestPackVolt;
}

bool BMSModuleManager::isFaulted() const
{
    return isFaulted_;
}

void BMSModuleManager::setBatteryID(int id)
{
    batteryID = id;
}

void BMSModuleManager::setPstrings(int Pstrings)
{
    Pstring = Pstrings;
}

void BMSModuleManager::setSensors(int sensor,float Ignore)
{
  for (int x = 1; x <= MAX_MODULE_ADDR; x++)
    {
        if (modules[x].isExisting()) 
        {
          modules[x].settempsensor(sensor);
          modules[x].setIgnoreCell(Ignore);
        }
    }
}

float BMSModuleManager::getAvgTemperature()
{
    float avg = 0.0f;
    int y = 0; //counter for modules below -70 (no sensors connected)
    for (int x = 1; x <= MAX_MODULE_ADDR; x++)
    {
        if (modules[x].isExisting())
        {
          if (modules[x].getAvgTemp() > -70)
          {
            avg += modules[x].getAvgTemp();
          }
          else
          {
            y++;
          }
        }
    }
    int validModules = numFoundModules - y;
    if (validModules > 0) {
        avg = avg / (float)validModules;
    }

    return avg;
}

float BMSModuleManager::getAvgCellVolt()
{
    float avg = 0.0f;
    for (int x = 1; x <= MAX_MODULE_ADDR; x++)
    {
        if (modules[x].isExisting()) avg += modules[x].getAverageV();
    }
    if (numFoundModules > 0) {
        avg = avg / (float)numFoundModules;
    }

    return avg;
}

void BMSModuleManager::printPackSummary()
{
    uint8_t faults;
    uint8_t alerts;
    uint8_t COV;
    uint8_t CUV;
    
    Logger::console("");
    Logger::console("");
    Logger::console("");
    Logger::console("                                     Pack Status:");
    if (isFaulted_) Logger::console("                                       FAULTED!");
    else Logger::console("                                   All systems go!");
    Logger::console("Modules: %i    Voltage: %fV   Avg Cell Voltage: %fV     Avg Temp: %fC ", numFoundModules, 
                    getPackVoltage(),getAvgCellVolt(), getAvgTemperature());
    Logger::console("");
    for (int y = 1; y < MAX_BMS_MODULES; y++)
    {
        if (modules[y].isExisting())
        {
            faults = modules[y].getFaults();
            alerts = modules[y].getAlerts();
            COV = modules[y].getCOVCells();
            CUV = modules[y].getCUVCells();
            
            Logger::console("                               Module #%i", y);
            
            Logger::console("  Voltage: %fV   (%fV-%fV)     Temperatures: (%fC-%fC)", modules[y].getModuleVoltage(), 
                            modules[y].getLowCellV(), modules[y].getHighCellV(), modules[y].getLowTemp(), modules[y].getHighTemp());
            if (faults > 0)
            {
                Logger::console("  MODULE IS FAULTED:");
                if (faults & 1)
                {
                    SERIALCONSOLE.print("    Overvoltage Cell Numbers (1-6): ");
                    for (int i = 0; i < 6; i++)
                    {
                        if (COV & (1 << i)) 
                        {
                            SERIALCONSOLE.print(i+1);
                            SERIALCONSOLE.print(" ");
                        }
                    }
                    SERIALCONSOLE.println();
                }
                if (faults & 2)
                {
                    SERIALCONSOLE.print("    Undervoltage Cell Numbers (1-6): ");
                    for (int i = 0; i < 6; i++)
                    {
                        if (CUV & (1 << i)) 
                        {
                            SERIALCONSOLE.print(i+1);
                            SERIALCONSOLE.print(" ");
                        }
                    }
                    SERIALCONSOLE.println();
                }
                if (faults & 4)
                {
                    Logger::console("    CRC error in received packet");
                }
                if (faults & 8)
                {
                    Logger::console("    Power on reset has occurred");
                }
                if (faults & 0x10)
                {
                    Logger::console("    Test fault active");
                }
                if (faults & 0x20)
                {
                    Logger::console("    Internal registers inconsistent");
                }
            }
            if (alerts > 0)
            {
                Logger::console("  MODULE HAS ALERTS:");
                if (alerts & 1)
                {
                    Logger::console("    Over temperature on TS1");
                }
                if (alerts & 2)
                {
                    Logger::console("    Over temperature on TS2");
                }
                if (alerts & 4)
                {
                    Logger::console("    Sleep mode active");
                }
                if (alerts & 8)
                {
                    Logger::console("    Thermal shutdown active");
                }
                if (alerts & 0x10)
                {
                    Logger::console("    Test Alert");
                }
                if (alerts & 0x20)
                {
                    Logger::console("    OTP EPROM Uncorrectable Error");
                }
                if (alerts & 0x40)
                {
                    Logger::console("    GROUP3 Regs Invalid");
                }
                if (alerts & 0x80)
                {
                    Logger::console("    Address not registered");
                }                
            }
            if (faults > 0 || alerts > 0) SERIALCONSOLE.println();
        }
    }
}

void BMSModuleManager::printPackDetails()
{
    uint8_t faults;
    uint8_t alerts;
    uint8_t COV;
    uint8_t CUV;
    int cellNum = 0;
    
    Logger::console("");
    Logger::console("");
    Logger::console("");
    Logger::console("                                         Pack Status:");
    if (isFaulted_) Logger::console("                                           FAULTED!");
    else Logger::console("                                      All systems go!");
    Logger::console("Modules: %i    Voltage: %fV   Avg Cell Voltage: %fV  Low Cell Voltage: %fV   High Cell Voltage: %fV   Avg Temp: %fC ",
                    numFoundModules,
                    getPackVoltage(),
                    getAvgCellVolt(),
                    getLowCellVolt(),
                    getHighCellVolt(),
                    getAvgTemperature());
    Logger::console("");
    for (int y = 1; y < MAX_BMS_MODULES; y++)
    {
        if (modules[y].isExisting())
        {
            faults = modules[y].getFaults();
            alerts = modules[y].getAlerts();
            COV = modules[y].getCOVCells();
            CUV = modules[y].getCUVCells();
            
            SERIALCONSOLE.print("Module #");
            SERIALCONSOLE.print(y);
            if (y < 10) SERIALCONSOLE.print(" ");
            SERIALCONSOLE.print("  ");
            SERIALCONSOLE.print(modules[y].getModuleVoltage());
            SERIALCONSOLE.print("V");
            for (int i = 0; i < 6; i++)
            {
                if (cellNum < 10) SERIALCONSOLE.print(" ");
                SERIALCONSOLE.print("  Cell");
                SERIALCONSOLE.print(cellNum++);                
                SERIALCONSOLE.print(": ");
                SERIALCONSOLE.print(modules[y].getCellVoltage(i));
                SERIALCONSOLE.print("V");
            }   
            SERIALCONSOLE.print("  Neg Term Temp: ");
            SERIALCONSOLE.print(modules[y].getTemperature(0));
            SERIALCONSOLE.print("C  Pos Term Temp: ");
            SERIALCONSOLE.print(modules[y].getTemperature(1));
            SERIALCONSOLE.println("C");

        }
    }
}

const BMSModuleManager::ModuleCommStats& BMSModuleManager::getModuleCommStats(int moduleIndex) const
{
    static ModuleCommStats emptyStats;
    if (moduleIndex < 0 || moduleIndex > MAX_MODULE_ADDR) {
        return emptyStats;
    }
    return commStats_[moduleIndex];
}

void BMSModuleManager::resetModuleCommStats(int moduleIndex)
{
    if (moduleIndex >= 0 && moduleIndex <= MAX_MODULE_ADDR) {
        commStats_[moduleIndex].successCount = 0;
        commStats_[moduleIndex].failureCount = 0;
    }
}

void BMSModuleManager::resetAllCommStats()
{
    for (int i = 0; i <= MAX_MODULE_ADDR; i++) {
        commStats_[i].successCount = 0;
        commStats_[i].failureCount = 0;
    }
}
