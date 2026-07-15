#ifndef _OV_Frame_H
#define _OV_Frame_H

#include "system.h"
#include "sccb.h"

extern volatile uint16_t RGB_FrameNum;
void jpeg_data_process(void);
void  RGB_Refresh_LCD(void);
void JPEG_mode(void);
void rgblcd_dcmi_rx_callback(void);
void RGB565_mode(void);
uint8_t RGB565_to_Gray(uint16_t rgb565_pixel);
void RGB565_To_Gray_Resize(const uint16_t* rgb565_buf, uint16_t src_w, uint16_t src_h, uint8_t* gray28x28);
void lcdtest(void);
void RGB565_To_64x64_RGB888_Float(const uint16_t* rgb565_buf, uint16_t src_w, uint16_t src_h, float* out_tensor);
void RGB565_To_64x64_Gray3CH_Float(const uint16_t* rgb565_buf, uint16_t src_w, uint16_t src_h, float* out_tensor);
#define RGB_Width   240
#define RGB_Height  320
extern uint16_t RGB_DATA[RGB_Height][RGB_Width];
void Start_LCD_DMA_Refresh(void);
extern u32 RGB_Line_Buf[2][RGB_Width*2];
#endif
