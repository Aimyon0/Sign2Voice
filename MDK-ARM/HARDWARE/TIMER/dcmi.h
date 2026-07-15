#ifndef __DCMI_H
#define __DCMI_H
#include "system.h"
//////////////////////////////////////////////////////////////////////////////////	 

/************************************************************************************************/

//DCMI��������	   

//STM32H7����ģ��-HAL�⺯���汾
//DevEBox  ��Խ����
//�Ա����̣�mcudev.taobao.com
//�Ա����̣�shop389957290.taobao.com	


/************************************************************************************************/							  
////////////////////////////////////////////////////////////////////////////////// 	

extern void (*dcmi_rx_callback)(void);//DCMI DMA���ջص�����

extern DCMI_HandleTypeDef DCMI_Handler;
extern void jpeg_data_process(void);        //DCMI���
extern DMA_HandleTypeDef  DMADMCI_Handler;     //DMA���

void DCMI_Init(void);
void DCMI_DMA_Init(u32 mem0addr,u32 mem1addr,u32 memsize,u32 memblen,u32 meminc);
void DCMI_Start(void);
void DCMI_Stop(void);
//void DCMI_Set_Window(u16 sx,u16 sy,u16 width,u16 height);
//void DCMI_CR_Set(u8 pclk,u8 hsync,u8 vsync);
#endif




































/************************************************************************************************/

//DCMI��������	   

//STM32H7����ģ��-HAL�⺯���汾
//DevEBox  ��Խ����
//�Ա����̣�mcudev.taobao.com
//�Ա����̣�shop389957290.taobao.com	


/************************************************************************************************/	
