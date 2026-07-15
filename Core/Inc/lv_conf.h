/**
 * @file lv_conf.h
 * LVGL v9.6.0-dev configuration for STM32H743 + FreeRTOS + ILI9341/ST7789V (320x240 RGB565)
 * Optimized for camera-overlay mode: LVGL draws only top bar + bottom panel,
 * camera uses SPI DMA for the middle 200px band.
 *
 * Copy this file alongside the lvgl source tree or ensure it's in the include path.
 */

#ifndef LV_CONF_H
#define LV_CONF_H

/*====================
   COLOR SETTINGS
 *====================*/

/** Color depth: 16-bit RGB565 (matches ST7789V 16-bit interface) */
#define LV_COLOR_DEPTH 16

/*=========================
   STDLIB WRAPPER SETTINGS
 *=========================*/

#define LV_USE_STDLIB_MALLOC    LV_STDLIB_BUILTIN
#define LV_USE_STDLIB_STRING    LV_STDLIB_BUILTIN
#define LV_USE_STDLIB_SPRINTF   LV_STDLIB_BUILTIN

#define LV_STDINT_INCLUDE       <stdint.h>
#define LV_STDDEF_INCLUDE       <stddef.h>
#define LV_STDBOOL_INCLUDE      <stdbool.h>
#define LV_INTTYPES_INCLUDE     <inttypes.h>
#define LV_LIMITS_INCLUDE       <limits.h>
#define LV_STDARG_INCLUDE       <stdarg.h>

#if LV_USE_STDLIB_MALLOC == LV_STDLIB_BUILTIN
    /** 32KB memory pool */
    #define LV_MEM_SIZE (32 * 1024U)
    #define LV_MEM_POOL_EXPAND_SIZE 0
    #define LV_MEM_ADR 0
#endif

/*====================
   HAL SETTINGS
 *====================*/

/** Default display refresh period (30fps ~= 33ms) */
#define LV_DEF_REFR_PERIOD  33      /**< [ms] */

/** Default DPI. Adjust to match your screen (2.0" 240x320 ≈ 200 DPI) */
#define LV_DPI_DEF 130

/*=================
 * OPERATING SYSTEM
 *=================*/
/** Use FreeRTOS (underlying kernel; CMSIS-RTOS v1 wrapper also uses FreeRTOS) */
#define LV_USE_OS   LV_OS_NONE  /* STEP 2: test without internal draw thread */

#if LV_USE_OS == LV_OS_FREERTOS
    /** Use direct task notification (45% faster, less RAM) */
    #define LV_USE_FREERTOS_TASK_NOTIFY 1
    #define LV_OS_IDLE_PERCENT_CUSTOM 0
#endif

/*========================
 * RENDERING CONFIGURATION
 *========================*/

#define LV_DRAW_BUF_STRIDE_ALIGN                1
#define LV_DRAW_BUF_ALIGN                       4
#define LV_DRAW_TRANSFORM_USE_MATRIX            0

/** Buffer size used for "simple" layers during rendering */
#define LV_DRAW_LAYER_SIMPLE_BUF_SIZE    (12 * 1024)    /**< [bytes] */

/** No global limit on layer memory */
#define LV_DRAW_LAYER_MAX_MEMORY 0

/** Drawing thread stack size (FreeRTOS) */
#define LV_DRAW_THREAD_STACK_SIZE    (8 * 1024)         /**< [bytes] */

/** Drawing thread priority (higher = more CPU for rendering) */
#define LV_DRAW_THREAD_PRIO LV_THREAD_PRIO_HIGH

/*--------- Software rendering ---------*/
#define LV_USE_DRAW_SW 1
#if LV_USE_DRAW_SW
    /** Only enable RGB565 support to save flash (our LCD uses RGB565) */
    #define LV_DRAW_SW_SUPPORT_RGB565       1
    #define LV_DRAW_SW_SUPPORT_RGB565_SWAPPED       0
    #define LV_DRAW_SW_SUPPORT_RGB565A8     0
    #define LV_DRAW_SW_SUPPORT_RGB888       0
    #define LV_DRAW_SW_SUPPORT_XRGB8888     0
    #define LV_DRAW_SW_SUPPORT_ARGB8888     0
    #define LV_DRAW_SW_SUPPORT_ARGB8888_PREMULTIPLIED 0
    #define LV_DRAW_SW_SUPPORT_L8           0
    #define LV_DRAW_SW_SUPPORT_AL88         0
    #define LV_DRAW_SW_SUPPORT_A8           0
    #define LV_DRAW_SW_SUPPORT_I1           0

    #define LV_DRAW_SW_I1_LUM_THRESHOLD 127

    /** Single draw unit (no parallel rendering without multi-core) */
    #define LV_DRAW_SW_DRAW_UNIT_CNT    1

    #define LV_USE_DRAW_SW_ASM     LV_DRAW_SW_ASM_NONE

    /** Enable complex SW rendering (rounded corners, shadows, arcs, etc.) */
    #define LV_DRAW_SW_COMPLEX          1

    #if LV_DRAW_SW_COMPLEX == 1
        #define LV_DRAW_SW_SHADOW_CACHE_SIZE 0
        #define LV_DRAW_SW_CIRCLE_CACHE_SIZE 4
    #endif

    #define LV_USE_DRAW_SW_COMPLEX_GRADIENTS    0
#endif

/*--------- Accelerated rendering (DMA2D, GPU, etc.) ---------*/
#define LV_USE_DRAW_DMA2D 0
#define LV_USE_NEMA_GFX 0
#define LV_USE_PXP 0
#define LV_USE_G2D 0
#define LV_USE_DRAW_DAVE2D 0
#define LV_USE_DRAW_SDL 0
#define LV_USE_DRAW_VG_LITE 0
#define LV_USE_DRAW_OPENGLES 0
#define LV_USE_PPA  0
#define LV_USE_DRAW_EVE 0

/*=======================
 * FEATURE USAGE
 *=======================*/

/*--------- Logging ---------*/
#define LV_USE_LOG 0
#if LV_USE_LOG
    #define LV_LOG_LEVEL LV_LOG_LEVEL_WARN
    #define LV_LOG_PRINTF 0
#endif

/*--------- Asserts ---------*/
#define LV_USE_ASSERT_NULL          0
#define LV_USE_ASSERT_MALLOC        0
#define LV_USE_ASSERT_STYLE         0
#define LV_USE_ASSERT_MEM_INTEGRITY 0
#define LV_USE_ASSERT_OBJ           0

/*--------- Debug ---------*/
#define LV_USE_DEBUG 0

/*--------- Input devices ---------*/
#define LV_USE_KEYBOARD     0
#define LV_USE_MOUSEWHEEL    0
#define LV_USE_BUTTON        0
#define LV_USE_ENCODER       0

/*--------- Other features ---------*/
#define LV_USE_MATRIX           0
#define LV_USE_PRIVATE_API      0

/*==================
 *   FONT USAGE
 *===================*/
#define LV_FONT_MONTSERRAT_8  0
#define LV_FONT_MONTSERRAT_10 0
#define LV_FONT_MONTSERRAT_12 0
#define LV_FONT_MONTSERRAT_14 1   /**< Required by default theme */
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

/* Compressed font support (save flash) */
#define LV_FONT_MONTSERRAT_28_COMPRESSED    0

/*==================
 *  WIDGET USAGE
 *==================*/

/** Widgets: keep commonly-used ones, disable rarely-used to save flash */
#define LV_USE_ANIMIMG      1
#define LV_USE_ARC          1
#define LV_USE_BAR          1
#define LV_USE_BUTTON       1
#define LV_USE_BUTTONMATRIX 1
#define LV_USE_CALENDAR     1
#define LV_USE_CANVAS       1
#define LV_USE_CHART        1
#define LV_USE_CHECKBOX     1
#define LV_USE_DROPDOWN     1
#define LV_USE_IMAGE        1
#define LV_USE_IMAGEBUTTON  1
#define LV_USE_KEYBOARD     1
#define LV_USE_LABEL        1
#define LV_USE_LED          1
#define LV_USE_LINE         1
#define LV_USE_LIST         1
#define LV_USE_LOTTIE       0   /**< Requires ThorVG */
#define LV_USE_MENU         1
#define LV_USE_MSGBOX       1
#define LV_USE_ROLLER       1
#define LV_USE_SCALE        1
#define LV_USE_SLIDER       1
#define LV_USE_SPAN         1
#define LV_USE_SPINBOX      1
#define LV_USE_SPINNER      1
#define LV_USE_SWITCH       1
#define LV_USE_TABLE        1
#define LV_USE_TABVIEW      1
#define LV_USE_TEXTAREA     1
#define LV_USE_TILEVIEW     1
#define LV_USE_WIN          1

/*==================
 *  EXTRA COMPONENTS
 *==================*/

/*--------- File system ---------*/
#define LV_USE_FS_FATFS 0
#define LV_USE_FS_STDIO 0
#define LV_USE_FS_POSIX 0
#define LV_USE_FS_WIN32 0
#define LV_USE_FS_MEMFS 0
#define LV_USE_FS_LITTLEFS 0
#define LV_USE_FS_ARDUINO_ESP 0
#define LV_USE_FS_ARDUINO_SD 0

/*--------- Image decoders ---------*/
#define LV_USE_BMP      0
#define LV_USE_GIF      0
#define LV_USE_PNG      0
#define LV_USE_SVG      0
#define LV_USE_TJPGD    0
#define LV_USE_LIBJPEG_TURBO 0
#define LV_USE_FFMPEG   0

/*--------- Binary decoders (keep for image cache) ---------*/
#define LV_USE_BIN_DECODER 1
#define LV_BIN_DECODER_RAM_LOAD 1

/*--------- Other libraries ---------*/
#define LV_USE_RLE          0
#define LV_USE_RLE_DECODER  0
#define LV_USE_LODEPNG      0
#define LV_USE_BARCODE      0
#define LV_USE_QRCODE       0
#define LV_USE_FREETYPE     0
#define LV_USE_TINY_TTF     0
#define LV_USE_FONT_MANAGER 0

/*--------- Third-party ---------*/
#define LV_USE_THORVG_INTERNAL   0
#define LV_USE_THORVG_EXTERNAL   0
#define LV_USE_LOTTIE            0
#define LV_USE_BLEND2D           0

/*==================
 *  THEMES
 *==================*/
#define LV_USE_THEME_DEFAULT 1
#if LV_USE_THEME_DEFAULT
    #define LV_THEME_DEFAULT_DARK  0
    #define LV_THEME_DEFAULT_GROW  1
    #define LV_THEME_DEFAULT_TRANSITION_TIME 80
#endif

#define LV_USE_THEME_SIMPLE 0
#define LV_USE_THEME_MONO   0

/*==================
 *  LAYOUTS
 *==================*/
#define LV_USE_FLEX   1
#define LV_USE_GRID   1

/*==================
 *  OTHERS
 *==================*/
#define LV_USE_SNAPSHOT 0
#define LV_USE_SYSMON   0
#define LV_USE_GRIDNAV  0
#define LV_USE_FRAGMENT 0
#define LV_USE_FILE_EXPLORER 0
#define LV_USE_OBSERVER 1
#define LV_USE_IME_PINYIN 0

/*===================
 *  CACHE (v9.x)
 *===================*/
#define LV_CACHE_DEF_SIZE   0               /**< Default image cache size (0=disable) */
#define LV_IMAGE_HEADER_CACHE_DEF_CNT 0     /**< Disable image header cache */

/*============================
 *  COMPILER SETTINGS
 *============================*/
#define LV_ATTRIBUTE_MEM_ALIGN
#define LV_ATTRIBUTE_LARGE_CONST
#define LV_ATTRIBUTE_FAST_CODE
#define LV_ATTRIBUTE_NOINLINE

#endif /* LV_CONF_H */
