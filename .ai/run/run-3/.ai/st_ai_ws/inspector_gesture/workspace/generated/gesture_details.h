/**
  ******************************************************************************
  * @file    gesture.h
  * @date    2026-07-15T21:11:18+0800
  * @brief   ST.AI Tool Automatic Code Generator for Embedded NN computing
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  ******************************************************************************
  */
#ifndef STAI_GESTURE_DETAILS_H
#define STAI_GESTURE_DETAILS_H

#include "stai.h"
#include "layers.h"

const stai_network_details g_gesture_details = {
  .tensors = (const stai_tensor[14]) {
   { .size_bytes = 49152, .flags = (STAI_FLAG_HAS_BATCH|STAI_FLAG_CHANNEL_LAST), .format = STAI_FORMAT_FLOAT32, .shape = {4, (const int32_t[4]){1, 64, 64, 3}}, .scale = {0, NULL}, .zeropoint = {0, NULL}, .name = "serving_default_input_layer0_output" },
   { .size_bytes = 12289, .flags = (STAI_FLAG_HAS_BATCH|STAI_FLAG_CHANNEL_LAST), .format = STAI_FORMAT_S8, .shape = {4, (const int32_t[4]){1, 64, 64, 3}}, .scale = {1, (const float[1]){0.003921568859368563}}, .zeropoint = {1, (const int16_t[1]){-128}}, .name = "conversion_0_output" },
   { .size_bytes = 65536, .flags = (STAI_FLAG_HAS_BATCH|STAI_FLAG_CHANNEL_LAST), .format = STAI_FORMAT_S8, .shape = {4, (const int32_t[4]){1, 64, 64, 16}}, .scale = {1, (const float[1]){0.02114875055849552}}, .zeropoint = {1, (const int16_t[1]){-128}}, .name = "conv2d_1_output" },
   { .size_bytes = 16384, .flags = (STAI_FLAG_HAS_BATCH|STAI_FLAG_CHANNEL_LAST), .format = STAI_FORMAT_S8, .shape = {4, (const int32_t[4]){1, 32, 32, 16}}, .scale = {1, (const float[1]){0.02114875055849552}}, .zeropoint = {1, (const int16_t[1]){-128}}, .name = "pool_2_output" },
   { .size_bytes = 18496, .flags = (STAI_FLAG_HAS_BATCH|STAI_FLAG_CHANNEL_LAST), .format = STAI_FORMAT_S8, .shape = {4, (const int32_t[4]){1, 34, 34, 16}}, .scale = {1, (const float[1]){0.02114875055849552}}, .zeropoint = {1, (const int16_t[1]){-128}}, .name = "conv2d_3_pad_before_output" },
   { .size_bytes = 24576, .flags = (STAI_FLAG_HAS_BATCH|STAI_FLAG_CHANNEL_LAST), .format = STAI_FORMAT_S8, .shape = {4, (const int32_t[4]){1, 32, 32, 24}}, .scale = {1, (const float[1]){0.029933137819170952}}, .zeropoint = {1, (const int16_t[1]){-128}}, .name = "conv2d_3_output" },
   { .size_bytes = 6144, .flags = (STAI_FLAG_HAS_BATCH|STAI_FLAG_CHANNEL_LAST), .format = STAI_FORMAT_S8, .shape = {4, (const int32_t[4]){1, 16, 16, 24}}, .scale = {1, (const float[1]){0.029933137819170952}}, .zeropoint = {1, (const int16_t[1]){-128}}, .name = "pool_4_output" },
   { .size_bytes = 7776, .flags = (STAI_FLAG_HAS_BATCH|STAI_FLAG_CHANNEL_LAST), .format = STAI_FORMAT_S8, .shape = {4, (const int32_t[4]){1, 18, 18, 24}}, .scale = {1, (const float[1]){0.029933137819170952}}, .zeropoint = {1, (const int16_t[1]){-128}}, .name = "conv2d_5_pad_before_output" },
   { .size_bytes = 8192, .flags = (STAI_FLAG_HAS_BATCH|STAI_FLAG_CHANNEL_LAST), .format = STAI_FORMAT_S8, .shape = {4, (const int32_t[4]){1, 16, 16, 32}}, .scale = {1, (const float[1]){0.04613778367638588}}, .zeropoint = {1, (const int16_t[1]){-128}}, .name = "conv2d_5_output" },
   { .size_bytes = 2048, .flags = (STAI_FLAG_HAS_BATCH|STAI_FLAG_CHANNEL_LAST), .format = STAI_FORMAT_S8, .shape = {4, (const int32_t[4]){1, 8, 8, 32}}, .scale = {1, (const float[1]){0.04613778367638588}}, .zeropoint = {1, (const int16_t[1]){-128}}, .name = "pool_6_output" },
   { .size_bytes = 64, .flags = (STAI_FLAG_HAS_BATCH|STAI_FLAG_CHANNEL_LAST), .format = STAI_FORMAT_S8, .shape = {2, (const int32_t[2]){1, 64}}, .scale = {1, (const float[1]){0.1007503792643547}}, .zeropoint = {1, (const int16_t[1]){-128}}, .name = "gemm_11_output" },
   { .size_bytes = 5, .flags = (STAI_FLAG_HAS_BATCH|STAI_FLAG_CHANNEL_LAST), .format = STAI_FORMAT_S8, .shape = {2, (const int32_t[2]){1, 5}}, .scale = {1, (const float[1]){0.31296828389167786}}, .zeropoint = {1, (const int16_t[1]){-7}}, .name = "gemm_12_output" },
   { .size_bytes = 5, .flags = (STAI_FLAG_HAS_BATCH|STAI_FLAG_CHANNEL_LAST), .format = STAI_FORMAT_S8, .shape = {2, (const int32_t[2]){1, 5}}, .scale = {1, (const float[1]){0.00390625}}, .zeropoint = {1, (const int16_t[1]){-128}}, .name = "nl_13_output" },
   { .size_bytes = 20, .flags = (STAI_FLAG_HAS_BATCH|STAI_FLAG_CHANNEL_LAST), .format = STAI_FORMAT_FLOAT32, .shape = {2, (const int32_t[2]){1, 5}}, .scale = {0, NULL}, .zeropoint = {0, NULL}, .name = "conversion_14_output" }
  },
  .nodes = (const stai_node_details[13]){
    {.id = 0, .type = AI_LAYER_NL_TYPE, .input_tensors = {1, (const int32_t[1]){0}}, .output_tensors = {1, (const int32_t[1]){1}} }, /* conversion_0 */
    {.id = 1, .type = AI_LAYER_OPTIMIZED_CONV2D_TYPE, .input_tensors = {1, (const int32_t[1]){1}}, .output_tensors = {1, (const int32_t[1]){2}} }, /* conv2d_1 */
    {.id = 2, .type = AI_LAYER_POOL_TYPE, .input_tensors = {1, (const int32_t[1]){2}}, .output_tensors = {1, (const int32_t[1]){3}} }, /* pool_2 */
    {.id = 3, .type = AI_LAYER_PAD_TYPE, .input_tensors = {1, (const int32_t[1]){3}}, .output_tensors = {1, (const int32_t[1]){4}} }, /* conv2d_3_pad_before */
    {.id = 3, .type = AI_LAYER_CONV2D_TYPE, .input_tensors = {1, (const int32_t[1]){4}}, .output_tensors = {1, (const int32_t[1]){5}} }, /* conv2d_3 */
    {.id = 4, .type = AI_LAYER_POOL_TYPE, .input_tensors = {1, (const int32_t[1]){5}}, .output_tensors = {1, (const int32_t[1]){6}} }, /* pool_4 */
    {.id = 5, .type = AI_LAYER_PAD_TYPE, .input_tensors = {1, (const int32_t[1]){6}}, .output_tensors = {1, (const int32_t[1]){7}} }, /* conv2d_5_pad_before */
    {.id = 5, .type = AI_LAYER_CONV2D_TYPE, .input_tensors = {1, (const int32_t[1]){7}}, .output_tensors = {1, (const int32_t[1]){8}} }, /* conv2d_5 */
    {.id = 6, .type = AI_LAYER_POOL_TYPE, .input_tensors = {1, (const int32_t[1]){8}}, .output_tensors = {1, (const int32_t[1]){9}} }, /* pool_6 */
    {.id = 11, .type = AI_LAYER_DENSE_TYPE, .input_tensors = {1, (const int32_t[1]){9}}, .output_tensors = {1, (const int32_t[1]){10}} }, /* gemm_11 */
    {.id = 12, .type = AI_LAYER_DENSE_TYPE, .input_tensors = {1, (const int32_t[1]){10}}, .output_tensors = {1, (const int32_t[1]){11}} }, /* gemm_12 */
    {.id = 13, .type = AI_LAYER_SM_TYPE, .input_tensors = {1, (const int32_t[1]){11}}, .output_tensors = {1, (const int32_t[1]){12}} }, /* nl_13 */
    {.id = 14, .type = AI_LAYER_NL_TYPE, .input_tensors = {1, (const int32_t[1]){12}}, .output_tensors = {1, (const int32_t[1]){13}} } /* conversion_14 */
  },
  .n_nodes = 13
};
#endif

