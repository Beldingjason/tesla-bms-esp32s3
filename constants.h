#ifndef CONSTANTS_H_
#define CONSTANTS_H_

// Timing constants (milliseconds)
#define LOOP_INTERVAL_MS 500
#define GUI_UPDATE_INTERVAL_MS 5000
#define PAGE_SWITCH_INTERVAL_MS 5000
#define STATUS_UPDATE_INTERVAL_MS 1000
#define WIFI_CONNECT_WAIT_MAX_MS 30000  // WiFi connection timeout before falling back to SmartConfig
#define STARTUP_DELAY_MS 3000
#define BALANCE_TIME_MS (5 * 60 * 1000)  // 5 minutes

// BMS Communication delays
#define BMS_COMMAND_DELAY_MS 2
#define BMS_READ_DELAY_MS 50

// BMS Register balance time limit
#define BMS_BALANCE_TIME_LIMIT_SEC 60

// Voltage constants
#define CELL_VOLTAGE_INIT_HIGH 5.0f
#define CELL_VOLTAGE_INIT_LOW 0.0f
#define MODULE_VOLTAGE_INIT_HIGH 200.0f
#define MODULE_VOLTAGE_INIT_LOW 0.0f
#define VOLTAGE_CONVERSION_MODULE 0.002034609f
#define VOLTAGE_CONVERSION_CELL 0.000381493f

// Temperature constants
// Note: These "backwards" values are used for min/max tracking initialization
#define TEMP_INIT_FOR_MIN_TRACKING 200.0f   // Initialize minimum temp tracker to high value
#define TEMP_INIT_FOR_MAX_TRACKING -100.0f  // Initialize maximum temp tracker to low value
#define TEMP_NO_SENSOR_THRESHOLD -70.0f
#define TEMP_KELVIN_OFFSET 273.15f

// SOC calculation constants (per 6S module)
#define SOC_MIN_MODULE_VOLTAGE 18.00f  // Minimum module voltage for SOC calculation (6S @ 3.0V/cell)
#define SOC_MAX_MODULE_VOLTAGE 25.2f   // Maximum module voltage for SOC calculation (6S @ 4.2V/cell)
#define CELLS_PER_MODULE 6

// LCD constants
#define LCD_BACKLIGHT_PWM_CHANNEL 0
#define LCD_BACKLIGHT_PWM_FREQ 10000
#define LCD_BACKLIGHT_PWM_RESOLUTION 8
#define LCD_BACKLIGHT_MAX_BRIGHTNESS 0xFF
#define LCD_PANEL_GAP_OFFSET 35

// ADC constants
#define ADC_RESOLUTION 4096
#define ADC_REFERENCE_VOLTAGE 3.3f
#define BAT_VOLTAGE_DIVIDER_RATIO 2

// Serial buffer constants
#define SERIAL_CMD_BUFFER_SIZE 80
#define SERIAL_CMD_MAX_INDEX 79

// BMS array constants
#define MAX_BMS_MODULES 63

// Retry constants
#define BMS_COMM_RETRY_COUNT 4

// Balance configuration constants
#define BMS_BALANCE_VOLTAGE_MIN 4.0f      // Volts - minimum voltage to allow balancing
#define BMS_BALANCE_VOLTAGE_DELTA 0.050f  // Volts - 50mV delta to trigger balancing (Tesla spec)

#endif // CONSTANTS_H_
