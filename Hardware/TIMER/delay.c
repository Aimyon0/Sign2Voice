#include "delay.h"

static uint32_t fac_us = 0;  // CPU cycles per microsecond

void delay_init(uint16_t SYSCLK_MHz)
{
    /* Use DWT cycle counter instead of SysTick to avoid conflict with FreeRTOS */
    fac_us = SYSCLK_MHz;  // SYSCLK MHz = cycles per microsecond

    /* Enable DWT cycle counter (must unlock first on Cortex-M7) */
    CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
    DWT->LAR = 0xC5ACCE55;   /* unlock DWT registers */
    DWT->CYCCNT = 0;
    DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;
}

void delay_us(uint32_t nus)
{
    uint32_t start = DWT->CYCCNT;
    uint32_t ticks = nus * fac_us;
    while ((DWT->CYCCNT - start) < ticks) {
        /* busy wait */
    }
}

void delay_ms(uint16_t nms)
{
    while (nms--)
        delay_us(1000);
}
