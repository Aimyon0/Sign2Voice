/**
 * @file lv_port_tick.h
 * LVGL v9.x Tick Port for STM32H743 + FreeRTOS
 *
 * Provides the 1 ms time base that LVGL requires for animations,
 * timers, and the internal scheduler.
 *
 * Two methods are offered:
 *   A) ISR-based (recommended, high precision):
 *      Place lv_tick_inc(1) in a 1 ms interrupt, e.g. SysTick_Handler
 *      or a dedicated hardware timer (TIM6/TIM7).
 *
 *   B) Task-based (simpler, no ISR changes):
 *      Call lv_port_tick_init() to create a FreeRTOS software timer
 *      that fires at 1 kHz.  Slightly less precise but requires zero
 *      changes to existing ISR code.
 */

#ifndef LV_PORT_TICK_H
#define LV_PORT_TICK_H

#ifdef __cplusplus
extern "C" {
#endif

#include "lvgl.h"

/**
 * @brief  Start the LVGL 1 ms tick source (FreeRTOS software timer).
 *
 * @note   This is method B — a FreeRTOS software timer calls
 *         lv_tick_inc(1) every 1 ms.  It requires the FreeRTOS
 *         scheduler to be running.
 *
 *         For production, prefer method A: add lv_tick_inc(1)
 *         directly inside SysTick_Handler() or a hardware timer ISR,
 *         and do NOT call this function.
 */
void lv_port_tick_init(void);

#ifdef __cplusplus
}
#endif

#endif /* LV_PORT_TICK_H */
