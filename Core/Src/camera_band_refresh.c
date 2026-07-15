/**
 * @file camera_band_refresh.c
 * Camera-band SPI DMA refresh — replaces full-screen Start_LCD_DMA_Refresh()
 */

#include "camera_band_refresh.h"
#include "lv_port_disp.h"
#include "main.h"
#include "spi.h"
#include "lcd.h"
#include "OV_Frame.h"
#include <string.h>

extern uint16_t RGB_DATA_Contiguous[];
extern volatile uint8_t lcd_dma_done_flag;

#define MAX_DMA_CHUNK  65535U

void camera_band_refresh(void)
{
    uint16_t cam_w = (uint16_t)lcddev.width;
    uint16_t cam_h = (uint16_t)(UI_CAMERA_Y1 - UI_CAMERA_Y0);
    uint32_t row_bytes = (uint32_t)cam_w * 2UL;
    uint32_t total_bytes = (uint32_t)cam_h * row_bytes;
    uint16_t y;

    if (HAL_SPI_GetState(&hspi2) != HAL_SPI_STATE_READY) return;
    if (lcd_spi_mutex && xSemaphoreTake(lcd_spi_mutex, pdMS_TO_TICKS(5)) != pdTRUE) return;

    for (y = 0; y < cam_h; y++) {
        memcpy(&RGB_DATA_Contiguous[y * cam_w],
               &RGB_DATA[UI_CAMERA_Y0 + y][0],
               row_bytes);
    }

    {
        uint32_t words = (total_bytes) / 4U;
        uint32_t *p32  = (uint32_t *)RGB_DATA_Contiguous;
        uint32_t i;
        for (i = 0; i < words; i++) {
            uint32_t v = p32[i];
            p32[i] = ((v & 0x00FF00FFU) << 8) | ((v & 0xFF00FF00U) >> 8);
        }
    }

    SCB_CleanDCache_by_Addr((uint32_t *)RGB_DATA_Contiguous, total_bytes);

    LCD_Set_Window(0, (uint16_t)UI_CAMERA_Y0, cam_w, cam_h);
    LCD_SetCursor(0, (uint16_t)UI_CAMERA_Y0);
    LCD_WR_REG(lcddev.wramcmd);
    LCD_RS_SET;
    LCD_CS_CLR;

    {
        uint32_t sent = 0;
        while (sent < total_bytes) {
            uint16_t chunk = (uint16_t)((total_bytes - sent) > MAX_DMA_CHUNK
                                        ? MAX_DMA_CHUNK : (total_bytes - sent));
            lcd_dma_done_flag = 0;
            __HAL_SPI_CLEAR_EOTFLAG(&hspi2);
            if (HAL_SPI_Transmit_DMA(&hspi2, (uint8_t *)RGB_DATA_Contiguous + sent, chunk) != HAL_OK) break;
            uint32_t timeout = 1000000UL;
            while (lcd_dma_done_flag == 0 && timeout--) {}
            if (timeout == 0) break;
            timeout = 1000000UL;
            while (__HAL_SPI_GET_FLAG(&hspi2, SPI_FLAG_EOT) == RESET && timeout--) {}
            if (timeout == 0) break;
            sent += chunk;
        }
    }

    LCD_CS_SET;
    if (lcd_spi_mutex) xSemaphoreGive(lcd_spi_mutex);
}
