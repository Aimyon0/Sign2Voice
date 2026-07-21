#include "camera_service.h"
#include "OV_Frame.h"
#include "ov2640.h"
#include "dcmi.h"
#include "LCD.h"
#include "main.h"
#include "log.h"
static int g_started = 0;
ErrorCode camera_service_init(void) { return ERR_OK; }
ErrorCode camera_service_start(void) {
    OV2640_RGB565_Mode(); DCMI_Init();
    dcmi_rx_callback=rgblcd_dcmi_rx_callback;
    DCMI_DMA_Init((u32)RGB_Line_Buf[0],(u32)RGB_Line_Buf[1],lcddev.width/2,DMA_MDATAALIGN_HALFWORD,DMA_MINC_ENABLE);
    OV2640_OutSize_Set(lcddev.width,lcddev.height); DCMI_Start(); g_started=1;
    return ERR_OK;
}
ErrorCode camera_service_stop(void) { if(g_started){DCMI->CR&=~DCMI_CR_ENABLE;g_started=0;} return ERR_OK; }
ErrorCode camera_service_deinit(void) { g_started=0; return ERR_OK; }
int camera_service_frame_ready(void) { return (RGB_FrameNum>0); }
ErrorCode camera_service_get_rgb888(float *buf) {
    if(!buf) return ERR_NULL_POINTER;
    RGB565_To_64x64_RGB888_Float(&RGB_DATA[0][0],lcddev.width,lcddev.height,buf);
    return ERR_OK;
}
void camera_service_invalidate_cache(void) { SCB_InvalidateDCache_by_Addr((uint32_t*)RGB_DATA,sizeof(RGB_DATA)); }
void camera_service_ack_frame(void) { RGB_FrameNum=0; }
