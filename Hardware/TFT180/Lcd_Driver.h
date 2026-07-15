#ifndef _Lcd_Driver_H
#define _Lcd_Driver_H



#include "sys.h"


/**************************************************************************************/

//STM32H7工程模板-HAL库函数版本
//DevEBox  大越创新
//嵌入式开发网
//淘宝店铺：mcudev.taobao.com
//淘宝店铺：shop389957290.taobao.com	

/**************************************************************************************/

//定义LCD的尺寸

#if USE_HORIZONTAL==1	//使用横屏
#define X_MAX_PIXEL	        160
#define Y_MAX_PIXEL	        128
#else
#define X_MAX_PIXEL	        128
#define Y_MAX_PIXEL	        160
#endif



//LCD重要参数集
typedef struct  
{										    
	u16 width;			//LCD 宽度
	u16 height;			//LCD 高度
	u16 id;				//LCD ID
	u8  dir;			//横屏还是竖屏控制：0，竖屏；1，横屏。	
	u16	 wramcmd;		//开始写gram指令
	u16  setxcmd;		//设置x坐标指令
	u16  setycmd;		//设置y坐标指令	 
}_lcd_dev; 	

//LCD参数
extern _lcd_dev lcddev;	//管理LCD重要参数


#define RED  	  0xf800
#define GREEN	  0x07e0
#define BLUE 	  0x001f
#define WHITE	  0xffff
#define BLACK	  0x0000
#define YELLOW  0xFFE0
#define GRAY0   0xEF7D   	//灰色0 3165 00110 001011 00101
#define GRAY1   0x8410      	//灰色1      00000 000000 00000
#define GRAY2   0x4208      	//灰色2  1111111111011111

/////////////////////////////////////用户配置区///////////////////////////////////	 
//支持横竖屏快速定义切换，支持8/16位模式切换
#define USE_HORIZONTAL  	0	    //定义是否使用横屏 		0,不使用.  1,使用.
//使用模拟SPI作为测试



//液晶控制口置1操作语句宏定义
#define	LCD_SDA_SET  	HAL_GPIO_WritePin(GPIOB,GPIO_PIN_15,GPIO_PIN_SET) 	   //PB15置1 

#define	LCD_SCL_SET  	HAL_GPIO_WritePin(GPIOB,GPIO_PIN_13,GPIO_PIN_SET)	     //PB13置1 

#define	LCD_CS_SET  	HAL_GPIO_WritePin(GPIOB,GPIO_PIN_12,GPIO_PIN_SET)	     //PB12置1  

#define LCD_RST_Set  HAL_GPIO_WritePin(GPIOB,GPIO_PIN_14,GPIO_PIN_SET)	   //PB14置1 

#define	LCD_RS_SET  	HAL_GPIO_WritePin(GPIOB,GPIO_PIN_1,GPIO_PIN_SET)		   //PB1置1  

#define	LCD_BLK_SET  	HAL_GPIO_WritePin(GPIOB,GPIO_PIN_0,GPIO_PIN_SET) 	     //PB0置1 


//液晶控制口置0操作语句宏定义

#define	LCD_SDA_CLR  	HAL_GPIO_WritePin(GPIOB,GPIO_PIN_15,GPIO_PIN_RESET)  	  //PB15置0 //DIN

#define	LCD_SCL_CLR  	HAL_GPIO_WritePin(GPIOB,GPIO_PIN_13,GPIO_PIN_RESET)	    //PB13置0 //CLK

#define	LCD_CS_CLR  	HAL_GPIO_WritePin(GPIOB,GPIO_PIN_12,GPIO_PIN_RESET)	   	//PB12置0/CS

#define LCD_RST_Clr  HAL_GPIO_WritePin(GPIOB,GPIO_PIN_14,GPIO_PIN_RESET)	  //PB14置0 //RES

#define	LCD_RS_CLR  	HAL_GPIO_WritePin(GPIOB,GPIO_PIN_1,GPIO_PIN_RESET)		 	//PB1置0 //DC

#define	LCD_BLK_CLR  	HAL_GPIO_WritePin(GPIOB,GPIO_PIN_0,GPIO_PIN_RESET)  	  //PB0置0 //DIN



void LCD_GPIO_Init(void);//初始化IO口
void  SPI_WriteData(u8 Data);//STM32_模拟SPI写一个字节数据底层函数
void LCD_WriteData_16Bit(uint16_t Data);//向液晶屏写一个16位数据

void Lcd_WriteIndex(u8 Index);//写控制器寄存器地址
void Lcd_WriteData(u8 Data);//  写寄存器数据
void Lcd_WriteReg(u8 Index,u8 Data);////写寄存器函数

u16 Lcd_ReadReg(u8 LCD_Reg);

void Lcd_Reset(void);
void Lcd_Init(void);
void Lcd_Clear(u16 Color);
void Lcd_SetXY(u16 x,u16 y);
void Gui_DrawPoint(u16 x,u16 y,u16 Data);
//unsigned int Lcd_ReadPoint(u16 x,u16 y);














#endif










/**************************************************************************************/

//STM32H7工程模板-HAL库函数版本
//DevEBox  大越创新
//嵌入式开发网
//淘宝店铺：mcudev.taobao.com
//淘宝店铺：shop389957290.taobao.com	

/**************************************************************************************/



