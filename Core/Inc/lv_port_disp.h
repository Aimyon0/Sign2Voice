/**
 * @file lv_port_disp.h
 * LVGL v9.x Display Port — 320×240 RGB565, overlay mode
 *
 * Architecture:
 *  ┌──────────────────────────┐ y=0
 *  │ TOP BAR     (LVGL draws) │ h = UI_TOP_H (22px)
 *  ├──────────────────────────┤ y=22
 *  │ CAMERA BAND (DMA direct) │ h = 150px
 *  │            ❌ LVGL skips  │
 *  ├──────────────────────────┤ y=172
 *  │ BOTTOM      (LVGL draws) │ h = 68px
 *  └──────────────────────────┘ y=240
 *
 * SPI arbitration:
 *  - lcd_spi_mutex (FreeRTOS) serialises camera DMA and LVGL flush.
 *  - Both sides must: xSemaphoreTake(mutex, timeout) → SPI ops → xSemaphoreGive(mutex)
 */

#ifndef LV_PORT_DISP_H
#define LV_PORT_DISP_H

#ifdef __cplusplus
extern "C" {
#endif

#include "lvgl.h"

/*------------------
 *  Display layout
 *------------------*/
#define LV_PORT_DISP_HOR_RES    240
#define LV_PORT_DISP_VER_RES    320

/* UI band definitions (match ui_overlay.h) */
#define UI_TOP_H     20    /* Top status bar height (px) */
#define UI_CAMERA_Y0 20    /* Camera band start row */
#define UI_CAMERA_Y1 240   /* Camera band end row (20 + 220) */
/* Single 60-line buffer = 37.5KB. Fits max flush chunk, safe for swap. */
#define LV_PORT_DISP_BUF_LINES  60
#define LV_PORT_DISP_BUF_SIZE   (LV_PORT_DISP_HOR_RES * LV_PORT_DISP_BUF_LINES)
/* LV_PORT_DISP_BUF_SIZE = 240 * 60 = 14400 pixels */

/*------------------
 *  SPI mutex
 *------------------*/
#include "FreeRTOS.h"
#include "semphr.h"

/** Global mutex protecting SPI2 from concurrent access.
 *  Initialised once by lv_port_disp_init().
 *  Camera DMA refresh MUST take this mutex too (see camera_band_refresh). */
extern SemaphoreHandle_t lcd_spi_mutex;

/*------------------
 *  API
 *------------------*/

/**
 * @brief  Initialise LVGL display driver + SPI mutex.
 * @note   Must be called after: LCD_Init(), lv_init(), scheduler start.
 * @return lv_display_t* or NULL on failure.
 */
lv_display_t *lv_port_disp_init(void);

#ifdef __cplusplus
}
#endif

#endif /* LV_PORT_DISP_H */
