
/**
  ******************************************************************************
  * @file    app_x-cube-ai.c
  * @author  X-CUBE-AI C code generator
  * @brief   AI program body
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

 /*
  * Description
  *   v1.0 - Minimum template to show how to use the Embedded Client API
  *          model. Only one input and one output is supported. All
  *          memory resources are allocated statically (AI_NETWORK_XX, defines
  *          are used).
  *          Re-target of the printf function is out-of-scope.
  *   v2.0 - add multiple IO and/or multiple heap support
  *
  *   For more information, see the embeded documentation:
  *
  *       [1] %X_CUBE_AI_DIR%/Documentation/index.html
  *
  *   X_CUBE_AI_DIR indicates the location where the X-CUBE-AI pack is installed
  *   typical : C:\Users\[user_name]\STM32Cube\Repository\STMicroelectronics\X-CUBE-AI\7.1.0
  */

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/

#if defined ( __ICCARM__ )
#elif defined ( __CC_ARM ) || ( __GNUC__ )
#endif

/* System headers */
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <inttypes.h>
#include <string.h>

#include "app_x-cube-ai.h"
#include "main.h"
#include "ai_datatypes_defines.h"
#include "gesture.h"
#include "gesture_data.h"

/* USER CODE BEGIN includes */
#include <stdio.h>
__attribute__((section("AI_RAM")))
float ai_input_data[64*64*3] __attribute__((aligned(32)));
/* USER CODE END includes */

/* IO buffers ----------------------------------------------------------------*/

#if !defined(AI_GESTURE_INPUTS_IN_ACTIVATIONS)
AI_ALIGNED(4) ai_i8 data_in_1[AI_GESTURE_IN_1_SIZE_BYTES];
ai_i8* data_ins[AI_GESTURE_IN_NUM] = {
data_in_1
};
#else
ai_i8* data_ins[AI_GESTURE_IN_NUM] = {
NULL
};
#endif

#if !defined(AI_GESTURE_OUTPUTS_IN_ACTIVATIONS)
AI_ALIGNED(4) ai_i8 data_out_1[AI_GESTURE_OUT_1_SIZE_BYTES];
ai_i8* data_outs[AI_GESTURE_OUT_NUM] = {
data_out_1
};
#else
ai_i8* data_outs[AI_GESTURE_OUT_NUM] = {
NULL
};
#endif

/* Activations buffers -------------------------------------------------------*/

AI_ALIGNED(32)
static uint8_t pool0[AI_GESTURE_DATA_ACTIVATION_1_SIZE];

ai_handle data_activations0[] = {pool0};

/* AI objects ----------------------------------------------------------------*/

static ai_handle gesture_finetuned = AI_HANDLE_NULL;

 ai_buffer* ai_input;
 ai_buffer* ai_output;

static void ai_log_err(const ai_error err, const char *fct)
{
  if (fct)
    printf("TEMPLATE - Error (%s) - type=0x%02x code=0x%02x\r\n", fct,
        err.type, err.code);
  else
    printf("TEMPLATE - Error - type=0x%02x code=0x%02x\r\n", err.type, err.code);
  do {} while (1);
}

static int ai_boostrap(ai_handle *act_addr)
{
  ai_error err;
  err = ai_gesture_create_and_init(&gesture_finetuned, act_addr, NULL);
  if (err.type != AI_ERROR_NONE) {
    ai_log_err(err, "ai_gesture_create_and_init");
    return -1;
  }
  ai_input = ai_gesture_inputs_get(gesture_finetuned, NULL);
  ai_output = ai_gesture_outputs_get(gesture_finetuned, NULL);
#if defined(AI_GESTURE_INPUTS_IN_ACTIVATIONS)
  for (int idx=0; idx < AI_GESTURE_IN_NUM; idx++) {
	data_ins[idx] = ai_input[idx].data;
  }
#else
  for (int idx=0; idx < AI_GESTURE_IN_NUM; idx++) {
	  ai_input[idx].data = data_ins[idx];
  }
#endif
#if defined(AI_GESTURE_OUTPUTS_IN_ACTIVATIONS)
  for (int idx=0; idx < AI_GESTURE_OUT_NUM; idx++) {
	data_outs[idx] = ai_output[idx].data;
  }
#else
  for (int idx=0; idx < AI_GESTURE_OUT_NUM; idx++) {
	ai_output[idx].data = data_outs[idx];
  }
#endif
  return 0;
}

static int ai_run(void)
{
  ai_i32 batch;
  batch = ai_gesture_run(gesture_finetuned, ai_input, ai_output);
  if (batch != 1) {
    ai_log_err(ai_gesture_get_error(gesture_finetuned),
        "ai_gesture_run");
    return -1;
  }
  return 0;
}

/* USER CODE BEGIN 2 */
#define THRESHOLD 0.1f

int acquire_and_process_data(ai_i8* data[])
{
    data[0] = (ai_i8*)ai_input_data;
    return 0;
}

int post_process(ai_i8* data[])
{
    float* output = (float*)data[0];
    const int class_num = 5;

    int max_index = 0;
    float max_prob = output[0];

    for (int i = 1; i < class_num; ++i) {
        if (output[i] > max_prob) {
            max_prob = output[i];
            max_index = i;
        }
    }

    if (max_prob < 0.0f) max_prob = 0.0f;
    if (max_prob > 1.0f) max_prob = 1.0f;

    g_predicted_prob = max_prob;
    g_predicted_class = max_index;

    return 0;
}
  /* USER CODE END 2 */

/* Global variables -----------------------------------------------------------*/
const char* class_names[] = {
    "fist", "like", "no_gesture", "ok", "palm"
};

int g_predicted_class = -1;
float g_predicted_prob = 0.0f;

/* Entry points --------------------------------------------------------------*/

void MX_X_CUBE_AI_Init(void)
{
        /* USER CODE BEGIN 5 */
  ai_boostrap(data_activations0);
    /* USER CODE END 5 */
}

void MX_X_CUBE_AI_Process(void)
{
        /* USER CODE BEGIN 6 */
    int res = -1;
    if (gesture_finetuned) {
        int count = 0;
        do {
            res = acquire_and_process_data(data_ins);
            if (res != 0) break;
            for (int i = 0; i < AI_GESTURE_IN_NUM; i++) {
                ai_input[i].data = (void*)data_ins[i];
            }
            for (int i = 0; i < AI_GESTURE_OUT_NUM; i++) {
                ai_output[i].data = data_outs[i];
            }
            res = ai_run();
            if (res != 0) break;
            res = post_process(data_outs);
            count++;
            if (count > 1) break;
        } while (res == 0);
        if (res != 0) {
            ai_error err = {AI_ERROR_INVALID_STATE, AI_ERROR_CODE_NETWORK};
            ai_log_err(err, "Process has FAILED");
        }
    } else {
        ai_error err = {AI_ERROR_INVALID_HANDLE, AI_ERROR_CODE_NETWORK};
        ai_log_err(err, "model is NULL");
    }
    /* USER CODE END 6 */
}
#ifdef __cplusplus
}
#endif
