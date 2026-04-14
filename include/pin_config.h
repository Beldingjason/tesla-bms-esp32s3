#ifndef PIN_CONFIG_H_
#define PIN_CONFIG_H_

#define USE_WIFI                      0
// #define WIFI_SSID                    "Your-ssid"
// #define WIFI_PASSWORD               "Your-password"

#define NTP_SERVER1                  "pool.ntp.org"
#define NTP_SERVER2                  "time.nist.gov"
#define GMT_OFFSET_SEC               0
#define DAY_LIGHT_OFFSET_SEC         0

#define TESLA_BMS_LCD_BUS_I80        1
#define TESLA_BMS_LCD_BUS_SPI        2

#if defined(TESLA_BMS_TARGET_LILYGO_T_DISPLAY_S3) && defined(TESLA_BMS_TARGET_ESP32_S3_LCD_1_3_B)
#error "Select exactly one target board"
#elif !defined(TESLA_BMS_TARGET_LILYGO_T_DISPLAY_S3) && !defined(TESLA_BMS_TARGET_ESP32_S3_LCD_1_3_B)
#error "No target board selected. Define a TESLA_BMS_TARGET_* build flag."
#endif

#if defined(TESLA_BMS_TARGET_LILYGO_T_DISPLAY_S3)

/* LCD CONFIG */
#define TESLA_BMS_LCD_BUS            TESLA_BMS_LCD_BUS_I80
#define EXAMPLE_LCD_PIXEL_CLOCK_HZ   (6528000)
#define EXAMPLE_LCD_H_RES            320
#define EXAMPLE_LCD_V_RES            170
#define LVGL_LCD_BUF_SIZE            (EXAMPLE_LCD_H_RES * EXAMPLE_LCD_V_RES)
#define EXAMPLE_PSRAM_DATA_ALIGNMENT 64
#define LCD_PANEL_INVERT_COLORS      1
#define LCD_PANEL_SWAP_XY            1
#define LCD_PANEL_MIRROR_X           0
#define LCD_PANEL_MIRROR_Y           1
#define LCD_PANEL_GAP_X              0
#define LCD_PANEL_GAP_Y              35

#define PIN_LCD_BL                   38
#define PIN_LCD_D0                   39
#define PIN_LCD_D1                   40
#define PIN_LCD_D2                   41
#define PIN_LCD_D3                   42
#define PIN_LCD_D4                   45
#define PIN_LCD_D5                   46
#define PIN_LCD_D6                   47
#define PIN_LCD_D7                   48
#define PIN_POWER_ON                 15
#define PIN_LCD_RES                  5
#define PIN_LCD_CS                   6
#define PIN_LCD_DC                   7
#define PIN_LCD_WR                   8
#define PIN_LCD_RD                   9

#define PIN_BUTTON_1                 0
#define PIN_BUTTON_2                 14
#define PIN_BAT_VOLT                 4
#define PIN_BMS_FAULT                11

#define PIN_IIC_SCL                  17
#define PIN_IIC_SDA                  18

#define PIN_TOUCH_INT                16
#define PIN_TOUCH_RES                21

#elif defined(TESLA_BMS_TARGET_ESP32_S3_LCD_1_3_B)

/* Waveshare ESP32-S3-LCD-1.3 / ESP32-S3-LCD-1.3-B */
#define TESLA_BMS_LCD_BUS            TESLA_BMS_LCD_BUS_SPI
#define EXAMPLE_LCD_PIXEL_CLOCK_HZ   (40000000)
#define EXAMPLE_LCD_H_RES            240
#define EXAMPLE_LCD_V_RES            240
#define LVGL_LCD_BUF_SIZE            (EXAMPLE_LCD_H_RES * EXAMPLE_LCD_V_RES)
#define EXAMPLE_PSRAM_DATA_ALIGNMENT 64
#define LCD_PANEL_INVERT_COLORS      0
#define LCD_PANEL_SWAP_XY            0
#define LCD_PANEL_MIRROR_X           0
#define LCD_PANEL_MIRROR_Y           0
#define LCD_PANEL_GAP_X              0
#define LCD_PANEL_GAP_Y              0
#define LCD_SPI_HOST                 SPI2_HOST

#define PIN_LCD_BL                   20
#define PIN_POWER_ON                 (-1)
#define PIN_LCD_RES                  42
#define PIN_LCD_CS                   39
#define PIN_LCD_DC                   38
#define PIN_LCD_MOSI                 41
#define PIN_LCD_SCLK                 40

#define PIN_BUTTON_1                 0
#define PIN_BUTTON_2                 14
#define PIN_BAT_VOLT                 4
#define PIN_BMS_FAULT                11

#define PIN_IIC_SCL                  7
#define PIN_IIC_SDA                  6

#define PIN_TOUCH_INT                (-1)
#define PIN_TOUCH_RES                (-1)

#endif
#endif // PIN_CONFIG_H_
