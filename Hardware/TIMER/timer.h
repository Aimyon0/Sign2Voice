#ifndef _TIMER_H
#define _TIMER_H
#include "system.h"
#include "stm32h7xx_hal.h"
extern TIM_HandleTypeDef TIM3_Handler;      //定时器3句柄 
extern TIM_HandleTypeDef TIM6_Handler;      //定时器6句柄 

void TIM3_Init(u16 arr,u16 psc);
void TIM6_Init(u16 arr,u16 psc);

void TIM3_Start(void);
void TIM3_Stop(void);
u32 GetTicknum(void);
#endif























