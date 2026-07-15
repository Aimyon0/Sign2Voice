#ifndef __DCMI_H
#define __DCMI_H
#include "sys.h"
//////////////////////////////////////////////////////////////////////////////////	 

/************************************************************************************************/

//DCMI驱动代码	   

//STM32H7工程模板-HAL库函数版本
//DevEBox  大越创新
//淘宝店铺：mcudev.taobao.com
//淘宝店铺：shop389957290.taobao.com	


/************************************************************************************************/							  
////////////////////////////////////////////////////////////////////////////////// 	

extern void (*dcmi_rx_callback)(void);//DCMI DMA接收回调函数

extern DCMI_HandleTypeDef DCMI_Handler;        //DCMI句柄
extern DMA_HandleTypeDef  DMADMCI_Handler;     //DMA句柄

void DCMI_Init(void);
void DCMI_DMA_Init(u32 mem0addr,u32 mem1addr,u32 memsize,u32 memblen,u32 meminc);
void DCMI_Start(void);
void DCMI_Stop(void);
//void DCMI_Set_Window(u16 sx,u16 sy,u16 width,u16 height);
//void DCMI_CR_Set(u8 pclk,u8 hsync,u8 vsync);
#endif




































/************************************************************************************************/

//DCMI驱动代码	   

//STM32H7工程模板-HAL库函数版本
//DevEBox  大越创新
//淘宝店铺：mcudev.taobao.com
//淘宝店铺：shop389957290.taobao.com	


/************************************************************************************************/	
