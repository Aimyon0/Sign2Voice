#include "usart2.h"
//////////////////////////////////////////////////////////////////////////////////	 

/************************************************************************************************/
//USART驱动代码	

//STM32H7工程模板-HAL库函数版本
//DevEBox  大越创新
//淘宝店铺：mcudev.taobao.com
//淘宝店铺：shop389957290.taobao.com	


/************************************************************************************************/								  
////////////////////////////////////////////////////////////////////////////////// 	


UART_HandleTypeDef USART2_Handler;  //USART2句柄




/************************************************************************************************/

//初始化IO 串口2
//bound:波特率

//STM32H7工程模板-HAL库函数版本
//DevEBox  大越创新
//淘宝店铺：mcudev.taobao.com
//淘宝店铺：shop389957290.taobao.com	


/************************************************************************************************/
void usart2_init(u32 bound)
{
    //GPIO端口设置
	GPIO_InitTypeDef GPIO_Initure;
	
	__HAL_RCC_GPIOA_CLK_ENABLE();			//使能GPIOA时钟
	__HAL_RCC_USART2_CLK_ENABLE();			//使能USART2时钟
	
	GPIO_Initure.Pin=GPIO_PIN_2|GPIO_PIN_3; //PA2,3
	GPIO_Initure.Mode=GPIO_MODE_AF_PP;		//复用推挽输出
	GPIO_Initure.Pull=GPIO_PULLUP;			//上拉
	GPIO_Initure.Speed=GPIO_SPEED_FREQ_VERY_HIGH;		//高速
	GPIO_Initure.Alternate=GPIO_AF7_USART2;	//复用为USART2
	HAL_GPIO_Init(GPIOA,&GPIO_Initure);	   	//初始化PA2,3
    
    //USART 初始化设置
	USART2_Handler.Instance=USART2;					    //USART2
	USART2_Handler.Init.BaudRate=bound;				    //波特率
	USART2_Handler.Init.WordLength=UART_WORDLENGTH_8B;	//字长为8位数据格式
	USART2_Handler.Init.StopBits=UART_STOPBITS_1;		//一个停止位
	USART2_Handler.Init.Parity=UART_PARITY_NONE;		//无奇偶校验位
	USART2_Handler.Init.HwFlowCtl=UART_HWCONTROL_NONE;	//无硬件流控
	USART2_Handler.Init.Mode=UART_MODE_TX;			    //发模式
	HAL_UART_Init(&USART2_Handler);					    //HAL_UART_Init()会使能USART2
}





































/************************************************************************************************/


//STM32H7工程模板-HAL库函数版本
//DevEBox  大越创新
//淘宝店铺：mcudev.taobao.com
//淘宝店铺：shop389957290.taobao.com	


/************************************************************************************************/





