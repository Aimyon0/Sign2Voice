#include "timer.h"
#include "stm32h7xx_hal.h"
TIM_HandleTypeDef TIM3_Handler;       //��ʱ����� 
TIM_HandleTypeDef TIM6_Handler;       //��ʱ��6��� 

__IO u32 ticknum=0;
void TIM3_Init(u16 arr,u16 psc)
{  
    TIM3_Handler.Instance=TIM3;                          //ͨ�ö�ʱ��3
    TIM3_Handler.Init.Prescaler=psc;                     //��Ƶ
    TIM3_Handler.Init.CounterMode=TIM_COUNTERMODE_UP;    //���ϼ�����
    TIM3_Handler.Init.Period=arr;                        //�Զ�װ��ֵ
    TIM3_Handler.Init.ClockDivision=TIM_CLOCKDIVISION_DIV1;
    HAL_TIM_Base_Init(&TIM3_Handler);
    
    HAL_TIM_Base_Start_IT(&TIM3_Handler); //ʹ�ܶ�ʱ��3�Ͷ�ʱ��3�ж�   
}
void TIM6_Init(u16 arr,u16 psc)
{  
    TIM6_Handler.Instance=TIM6;                          //ͨ�ö�ʱ��6
    TIM6_Handler.Init.Prescaler=psc;                     //��Ƶ
    TIM6_Handler.Init.CounterMode=TIM_COUNTERMODE_UP;    //���ϼ�����
    TIM6_Handler.Init.Period=arr;                        //�Զ�װ��ֵ
    TIM6_Handler.Init.ClockDivision=TIM_CLOCKDIVISION_DIV1;
    HAL_TIM_Base_Init(&TIM6_Handler);
    
    HAL_TIM_Base_Start_IT(&TIM6_Handler); //ʹ�ܶ�ʱ��6�Ͷ�ʱ��6�ж�   
}
//��ʱ���ײ�����������ʱ�ӣ������ж����ȼ�
void HAL_TIM_Base_MspInit(TIM_HandleTypeDef *htim)
{
    __HAL_RCC_TIM3_CLK_ENABLE();            //ʹ��TIM3ʱ��
	__HAL_RCC_TIM6_CLK_ENABLE();            //ʹ��TIM6ʱ��
	
    HAL_NVIC_SetPriority(TIM3_IRQn,1,3);    //�����ж����ȼ�����ռ���ȼ�1�������ȼ�3
    HAL_NVIC_EnableIRQ(TIM3_IRQn);          //����ITM3�ж�   
	
	HAL_NVIC_SetPriority(TIM6_DAC_IRQn,0,3);//�����ж����ȼ�����ռ���ȼ�0�������ȼ�3
    HAL_NVIC_EnableIRQ(TIM6_DAC_IRQn);      //����ITM6�ж� 
}
void TIM3_IRQHandler(void)
{
    HAL_TIM_IRQHandler(&TIM3_Handler);
}	
void TIM6_DAC_IRQHandler(void)
{
    HAL_TIM_IRQHandler(&TIM6_Handler);
}
u16 frame;
vu8 frameup;

//��ȡticknum
u32 GetTicknum(void)
{
	return ticknum;
}
void TIM3_Start(void)
{	
	__HAL_TIM_ENABLE(&TIM3_Handler);
	ticknum=0;
}
void TIM3_Stop(void)
{	
	__HAL_TIM_DISABLE(&TIM3_Handler);
}








































/*********************************************************************************/


//STM32H7����ģ��-HAL�⺯���汾
//DevEBox  ��Խ����
//�Ա����̣�mcudev.taobao.com
//�Ա����̣�shop389957290.taobao.com	


/**********************************************************************************/	







