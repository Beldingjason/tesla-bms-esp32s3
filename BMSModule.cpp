#include "config.h"
#include "BMSModule.h"
#include "BMSUtil.h"
#include "Logger.h"
#include "constants.h"
#include <math.h>


BMSModule::BMSModule()
{
    for (int i = 0; i < 6; i++)
    {
        cellVolt[i] = 0.0f;
        lowestCellVolt[i] = CELL_VOLTAGE_INIT_HIGH;
        highestCellVolt[i] = CELL_VOLTAGE_INIT_LOW;
    }
    moduleVolt = 0.0f;
    temperatures[0] = 0.0f;
    temperatures[1] = 0.0f;
    lowestTemperature = TEMP_INIT_FOR_MIN_TRACKING;
    highestTemperature = TEMP_INIT_FOR_MAX_TRACKING;
    lowestModuleVolt = MODULE_VOLTAGE_INIT_HIGH;
    highestModuleVolt = MODULE_VOLTAGE_INIT_LOW;
    exists = false;
    alerts = 0;
    faults = 0;
    COVFaults = 0;
    CUVFaults = 0;
    IgnoreCell = 0.0f;
    sensor = 0;
    moduleAddress = 0;
}

/*
Reading the status of the board to identify any flags, will be more useful when implementing a sleep cycle
*/
bool BMSModule::readStatus()
{
  uint8_t payload[3];
  uint8_t buff[8];
  payload[0] = moduleAddress << 1; // address
  payload[1] = REG_ALERT_STATUS;   // Alert Status start
  payload[2] = 0x04;

  int retLen = BMSUtil::sendDataWithReply(payload, 3, false, buff, sizeof(buff));
  if (retLen < 8) {
    Logger::warn("Module %i status read length %i (expected >=8)", moduleAddress, retLen);
    alerts = faults = COVFaults = CUVFaults = 0;
    return false;
  }

  const uint8_t receivedCRC = buff[retLen - 1];
  const uint8_t calculatedCRC = BMSUtil::genCRC(buff, retLen - 1);
  if (receivedCRC != calculatedCRC) {
    Logger::warn("Module %i status CRC mismatch (%x != %x)", moduleAddress, receivedCRC, calculatedCRC);
    alerts = faults = COVFaults = CUVFaults = 0;
    return false;
  }

  if (buff[0] != (moduleAddress << 1) || buff[1] != REG_ALERT_STATUS) {
    Logger::warn("Module %i status header unexpected (addr=%x cmd=%x)", moduleAddress, buff[0], buff[1]);
    alerts = faults = COVFaults = CUVFaults = 0;
    return false;
  }

  if (buff[2] < 4) {
    Logger::warn("Module %i status payload too short (%u)", moduleAddress, buff[2]);
    alerts = faults = COVFaults = CUVFaults = 0;
    return false;
  }

  alerts = buff[3];
  faults = buff[4];
  COVFaults = buff[5];
  CUVFaults = buff[6];
  return true;
}

uint8_t BMSModule::getFaults()
{
    return faults;
}

uint8_t BMSModule::getAlerts()
{
    return alerts;
}

uint8_t BMSModule::getCOVCells()
{
    return COVFaults;
}

uint8_t BMSModule::getCUVCells()
{
    return CUVFaults;
}

/*
Reading the setpoints, after a reset the default tesla setpoints are loaded
Default response : 0x10, 0x80, 0x31, 0x81, 0x08, 0x81, 0x66, 0xff
*/
/*
void BMSModule::readSetpoint()
{
  uint8_t payload[3];
  uint8_t buff[12];
  payload[0] = moduleAddress << 1; //adresss
  payload[1] = 0x40;//Alert Status start
  payload[2] = 0x08;//two registers
  sendData(payload, 3, false);
  delay(2);
  getReply(buff);

  OVolt = 2.0+ (0.05* buff[5]);
  UVolt = 0.7 + (0.1* buff[7]);
  Tset = 35 + (5 * (buff[9] >> 4));
} */

bool BMSModule::readModuleValues()
{
    uint8_t payload[4];
    uint8_t buff[50];
    uint8_t calcCRC;
    bool retVal = false;
    int retLen;
    float tempCalc;
    float tempTemp;
    
    payload[0] = moduleAddress << 1;
    
    bool statusOk = readStatus();
    if (!statusOk) {
        Logger::warn("Module %i status read failed", moduleAddress);
    }
    Logger::debug("Module %i   alerts=%X   faults=%X   COV=%X   CUV=%X", moduleAddress, alerts, faults, COVFaults, CUVFaults);
    
    payload[1] = REG_ADC_CTRL;
    payload[2] = 0b00111101; //ADC Auto mode, read every ADC input we can (Both Temps, Pack, 6 cells)
    BMSUtil::sendDataWithReply(payload, 3, true, buff, 3);
 
    payload[1] = REG_IO_CTRL;
    payload[2] = 0b00000011; //enable temperature measurement VSS pins
    BMSUtil::sendDataWithReply(payload, 3, true, buff, 3);
            
    payload[1] = REG_ADC_CONV; //start all ADC conversions
    payload[2] = 1;
    BMSUtil::sendDataWithReply(payload, 3, true, buff, 3);
                
    payload[1] = REG_GPAI; //start reading registers at the module voltage registers
    payload[2] = 0x12; //read 18 bytes (Each value takes 2 - ModuleV, CellV1-6, Temp1, Temp2)
    retLen = BMSUtil::sendDataWithReply(payload, 3, false, buff, 22);
    if (retLen != 22)
    {
        Logger::warn("Module %i returned unexpected length: %i", moduleAddress, retLen);
        return retVal;
    }

    calcCRC = BMSUtil::genCRC(buff, retLen-1);
    Logger::debug("Sent CRC: %x     Calculated CRC: %x", buff[21], calcCRC);

    //18 data bytes, address, command, length, and CRC = 22 bytes returned
    //Also validate CRC to ensure we didn't get garbage data.
    if (buff[21] == calcCRC)
    {
        if (buff[0] == (moduleAddress << 1) && buff[1] == REG_GPAI && buff[2] == 0x12) //Also ensure this is actually the reply to our intended query
        {
            //payload is 2 bytes gpai, 2 bytes for each of 6 cell voltages, 2 bytes for each of two temperatures (18 bytes of data)
            moduleVolt = (buff[3] * 256 + buff[4]) * VOLTAGE_CONVERSION_MODULE;
            if (moduleVolt > highestModuleVolt) highestModuleVolt = moduleVolt;
            if (moduleVolt < lowestModuleVolt) lowestModuleVolt = moduleVolt;
            for (int i = 0; i < 6; i++)
            {
                cellVolt[i] = (buff[5 + (i * 2)] * 256 + buff[6 + (i * 2)]) * VOLTAGE_CONVERSION_CELL;
                if (lowestCellVolt[i] > cellVolt[i] && cellVolt[i] >= IgnoreCell) lowestCellVolt[i] = cellVolt[i];
                if (highestCellVolt[i] < cellVolt[i]) highestCellVolt[i] = cellVolt[i];
            }
            
            auto decodeTemperature = [](uint16_t raw, uint16_t adjustment, float scale) -> float {
                float denominator = (static_cast<float>(raw) + static_cast<float>(adjustment)) / scale;
                if (denominator <= 0.0f) {
                    return NAN;
                }
                float tempTempLocal = (1.78f / denominator) - 3.57f;
                if (tempTempLocal <= 0.0f) {
                    return NAN;
                }
                tempTempLocal *= 1000.0f;
                float lnValue = logf(tempTempLocal);
                float denominatorSteinhart = 0.0007610373573f +
                                             (0.0002728524832f * lnValue) +
                                             (powf(lnValue, 3) * 0.0000001022822735f);
                if (denominatorSteinhart == 0.0f) {
                    return NAN;
                }
                float tempCalcLocal = 1.0f / denominatorSteinhart;
                float celsius = tempCalcLocal - TEMP_KELVIN_OFFSET;
                if (!isfinite(celsius)) {
                    return NAN;
                }
                return celsius;
            };

            const uint16_t rawTemp0 = static_cast<uint16_t>(buff[17] * 256 + buff[18]);
            const uint16_t rawTemp1 = static_cast<uint16_t>(buff[19] * 256 + buff[20]);
            float decodedTemp0 = decodeTemperature(rawTemp0, 2, 33046.0f);
            float decodedTemp1 = decodeTemperature(rawTemp1, 9, 33068.0f);

            if (isnan(decodedTemp0)) {
                Logger::warn("Module %i temperature sensor 0 returned invalid data (raw=%u)", moduleAddress, rawTemp0);
                temperatures[0] = TEMP_INIT_FOR_MIN_TRACKING;  // Set to high value so it won't affect min tracking
            } else {
                temperatures[0] = decodedTemp0;
            }

            if (isnan(decodedTemp1)) {
                Logger::warn("Module %i temperature sensor 1 returned invalid data (raw=%u)", moduleAddress, rawTemp1);
                temperatures[1] = TEMP_INIT_FOR_MIN_TRACKING;  // Set to high value so it won't affect min tracking
            } else {
                temperatures[1] = decodedTemp1;
            }

            if (isfinite(getLowTemp()) && getLowTemp() < lowestTemperature) lowestTemperature = getLowTemp();
            if (isfinite(getHighTemp()) && getHighTemp() > highestTemperature) highestTemperature = getHighTemp();

            Logger::debug("Got voltage and temperature readings");
            retVal = true;
        }        
        else
        {
            Logger::error("Unexpected reply header for module %i (addr=%x cmd=%x len=%x)", 
                          moduleAddress, buff[0], buff[1], buff[2]);
        }
    }
    else
    {
        Logger::error("Invalid module response received for module %i  len: %i   crc: %i   calc: %i", 
                      moduleAddress, retLen, buff[21], calcCRC);
    }
     
     //turning the temperature wires off here seems to cause weird temperature glitches
   // payload[1] = REG_IO_CTRL;
   // payload[2] = 0b00000000; //turn off temperature measurement pins
   // BMSUtil::sendData(payload, 3, true);
   // delay(3);        
   // BMSUtil::getReply(buff, 50);    //TODO: we're not validating the reply here. Perhaps check to see if a valid reply came back    
    
    return retVal;
}

float BMSModule::getCellVoltage(int cell)
{
    if (cell < 0 || cell > 5) return 0.0f;
    return cellVolt[cell];
}

float BMSModule::getLowCellV()
{
    float lowVal = 10.0f;
    for (int i = 0; i < 6; i++) if (cellVolt[i] < lowVal && cellVolt[i] > IgnoreCell) lowVal = cellVolt[i];
    return lowVal;
}

float BMSModule::getHighCellV()
{
    float hiVal = 0.0f;
    for (int i = 0; i < 6; i++) if (cellVolt[i] > hiVal) hiVal = cellVolt[i];
    return hiVal;
}

float BMSModule::getAverageV()
{
    int x =0;
    float avgVal = 0.0f;
    for (int i = 0; i < 6; i++)
    {
      if (cellVolt[i] > IgnoreCell)
      {
        x++;
        avgVal += cellVolt[i];
      }
    }

    if (x > 0) {
        avgVal /= x;
    }
    return avgVal;
}

float BMSModule::getHighestModuleVolt()
{
    return highestModuleVolt;
}

float BMSModule::getLowestModuleVolt()
{
    return lowestModuleVolt;
}

float BMSModule::getHighestCellVolt(int cell)
{
    if (cell < 0 || cell > 5) return 0.0f;
    return highestCellVolt[cell];    
}

float BMSModule::getLowestCellVolt(int cell)
{
    if (cell < 0 || cell > 5) return 0.0f;
    return lowestCellVolt[cell];
}

float BMSModule::getHighestTemp()
{
    return highestTemperature;
}

float BMSModule::getLowestTemp()
{
    return lowestTemperature;
}

float BMSModule::getLowTemp()
{
   return (temperatures[0] < temperatures[1]) ? temperatures[0] : temperatures[1]; 
}

float BMSModule::getHighTemp()
{
   return (temperatures[0] < temperatures[1]) ? temperatures[1] : temperatures[0];     
}

float BMSModule::getAvgTemp()
{
  if (sensor == 0)
  {
    return (temperatures[0] + temperatures[1]) / 2.0f;
  }
  else
  {
    return temperatures[sensor-1];
  }
}

float BMSModule::getModuleVoltage()
{
    return moduleVolt;
}

float BMSModule::getTemperature(int temp)
{
    if (temp < 0 || temp > 1) return 0.0f;
    return temperatures[temp];
}

void BMSModule::setAddress(int newAddr)
{
    if (newAddr < 0 || newAddr > MAX_MODULE_ADDR) return;
    moduleAddress = newAddr;
}

int BMSModule::getAddress()
{
    return moduleAddress;
}

bool BMSModule::isExisting()
{
    return exists;
}

void BMSModule::settempsensor(int tempsensor)
{
  sensor = tempsensor;
}

void BMSModule::setExists(bool ex)
{
    exists = ex;
}

void BMSModule::setIgnoreCell(float Ignore)
{
  IgnoreCell = Ignore;
}
