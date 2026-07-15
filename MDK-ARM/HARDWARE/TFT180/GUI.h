#ifndef _GUI_H
#define _GUI_H


#include "sys.h"

/**************************************************************************************/

//STM32H7工程模板-HAL库函数版本
//DevEBox  大越创新
//嵌入式开发网
//淘宝店铺：mcudev.taobao.com
//淘宝店铺：shop389957290.taobao.com	

/**************************************************************************************/


void Gui_Circle(u16 X,u16 Y,u16 R,u16 fc); 
void Gui_DrawLine(u16 x0, u16 y0,u16 x1, u16 y1,u16 Color);  
void Gui_box(u16 x, u16 y, u16 w, u16 h,u16 bc);
void Gui_box2(u16 x,u16 y,u16 w,u16 h, u8 mode);
void DisplayButtonDown(u16 x1,u16 y1,u16 x2,u16 y2);
void DisplayButtonUp(u16 x1,u16 y1,u16 x2,u16 y2);
void Gui_DrawFont_GBK16(u16 x, u16 y, u16 fc, u16 bc, u8 *s);
void Gui_DrawFont_GBK24(u16 x, u16 y, u16 fc, u16 bc, u8 *s);
void Gui_DrawFont_Num32(u16 x, u16 y, u16 fc, u16 bc, u16 num) ;

void  Mainmenu(void);
void Num_Test(void);
void Font_Test(void);
void Color_Test(void);
void show_pic(void);
void show_Circle(void);

void Test_Demo(void);	//液晶屏测试例程


#endif






























/**************************************************************************************/

//STM32H7工程模板-HAL库函数版本
//DevEBox  大越创新
//嵌入式开发网
//淘宝店铺：mcudev.taobao.com
//淘宝店铺：shop389957290.taobao.com	

/**************************************************************************************/





