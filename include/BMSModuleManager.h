#ifndef BMSMODULEMANAGER_H_
#define BMSMODULEMANAGER_H_

#include "bms_config.h"
#include "BMSModule.h"

/**
 * @class BMSModuleManager
 * @brief Manages communication with and telemetry collection from Tesla BMS modules
 *
 * THREADING MODEL: This class is designed for SINGLE-THREADED access only.
 *
 * The class maintains mutable state (telemetry_, isFaulted_, etc.) that is
 * updated by collectTelemetry() and read by various getter methods. These
 * operations are NOT thread-safe and MUST be called from the same task/thread.
 *
 * If multi-threaded access is required, the caller MUST implement external
 * synchronization (e.g., mutex protection) around all calls to this class.
 *
 * Typical usage pattern (single-threaded):
 *   1. Call collectTelemetry() periodically to update internal state
 *   2. Call getTelemetry() / getters to read the latest collected data
 *   3. Call balanceCells() periodically to perform cell balancing
 *
 * All methods may call the watchdog callback (if configured) to prevent
 * watchdog timeouts during long operations.
 */
class BMSModuleManager
{
public:
    /**
     * @struct ModuleTelemetry
     * @brief Telemetry data for a single BMS module
     */
    struct ModuleTelemetry {
        bool present = false;               ///< True if module exists on the bus
        bool telemetryValid = false;        ///< True if telemetry data is current and valid
        float moduleVoltage = 0.0f;         ///< Total module voltage (V)
        float lowCell = 0.0f;               ///< Lowest cell voltage in module (V)
        float highCell = 0.0f;              ///< Highest cell voltage in module (V)
        float cellDelta = 0.0f;             ///< Voltage difference between highest and lowest cells (V)
        float lowTemp = 0.0f;               ///< Lowest temperature reading (°C), NAN if invalid
        float highTemp = 0.0f;              ///< Highest temperature reading (°C), NAN if invalid
        bool tempSensor0Fault = false;      ///< True if temperature sensor 0 has invalid data
        bool tempSensor1Fault = false;      ///< True if temperature sensor 1 has invalid data
        // Balancing telemetry
        bool isBalancing = false;           ///< True if module is currently balancing
        uint8_t balancingCellMask = 0;      ///< Bitmask of cells being balanced (bit 0-5)
        uint32_t lastBalanceTimeMs = 0;     ///< Timestamp of last balance operation (millis())
    };

    /**
     * @struct ModuleCommStats
     * @brief Communication statistics for a single BMS module
     */
    struct ModuleCommStats {
        uint32_t successCount = 0;          ///< Number of successful communication attempts
        uint32_t failureCount = 0;          ///< Number of failed communication attempts
        uint32_t getTotalAttempts() const { return successCount + failureCount; }
        float getSuccessRate() const {
            uint32_t total = getTotalAttempts();
            return total > 0 ? (float)successCount * 100.0f / (float)total : 0.0f;
        }
    };

    /**
     * @struct PackTelemetry
     * @brief Aggregated telemetry data for the entire battery pack
     */
    struct PackTelemetry {
        bool hasData = false;               ///< True if telemetry contains valid data
        float packVoltage = 0.0f;           ///< Total pack voltage (V)
        float lowestCell = 0.0f;            ///< Lowest cell voltage across all modules (V)
        float highestCell = 0.0f;           ///< Highest cell voltage across all modules (V)
        float cellDelta = 0.0f;             ///< Voltage difference between highest and lowest cells (V)
        float lowestPackVolt = 0.0f;        ///< Historical lowest pack voltage (V)
        float highestPackVolt = 0.0f;       ///< Historical highest pack voltage (V)
        float lowestPackTemp = 0.0f;        ///< Historical lowest temperature (°C), NAN if no valid data
        float highestPackTemp = 0.0f;       ///< Historical highest temperature (°C), NAN if no valid data
        bool faultPinActive = false;        ///< True if BMS fault pin is asserted (active low)
        bool anyTempSensorFaults = false;   ///< True if any temperature sensors have invalid data
        ModuleTelemetry modules[MAX_MODULE_ADDR + 1]; ///< Per-module telemetry (index = module address)
    };

    using WatchdogCallback = void (*)(); ///< Callback function type for watchdog reset

    BMSModuleManager();

    // === Module Management ===

    /** @brief Balance cells across all modules by activating balancing resistors for high cells */
    void balanceCells();

    /** @brief Set up uninitialized modules by assigning them addresses */
    void setupBoards();

    /** @brief Scan for modules on the bus and count them */
    void findBoards();

    /** @brief Reset all modules to address 0 and reassign sequentially */
    void renumberBoardIDs();

    /** @brief Clear fault status registers on all modules */
    void clearFaults();

    /** @brief Put all modules into sleep mode (low power state) */
    void sleepBoards();

    /** @brief Wake all modules from sleep mode */
    void wakeBoards();

    // === Telemetry ===

    /**
     * @brief Collect voltage and temperature telemetry from all modules
     * @return true if valid telemetry was collected, false otherwise
     *
     * This method polls all present modules for their current state and updates
     * the internal telemetry structure. If some modules fail to respond, telemetry
     * from successfully read modules is still returned, but hasData may be false
     * if too many failures occur.
     *
     * Execution time scales with number of modules present. Watchdog callback
     * is called periodically during collection.
     */
    bool collectTelemetry();

    /**
     * @brief Get the most recently collected telemetry data
     * @return const reference to PackTelemetry structure
     *
     * Returns the telemetry from the last successful call to collectTelemetry().
     * Check hasData field to verify if data is valid.
     */
    const PackTelemetry &getTelemetry() const;

    // === Configuration ===

    /**
     * @brief Set callback function to reset watchdog timer
     * @param callback Function pointer to watchdog reset function, or nullptr to disable
     */
    void setWatchdogCallback(WatchdogCallback callback);

    /**
     * @brief Set the battery ID for CAN communication
     * @param id Battery ID (valid range: 0-255)
     */
    void setBatteryID(int id);

    /**
     * @brief Set the number of parallel strings in the pack configuration
     * @param Pstrings Number of parallel strings (valid range: 1-100)
     *
     * This value is used to calculate pack voltage. For example, if you have
     * 2 modules in series and 3 parallel strings, set this to 3.
     */
    void setPstrings(int Pstrings);

    /**
     * @brief Configure temperature sensor selection and cell voltage ignore threshold
     * @param sensor Temperature sensor to use (0=both, 1=sensor0, 2=sensor1)
     * @param Ignore Cell voltage below which cells are ignored (0 to disable, or >= 2.5V)
     */
    void setSensors(int sensor, float Ignore);
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
    void resetHistoricalData();
    /*
    void processCANMsg(CAN_FRAME &frame);
    */
    void printPackSummary();
    void printPackDetails();
    void printDiagnostics();
    

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
};


#endif // BMSMODULEMANAGER_H_
