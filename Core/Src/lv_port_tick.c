/**
 * @file lv_port_tick.c
 * LVGL v9.x Tick Port — 1 ms time base via FreeRTOS software timer
 *
 * METHOD B (task-based):
 *   Uses a FreeRTOS software timer that fires every 1 ms to call
 *   lv_tick_inc(1).  This is a convenient zero-intrusion solution
 *   during development.  The jitter is typically < 1%.
 *
 *   For release firmware, consider METHOD A instead:
 *   Simply add `lv_tick_inc(1)` inside the existing SysTick_Handler()
 *   or a hardware timer ISR, and remove this file from the build.
 *
 * Usage:
 *   lv_port_tick_init();  // Call once after lv_init() and scheduler start
 *
 * Dependencies:
 *   - FreeRTOS (configTICK_RATE_HZ should be 1000 for 1 ms granularity)
 */

#include "lv_port_tick.h"

/* FreeRTOS includes */
#include "FreeRTOS.h"
#include "timers.h"

/* ================================================================
 *  FreeRTOS software timer
 * ================================================================ */

/**
 * @brief  FreeRTOS timer callback — fires every 1 ms.
 *
 * Calls lv_tick_inc(1) to advance LVGL's internal millisecond counter.
 */
static void lv_tick_timer_cb(TimerHandle_t xTimer)
{
    (void)xTimer;
    lv_tick_inc(1);
}

/* ================================================================
 *  Public API
 * ================================================================ */

void lv_port_tick_init(void)
{
    /*
     * Create a FreeRTOS software timer with 1 ms period.
     *
     * pdMS_TO_TICKS(1) = 1 FreeRTOS tick when configTICK_RATE_HZ = 1000.
     * If configTICK_RATE_HZ is lower (e.g. 100), the timer granularity
     * will be coarser and LVGL timing accuracy will suffer.
     *
     * The timer is auto-reloading and starts immediately.
     */
    TimerHandle_t tick_timer = xTimerCreate(
        "lv_tick",                    /* timer name */
        pdMS_TO_TICKS(1),             /* period: 1 ms → 1 tick */
        pdTRUE,                       /* auto-reload */
        NULL,                         /* timer ID (unused) */
        lv_tick_timer_cb              /* callback */
    );

    if (tick_timer != NULL) {
        xTimerStart(tick_timer, 0);   /* start without block */
    }
    /* If tick_timer is NULL (out of memory), LVGL timers will not work.
     * configTIMER_TASK_STACK_DEPTH and configSUPPORT_DYNAMIC_ALLOCATION
     * must be set appropriately in FreeRTOSConfig.h. */
}
