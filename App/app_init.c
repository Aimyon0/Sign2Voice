/**
 * @file    app_init.c
 * @brief   Initialises all services: AI, display, camera, audio.
 */

#include "app_init.h"
#include "ai_service.h"
#include "ui_service.h"
#include "gesture_app.h"

#include "delay.h"
#include "led.h"
#include "key.h"
#include "LCD.h"
#include "ov2640.h"
#include "spi.h"

int app_init(void)
{
    /* 1. System helpers */
    LED_Init();
    KEY_Init();
    HAL_Delay(500);

    /* 2. LCD (must come before camera) */
    SPI2_SetSpeed(SPI_BAUDRATEPRESCALER_4);
    LCD_Init();

    /* 3. Camera sensor init (no streaming yet) */
    OV2640_Init();
    SPI2_SetSpeed(SPI_BAUDRATEPRESCALER_4);

    /* 4. LVGL + overlay (init before scheduler, tick in task) */
    ui_service_init();

    /* 5. Gesture app state */
    gesture_app_init();

    /* NOTE: audio_service_init() must be called from a task context
     *       (display_lcd1) because MP3_Init() uses osDelay(). */

    return 0;
}
