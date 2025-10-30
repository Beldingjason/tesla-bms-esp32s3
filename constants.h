#ifndef CONSTANTS_H_
#define CONSTANTS_H_

/*
 * =====================================================================
 * TESLA BMS ESP32-S3 CONFIGURATION CONSTANTS
 * =====================================================================
 * This file contains all configuration constants used throughout the
 * BMS system. Modify these values carefully as they affect critical
 * battery management functions.
 * =====================================================================
 */

// =====================================================================
// TIMING CONSTANTS (milliseconds)
// =====================================================================

// Main loop interval - how often telemetry is collected
// Range: 100-2000ms (faster = more responsive, higher CPU usage)
#define LOOP_INTERVAL_MS 500

// GUI update interval - how often display elements are refreshed
// Range: 1000-10000ms
#define GUI_UPDATE_INTERVAL_MS 5000

// Page switch interval - automatic page rotation time
// Range: 2000-30000ms
#define PAGE_SWITCH_INTERVAL_MS 5000

// Status display refresh rate
// Range: 500-5000ms
#define STATUS_UPDATE_INTERVAL_MS 1000

// WiFi connection timeout before falling back to SmartConfig
// Range: 5000-60000ms (5-60 seconds)
// Reduced from 30s to prevent long blocking during startup
#define WIFI_CONNECT_WAIT_MAX_MS 15000

// Delay at startup to allow modules to stabilize
// Range: 1000-5000ms
#define STARTUP_DELAY_MS 3000

// Time between cell balancing cycles
// Range: 60000-600000ms (1-10 minutes)
// Longer intervals reduce wear, shorter intervals balance faster
#define BALANCE_TIME_MS (5 * 60 * 1000)  // 5 minutes

// =====================================================================
// BMS COMMUNICATION CONSTANTS
// =====================================================================

// Delay between BMS commands to allow module processing
// Range: 1-10ms
// DO NOT reduce below 2ms - modules need time to process commands
#define BMS_COMMAND_DELAY_MS 2

// Delay when reading back BMS registers for verification
// Range: 10-100ms
#define BMS_READ_DELAY_MS 50

// Delay during module setup/addressing operations
// Range: 2-10ms
#define BMS_SETUP_DELAY_MS 3

// Time limit for cell balancing resistors (per Tesla specification)
// Range: 10-300 seconds
// Hardware safety limit - balancing stops after this time if not retriggered
#define BMS_BALANCE_TIME_LIMIT_SEC 60

// =====================================================================
// VOLTAGE CONSTANTS
// =====================================================================

// Initial values for min/max voltage tracking
// These intentionally "backwards" values ensure first reading becomes the baseline
#define CELL_VOLTAGE_INIT_HIGH 5.0f       // Start high for minimum tracking
#define CELL_VOLTAGE_INIT_LOW 0.0f        // Start low for maximum tracking
#define MODULE_VOLTAGE_INIT_HIGH 200.0f   // Start high for minimum tracking
#define MODULE_VOLTAGE_INIT_LOW 0.0f      // Start low for maximum tracking

// ADC to voltage conversion factors (derived from Tesla BMS hardware)
// DO NOT MODIFY - These are hardware-specific calibration values
#define VOLTAGE_CONVERSION_MODULE 0.002034609f  // Module voltage ADC multiplier
#define VOLTAGE_CONVERSION_CELL 0.000381493f    // Cell voltage ADC multiplier

// =====================================================================
// TEMPERATURE CONSTANTS
// =====================================================================

// Initial values for min/max temperature tracking
// Note: These "backwards" values ensure first reading becomes the baseline
#define TEMP_INIT_FOR_MIN_TRACKING 200.0f    // Start high for minimum tracking
#define TEMP_INIT_FOR_MAX_TRACKING -100.0f   // Start low for maximum tracking

// Temperature sensor validation threshold (no longer used - kept for reference)
// Modern code uses NaN (Not a Number) for invalid sensor readings
#define TEMP_NO_SENSOR_THRESHOLD -70.0f

// Kelvin to Celsius conversion offset
#define TEMP_KELVIN_OFFSET 273.15f

// =====================================================================
// STATE OF CHARGE (SOC) CALCULATION CONSTANTS
// =====================================================================

// Per-module voltage range for SOC calculation (each module is 6S)
// Based on lithium-ion chemistry: 3.0V/cell (empty) to 4.2V/cell (full)
#define SOC_MIN_MODULE_VOLTAGE 18.00f  // 6 cells × 3.0V = 18V (0% SOC)
#define SOC_MAX_MODULE_VOLTAGE 25.2f   // 6 cells × 4.2V = 25.2V (100% SOC)
#define CELLS_PER_MODULE 6             // Tesla modules have 6 cells in series

// =====================================================================
// LCD DISPLAY CONSTANTS
// =====================================================================

#define LCD_BACKLIGHT_PWM_CHANNEL 0      // ESP32 PWM channel for backlight
#define LCD_BACKLIGHT_PWM_FREQ 10000     // PWM frequency in Hz
#define LCD_BACKLIGHT_PWM_RESOLUTION 8   // 8-bit resolution (0-255)
#define LCD_BACKLIGHT_MAX_BRIGHTNESS 0xFF // Maximum brightness value
#define LCD_PANEL_GAP_OFFSET 35          // Hardware-specific display offset

// =====================================================================
// ADC (ANALOG-TO-DIGITAL CONVERTER) CONSTANTS
// =====================================================================

#define ADC_RESOLUTION 4096                // 12-bit ADC (0-4095)
#define ADC_REFERENCE_VOLTAGE 3.3f         // ESP32 reference voltage
#define BAT_VOLTAGE_DIVIDER_RATIO 2        // Hardware voltage divider ratio

// =====================================================================
// SERIAL COMMUNICATION CONSTANTS
// =====================================================================

#define SERIAL_CMD_BUFFER_SIZE 80          // Command buffer size
#define SERIAL_CMD_MAX_INDEX 79            // Maximum buffer index

// =====================================================================
// BMS MODULE CONSTANTS
// =====================================================================

// Maximum number of BMS modules supported
// Range: 1-62 (address 0 is reserved for uninitialized modules, 63 is broadcast)
#define MAX_BMS_MODULES 63

// Communication retry count for failed operations
// Range: 1-10 (higher = more resilient, slower on failures)
#define BMS_COMM_RETRY_COUNT 4

// =====================================================================
// CELL BALANCING CONFIGURATION
// =====================================================================

// Minimum cell voltage required before balancing is allowed
// Range: 3.0-4.2V
// Safety measure to prevent balancing at low voltages
#define BMS_BALANCE_VOLTAGE_MIN 4.0f

// Voltage difference threshold to trigger cell balancing
// Range: 0.010-0.100V (10-100mV)
// Per Tesla specification: 50mV
// Lower values balance more aggressively, higher values reduce balancing cycles
#define BMS_BALANCE_VOLTAGE_DELTA 0.050f

// =====================================================================
// BMS COMMUNICATION BUFFER SIZES
// =====================================================================

// Buffer sizes for BMS module communication responses
// These are based on the Tesla BMS protocol specification
#define BMS_RESPONSE_BUFFER_SIZE_SMALL 8      // For short responses (status, simple queries)
#define BMS_RESPONSE_BUFFER_SIZE_STATUS 8     // For status register reads (alerts, faults)
#define BMS_RESPONSE_BUFFER_SIZE_MODULE 22    // For module values (voltages, temperatures)
#define BMS_RESPONSE_BUFFER_SIZE_GENERAL 30   // For general operations (setup, config)

// =====================================================================
// TEMPERATURE VALIDATION CONSTANTS
// =====================================================================

// Valid temperature range for Tesla battery modules
// Based on lithium-ion operating specifications and sensor capabilities
#define TEMP_MIN_VALID -40.0f   // Minimum valid temperature (°C)
#define TEMP_MAX_VALID 85.0f    // Maximum valid temperature (°C)

// =====================================================================
// INPUT VALIDATION CONSTANTS
// =====================================================================

// Valid range for parallel string configuration
#define PSTRING_MIN 1           // Minimum parallel strings
#define PSTRING_MAX 100         // Maximum parallel strings (reasonable limit)

// Valid range for battery ID
#define BATTERY_ID_MIN 0        // Minimum battery ID
#define BATTERY_ID_MAX 255      // Maximum battery ID (8-bit value)

// Valid voltage setpoint ranges (per cell, in volts)
#define VOLTAGE_SETPOINT_MIN 2.5f    // Minimum safe cell voltage
#define VOLTAGE_SETPOINT_MAX 4.3f    // Maximum safe cell voltage

// Valid temperature setpoint ranges (in °C)
#define TEMP_SETPOINT_MIN -40.0f     // Minimum temperature setpoint
#define TEMP_SETPOINT_MAX 100.0f     // Maximum temperature setpoint

// =====================================================================
// WATCHDOG AND TIMING INSTRUMENTATION
// =====================================================================

// Enable/disable performance timing measurements
// Set to 1 to enable detailed timing logs for watchdog verification
#define ENABLE_TIMING_INSTRUMENTATION 0

#endif // CONSTANTS_H_
