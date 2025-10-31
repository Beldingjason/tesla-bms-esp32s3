/**
 * @file lv_conf.h
 * LVGL Configuration for Tesla BMS ESP32-S3 T-Display
 * Based on LVGL v8.4.0
 */

#ifndef LV_CONF_H
#define LV_CONF_H

#include <stdint.h>

/*====================
   COLOR SETTINGS
 *====================*/

/* Color depth: 16 (RGB565) for ST7789V display */
#define LV_COLOR_DEPTH 16

/* Swap the 2 bytes of RGB565 color for SPI interface */
#define LV_COLOR_16_SWAP 0

/* Enable transparent background features */
#define LV_COLOR_SCREEN_TRANSP 0

/* Color mix rounding */
#define LV_COLOR_MIX_ROUND_OFS 0

/* Chroma key color */
#define LV_COLOR_CHROMA_KEY lv_color_hex(0x00ff00)

/*=========================
   MEMORY SETTINGS
 *=========================*/

/* Use custom malloc/free (Arduino) */
#define LV_MEM_CUSTOM 1
#if LV_MEM_CUSTOM == 1
    #define LV_MEM_CUSTOM_INCLUDE <stdlib.h>
    #define LV_MEM_CUSTOM_ALLOC   malloc
    #define LV_MEM_CUSTOM_FREE    free
    #define LV_MEM_CUSTOM_REALLOC realloc
#endif

/* Number of intermediate memory buffers */
#define LV_MEM_BUF_MAX_NUM 16

/* Use standard memcpy/memset */
#define LV_MEMCPY_MEMSET_STD 0

/*====================
   HAL SETTINGS
 *====================*/

/* Display refresh period */
#define LV_DISP_DEF_REFR_PERIOD 30      /* [ms] */

/* Input device read period */
#define LV_INDEV_DEF_READ_PERIOD 30     /* [ms] */

/* Use Arduino millis() for tick source */
#define LV_TICK_CUSTOM 1
#if LV_TICK_CUSTOM
    #define LV_TICK_CUSTOM_INCLUDE "Arduino.h"
    #define LV_TICK_CUSTOM_SYS_TIME_EXPR (millis())
#endif

/* DPI setting for widget sizes */
#define LV_DPI_DEF 130

/*==========================
   COMPILER SETTINGS
 *==========================*/

/* For big endian systems set to 1 */
#define LV_BIG_ENDIAN_SYSTEM 0

/* Define a custom attribute for large constant arrays */
#define LV_ATTRIBUTE_LARGE_CONST

/* Prefix performance critical functions for faster execution */
#define LV_ATTRIBUTE_FAST_MEM

/* Prefix variables in large RAM */
#define LV_ATTRIBUTE_LARGE_RAM_ARRAY

/* Export integer constant to binding */
#define LV_EXPORT_CONST_INT(int_value) struct _silence_gcc_warning

/* Extend default -32k..32k coordinate range to -4M..4M */
#define LV_USE_LARGE_COORD 0

/*===================
   FONT USAGE
 *===================*/

/* Montserrat fonts with various pixel heights */
#define LV_FONT_MONTSERRAT_8  0
#define LV_FONT_MONTSERRAT_10 0
#define LV_FONT_MONTSERRAT_12 0
#define LV_FONT_MONTSERRAT_14 1
#define LV_FONT_MONTSERRAT_16 1
#define LV_FONT_MONTSERRAT_18 0
#define LV_FONT_MONTSERRAT_20 1
#define LV_FONT_MONTSERRAT_22 0
#define LV_FONT_MONTSERRAT_24 1
#define LV_FONT_MONTSERRAT_26 0
#define LV_FONT_MONTSERRAT_28 0
#define LV_FONT_MONTSERRAT_30 0
#define LV_FONT_MONTSERRAT_32 0
#define LV_FONT_MONTSERRAT_34 0
#define LV_FONT_MONTSERRAT_36 0
#define LV_FONT_MONTSERRAT_38 0
#define LV_FONT_MONTSERRAT_40 0
#define LV_FONT_MONTSERRAT_42 0
#define LV_FONT_MONTSERRAT_44 0
#define LV_FONT_MONTSERRAT_46 0
#define LV_FONT_MONTSERRAT_48 0

/* Demonstrate special features */
#define LV_FONT_MONTSERRAT_12_SUBPX 0
#define LV_FONT_MONTSERRAT_28_COMPRESSED 0
#define LV_FONT_DEJAVU_16_PERSIAN_HEBREW 0
#define LV_FONT_SIMSUN_16_CJK 0

/* Pixel perfect monospace font */
#define LV_FONT_UNSCII_8 0
#define LV_FONT_UNSCII_16 0

/* Custom font for the project */
#define LV_FONT_CUSTOM_DECLARE

/* Default font */
#define LV_FONT_DEFAULT &lv_font_montserrat_14

/* Enable built-in fonts */
#define LV_FONT_FMT_TXT_LARGE 0

/* Set Freetype support */
#define LV_USE_FONT_COMPRESSED 0
#define LV_USE_FONT_SUBPX 0

/*=================
   TEXT SETTINGS
 *=================*/

/* Select character encoding */
#define LV_TXT_ENC LV_TXT_ENC_UTF8

/* Can break texts on these characters */
#define LV_TXT_BREAK_CHARS " ,.;:-_"

/* Line wrap with longer texts */
#define LV_TXT_LINE_BREAK_LONG_LEN 0

/* Minimum word length for line break */
#define LV_TXT_LINE_BREAK_LONG_PRE_MIN_LEN 3
#define LV_TXT_LINE_BREAK_LONG_POST_MIN_LEN 3

/* Character used for newline */
#define LV_TXT_COLOR_CMD "#"

/* Support bi-directional text */
#define LV_USE_BIDI 0
#if LV_USE_BIDI
    #define LV_BIDI_BASE_DIR_DEF LV_BASE_DIR_AUTO
#endif

/* Enable Arabic/Persian processing */
#define LV_USE_ARABIC_PERSIAN_CHARS 0

/*=================
   WIDGET USAGE
 *=================*/

/* Enable standard widgets */
#define LV_USE_ARC        1
#define LV_USE_BAR        1
#define LV_USE_BTN        1
#define LV_USE_BTNMATRIX  1
#define LV_USE_CANVAS     1
#define LV_USE_CHECKBOX   1
#define LV_USE_DROPDOWN   1
#define LV_USE_IMG        1
#define LV_USE_LABEL      1
#define LV_USE_LINE       1
#define LV_USE_ROLLER     1
#define LV_USE_SLIDER     1
#define LV_USE_SWITCH     1
#define LV_USE_TEXTAREA   1
#define LV_USE_TABLE      1

/*==================
 * EXTRA COMPONENTS
 *==================*/

/* Enable complex widgets */
#define LV_USE_ANIMIMG    0
#define LV_USE_CALENDAR   0
#define LV_USE_CHART      0
#define LV_USE_COLORWHEEL 0
#define LV_USE_IMGBTN     0
#define LV_USE_KEYBOARD   0
#define LV_USE_LED        0
#define LV_USE_LIST       0
#define LV_USE_MENU       0
#define LV_USE_METER      0
#define LV_USE_MSGBOX     0
#define LV_USE_SPAN       0
#define LV_USE_SPINBOX    0
#define LV_USE_SPINNER    0
#define LV_USE_TABVIEW    0
#define LV_USE_TILEVIEW   1
#define LV_USE_WIN        0

/*==================
 * EXTRA COMPONENTS
 *==================*/

/* Message API for pub/sub pattern */
#define LV_USE_MSG 1

/*==================
 * THEMES
 *==================*/

/* Default theme for this board */
#define LV_USE_THEME_DEFAULT 1
#if LV_USE_THEME_DEFAULT
    #define LV_THEME_DEFAULT_DARK 0
    #define LV_THEME_DEFAULT_GROW 1
    #define LV_THEME_DEFAULT_TRANSITION_TIME 80
#endif

#define LV_USE_THEME_BASIC 1
#define LV_USE_THEME_MONO 1

/*==================
 * LAYOUTS
 *==================*/

#define LV_USE_FLEX 1
#define LV_USE_GRID 1

/*=====================
 * COMPILER SETTINGS
 *=====================*/

/* For GCC/LLVM define 1 */
#define LV_USE_GC_BUILTIN 0

/* Size of stack for draw task */
#define LV_DRAW_COMPLEX 1

/* Buffer size for log */
#define LV_LOG_LEVEL LV_LOG_LEVEL_WARN
#define LV_LOG_PRINTF 0

/* Garbage collector */
#define LV_ENABLE_GC 0

/*===================
 * OTHERS
 *==================*/

/* 1: Show CPU usage and FPS count */
#define LV_USE_PERF_MONITOR 0
#define LV_USE_MEM_MONITOR 0

/* 1: Draw random colored rectangles over object */
#define LV_USE_REFR_DEBUG 0

/* Change color on draw area */
#define LV_USE_OBJ_ID 0
#define LV_USE_OBJ_ID_BUILTIN 0

/* Use Tiny TTF library */
#define LV_USE_TINY_TTF 0

/* Use GPU */
#define LV_USE_GPU_ARM2D 0
#define LV_USE_GPU_STM32_DMA2D 0
#define LV_USE_GPU_SWM341_DMA2D 0
#define LV_USE_GPU_NXP_PXP 0
#define LV_USE_GPU_NXP_VG_LITE 0
#define LV_USE_GPU_SDL 0

/* Use external renderer */
#define LV_USE_EXTERNAL_RENDERER 0

/* File system interfaces */
#define LV_USE_FS_STDIO 0
#define LV_USE_FS_POSIX 0
#define LV_USE_FS_WIN32 0
#define LV_USE_FS_FATFS 0

/* PNG decoder */
#define LV_USE_PNG 0

/* BMP decoder */
#define LV_USE_BMP 0

/* JPG + split JPG decoder */
#define LV_USE_SJPG 0

/* GIF decoder */
#define LV_USE_GIF 0

/* QR code generator */
#define LV_USE_QRCODE 0

/* FreeType library */
#define LV_USE_FREETYPE 0

/* RLE compressed images */
#define LV_USE_RLE 0

/* FFmpeg library for image/video decoding */
#define LV_USE_FFMPEG 0

/* Tiny TTF library */
#define LV_USE_TINY_TTF 0

/* Rlottie library for Lottie animations */
#define LV_USE_RLOTTIE 0

/* Enable snapshot */
#define LV_USE_SNAPSHOT 0

/* Monkey test */
#define LV_USE_MONKEY 0

/* Grid navigation */
#define LV_USE_GRIDNAV 0

/* Fragment */
#define LV_USE_FRAGMENT 0

/* LVGL Demos */
#define LV_USE_DEMO_WIDGETS 0
#define LV_USE_DEMO_KEYPAD_AND_ENCODER 0
#define LV_USE_DEMO_BENCHMARK 0
#define LV_USE_DEMO_STRESS 0
#define LV_USE_DEMO_MUSIC 0

/* Assert */
#define LV_ASSERT_HANDLER_INCLUDE <stdint.h>
#define LV_ASSERT_HANDLER while(1);

/* Use standard memcpy/memset */
#define LV_MEMCPY_MEMSET_STD 0

#endif /* LV_CONF_H */
