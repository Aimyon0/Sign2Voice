#include "stm32h7xx_hal.h"

#include "OV_Frame.h"
#include "key.h"
#include "LCD.h"
#include "Dis_Picture.h" 
#include "Text.h"	
#include "GBK_LibDrive.h"	

#include "mpu.h"

#include "timer.h"
#include "ov2640.h"
#include "dcmi.h"

#include "usart.h"
#include "usart2.h"


extern	uint8_t Print_buf[32];	//��Ϣ������

extern   uint8_t Key_Flag; //��ֵ

//////////////////////////////////////////////////////////////////////////////////	 

/************************************************************************************************/
//OV2640--֡������ʾ���� ��������

//STM32H7����ģ��-HAL�⺯���汾
//DevEBox  ��Խ����
//�Ա����̣�mcudev.taobao.com
//�Ա����̣�shop389957290.taobao.com	


/************************************************************************************************/							  
////////////////////////////////////////////////////////////////////////////////// 



/************************************************************************************************/

//OV2640 ����ͷ ������ʾ����  ʹ��2.0��240x320������ʾ��


//STM32H7����ģ��-HAL�⺯���汾
//DevEBox  ��Խ����
//�Ա����̣�mcudev.taobao.com
//�Ա����̣�shop389957290.taobao.com	


/************************************************************************************************/


u8 OV_mode=0;							//bit0:0,RGB565ģʽ;1,JPEGģʽ

u16 yoffset=0;							//y�����ƫ����




// RGB_Width / RGB_Height / jpeg_buf_size / union OVxxx  �Ѷ����� OV_Frame.h
// ���ﲻ���ظ�����
union OVxxx OVxxx;



u32 RGB_Line_Buf[2][RGB_Width*2];//RGB��ʱ,����ͷ����һ��һ�ж�ȡ,�����л���


u32 jpeg_data_len=0; 			       //buf�е�JPEG��Ч���ݳ���

u8 jpeg_data_ok=0;				       //JPEG���ݲɼ���ɱ�־





/************************************************************************************************/

//0,����û�вɼ���;
//1,���ݲɼ�����,���ǻ�û����;
//2,�����Ѿ����������,���Կ�ʼ��һ֡����

//JPEG�ߴ�֧���б�

//STM32H7����ģ��-HAL�⺯���汾
//DevEBox  ��Խ����
//�Ա����̣�mcudev.taobao.com
//�Ա����̣�shop389957290.taobao.com	


/************************************************************************************************/


const u16 jpeg_img_size_tbl[][2]=
{
    80,60,  
		160,120,	//QQVGA
	  240,240,	//QVGA
    320,240,	//QVGA
    640,480,	//VGA
    800,600,	//SVGA
    1024,768,	//XGA
    1280,800,	//WXGA
    1440,900,	//WXGA+
    1280,1024,	//SXGA
    1600,1200,	//UXGA
    1920,1080,	//1080P
    2048,1536,	//QXGA
    2592,1944,	//500W
};





const u8*EFFECTS_TBL[7]= {"Normal","Cool","Warm","B&W","Yellowish ","Inverse","Greenish"};	//7����Ч
const u8*JPEG_SIZE_TBL[12]= {"QQVGA","QVGA","VGA","SVGA","XGA","WXGA","WXGA+","SXGA","UXGA","1080P","QXGA","500W"}; //JPEGͼƬ 12�ֳߴ�


//����һ���ṹ��

union TData
{
	
   uint32_t B32_temp;
   uint8_t  B8_Temp[4];
	
} TData; 



/************************************************************************************************/
//����ʾ���������ݽ���ˢ������

//STM32H7����ģ��-HAL�⺯���汾
//DevEBox  ��Խ����
//�Ա����̣�mcudev.taobao.com
//�Ա����̣�shop389957290.taobao.com	


/************************************************************************************************/



  u16 Num_Dot;
  u16 Num_H;//����
	u16 *R_Buf; //ˢ������ָ��
	
static void RGB_Refresh_LCD(void)
{
    uint16_t *R_Buf;
    uint16_t Num_H, Num_Dot;

    LCD_Set_Window(0, 0, lcddev.width, lcddev.height);  // ����ȫ������
    LCD_SetCursor(0, 0);                                // ���ù��
    LCD_WR_REG(lcddev.wramcmd);                         // д��RAM����

    for (Num_H = 0; Num_H < lcddev.height; Num_H++)
    {
        R_Buf = &OVxxx.RGB_DATA[Num_H][0];              // ��ȡ��ǰ�е����ص�ַ

        for (Num_Dot = 0; Num_Dot < lcddev.width; Num_Dot++)
        {
            LCD_WR_DATA16(*R_Buf++);                    // д��һ������
        }
    }
}


/************************************************************************************************/
//����JPEG����
//���ɼ���һ֡JPEG���ݺ�,���ô˺���,�л�JPEG BUF.��ʼ��һ֡�ɼ�.

//STM32H7����ģ��-HAL�⺯���汾
//DevEBox  ��Խ����
//�Ա����̣�mcudev.taobao.com
//�Ա����̣�shop389957290.taobao.com	


/************************************************************************************************/

volatile uint16_t  RGB_FrameNum=0;

volatile uint16_t  curline=0;							//����ͷ�������,��ǰ�б��

void jpeg_data_process(void)
{
    u16 i;
    u16 rlen;			//ʣ�����ݳ���
    u32 *pbuf;
	
		curline=0;	//������λ

    if(OV_mode&0X01)	//ֻ����JPEG��ʽ��,����Ҫ������.
    {
        if(jpeg_data_ok==0)	//jpeg���ݻ�δ�ɼ���?
        {
            DMA1_Stream1->CR&=~(1<<0);		//ֹͣ��ǰ����
					
            while(DMA1_Stream1->CR&0X01);	//�ȴ�DMA1_Stream1������
					
            rlen=jpeg_buf_size-DMA1_Stream1->NDTR;//�õ�ʣ�����ݳ���
				
					  jpeg_data_len+=rlen;			            //����ʣ�೤��
            
					  jpeg_data_ok=1; 				             //���JPEG���ݲɼ����,�ȴ�������������
					
        }
        if(jpeg_data_ok==2)	//��һ�ε�jpeg�����Ѿ���������
        {
										
            DMA1_Stream1->NDTR=jpeg_buf_size;//���䳤��Ϊjpeg_buf_size*4�ֽ�
            DMA1_Stream1->CR|=1<<0;			     //���´���
            jpeg_data_ok=0;					         //�������δ�ɼ�
            jpeg_data_len=0;				         //�������¿�ʼ
					
        }
				
    } 
		else
    {
			 
			RGB_FrameNum++;//֡������
      	
    }
}






/************************************************************************************************/
//JPEG����
//JPEG����,ͨ������2���͸�����.

//STM32H7����ģ��-HAL�⺯���汾
//DevEBox  ��Խ����
//�Ա����̣�mcudev.taobao.com
//�Ա����̣�shop389957290.taobao.com	


/************************************************************************************************/

void JPEG_mode(void)
{
    u32 i,jpgstart,jpglen;
    u8  *p;
    
	  u8  headok=0;
	  u8  effect=0;
	  u8  contrast=2;
    u8 size=1;			//Ĭ����QVGA 320*240�ߴ�

	   uart_init(1500000);				       //����1 ��ʼ��
	  //usart2_init(921600);			     //��ʼ������2������Ϊ  921600
	  //
    LCD_Clear(WHITE);  //����
    	
    Draw_Font16B(30,50,BLUE,"DevEBox STM32H750 ");
    Draw_Font16B(30,70,BLUE,"OV2640 JPEG Mode");
	
    Draw_Font16B(30,90,BLUE,"KEY1:Contrast");			//�Աȶ�
    Draw_Font16B(30,110,BLUE,"KEY2:JPEG Size"); 		//����JPEG�ĳߴ���

    sprintf((char*)Print_buf,"JPEG Size:%s",JPEG_SIZE_TBL[size]);
	  
    Draw_Font16B(30,130,BLUE,Print_buf);					       //��ʾ��ǰJPEG�ֱ���
	
	  delay_ms(800);//��ʱ��ʾ
	
	
    OV2640_JPEG_Mode();		//JPEGģʽ
		

    DCMI_Init();			    //DCMI����
 		
    DCMI_DMA_Init((u32)& OVxxx.JPEG_DATA, 0,jpeg_buf_size, DMA_MDATAALIGN_WORD, DMA_MINC_ENABLE);//DCMI DMA����--��ʹ��˫������
		
    OV2640_OutSize_Set(jpeg_img_size_tbl[size][0], jpeg_img_size_tbl[size][1]);  //��������ߴ�
		
    DCMI_Start(); 		//��������
		
		
    while(1)
    {
        if(jpeg_data_ok==1)	//�Ѿ��ɼ���һ֡ͼ����
        {
            p=(u8*)OVxxx.JPEG_DATA;
					
//            printf("Jpeg_data:%d\r\n",jpeg_data_len*4);//��ӡ֡��
					
            Draw_Font16B(30,150,BLUE,"Sending JPEG ..."); //��ʾ���ڴ�������
					
            jpglen=0;	//����jpg�ļ���СΪ0
            headok=0;	//���jpgͷ���
					
            for(i=0; i<jpeg_data_len*4; i++) //����0xFF,0xD8��0xFF,0xD9,��ȡjpg�ļ���С
            {
                if((p[i]==0XFF)&&(p[i+1]==0XD8))//�ҵ�FF D8
                {
                    jpgstart=i;
                    headok=1;	//����ҵ�jpgͷ(FF D8)
                }
                if((p[i]==0XFF)&&(p[i+1]==0XD9)&&headok)//�ҵ�ͷ�Ժ�,����FF D9
                {
                    jpglen=i-jpgstart+2;
                    break;
                }
            }
            if(jpglen)	//������jpeg����
            {
                p+=jpgstart;			//ƫ�Ƶ�0xFF,0xD8��
                for(i=0; i<jpglen; i++)	//��������jpg�ļ�
                {
                    USART1->TDR=p[i];
                    while((USART1->ISR&0X40)==0);	//ѭ������,ֱ���������

                }
            }
						
						Key_Flag=KEY_Scan(0);   //��ȡ��ֵ
						
            if(Key_Flag)	//�а�������,��Ҫ����
            {

                Draw_Font16B(30,150,BLUE,"Quit Sending  ");//��ʾ�˳����ݴ���
							
                switch(Key_Flag)
                {
                case KEY1_PRES:	//�Աȶ�����
                    contrast++;
                    if(contrast>6)contrast=0;
                    OV2640_Contrast(contrast);
                    sprintf((char*)Print_buf,"Contrast:%d",(signed char)contrast-3);
                    break;
                case KEY2_PRES:	//��������ߴ�
                    size++;
                    if(size>11)size=0;
                    OV2640_OutSize_Set(jpeg_img_size_tbl[size][0],jpeg_img_size_tbl[size][1]);//��������ߴ�
                    sprintf((char*)Print_buf,"JPEG Size:%s",JPEG_SIZE_TBL[size]);
                    break;
								
								

                }
								
                Draw_Font16B(30,180,BLUE,"               ");//��ʾ��ʾ����
								
                Draw_Font16B(30,180,BLUE,Print_buf);//��ʾ��ʾ����
								
                delay_ms(800);
								
            } 
						
						else Draw_Font16B(30,150,BLUE,"Send complete!!");//��ʾ�����������
						
            jpeg_data_ok=2;	//���jpeg���ݴ�������,������DMAȥ�ɼ���һ֡��.
        }
    }
}
								//�����������ޣ��������ܣ������Լ�ѡ������
								
////                case KEY0_PRES:	//�Աȶ�����
////                    contrast++;
////                    if(contrast > 4)contrast = 0;
////                    OV2640_Contrast(contrast);
////                    sprintf((char*)msgbuf, "Contrast:%d", (signed char)contrast - 2);
////                    break;
////                case KEY1_PRES:	//���Ͷ�Saturation
////                    saturation++;
////                    if(saturation > 4)saturation = 0;
////                    OV2640_Color_Saturation(saturation);
////                    sprintf((char*)msgbuf, "Saturation:%d", (signed char)saturation - 2);
////                    break;
////                case KEY2_PRES:	//��Ч����
////                    effect++;
////                    if(effect > 6)effect = 0;
////                    OV2640_Special_Effects(effect);//������Ч
////                    sprintf((char*)msgbuf, "%s", EFFECTS_TBL[effect]);
////                    break;
////                case WKUP_PRES:	//JPEG����ߴ�����
////                    size++;
////                    if(size > 8)size = 0;
////                    OV2640_OutSize_Set(jpeg_img_size_tbl[size][0], jpeg_img_size_tbl[size][1]); //��������ߴ�
////                    sprintf((char*)msgbuf, "JPEG Size:%s", JPEG_SIZE_TBL[size]);
////                    break;


/************************************************************************************************/
//����ת�溬˰

//STM32H7����ģ��-HAL�⺯���汾
//DevEBox  ��Խ����
//�Ա����̣�mcudev.taobao.com
//�Ա����̣�shop389957290.taobao.com	


/************************************************************************************************/

static void  Copy_RAM_Data(u16 *P1, u16 *P2, u16 Num)
{
	u16 i;
	
	for(i=0;i<Num;i++)
	{
		*P1++=*P2++;

	}
	
}



/************************************************************************************************/
//RGB�����ݽ��ջص�����

//STM32H7����ģ��-HAL�⺯���汾
//DevEBox  ��Խ����
//�Ա����̣�mcudev.taobao.com
//�Ա����̣�shop389957290.taobao.com	


/************************************************************************************************/



void rgblcd_dcmi_rx_callback(void)
{
    if(DMA1_Stream1->CR & (1<<19))  // ʹ�� buf1����ȡ buf0
    { 
        Copy_RAM_Data(&OVxxx.RGB_DATA[curline][0], (u16*)RGB_Line_Buf[0], lcddev.width);
    }
    else  // ʹ�� buf0����ȡ buf1
    {
        Copy_RAM_Data(&OVxxx.RGB_DATA[curline][0], (u16*)RGB_Line_Buf[1], lcddev.width);	
    }

    if (++curline >= lcddev.height)
    {
        RGB_FrameNum = 1;  // һ֡�ɼ���ϣ���ѭ��ˢ����ʾ
        curline = 0;       // �����м���
    }
}






u8 Key_N;

void RGB565_mode(void)
{
    
    float fac=0;
    u8 effect=0,contrast=2;
    u8 scale=1;		//Ĭ����ȫ�ߴ�����
    
    u16 outputheight=0;

    LCD_Clear(WHITE);
	    	
    Draw_Font16B(30,50,BLUE,"DevEBox STM32H750");
    Draw_Font16B(30,70,BLUE,"OV2640 RGB565 Mode");
	
    Draw_Font16B(30,90,BLUE, "KEY1:DCMI_Start/Stop");	 //DCMI���� ��ʼ��ֹͣ
    Draw_Font16B(30,110,BLUE,"KEY2: Contrast"); 		   //ִ�����öԱȶ�

	
	
    OV2640_RGB565_Mode();	//RGB565ģʽ
		
    DCMI_Init();			//DCMI����


		dcmi_rx_callback=rgblcd_dcmi_rx_callback;//RGB���������ݻص�����
		
		DCMI_DMA_Init((u32)RGB_Line_Buf[0],(u32)RGB_Line_Buf[1],lcddev.width/2,DMA_MDATAALIGN_HALFWORD,DMA_MINC_ENABLE);//DCMI DMA���� 
		
		
//    TIM3->CR1&=~(0x01);		//�رն�ʱ��3,�ر�֡��ͳ�ƣ��򿪵Ļ���RGB�����ڴ��ڴ�ӡ��ʱ�򣬻ᶶ

	
		    
		
    OV2640_OutSize_Set(lcddev.width, lcddev.height);//����������ʾ
						
    DCMI_Start(); 			//��������
			
    LCD_Clear(BLACK);
		
    while(1)
    {
			
        Key_Flag=KEY_Scan(0);   //��ȡ��ֵ
       
			  if(Key_Flag==KEY1_PRES)//����1�л�����
				{
						Key_N++;
						if(Key_N>=3)Key_N=0;
		
					if(Key_N==1)
						{
							Draw_Font16B(30,90,BLUE,"OV2640: DCMI_Stop");//��ʾ��ʾ����
					    delay_ms(800);
							
							DCMI_Stop(); //��KEY1����,ֹͣ��ʾ
						}
					else if(Key_N==2) 
						{	
							Draw_Font16B(30,90,BLUE,"OV2640: DCMI_Start");//��ʾ��ʾ����
					    delay_ms(800);
							
							DCMI_Start();	//���¿�ʼ����
						}					
				}
			 else if(Key_Flag==KEY2_PRES)//����1�л�����
				{
					contrast++;
					
          if(contrast > 4)contrast = 0;
					
          OV2640_Contrast(contrast);
					
          sprintf((char*)Print_buf, "Contrast:%d", (signed char)contrast - 2);
					
					Draw_Font16B(30,90,BLUE,Print_buf);//��ʾ��ʾ����
					
					delay_ms(800);
         
					
				}
				
				

				
			if (RGB_FrameNum > 0)
{
    RGB_FrameNum = 0;
    RGB_Refresh_LCD(); // ��֡ˢ��һ�Σ���˺��

				 
	    }
}




































/************************************************************************************************/

//STM32H7����ģ��-HAL�⺯���汾
//DevEBox  ��Խ����
//�Ա����̣�mcudev.taobao.com
//�Ա����̣�shop389957290.taobao.com	


/************************************************************************************************/




}
#include <stdint.h>

// RGB565ת�Ҷȣ�����ΪRGB565ͼ�񻺳壬���Ϊ�ҶȻ��壨uint8_t��
// width, height ������ͼ�����
void RGB565_to_Gray(const uint16_t *rgb565_buf, uint8_t *gray_buf, uint16_t width, uint16_t height)
{
    uint32_t pixel_count = width * height;
    for(uint32_t i = 0; i < pixel_count; i++)
    {
        uint16_t pixel = rgb565_buf[i];
        // ��ȡR G B
        uint8_t r5 = (pixel >> 11) & 0x1F;
        uint8_t g6 = (pixel >> 5) & 0x3F;
        uint8_t b5 = pixel & 0x1F;

        // ת��8λRGB
        uint8_t r8 = (r5 * 527 + 23) >> 6;  // 5-bit to 8-bit
        uint8_t g8 = (g6 * 259 + 33) >> 6;  // 6-bit to 8-bit
        uint8_t b8 = (b5 * 527 + 23) >> 6;  // 5-bit to 8-bit

        // ת�Ҷȣ����ƹ�ʽ
        uint8_t gray = (uint8_t)((r8 * 299 + g8 * 587 + b8 * 114) / 1000);

        gray_buf[i] = gray;
    }
}

// ��������ţ�����Ҷ�ͼ�ߴ磺src_w * src_h���������Ϊ dst_w * dst_h
void resize_nearest_neighbor(const uint8_t *src, uint16_t src_w, uint16_t src_h,
                             uint8_t *dst, uint16_t dst_w, uint16_t dst_h)
{
    for(uint16_t y = 0; y < dst_h; y++)
    {
        uint16_t src_y = y * src_h / dst_h;
        for(uint16_t x = 0; x < dst_w; x++)
        {
            uint16_t src_x = x * src_w / dst_w;
            dst[y * dst_w + x] = src[src_y * src_w + src_x];
        }
    }
}
void normalize_input(const uint8_t *gray_in, float *float_out, int size)
{
    for(int i=0; i<size; i++)
    {
        float_out[i] = gray_in[i] / 255.0f;  // ��һ����0-1
    }
}

//���������࣬��Ҫ�����������ܺ�ģʽ�����޸�����	
				
////            case KEY0_PRES:	//�Աȶ�����
////                contrast++;
////                if(contrast > 4)contrast = 0;
////                OV2640_Contrast(contrast);
////                sprintf((char*)msgbuf, "Contrast:%d", (signed char)contrast - 2);
////                break;
////            case KEY1_PRES:	//���Ͷ�Saturation
////                saturation++;
////                if(saturation > 4)saturation = 0;
////                OV2640_Color_Saturation(saturation);
////                sprintf((char*)msgbuf, "Saturation:%d", (signed char)saturation - 2);
////                break;
////            case KEY2_PRES:	//��Ч����
////                effect++;
////                if(effect > 6)effect = 0;
////                OV2640_Special_Effects(effect);//������Ч
////                sprintf((char*)msgbuf, "%s", EFFECTS_TBL[effect]);
////                break;
////            case WKUP_PRES:	//1:1�ߴ�(��ʾ��ʵ�ߴ�)/����--��Ҫ�����ڴ�ʹ�ú���Ļ���ʹ��
////                scale = !scale;
////                if(scale == 0)
////                {
////                    OV2640_ImageWin_Set((1600 - lcddev.width) / 2, (1200 - lcddev.height) / 2, lcddev.width, lcddev.height); //1:1��ʵ�ߴ�
////                    OV2640_OutSize_Set(lcddev.width, lcddev.height);
////                    sprintf((char*)msgbuf, "Full Size 1:1");
////                }
////                else
////                {
////                    OV2640_ImageWin_Set(0, 0, 1600, 1200);				//ȫ�ߴ�����
////                    OV2640_OutSize_Set(lcddev.width, lcddev.height);
////                    sprintf((char*)msgbuf, "Scale");
////                }
////                break;