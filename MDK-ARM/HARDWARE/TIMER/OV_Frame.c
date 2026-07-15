#include <stdio.h>
#include "cmsis_os.h"
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
#include "app_x-cube-ai.h"
#include "usart.h"
#include "usart2.h"
#include "app_x-cube-ai.h"
static int frame_counter = 0;
void RGB565_To_Gray_Resize(const uint16_t* rgb565_buf, uint16_t src_w, uint16_t src_h, uint8_t* gray28x28);
extern	uint8_t Print_buf[32];	//��Ϣ������
extern   uint8_t Key_Flag; //��ֵ
u8 OV_mode=0;							//bit0:0,RGB565ģʽ;1,JPEGģʽ
u16 yoffset=0;							//y������?����
#define MAX_DMA_SIZE 65535
extern	uint8_t Print_buf[32];	//��Ϣ������
volatile uint8_t lcd_dma_done_flag = 1;
#define LCD_RS_SET   HAL_GPIO_WritePin(GPIOB,GPIO_PIN_1,GPIO_PIN_SET)   // PB1 ��Ϊ DC/RS
#define LCD_RS_CLR   HAL_GPIO_WritePin(GPIOB,GPIO_PIN_1,GPIO_PIN_RESET)
volatile int dma_chunk_count = 0;
#define jpeg_buf_size   (RGB_Width*RGB_Height+1024*40)

uint16_t RGB_DATA_Contiguous[RGB_Width * RGB_Height]
    __attribute__((section(".AXI_RAM"), aligned(32)));
extern int g_predicted_digit;
void RGB565_To_64x64_RGB888_Float(const uint16_t* rgb565_buf, uint16_t src_w, uint16_t src_h, float* out_tensor);
uint16_t RGB_DATA[RGB_Height][RGB_Width]
    __attribute__((section(".AXI_RAM"), aligned(32)));

u32 RGB_Line_Buf[2][RGB_Width*2]
    __attribute__((section(".D2_RAM"), aligned(32)));


u32 jpeg_data_len=0; 			       //buf�е�JPEG��Ч���ݳ���

u8 jpeg_data_ok=0;				       //JPEG���ݲɼ���ɱ��?

void HAL_SPI_TxCpltCallback(SPI_HandleTypeDef *hspi)
{
    if(hspi->Instance == SPI2)
    {
        lcd_dma_done_flag = 1;
    }
}
void Start_LCD_DMA_Refresh(void)
{
    uint32_t timeout;
    uint32_t total_bytes = 240 * 320 * 2;
    uint32_t sent_bytes = 0;

    HAL_StatusTypeDef ret;
    uint16_t chunk_size;

    int y;
    uint32_t i;

    /* SPI忙，跳过当前�? */
    if(HAL_SPI_GetState(&hspi2)
        != HAL_SPI_STATE_READY)
    {
        return;
    }

    /* 行拷�? */
    for(y = 0; y < lcddev.height; y++)
    {
        memcpy(
            &RGB_DATA_Contiguous[
                y * lcddev.width
            ],
            &RGB_DATA[y][0],
            lcddev.width * 2
        );
    }

    /* 更快的RGB565字节交换 */
    uint32_t *p32 =
        (uint32_t*)RGB_DATA_Contiguous;

    uint32_t words =
        (lcddev.width
        * lcddev.height) / 2;

    for(i = 0; i < words; i++)
    {
        uint32_t v = p32[i];

        p32[i] =
            ((v & 0x00FF00FF) << 8) |
            ((v & 0xFF00FF00) >> 8);
    }

    /* SPI DMA读前写回Cache */
    SCB_CleanDCache_by_Addr(
        (uint32_t*)
        RGB_DATA_Contiguous,
        sizeof(RGB_DATA_Contiguous)
    );

    /* LCD窗口 */
    LCD_Set_Window(
        0,
        0,
        lcddev.width,
        lcddev.height
    );

    LCD_SetCursor(0, 0);

    LCD_WR_REG(
        lcddev.wramcmd
    );

    LCD_RS_SET;
    LCD_CS_CLR;

    while(sent_bytes < total_bytes)
    {
        chunk_size =
            ((total_bytes
            - sent_bytes)
            > MAX_DMA_SIZE)
            ? MAX_DMA_SIZE
            : (total_bytes
            - sent_bytes);

        lcd_dma_done_flag = 0;

        __HAL_SPI_CLEAR_EOTFLAG(
            &hspi2
        );

        ret =
        HAL_SPI_Transmit_DMA(
            &hspi2,
            (uint8_t*)
            RGB_DATA_Contiguous
            + sent_bytes,
            chunk_size
        );

        if(ret != HAL_OK)
        {
            break;
        }

        /* 等DMA结束 */
        timeout = 1000000;

        while(
            lcd_dma_done_flag == 0
            && timeout--
        );

        if(timeout == 0)
        {
            break;
        }

        /* 等SPI真�?�发�? */
        timeout = 1000000;

        while(
            __HAL_SPI_GET_FLAG(
                &hspi2,
                SPI_FLAG_EOT
            ) == RESET
            && timeout--
        );

        if(timeout == 0)
        {
            break;
        }

        sent_bytes += chunk_size;
    }

    LCD_CS_SET;
}



const u16 jpeg_img_size_tbl[][2]=
{
    //160,120,	//QQVGA
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
union TData
{
	
   uint32_t B32_temp;
   uint8_t  B8_Temp[4];
	
} TData; 
//����ʾ���������ݽ���ˢ������
  u16 Num_Dot;
  u16 Num_H;//����
	u16 *R_Buf; //ˢ������ָ��
	
void  RGB_Refresh_LCD(void)
{
	  
		LCD_Set_Window(0,0,lcddev.width,lcddev.height);//����ȫ������
		
    LCD_SetCursor(0,0);              //������Ļ��ʾ���?	
	
    LCD_WR_REG(lcddev.wramcmd);	  
	
		for(Num_H=0;Num_H<lcddev.height;Num_H++)
		{
			
			R_Buf=&RGB_DATA[Num_H][0];
			
			for(Num_Dot=0;Num_Dot<lcddev.width;Num_Dot++)
			{
				
				LCD_WR_DATA16(*R_Buf);	
				R_Buf++;
				
			}
			
			Num_H++;//������ʾ����
			
			R_Buf=&RGB_DATA[Num_H][0];
			
			for(Num_Dot=0;Num_Dot<lcddev.width;Num_Dot++)
			{
				
				LCD_WR_DATA16(*R_Buf);	
				R_Buf++;
				

			}
 
	  }
	
	
}
//����JPEG����
//���ɼ���һ֡JPEG���ݺ�,���ô˺���,�л�JPEG BUF.��ʼ��һ֡�ɼ�.
volatile uint16_t  RGB_FrameNum=0;

volatile uint16_t  curline=0;							//����ͷ�������?,��ǰ�б��?
void jpeg_data_process(void)
{
    u16 rlen;
    curline = 0;

    if (OV_mode & 0X01)
    {
        if (jpeg_data_ok == 0)
        {
            DMA1_Stream1->CR &= ~(1 << 0);
            while (DMA1_Stream1->CR & 0X01);
            rlen = jpeg_buf_size - DMA1_Stream1->NDTR;
            jpeg_data_len += rlen;
            jpeg_data_ok = 1;
        }
        if (jpeg_data_ok == 2)
        {
            DMA1_Stream1->NDTR = jpeg_buf_size;
            DMA1_Stream1->CR |= 1 << 0;
            jpeg_data_ok = 0;
            jpeg_data_len = 0;
        }
    }
    else
    {
        RGB_FrameNum++;
    }
}




//JPEG����
//JPEG����,ͨ������2���͸�����.
#if 0
void JPEG_mode(void)
{
    u32 i,jpgstart,jpglen;
    u8  *p;
    
	  u8  headok=0;
	  u8  effect=0;
	  u8  contrast=2;
    u8 size=1;			//Ĭ����QVGA 320*240�ߴ�

	   uart_init(460800);				       //����1 ��ʼ��
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
		
    OV2640_OutSize_Set(jpeg_img_size_tbl[size][0], jpeg_img_size_tbl[size][1]);  //��������ߴ�?
		
    DCMI_Start(); 		//��������
		
		
    while(1)
    {
        if(jpeg_data_ok==1)	//�Ѿ��ɼ���һ֡ͼ����
        {
            p=(u8*)OVxxx.JPEG_DATA;
					
//            printf("Jpeg_data:%d\r\n",jpeg_data_len*4);//��ӡ֡��
					
            Draw_Font16B(30,150,BLUE,"Sending JPEG ..."); //��ʾ���ڴ�������
					
            jpglen=0;	//����jpg�ļ���СΪ0
            headok=0;	//���jpgͷ���?
					
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
                    while((USART1->ISR&0X40)==0);	//ѭ������,ֱ���������?

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
                case KEY2_PRES:	//��������ߴ�?
                    size++;
                    if(size>11)size=0;
                    OV2640_OutSize_Set(jpeg_img_size_tbl[size][0],jpeg_img_size_tbl[size][1]);//��������ߴ�?
                    sprintf((char*)Print_buf,"JPEG Size:%s",JPEG_SIZE_TBL[size]);
                    break;
								
								
							
								
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
////                case WKUP_PRES:	//JPEG����ߴ�����?
////                    size++;
////                    if(size > 8)size = 0;
////                    OV2640_OutSize_Set(jpeg_img_size_tbl[size][0], jpeg_img_size_tbl[size][1]); //��������ߴ�?
////                    sprintf((char*)msgbuf, "JPEG Size:%s", JPEG_SIZE_TBL[size]);
////                    break;
                }
								
                Draw_Font16B(30,180,BLUE,"               ");//��ʾ��ʾ����
								
                Draw_Font16B(30,180,BLUE,Print_buf);//��ʾ��ʾ����
								
                delay_ms(800);
								
            } 
						
						else Draw_Font16B(30,150,BLUE,"Send complete!!");//��ʾ�����������?
						
            jpeg_data_ok=2;	//���jpeg���ݴ�������,������DMAȥ�ɼ���һ֡��.
        }
    }
}
//����ת�溯��
#endif
static void  Copy_RAM_Data(u16 *P1, u16 *P2, u16 Num)
{
	u16 i;
	
	for(i=0;i<Num;i++)
	{
		*P1++=*P2++;

	}
	
}
volatile int RBG_dbg_cnt = -1;
//RGB color data receive callback
void rgblcd_dcmi_rx_callback(void)
{
    u16 *src_buf;
    u32 addr, aligned_addr, size;

    if(DMA1_Stream1->CR&(1<<19))//DMA uses buf1, read buf0
    {
        src_buf = (u16*)RGB_Line_Buf[0];
    }
    else                        //DMA uses buf0, read buf1
    {
        src_buf = (u16*)RGB_Line_Buf[1];
    }

    /* Invalidate D-Cache for the DMA buffer before CPU read */
    addr = (u32)src_buf;
    aligned_addr = addr & ~0x1FU;
    size = ((addr + lcddev.width * 2 + 31U) & ~0x1FU) - aligned_addr;
    SCB_InvalidateDCache_by_Addr((uint32_t*)aligned_addr, size);

    Copy_RAM_Data(&RGB_DATA[curline][0], src_buf, lcddev.width);
    if(curline < lcddev.height) ++curline;

    { static uint32_t _cc; RBG_dbg_cnt = (int)++_cc; }  /* ISR called N times */

    if (curline >= lcddev.height)
    {
        RGB_FrameNum = 1;     // one frame ready for refresh
        curline = 0;          // reset for next frame
    }
}
u8 Key_N;
void lcdtest(void)
{				u8 contrast=2;      
//		printf("1\n");
		OV2640_Init();
    LCD_Clear(WHITE);	    	
    Draw_Font16B(30,50,BLUE,"DevEBox STM32H750");
    Draw_Font16B(30,70,BLUE,"OV2640 RGB565 Mode");	
    Draw_Font16B(30,90,BLUE, "KEY1:DCMI_Start/Stop");	 //DCMI���� ��ʼ��ֹͣ
    Draw_Font16B(30,110,BLUE,"KEY2: Contrast"); 
//		printf("LCD Width");
	//ִ�����öԱȶ�		
    OV2640_RGB565_Mode();	//RGB565ģʽ		
    DCMI_Init();			//DCMI����
		dcmi_rx_callback=rgblcd_dcmi_rx_callback;//RGB���������ݻص�����		
		DCMI_DMA_Init((u32)RGB_Line_Buf[0],(u32)RGB_Line_Buf[1],lcddev.width/2,DMA_MDATAALIGN_HALFWORD,DMA_MINC_ENABLE);//DCMI DMA����				    		
    OV2640_OutSize_Set(lcddev.width, lcddev.height);//����������ʾ						
    DCMI_Start(); 			//��������			
    LCD_Clear(BLACK);	
//		printf("gogogo\n");	
    while(1)
    {		
        /*Key_Flag=KEY_Scan(0);   //��ȡ��ֵ       
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
			if (RGB_FrameNum > 0) {
				if (lcd_dma_done_flag == 1) {
        RGB_FrameNum = 0;
        lcd_dma_done_flag = 0;
        Start_LCD_DMA_Refresh();
				} else {
//						printf("LCD still busy, skip this frame\n");
    }
}*/
        osDelay(10);
	    }
        }
void RGB565_mode(void)
{		
    static int predicted_digit = -1;  // Ԥ����
    static uint32_t last_infer_time = 0;
    OV2640_RGB565_Mode();
    DCMI_Init();
    dcmi_rx_callback = rgblcd_dcmi_rx_callback;
    DCMI_DMA_Init((u32)RGB_Line_Buf[0], (u32)RGB_Line_Buf[1],lcddev.width / 2, DMA_MDATAALIGN_HALFWORD, DMA_MINC_ENABLE);
    OV2640_OutSize_Set(lcddev.width, lcddev.height);
    DCMI_Start();
//    printf("RGB565 Mode started.\r\n");
    while (1)
    {
        if (RGB_FrameNum > 0)  // һ֡ͼ���Ѳɼ����?
        {
            RGB_FrameNum = 0;	 
            frame_counter++;
        
            SCB_InvalidateDCache_by_Addr((uint32_t*)RGB_DATA, sizeof(RGB_DATA));
            Start_LCD_DMA_Refresh(); 

            if (frame_counter % 30 == 0)
            {
                last_infer_time = HAL_GetTick();
                RGB565_To_64x64_RGB888_Float(&RGB_DATA[0][0], lcddev.width, lcddev.height, ai_input_data);					
                MX_X_CUBE_AI_Process();
//								printf("Input data sample after assign: %f %f %f %f\n", 
//    ((float*)ai_input[0].data)[0], 
//    ((float*)ai_input[0].data)[1], 
//    ((float*)ai_input[0].data)[2], 
//    ((float*)ai_input[0].data)[3])

		LCD_Fill(10, 10, 230, 40, WHITE);  // ֻ���Ԥ���ı�����?

    // ���ݽ����ʾԤ������?
  
            }
						  if (g_predicted_class >= 0 && g_predicted_class < 5) {
							LCD_Fill(10, 10, 230, 40, WHITE);  // ����?��������
							Draw_Font16B(20, 20, BLUE, (u8*)class_names[g_predicted_class]);  // ��ʾ�����?
							} else {
							Draw_Font16B(20, 20, BLUE, "Background");
							}

        }
        delay_ms(10);
    }
}
void RGB565_To_64x64_RGB888_Float(const uint16_t* rgb565_buf, uint16_t src_w, uint16_t src_h, float* out_tensor)
{
    const int dst_w = 64;
    const int dst_h = 64;

    float x_ratio = ((float)(src_w - 1)) / (dst_w - 1);
    float y_ratio = ((float)(src_h - 1)) / (dst_h - 1);

    for (int y = 0; y < dst_h; y++) {
        float src_y = y * y_ratio;
        int y_low = (int)src_y;
        int y_high = (y_low < src_h - 1) ? y_low + 1 : y_low;
        float y_lerp = src_y - y_low;

        for (int x = 0; x < dst_w; x++) {
            float src_x = x * x_ratio;
            int x_low = (int)src_x;
            int x_high = (x_low < src_w - 1) ? x_low + 1 : x_low;
            float x_lerp = src_x - x_low;

            // ��ȡ4���ڽ����أ�RGB��
            uint16_t p00 = rgb565_buf[y_low * src_w + x_low];
            uint16_t p01 = rgb565_buf[y_low * src_w + x_high];
            uint16_t p10 = rgb565_buf[y_high * src_w + x_low];
            uint16_t p11 = rgb565_buf[y_high * src_w + x_high];

            // ��ȡ R/G/B��ӳ�䵽 0~255��
            uint8_t r00 = ((p00 >> 11) & 0x1F) * 255 / 31;
            uint8_t g00 = ((p00 >> 5) & 0x3F) * 255 / 63;
            uint8_t b00 = (p00 & 0x1F) * 255 / 31;

            uint8_t r01 = ((p01 >> 11) & 0x1F) * 255 / 31;
            uint8_t g01 = ((p01 >> 5) & 0x3F) * 255 / 63;
            uint8_t b01 = (p01 & 0x1F) * 255 / 31;

            uint8_t r10 = ((p10 >> 11) & 0x1F) * 255 / 31;
            uint8_t g10 = ((p10 >> 5) & 0x3F) * 255 / 63;
            uint8_t b10 = (p10 & 0x1F) * 255 / 31;

            uint8_t r11 = ((p11 >> 11) & 0x1F) * 255 / 31;
            uint8_t g11 = ((p11 >> 5) & 0x3F) * 255 / 63;
            uint8_t b11 = (p11 & 0x1F) * 255 / 31;

            // ˫���Բ�ֵ��ÿ��ͨ����
            float r_top = r00 + (r01 - r00) * x_lerp;
            float r_bot = r10 + (r11 - r10) * x_lerp;
            float r = r_top + (r_bot - r_top) * y_lerp;

            float g_top = g00 + (g01 - g00) * x_lerp;
            float g_bot = g10 + (g11 - g10) * x_lerp;
            float g = g_top + (g_bot - g_top) * y_lerp;

            float b_top = b00 + (b01 - b00) * x_lerp;
            float b_bot = b10 + (b11 - b10) * x_lerp;
            float b = b_top + (b_bot - b_top) * y_lerp;

            
            int dst_idx = (y * dst_w + x) * 3;
            out_tensor[dst_idx + 0] = r / 255.0f;
            out_tensor[dst_idx + 1] = g / 255.0f;
            out_tensor[dst_idx + 2] = b / 255.0f;
        }
    }
}

/* Convert RGB565 camera image �? 64×64 grayscale �? 3-channel float (replicates gray).
 * Matches Python MNIST preprocessing: grayscale, resize, expand to RGB, /255.0. */
void RGB565_To_64x64_Gray3CH_Float(const uint16_t* rgb565_buf, uint16_t src_w, uint16_t src_h, float* out_tensor)
{
    const int dst_w = 64;
    const int dst_h = 64;

    float x_ratio = ((float)(src_w - 1)) / (dst_w - 1);
    float y_ratio = ((float)(src_h - 1)) / (dst_h - 1);

    for (int y = 0; y < dst_h; y++) {
        float src_y = y * y_ratio;
        int y_low = (int)src_y;
        int y_high = (y_low < src_h - 1) ? y_low + 1 : y_low;
        float y_lerp = src_y - y_low;

        for (int x = 0; x < dst_w; x++) {
            float src_x = x * x_ratio;
            int x_low = (int)src_x;
            int x_high = (x_low < src_w - 1) ? x_low + 1 : x_low;
            float x_lerp = src_x - x_low;

            /* Extract grayscale from 4 neighbors using luminance: Y = 0.299R + 0.587G + 0.114B */
            uint16_t p00 = rgb565_buf[y_low * src_w + x_low];
            uint16_t p01 = rgb565_buf[y_low * src_w + x_high];
            uint16_t p10 = rgb565_buf[y_high * src_w + x_low];
            uint16_t p11 = rgb565_buf[y_high * src_w + x_high];

            float gy00 = ((p00 >> 11) & 0x1F) * 255.0f / 31.0f * 0.299f
                       + ((p00 >> 5)  & 0x3F) * 255.0f / 63.0f * 0.587f
                       + ((p00)       & 0x1F) * 255.0f / 31.0f * 0.114f;
            float gy01 = ((p01 >> 11) & 0x1F) * 255.0f / 31.0f * 0.299f
                       + ((p01 >> 5)  & 0x3F) * 255.0f / 63.0f * 0.587f
                       + ((p01)       & 0x1F) * 255.0f / 31.0f * 0.114f;
            float gy10 = ((p10 >> 11) & 0x1F) * 255.0f / 31.0f * 0.299f
                       + ((p10 >> 5)  & 0x3F) * 255.0f / 63.0f * 0.587f
                       + ((p10)       & 0x1F) * 255.0f / 31.0f * 0.114f;
            float gy11 = ((p11 >> 11) & 0x1F) * 255.0f / 31.0f * 0.299f
                       + ((p11 >> 5)  & 0x3F) * 255.0f / 63.0f * 0.587f
                       + ((p11)       & 0x1F) * 255.0f / 31.0f * 0.114f;

            /* Bilinear interpolate grayscale */
            float gray_top = gy00 + (gy01 - gy00) * x_lerp;
            float gray_bot = gy10 + (gy11 - gy10) * x_lerp;
            float gray = gray_top + (gray_bot - gray_top) * y_lerp;

            /* Normalize and replicate to 3 channels */
            float val = gray / 255.0f;
            int dst_idx = (y * dst_w + x) * 3;
            out_tensor[dst_idx + 0] = val;
            out_tensor[dst_idx + 1] = val;
            out_tensor[dst_idx + 2] = val;
        }
    }
}











				
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
////            case WKUP_PRES:	//1:1�ߴ�(��ʾ��ʵ�ߴ�)/����--��Ҫ�����ڴ�ʹ�ú���Ļ���ʹ��?
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






























