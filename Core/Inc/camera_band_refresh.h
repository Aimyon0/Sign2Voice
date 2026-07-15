/**
 * @file camera_band_refresh.h
 * Camera-band SPI DMA refresh — replaces full-screen Start_LCD_DMA_Refresh()
 *
 * Instead of writing 240×320 = 76800 px to the LCD (which would overwrite
 * LVGL's top bar and bottom panel), this function ONLY writes the camera
 * band: rows [UI_CAMERA_Y0, UI_CAMERA_Y1) = [22, 172), i.e. 150 rows.
 *
 * It takes the lcd_spi_mutex to serialise with LVGL flush, preventing:
 *  - DMA busy errors (both trying to use SPI2 at once)
 *  - Screen tearing / corruption
 *  - SPI peripheral lock-up
 *
 * Usage (in camera/display task):
 *   #include "camera_band_refresh.h"
 *   ...
 *   if (RGB_FrameNum > 0) {
 *       RGB_FrameNum = 0;
 *       SCB_InvalidateDCache_by_Addr((uint32_t*)RGB_DATA, sizeof(RGB_DATA));
 *       camera_band_refresh();   // ← replaces Start_LCD_DMA_Refresh()
 *   }
 */

#ifndef CAMERA_BAND_REFRESH_H
#define CAMERA_BAND_REFRESH_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief  DMA the camera band [UI_CAMERA_Y0 .. UI_CAMERA_Y1) to the LCD.
 *
 * Internally:
 *  1. xSemaphoreTake(lcd_spi_mutex, 100 ms)
 *  2. memcpy rows from RGB_DATA[UI_CAMERA_Y0..UI_CAMERA_Y1-1]
 *  3. Byte-swap for LCD endianness
 *  4. Clean D-Cache
 *  5. LCD_Set_Window(0, UI_CAMERA_Y0, 320, UI_CAMERA_H)  ← only 150 rows!
 *  6. SPI DMA chunks (same HAL_SPI_Transmit_DMA logic)
 *  7. xSemaphoreGive(lcd_spi_mutex)
 *
 * @note  If the mutex cannot be taken within 100 ms, the frame is skipped
 *        (returns immediately without blocking the camera pipeline).
 */
void camera_band_refresh(void);

#ifdef __cplusplus
}
#endif

#endif /* CAMERA_BAND_REFRESH_H */
