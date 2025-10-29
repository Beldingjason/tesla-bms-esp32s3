#ifndef BMSMODULEMANAGER_H_
#define BMSMODULEMANAGER_H_

#include "bms_config.h"
#include "BMSModule.h"

class BMSModuleManager
{
public:
    struct ModuleTelemetry {
        bool present = false;
        bool telemetryValid = false;
        float moduleVoltage = 0.0f;
        float lowCell = 0.0f;
        float highCell = 0.0f;
        float cellDelta = 0.0f;
        float lowTemp = 0.0f;
        float highTemp = 0.0f;
    };

    struct ModuleCommStats {
        uint32_t successCount = 0;
        uint32_t failureCount = 0;
        uint32_t getTotalAttempts() const { return successCount + failureCount; }
        float getSuccessRate() const {
            uint32_t total = getTotalAttempts();
            return total > 0 ? (float)successCount * 100.0f / (float)total : 0.0f;
        }
    };

    struct PackTelemetry {
        bool hasData = false;
        float packVoltage = 0.0f;
        float lowestCell = 0.0f;
        float highestCell = 0.0f;
        float cellDelta = 0.0f;
        float lowestPackVolt = 0.0f;
        float highestPackVolt = 0.0f;
        float lowestPackTemp = 0.0f;
        float highestPackTemp = 0.0f;
        bool faultPinActive = false;
        ModuleTelemetry modules[MAX_MODULE_ADDR + 1];
    };

    using WatchdogCallback = void (*)();

    BMSModuleManager();
    void balanceCells();
    void setupBoards();
    void findBoards();
    void renumberBoardIDs();
    void clearFaults();
    void sleepBoards();
    void wakeBoards();
    bool collectTelemetry();
    const PackTelemetry &getTelemetry() const;
    void setWatchdogCallback(WatchdogCallback callback);
    void readSetpoints();
    void setBatteryID(int id);
    void setPstrings(int Pstrings);
    void setUnderVolt(float newVal);
    void setOverVolt(float newVal);
    void setOverTemp(float newVal);
    void setBalanceV(float newVal);
    void setBalanceHyst(float newVal);
    void setSensors(int sensor,float Ignore);
    float getPackVoltage();
    float getAvgTemperature();
    float getAvgCellVolt();
    float getLowCellVolt();
    float getHighCellVolt();
    float getHighVoltage();
    float getLowVoltage();
    bool isFaulted() const;
    const ModuleCommStats& getModuleCommStats(int moduleIndex) const;
    void resetModuleCommStats(int moduleIndex);
    void resetAllCommStats();
    /*
    void processCANMsg(CAN_FRAME &frame);
    */
    void printPackSummary();
    void printPackDetails();
    

private:
    bool updateModuleTelemetry(int moduleIndex, ModuleTelemetry &outTelemetry);
    void applyWatchdogReset() const;

    PackTelemetry telemetry_;
    WatchdogCallback watchdogCallback_;
    int Pstring;
    BMSModule modules[MAX_MODULE_ADDR + 1]; // store data for as many modules as we've configured for.
    ModuleCommStats commStats_[MAX_MODULE_ADDR + 1]; // Communication statistics per module
    int batteryID;
    int numFoundModules;                    // The number of modules that seem to exist
    bool isFaulted_;
    float lowestPackVoltHistory_;
    float highestPackVoltHistory_;
    float lowestPackTempHistory_;
    float highestPackTempHistory_;
    /*
    void sendBatterySummary();
    void sendModuleSummary(int module);
    void sendCellDetails(int module, int cell);
    */

};

#endif // BMSMODULEMANAGER_H_
