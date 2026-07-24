/**
 * watchdog.c — IWDG via direct register access.
 * STM32H743 LSI ~32kHz, prescaler /256 → 8ms/tick, reload 500 → 4s timeout.
 * Must be called from task context (LSI needs time to stabilize after boot).
 */
#include "watchdog.h"
#include "stm32h7xx.h"

#define IWDG_KR  (*(volatile uint32_t*)0x58004C00U)
#define IWDG_PR  (*(volatile uint32_t*)0x58004C04U)
#define IWDG_RLR (*(volatile uint32_t*)0x58004C08U)
#define IWDG_SR  (*(volatile uint32_t*)0x58004C0CU)

#define KR_UNLOCK  0x5555U
#define KR_FEED    0xAAAAU
#define KR_START   0xCCCCU
#define PR_DIV256  0x06U
#define RLR_4S     500U

ErrorCode watchdog_init(void)
{
    uint32_t tout;

    IWDG_KR = KR_UNLOCK;

    /* Wait for prescaler update ready */
    tout = 100000U;
    while (IWDG_SR & 1U) if (--tout == 0) return ERR_TIMEOUT;
    IWDG_PR = PR_DIV256;

    /* Wait for reload update ready */
    tout = 100000U;
    while (IWDG_SR & 2U) if (--tout == 0) return ERR_TIMEOUT;
    IWDG_RLR = RLR_4S;

    IWDG_KR = KR_START;
    return ERR_OK;
}

void watchdog_feed(void)
{
    IWDG_KR = KR_FEED;
}
