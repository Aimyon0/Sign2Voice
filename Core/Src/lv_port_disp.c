#include "lv_port_disp.h"
#include "main.h"
#include "spi.h"
#include "lcd.h"
#include <string.h>

SemaphoreHandle_t lcd_spi_mutex = NULL;

__attribute__((section(".RAM_D2"))) __attribute__((aligned(32)))
static lv_color_t disp_buf1[LV_PORT_DISP_BUF_SIZE];

#define LCD_TX_BUF_SIZE (240 * 100)
__attribute__((section(".RAM_D2"))) __attribute__((aligned(32)))
static uint16_t lcd_tx_buf[LCD_TX_BUF_SIZE];

static void lv_port_disp_send_pixels(const lv_color_t *px_map, uint32_t count)
{
    if (count > LCD_TX_BUF_SIZE) return;
    uint32_t i;
    for (i = 0; i < count; i++) {
        uint16_t c = ((const uint16_t *)px_map)[i];
        lcd_tx_buf[i] = (c << 8) | (c >> 8);
    }
    uint32_t bytes = count * sizeof(uint16_t);
    SCB_CleanDCache_by_Addr((uint32_t *)lcd_tx_buf, (bytes + 31U) & ~0x1FU);
    HAL_SPI_Transmit(&hspi2, (uint8_t *)lcd_tx_buf, (uint16_t)bytes, HAL_MAX_DELAY);
}

static void lv_port_disp_flush(lv_display_t *disp, const lv_area_t *area,
                               uint8_t *px_map)
{
    if (lcd_spi_mutex == NULL)
        lcd_spi_mutex = xSemaphoreCreateMutex();

    uint16_t w = (uint16_t)(area->x2 - area->x1 + 1);
    uint16_t h = (uint16_t)(area->y2 - area->y1 + 1);

    if (lcd_spi_mutex && xSemaphoreTake(lcd_spi_mutex, pdMS_TO_TICKS(5)) == pdTRUE) {
        LCD_Set_Window((uint16_t)area->x1, (uint16_t)area->y1, w, h);
        LCD_WriteRAM_Prepare();
        LCD_CS_CLR;
        LCD_RS_SET;
        lv_port_disp_send_pixels((const lv_color_t *)px_map, (uint32_t)w * h);
        LCD_CS_SET;
        xSemaphoreGive(lcd_spi_mutex);
    }

    lv_display_flush_ready(disp);
}

lv_display_t *lv_port_disp_init(void)
{
    lv_display_t *disp = lv_display_create(LV_PORT_DISP_HOR_RES, LV_PORT_DISP_VER_RES);
    if (!disp) return NULL;
    lv_display_set_flush_cb(disp, lv_port_disp_flush);
    lv_display_set_buffers(disp, disp_buf1, NULL,
                           LV_PORT_DISP_BUF_SIZE * sizeof(lv_color_t),
                           LV_DISPLAY_RENDER_MODE_PARTIAL);
    return disp;
}
