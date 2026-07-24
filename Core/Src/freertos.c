/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : freertos.c
  * Description        : Code for freertos applications
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
#include "camera_service.h"
#include "ai_service.h"
#include "ui_service.h"
#include "audio_service.h"
#include "gesture_app.h"
#include "key.h"
#include "menu.h"
#include "lvgl.h"
#include "lv_port_tick.h"
#include "camera_band_refresh.h"
#include "config.h"
#include "watchdog.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
typedef enum { UI_STATE_READY, UI_STATE_INFER } ui_state_t;
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */
/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Variables */

/* ====== IPC: frameQueue (display → inference) ====== */
osMessageQDef(frameQueue, 4, uint32_t);
osMessageQId frameQueueHandle;

/* ====== IPC: resultQueue (inference → display) ====== */
typedef struct { int class; float prob; uint32_t inf_ms; } ui_event_t;
osMailQDef(resultQueue, 2, ui_event_t);
osMailQId  resultQueueHandle;

/* ====== UI state ====== */
static volatile ui_state_t g_ui_state = UI_STATE_READY;

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
  */
void MX_FREERTOS_Init(void) {
  /* USER CODE BEGIN Init */
  /* USER CODE END Init */

  /* USER CODE BEGIN RTOS_MUTEX */
  /* USER CODE END RTOS_MUTEX */

  /* Create the semaphores(s) */
  osSemaphoreDef(fimish);
  fimishHandle = osSemaphoreCreate(osSemaphore(fimish), 1);

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* USER CODE END RTOS_TIMERS */

  /* USER CODE BEGIN RTOS_QUEUES */
  frameQueueHandle  = osMessageCreate(osMessageQ(frameQueue), NULL);
  resultQueueHandle = osMailCreate(osMailQ(resultQueue), NULL);
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  osThreadDef(display_lcd, display_lcd1, osPriorityNormal, 0, 4096);
  display_lcdHandle = osThreadCreate(osThread(display_lcd), NULL);

  osThreadDef(inf, info, osPriorityNormal, 0, 2048);
  infHandle = osThreadCreate(osThread(inf), NULL);

  osThreadDef(mp3, out, osPriorityBelowNormal, 0, 1024);
  mp3Handle = osThreadCreate(osThread(mp3), NULL);

  /* USER CODE BEGIN RTOS_THREADS */
  /* USER CODE END RTOS_THREADS */
}

/* ====== display_lcd1 — Camera preview + UI + recognition orchestration ====== */
void display_lcd1(void const * argument)
{
  /* USER CODE BEGIN display_lcd1 */
    uint32_t fps_last = 0, fps_cnt = 0;
    uint32_t current_fps = 0, current_inf_ms = 0;

    /* Init LVGL tick (needs task context) */
    lv_port_tick_init();
    ui_service_set_fps("FPS --");
    ui_service_set_result("--");
    ui_service_set_confidence("--%");
    ui_service_set_status("Ready");
    ui_service_update();

    /* Init MP3 BEFORE menu (menu_init calls audio_service_set_volume) */
    audio_service_init();
    audio_service_set_volume(config_get_volume());

    menu_init();
    lv_timer_handler();
    osDelay(100);

    /* Start camera streaming + watchdog */
    watchdog_init();
    camera_service_start();

    /* Track menu→recognize transition for state reset */
    uint8_t _was_recog = 0;

    for (;;)
    {
        /* ── Menu state transition: reset sliding window ── */
        uint8_t _now_recog = menu_is_recognize();
        if (_now_recog && !_was_recog) {
            gesture_app_reset();
        }
        _was_recog = _now_recog;

        /* ── Camera frame ready? ── */
        if (menu_is_recognize() && camera_service_frame_ready()) {
            camera_service_ack_frame();
            camera_service_invalidate_cache();
            camera_band_refresh();

            /* FPS counter */
            fps_cnt++;
            if (HAL_GetTick() - fps_last >= 1000) {
                current_fps = fps_cnt;
                fps_last = HAL_GetTick();
                fps_cnt   = 0;
                menu_set_fps(current_fps);
                char b[16]; snprintf(b, sizeof(b), "FPS %lu", current_fps);
                ui_service_set_fps(b);
            }

            /* Trigger inference if idle */
            if (g_ui_state == UI_STATE_READY) {
                osMessagePut(frameQueueHandle, 1, 0);
            }
        }

        /* ── Inference result ready? ── */
        if (menu_is_recognize()) {
            osEvent evt = osMailGet(resultQueueHandle, 0);
            if (evt.status == osEventMail) {
                ui_event_t *e = (ui_event_t *)evt.value.p;
                SCB_InvalidateDCache_by_Addr((uint32_t *)e, sizeof(ui_event_t));

                current_inf_ms = e->inf_ms;

                gesture_app_on_result(e->class, e->prob, e->inf_ms);

                osMailFree(resultQueueHandle, e);
                g_ui_state = UI_STATE_READY;
            }
        }

        /* Update menu stats every cycle */
        menu_set_stats(current_fps, current_inf_ms, xPortGetFreeHeapSize());

        /* UI state display */
        {
            static ui_state_t _prev = (ui_state_t)-1;
            if (g_ui_state != _prev) {
                _prev = g_ui_state;
                if (menu_is_recognize() && g_ui_state == UI_STATE_INFER)
                    ui_service_set_status("Inferencing...");
            }
        }

        /* Key scan (50ms debounce) */
        {
            static uint32_t _kt;
            if (HAL_GetTick() - _kt >= 50) {
                _kt = HAL_GetTick();
                menu_process_key(KEY_Scan(0));
            }
        }

        /* LVGL refresh */
        if (menu_is_recognize()) ui_service_update();
        lv_timer_handler();

        /* Feed watchdog (~500 Hz, timeout 4 s) */
        watchdog_feed();

        osDelay(2);
    }
  /* USER CODE END display_lcd1 */
}

/* ====== info — Inference worker task ====== */
void info(void const * argument)
{
  /* USER CODE BEGIN info */
    for (;;)
    {
        /* Block until a frame is ready */
        uint32_t _d;
        xQueueReceive((QueueHandle_t)frameQueueHandle, &_d, portMAX_DELAY);

        g_ui_state = UI_STATE_INFER;

        camera_service_invalidate_cache();

        /* Preprocess: RGB565 → 64×64 RGB888 float */
        camera_service_get_rgb888(ai_service_get_input_buf());

        /* Run inference */
        uint32_t t0 = HAL_GetTick();
        ai_service_run();
        uint32_t elapsed = HAL_GetTick() - t0;

        /* Post result to display task */
        ui_event_t *e = osMailAlloc(resultQueueHandle, osWaitForever);
        if (e) {
            e->class  = ai_service_get_class();
            e->prob   = ai_service_get_prob();
            e->inf_ms = elapsed;
            osMailPut(resultQueueHandle, e);
        }
    }
  /* USER CODE END info */
}

/* ====== out — MP3 idle task (playback driven by display task) ====== */
void out(void const * argument)
{
  /* USER CODE BEGIN out */
    for(;;) { osDelay(1000); }
  /* USER CODE END out */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */
/* USER CODE END Application */
