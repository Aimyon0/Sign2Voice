/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : freertos.c
  * Description        : Code for freertos applications
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
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
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdio.h>
#include <string.h>
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
#include "lvgl.h"
#include "lv_port_tick.h"
#include "ui_overlay.h"
#include "camera_band_refresh.h"
#include "mp3.h"
#include "menu.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Variables */

/* ====== �???易状�??? ====== */
typedef enum { UI_STATE_READY, UI_STATE_INFER } ui_state_t;
static volatile ui_state_t g_ui_state = UI_STATE_READY;
extern u8 OV_mode;

/* ====== 帧事件队�?????: display �????? info ====== */
osMessageQDef(frameQueue, 4, uint32_t);
osMessageQId frameQueueHandle;

/* ====== 结果事件: info �????? display ====== */
typedef struct { int class; float prob; uint32_t inf_ms; } ui_event_t;
osMailQDef(resultQueue, 2, ui_event_t);
osMailQId  resultQueueHandle;

/* USER CODE END Variables */
osThreadId display_lcdHandle;
osThreadId infHandle;
osThreadId mp3Handle;
osSemaphoreId fimishHandle;

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */

/* USER CODE END FunctionPrototypes */

void display_lcd1(void const * argument);
void info(void const * argument);
void out(void const * argument);

void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/* GetIdleTaskMemory prototype (linked to static allocation support) */
void vApplicationGetIdleTaskMemory( StaticTask_t **ppxIdleTaskTCBBuffer, StackType_t **ppxIdleTaskStackBuffer, uint32_t *pulIdleTaskStackSize );

/* USER CODE BEGIN GET_IDLE_TASK_MEMORY */
static StaticTask_t xIdleTaskTCBBuffer;
static StackType_t xIdleStack[configMINIMAL_STACK_SIZE];

void vApplicationGetIdleTaskMemory( StaticTask_t **ppxIdleTaskTCBBuffer, StackType_t **ppxIdleTaskStackBuffer, uint32_t *pulIdleTaskStackSize )
{
  *ppxIdleTaskTCBBuffer = &xIdleTaskTCBBuffer;
  *ppxIdleTaskStackBuffer = &xIdleStack[0];
  *pulIdleTaskStackSize = configMINIMAL_STACK_SIZE;
  /* place for user code */
}

void vApplicationGetTimerTaskMemory( StaticTask_t **ppxTimerTaskTCBBuffer, StackType_t **ppxTimerTaskStackBuffer, uint32_t *pulTimerTaskStackSize )
{
  static StaticTask_t xTimerTaskTCBBuffer;
  static StackType_t xTimerTaskStack[configTIMER_TASK_STACK_DEPTH];
  *ppxTimerTaskTCBBuffer = &xTimerTaskTCBBuffer;
  *ppxTimerTaskStackBuffer = xTimerTaskStack;
  *pulTimerTaskStackSize = configTIMER_TASK_STACK_DEPTH;
}
/* USER CODE END GET_IDLE_TASK_MEMORY */

void vApplicationStackOverflowHook(TaskHandle_t xTask, char *pcTaskName)
{
    taskDISABLE_INTERRUPTS();
    for(;;);
}

/**
  * @brief  FreeRTOS initialization
  * @param  None
  * @retval None
  */
void MX_FREERTOS_Init(void) {
  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* Create the semaphores(s) */
  /* definition and creation of fimish */
  osSemaphoreDef(fimish);
  fimishHandle = osSemaphoreCreate(osSemaphore(fimish), 1);

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* USER CODE BEGIN RTOS_QUEUES */
  frameQueueHandle  = osMessageCreate(osMessageQ(frameQueue), NULL);
  resultQueueHandle = osMailCreate(osMailQ(resultQueue), NULL);
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* definition and creation of display_lcd */
  osThreadDef(display_lcd, display_lcd1, osPriorityNormal, 0, 4096);
  display_lcdHandle = osThreadCreate(osThread(display_lcd), NULL);

  /* definition and creation of inf */
  osThreadDef(inf, info, osPriorityNormal, 0, 2048);
  infHandle = osThreadCreate(osThread(inf), NULL);

  /* definition and creation of mp3 */
  osThreadDef(mp3, out, osPriorityBelowNormal, 0, 1024);
  mp3Handle = osThreadCreate(osThread(mp3), NULL);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

}

/* USER CODE BEGIN Header_display_lcd1 */
/**
  * @brief  Function implementing the display_lcd thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_display_lcd1 */
void display_lcd1(void const * argument)
{
  /* USER CODE BEGIN display_lcd1 */

    uint32_t fps_last = 0, fps_cnt = 0;
    static uint32_t current_fps = 0, current_inf_ms = 0;

    lv_port_tick_init();
    ui_overlay_set_fps("FPS --");
    ui_overlay_set_result("--");
    ui_overlay_set_confidence("--%");
    ui_overlay_set_status("Ready");
    ui_overlay_update();
    menu_init();
    lv_timer_handler();
    osDelay(100);

    MP3_Init();
    MP3_SetVolume(20);

    OV2640_RGB565_Mode();
    DCMI_Init();
    dcmi_rx_callback = rgblcd_dcmi_rx_callback;
    DCMI_DMA_Init((u32)RGB_Line_Buf[0],(u32)RGB_Line_Buf[1],lcddev.width/2,DMA_MDATAALIGN_HALFWORD,DMA_MINC_ENABLE);
    OV2640_OutSize_Set(lcddev.width, lcddev.height);
    DCMI_Start();

    /* sliding-window state (moved to function scope for menu-return reset) */
    static int sw_last = -1, sw_cnt = 0, last_play = -1;

    for (;;)
    {
        /* reset sliding-window when returning from menu to recognize */
        static uint8_t _was_recog = 0;
        uint8_t _now_recog = menu_is_recognize();
        if (_now_recog && !_was_recog) { sw_last = -1; sw_cnt = 0; last_play = -1; }
        _was_recog = _now_recog;

        if (menu_is_recognize() && RGB_FrameNum > 0)
        {
            RGB_FrameNum = 0;
            SCB_InvalidateDCache_by_Addr((uint32_t *)RGB_DATA, sizeof(RGB_DATA));
            camera_band_refresh();

            fps_cnt++;
            if (HAL_GetTick() - fps_last >= 1000) {
                current_fps = fps_cnt;  /* latch stable value BEFORE zeroing */
                fps_last = HAL_GetTick(); fps_cnt = 0;
                menu_set_fps(current_fps);
                char b[16]; snprintf(b, sizeof(b), "FPS %lu", current_fps);
                ui_overlay_set_fps(b);
            }

            if (g_ui_state == UI_STATE_READY) osMessagePut(frameQueueHandle, 1, 0);
        }

        if (menu_is_recognize()) {
            osEvent evt = osMailGet(resultQueueHandle, 0);
            if (evt.status == osEventMail) {
                ui_event_t *e = (ui_event_t *)evt.value.p;
                
                /* 【新增�?�极其关键：防止 H7 �??? D-Cache 缓存导致读取到第�???帧的老数�??? */
                SCB_InvalidateDCache_by_Addr((uint32_t *)e, sizeof(ui_event_t));

                int cls = e->class;

                /* 滑窗: N 帧同类才确认，低置信度帧跳过不参与 */
                int sw_n = menu_get_brightness();
                if (e->prob >= (menu_get_threshold() / 100.0f)) {
                    if (cls == sw_last) sw_cnt++; else { sw_last = cls; sw_cnt = 1; }
                }

                if (sw_cnt >= sw_n) {
                    if (cls >= 0 && cls < 5) {
                        extern const char *class_names[];
                        ui_overlay_set_result(class_names[cls]);
                    }
                    char c[16]; snprintf(c, sizeof(c), "%.0f%%", e->prob * 100.0f);
                    ui_overlay_set_confidence(c);
                    ui_overlay_set_status("Detected");
                    sw_cnt = 0; sw_last = -1;

                    static const int cls2track[5] = {1, 2, -1, 3, 4};
                    int track = (cls >= 0 && cls < 5) ? cls2track[cls] : -1;
                    if (track > 0 && track != last_play) { last_play = track; MP3_PlayIndex(track); }
                    else if (track < 0) last_play = -1;
                }

                current_inf_ms = e->inf_ms;
                osMailFree(resultQueueHandle, e);
                g_ui_state = UI_STATE_READY;
              }
            /* 每轮用锁存�?�更�??? stats, 不受 Mail 频率影响 */
            menu_set_stats(current_fps, current_inf_ms, xPortGetFreeHeapSize());
          }

          { static ui_state_t _prev = -1;
          if (g_ui_state != _prev) { _prev = g_ui_state;
            if (menu_is_recognize() && g_ui_state == UI_STATE_INFER)
                ui_overlay_set_status("Inferencing..."); } }

        static uint32_t _kt;
        if (HAL_GetTick() - _kt >= 50) { _kt = HAL_GetTick(); menu_process_key(KEY_Scan(0)); }

        if (menu_is_recognize()) ui_overlay_update();
        lv_timer_handler();
        osDelay(2);
    }

  /* USER CODE END display_lcd1 */
}

/* USER CODE BEGIN Header_info */
/**
* @brief Function implementing the inf thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_info */
void info(void const * argument)
{
  /* USER CODE BEGIN info */
//  printf("info start\r\n");
    for (;;)
    {
        /* 阻塞等待帧事�????? */
        uint32_t _d;
        xQueueReceive((QueueHandle_t)frameQueueHandle, &_d, portMAX_DELAY);

        g_ui_state = UI_STATE_INFER;

        SCB_InvalidateDCache_by_Addr(
            (uint32_t *)RGB_DATA,
            sizeof(RGB_DATA)
        );

        {
            uint32_t t0 = HAL_GetTick();
            RGB565_To_64x64_RGB888_Float(
                &RGB_DATA[0][0],
                lcddev.width,
                lcddev.height,
                ai_input_data
            );

            MX_X_CUBE_AI_Process();
            uint32_t elapsed = HAL_GetTick() - t0;

            extern float g_predicted_prob;
            ui_event_t *e = osMailAlloc(resultQueueHandle, osWaitForever);
            if (e) {
                e->class  = g_predicted_class;
                e->prob   = g_predicted_prob;
                e->inf_ms = elapsed;
                osMailPut(resultQueueHandle, e);
            }
        }
    }

  /* USER CODE END info */
}

/* USER CODE BEGIN Header_out */
/**
* @brief Function implementing the mp3 thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_out */
void out(void const * argument)
{
  /* USER CODE BEGIN out */
//    printf("out start (mp3 disabled)\r\n");

    //MP3_Init();
    //MP3_SetVolume(10);
   // MP3_PlayIndex(5);

    for(;;) { osDelay(1000); }
  /* USER CODE END out */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */

/* USER CODE END Application */
