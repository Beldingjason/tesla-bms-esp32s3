#ifndef CONSTANTS_H_
#define CONSTANTS_H_

// Timing constants (milliseconds)
#define LOOP_INTERVAL_MS 500
#define GUI_UPDATE_INTERVAL_MS 5000
#define PAGE_SWITCH_INTERVAL_MS 5000
#define STATUS_UPDATE_INTERVAL_MS 1000
#define WIFI_CONNECT_WAIT_MAX_MS 10000
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
#define TEMP_INIT_LOW 200.0f
#define TEMP_INIT_HIGH -100.0f
#define TEMP_NO_SENSOR_THRESHOLD -70.0f
#define TEMP_KELVIN_OFFSET 273.15f

// SOC calculation constants
#define SOC_MIN_CELL_VOLTAGE 18.00f  // Minimum cell voltage for SOC calculation
#define SOC_MAX_CELL_VOLTAGE 25.2f   // Maximum cell voltage for SOC calculation

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

#endif // CONSTANTS_H_
