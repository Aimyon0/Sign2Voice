/**
 * @file    camera_service.c
 * @brief   Camera service — wraps OV2640, DCMI, DMA and colour conversion.
 */

#include "camera_service.h"
#include "OV_Frame.h"
#include "ov2640.h"
#include "dcmi.h"
#include "LCD.h"
#include "main.h"

/* Cache maintenance for ARMv7-M */
#define SCB_InvalidateDCache_by_Addr SCB_InvalidateDCache_by_Addr

/* ---- Public API ---- */

int camera_service_init(void)
{
    /* Called after LCD_Init() from main.c.
     * Actual DCMI+DMA start is deferred to camera_service_start(). */
    return 0;
}

int camera_service_start(void)
{
    OV2640_RGB565_Mode();
    DCMI_Init();
    dcmi_rx_callback = rgblcd_dcmi_rx_callback;
    DCMI_DMA_Init((u32)RGB_Line_Buf[0], (u32)RGB_Line_Buf[1],
                  lcddev.width / 2, DMA_MDATAALIGN_HALFWORD, DMA_MINC_ENABLE);
    OV2640_OutSize_Set(lcddev.width, lcddev.height);
    DCMI_Start();
    return 0;
}

int camera_service_frame_ready(void)
{
    return (RGB_FrameNum > 0);
}

int camera_service_get_rgb888(float *buf)
{
    if (!buf) return -1;

    RGB565_To_64x64_RGB888_Float(
        &RGB_DATA[0][0],
        lcddev.width,
        lcddev.height,
        buf
    );
    return 0;
}

void camera_service_invalidate_cache(void)
{
    SCB_InvalidateDCache_by_Addr((uint32_t *)RGB_DATA, sizeof(RGB_DATA));
}

void camera_service_ack_frame(void)
{
    RGB_FrameNum = 0;
}
