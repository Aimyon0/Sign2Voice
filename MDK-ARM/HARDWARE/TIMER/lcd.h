#ifndef __LCD_H
#define __LCD_H		
#include "system.h"	 
#include "stdlib.h"
#include "delay.h"

#include "spi.h"


extern uint16_t D_Color; //������ɫ
extern uint16_t B_Color; //������ɫ
#include <stdint.h>

#ifndef _STDINT_H
/* exact-width signed integer types */
typedef   signed          char int8_t;
typedef   signed short     int int16_t;
typedef   signed           int int32_t;
typedef   signed       long long int64_t;


/* exact-width unsigned integer types */
typedef unsigned          char   uint8_t;
typedef unsigned short     int   uint16_t;
typedef unsigned           int   uint32_t;
typedef unsigned       long long uint64_t;
#endif

#ifndef u32
typedef uint32_t  u32;
#endif
#ifndef u16
typedef uint16_t u16;
#endif
#ifndef u8
typedef uint8_t  u8;
#endif



/******************************************************************************

  �ӿڶ��壬����ݽ����޸Ĳ��޸���ӦIO��ʼ��--�������Ӻ�IOҪ��Ӧ


//	#define LCD_SDI        	//PB15  //����������
//	#define LCD_SCL        	//PB13  //ʱ����
//	#define LCD_CS        	//PB12  //Ƭѡ	
//	#define LCD_SDO     		//PB14  //�������/��λ
//	#define LCD_RS         	//PB1   //����/�����л�
//	#define LCD_BLK         //PB0   //�������  

*******************************************************************************/ 



//Һ�����ƿ���1�������궨��
#define	LCD_SDA_SET  	HAL_GPIO_WritePin(GPIOB,GPIO_PIN_15,GPIO_PIN_SET) 	   //PB15��1    LCD_SDI�� PB15 //����������

#define	LCD_SCL_SET  	HAL_GPIO_WritePin(GPIOB,GPIO_PIN_13,GPIO_PIN_SET)	     //PB13��1    LCD_SCL�� PB13 //ʱ����

#define	LCD_CS_SET  	HAL_GPIO_WritePin(GPIOB,GPIO_PIN_12,GPIO_PIN_SET)	     //PB12��1    LCD_CS��  PB12 //Ƭѡ	

#define LCD_RST_Set   HAL_GPIO_WritePin(GPIOB,GPIO_PIN_14,GPIO_PIN_SET)	     //PB14��1      LCD_SDO ��PB14 //�������/��λ

#define	LCD_RS_SET  	HAL_GPIO_WritePin(GPIOB,GPIO_PIN_1,GPIO_PIN_SET)		   //PB1��1     LCD_RS�� PB1   //����/�����л�

#define	LCD_BLK_SET  	HAL_GPIO_WritePin(GPIOB,GPIO_PIN_0,GPIO_PIN_SET) 	     //PB0��1     LCD_BLK ��PB0   //�������  


//Һ�����ƿ���0�������궨��

#define	LCD_SDA_CLR  	HAL_GPIO_WritePin(GPIOB,GPIO_PIN_15,GPIO_PIN_RESET)  	  //PB15��0 //DIN  LCD_SDI�� PB15 //����������

#define	LCD_SCL_CLR  	HAL_GPIO_WritePin(GPIOB,GPIO_PIN_13,GPIO_PIN_RESET)	    //PB13��0 //CLK  LCD_SCL�� PB13 //ʱ����

#define	LCD_CS_CLR  	HAL_GPIO_WritePin(GPIOB,GPIO_PIN_12,GPIO_PIN_RESET)	   	//PB12��0/CS     LCD_CS��  PB12 //Ƭѡ	

#define LCD_RST_Clr   HAL_GPIO_WritePin(GPIOB,GPIO_PIN_14,GPIO_PIN_RESET)	    //PB14��0 //RES  LCD_SDO ��PB14 //�������/��λ

#define	LCD_RS_CLR  	HAL_GPIO_WritePin(GPIOB,GPIO_PIN_1,GPIO_PIN_RESET)		 	//PB1��0 //DC    LCD_RS�� PB1   //����/�����л�

#define	LCD_BLK_CLR  	HAL_GPIO_WritePin(GPIOB,GPIO_PIN_0,GPIO_PIN_RESET)  	  //PB0��0 //DIN   LCD_BLK ��PB0   //�������  



#define	LCD_BLK_On          LCD_BLK_SET   		 //������  LCD�������  
#define	LCD_BLK_Off         LCD_BLK_CLR    		 //�ر���  LCD�������


//LCD��Ҫ������
typedef struct  
{										    
	u16 width;			//LCD ����
	u16 height;			//LCD �߶�
	u16 id;				  //LCD ID
	u8  dir;			  //���������������ƣ������ͺ�����	
	u16	wramcmd;		//��ʼдgramָ��
	u16  setxcmd;		//����x����ָ��
	u16  setycmd;		//����y����ָ�� 
}_lcd_dev; 	  



/////////////////////////////////////�û�������///////////////////////////////////	

//֧�ֺ��������ٶ����л�


#define LCD_DIR_Mode  	  0	    //4�ֹ���ģʽ��0��1������ģʽ��2��3�Ǻ���ģʽ

#define USE_HORIZONTAL  	0     //�������ã� 		0,����ģʽ   1,����ģʽ.



//////////////////////////////////////////////////////////////////////////////////	

//LCD����
extern _lcd_dev lcddev;	//����LCD��Ҫ����

//LCD�Ļ�����ɫ�ͱ���ɫ	   

extern u16  POINT_COLOR;//Ĭ�Ϻ�ɫ    
extern u16  BACK_COLOR; //������ɫ.Ĭ��Ϊ��ɫ


//////////////////////////////////////////////////////////////////////////////////	



//////////////////////////////////////////////////////////////////////////////////
	 
//ɨ�跽����--ɨ�跽ʽ�в�ͬ��񣬿��ܶ��岻���Һ����µĲ��շ���ͬ���᷽ܽʽ��ֻ��һ�°���

#define L2R_U2D  0 //������,���ϵ���
#define L2R_D2U  1 //������,���µ���
#define R2L_U2D  2 //���ҵ���,���ϵ���
#define R2L_D2U  3 //���ҵ���,���µ���

#define U2D_L2R  4 //���ϵ���,������
#define U2D_R2L  5 //���ϵ���,���ҵ���
#define D2U_L2R  6 //���µ���,������
#define D2U_R2L  7 //���µ���,���ҵ���	 


///////////////////////////  ��ɫֵ  ///////////////////////////////////////////////////////

//������ɫ
#define WHITE         	 0xFFFF
#define BLACK         	 0x0000	  
#define BLUE         	   0x001F  
#define BRED             0xF81F
#define GRED 			       0xFFE0
#define GBLUE			       0x07FF
#define RED           	 0xF800
#define MAGENTA       	 0xF81F
#define GREEN         	 0x07E0
#define CYAN          	 0x7FFF
#define YELLOW        	 0xFFE0
#define BROWN 			     0xBC40 //��ɫ
#define BRRED 			     0xFC07 //�غ�ɫ
#define GRAY  			     0x8430 //��ɫ


//GUI��ɫ

#define DARKBLUE      	 0x01CF	//����ɫ
#define LIGHTBLUE      	 0x7D7C	//ǳ��ɫ  
#define GRAYBLUE       	 0x5458 //����ɫ
//������ɫΪPANEL����ɫ 
 
 
#define LIGHTGREEN     	 0x841F //ǳ��ɫ
//#define LIGHTGRAY        0XEF5B //ǳ��ɫ(PANNEL)
#define LGRAY 			     0xC618 //ǳ��ɫ(PANNEL),���屳��ɫ

#define GRAY0   0xEF7D   	    //��ɫ0 
#define GRAY1   0x8410      	//��ɫ1   
#define GRAY2   0x4208      	//��ɫ2  

#define LGRAYBLUE        0xA651 //ǳ����ɫ(�м����ɫ)
#define LBBLUE           0x2B12 //ǳ����ɫ(ѡ����Ŀ�ķ�ɫ)



////////////////////////��ʱ����--�궨��/////////////////////////////////////////////////



#define	LCD_Delay_us  	delay_us   //��ʱ����
#define	LCD_Delay_ms  	delay_ms   //��ʱ����




void LCD_GPIO_Init(void);            //Һ��IO��ʼ������
void LCD_WR_REG(vu16 regval);        //Һ����--д�Ĵ�������
void LCD_WR_DATA8(u8 data);          //д8λ����
void LCD_WR_DATA16(vu16 data);       //д16λ����
void LCD_Init(void);													   	              //��ʼ��
void LCD_HardwareRest(void);                                    //Ӳ��λ--���IO���ӣ�Ӳ����λ�ɿ���Ч
void LCD_SoftRest(void);                                        //����λ
void LCD_DisplayOn(void);													              //����ʾ
void LCD_DisplayOff(void);													            //����ʾ
void LCD_Clear(u16 Color);	 												            //����
void LCD_SetCursor(u16 Xpos, u16 Ypos);										      //���ù��
void LCD_DrawPoint(u16 x,u16 y);											          //����--ʹ�����õıʼ���ɫ
void LCD_Fast_DrawPoint(u16 x,u16 y,u16 color);								  //���ٻ���--ʹ�õ�ǰ������ɫ����

void LCD_Draw_Circle(u16 x0,u16 y0,u8 r, u16 Color);							 //��Բ
void LCD_DrawLine(u16 x1, u16 y1, u16 x2, u16 y2, u16 Color);		   //����
void LCD_DrawRectangle(u16 x1, u16 y1, u16 x2, u16 y2, u16 Color); //������

void LCD_Fill(u16 sx,u16 sy,u16 ex,u16 ey,u16 color);		   			             //��䵥ɫ
void LCD_Color_Fill(u16 sx,u16 sy,u16 ex,u16 ey,u16 *color);		             //���ָ����ɫ
void LCD_ShowChar(u16 x,u16 y,u8 num,u8 size,u16 color,u8 mode);					   //��ʾһ���ַ�
void LCD_ShowNum(u16 x,u16 y,u32 num,u8 len,u8 size,u16 color);  					    //��ʾһ������
void LCD_ShowxNum(u16 x,u16 y,u32 num,u8 len,u8 size,u16 color,u8 mode);				  //��ʾ ����
void LCD_ShowString(u16 x,u16 y,u16 width,u16 height,u8 size,u16 color,u8 *p);		//��ʾһ���ַ���,12/16����

void LCD_WriteReg(u16 LCD_Reg, u16 LCD_RegValue);               //д�Ĵ���

void LCD_WriteRAM_Prepare(void);                                //��ʼдGRAM  ����
void LCD_WriteRAM(u16 RGB_Code);		                            //LCDдGRAM
void LCD_Scan_Dir(u8 dir);							                        //������ɨ�跽��
void LCD_Display_Dir(u8 dir);						                        //������Ļ��ʾ����
void LCD_Set_Window(u16 sx,u16 sy,u16 width,u16 height);         //���ô���					

void Draw_Test(void);//��ͼ���ߺ�������
void Color_Test(void);//��ɫ�����ʾ����
void Font_Test(void);// ����������ʾ����
void GBK_LibFont_Test(void);//GBK ����������ʾ����
void Demo_Menu(void);//��ʾ����˵�



#endif  



