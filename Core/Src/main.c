/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "cmsis_os.h"
#include "dma.h"
#include "spi.h"
#include "usart.h"
#include "gpio.h"
#include "app_x-cube-ai.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "ai_platform.h"
#include "gesture.h"
#include "gesture_data.h"
#include "system.h"
#include "usart.h"
#include "delay.h"
#include "led.h"
#include "key.h"
#include "LCD.h"
#include "Dis_Picture.h"
#include "Text.h"
#include "GBK_LibDrive.h"

#include "mpu.h"
#include "timer.h"
#include "ov2640.h"
#include "dcmi.h"
#include "usart2.h"
#include <stdio.h>
#include "OV_Frame.h"
#include "lvgl.h"
#include "lv_port_disp.h"
#include "lv_port_indev.h"
#include "lv_port_tick.h"
#include "ui_overlay.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
u8 Stm32_Clock_Init(u32 plln,u32 pllm,u32 pllp,u32 pllq);
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
uint8_t Key_Flag;
uint8_t T_Num;
uint8_t Print_buf[32];
ai_handle network_handle = AI_HANDLE_NULL;
extern uint8_t  OV_mode;
extern uint16_t Camera_ID;
int g_predicted_digit = -1;

const char *msg = "12";

volatile int frame_counter = 0;

extern const char *class_names[];

extern float ai_input_data[64*64*3];


/* FreeRTOS */
osSemaphoreId frame_sem;
osMessageQId infer_queue;
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
void MX_FREERTOS_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */
  /* USER CODE END 1 */

  /* Enable I-Cache---------------------------------------------------------*/
  SCB_EnableICache();

  /* Enable D-Cache---------------------------------------------------------*/
  SCB_EnableDCache();

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */
  //SCB->CACR|=1<<2;
  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */
  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_USART1_UART_Init();
  MX_SPI2_Init();
  /* USER CODE BEGIN 2 */
  /* DCMI_Init() moved to display_lcd1 task */
  // �???? printf("delay_init\r\n") 之前�????


//  printf("delay_init\r\n");
  MX_X_CUBE_AI_Init();
  delay_init(SystemCoreClock / 1000000);
//  printf("LED_KEY_MPU\r\n");
  LED_Init();
  KEY_Init();
  //MPU_Memory_Protection();
  HAL_Delay(500);
  SPI2_SetSpeed(SPI_BAUDRATEPRESCALER_4);
//  printf("LCD_Init start\r\n");
  LCD_Init();
//  printf("LCD_Init done\r\n");
//  printf("OV2640_Init start\r\n");
  OV2640_Init();
//  printf("OV2640_Init done\r\n");
  SPI2_SetSpeed(SPI_BAUDRATEPRESCALER_4);

  /* ---- LVGL init (before scheduler starts) ---- */
//  printf("LVGL init\r\n");
  lv_init();
  lv_port_disp_init();
  lv_port_indev_init();
  ui_overlay_init();
  /* lv_port_tick_init() called in display_lcd1 after scheduler start */
//  printf("Init done, bypassing FreeRTOS for test\r\n");
  CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
  DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;
  DWT->CYCCNT = 0;
  

  /* USER CODE END 2 */

  /* Call init function for freertos objects (in freertos.c) */
  MX_FREERTOS_Init();

  /* Start scheduler */
  osKernelStart();

  /* We should never get here as control is now taken by the scheduler */
  /* Infinite loop */
  /* USER CODE BEGIN WHILE */

  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }

  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Supply configuration update enable
  */
  HAL_PWREx_ConfigSupply(PWR_LDO_SUPPLY);

  /** Configure the main internal regulator output voltage
  */
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  while(!__HAL_PWR_GET_FLAG(PWR_FLAG_VOSRDY)) {}

  __HAL_RCC_SYSCFG_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE0);

  while(!__HAL_PWR_GET_FLAG(PWR_FLAG_VOSRDY)) {}

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 2;
  RCC_OscInitStruct.PLL.PLLN = 64;
  RCC_OscInitStruct.PLL.PLLP = 2;
  RCC_OscInitStruct.PLL.PLLQ = 4;
  RCC_OscInitStruct.PLL.PLLR = 2;
  RCC_OscInitStruct.PLL.PLLRGE = RCC_PLL1VCIRANGE_3;
  RCC_OscInitStruct.PLL.PLLVCOSEL = RCC_PLL1VCOWIDE;
  RCC_OscInitStruct.PLL.PLLFRACN = 0;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  if (__HAL_RCC_GET_FLAG(RCC_FLAG_HSERDY) == RESET) Error_Handler();
  if (__HAL_RCC_GET_FLAG(RCC_FLAG_PLLRDY) == RESET) Error_Handler();

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2
                              |RCC_CLOCKTYPE_D3PCLK1|RCC_CLOCKTYPE_D1PCLK1;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.SYSCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB3CLKDivider = RCC_APB3_DIV2;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_APB1_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_APB2_DIV2;
  RCC_ClkInitStruct.APB4CLKDivider = RCC_APB4_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_4) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */
u8 Stm32_Clock_Init(u32 plln,u32 pllm,u32 pllp,u32 pllq)
{
	HAL_StatusTypeDef ret=HAL_OK;
	RCC_ClkInitTypeDef RCC_ClkInitStruct;
	RCC_OscInitTypeDef RCC_OscInitStruct;

	MODIFY_REG(PWR->CR3,PWR_CR3_SCUEN, 0);
	__HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

	while ((PWR->D3CR & (PWR_D3CR_VOSRDY)) != PWR_D3CR_VOSRDY) {}

	RCC_OscInitStruct.OscillatorType=RCC_OSCILLATORTYPE_HSE;
	RCC_OscInitStruct.HSEState=RCC_HSE_ON;
	RCC_OscInitStruct.HSIState=RCC_HSI_OFF;
	RCC_OscInitStruct.CSIState=RCC_CSI_OFF;
	RCC_OscInitStruct.PLL.PLLState=RCC_PLL_ON;
	RCC_OscInitStruct.PLL.PLLSource=RCC_PLLSOURCE_HSE;

	RCC_OscInitStruct.PLL.PLLN=plln;
	RCC_OscInitStruct.PLL.PLLM=pllm;
	RCC_OscInitStruct.PLL.PLLP=pllp;
	RCC_OscInitStruct.PLL.PLLQ=pllq;

	RCC_OscInitStruct.PLL.PLLVCOSEL = RCC_PLL1VCOWIDE;
	RCC_OscInitStruct.PLL.PLLRGE = RCC_PLL1VCIRANGE_2;
	ret=HAL_RCC_OscConfig(&RCC_OscInitStruct);
	if(ret!=HAL_OK) return 1;

	RCC_ClkInitStruct.ClockType=(RCC_CLOCKTYPE_SYSCLK|\
	                            RCC_CLOCKTYPE_HCLK |\
	                            RCC_CLOCKTYPE_D1PCLK1 |\
	                            RCC_CLOCKTYPE_PCLK1 |\
	                            RCC_CLOCKTYPE_PCLK2 |\
	                            RCC_CLOCKTYPE_D3PCLK1);

	RCC_ClkInitStruct.SYSCLKSource=RCC_SYSCLKSOURCE_PLLCLK;
	RCC_ClkInitStruct.SYSCLKDivider=RCC_SYSCLK_DIV1;
	RCC_ClkInitStruct.AHBCLKDivider=RCC_HCLK_DIV2;
	RCC_ClkInitStruct.APB1CLKDivider=RCC_APB1_DIV2;
	RCC_ClkInitStruct.APB2CLKDivider=RCC_APB2_DIV2;
	RCC_ClkInitStruct.APB3CLKDivider=RCC_APB3_DIV2;
	RCC_ClkInitStruct.APB4CLKDivider=RCC_APB4_DIV4;
	ret=HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_4);
	if(ret!=HAL_OK) return 1;

	__HAL_RCC_CSI_ENABLE() ;
	__HAL_RCC_SYSCFG_CLK_ENABLE() ;
	HAL_EnableCompensationCell();

	return 0;
}

/* USER CODE END 4 */

/**
  * @brief  Period elapsed callback in non blocking mode
  * @note   This function is called  when TIM1 interrupt took place, inside
  * HAL_TIM_IRQHandler(). It makes a direct call to HAL_IncTick() to increment
  * a global variable "uwTick" used as application time base.
  * @param  htim : TIM handle
  * @retval None
  */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  /* USER CODE BEGIN Callback 0 */

  /* USER CODE END Callback 0 */
  if (htim->Instance == TIM1) {
    HAL_IncTick();
  }
  /* USER CODE BEGIN Callback 1 */
  else if (htim->Instance == TIM3) {
    extern u16 frame;
    frame = 0;
  }
  else if (htim->Instance == TIM6) {
    extern vu8 frameup;
    frameup = 1;
  }
  /* USER CODE END Callback 1 */
}

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* Toggle LED to indicate error state */
  __disable_irq();
  /* Ensure GPIOB clock is enabled for LED */
  RCC->AHB4ENR |= RCC_AHB4ENR_GPIOBEN;
  (void)RCC->AHB4ENR;  /* delay after clock enable */
  while (1)
  {
    /* Toggle PB1 LED at ~1Hz */
    for(volatile uint32_t i = 0; i < 10000000; i++);
    GPIOB->ODR ^= GPIO_PIN_1;
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
