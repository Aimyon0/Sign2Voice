/**
  ******************************************************************************
  * @file    gesture.c
  * @author  AST Embedded Analytics Research Platform
  * @date    2026-07-24T21:07:09+0800
  * @brief   AI Tool Automatic Code Generator for Embedded NN computing
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


#include "gesture.h"
#include "gesture_data.h"

#include "ai_platform.h"
#include "ai_platform_interface.h"
#include "ai_math_helpers.h"

#include "core_common.h"
#include "core_convert.h"

#include "layers.h"



#undef AI_NET_OBJ_INSTANCE
#define AI_NET_OBJ_INSTANCE g_gesture
 
#undef AI_GESTURE_MODEL_SIGNATURE
#define AI_GESTURE_MODEL_SIGNATURE     "0x5f3c978ced3b7aff0142c989c3b31656"

#ifndef AI_TOOLS_REVISION_ID
#define AI_TOOLS_REVISION_ID     ""
#endif

#undef AI_TOOLS_DATE_TIME
#define AI_TOOLS_DATE_TIME   "2026-07-24T21:07:09+0800"

#undef AI_TOOLS_COMPILE_TIME
#define AI_TOOLS_COMPILE_TIME    __DATE__ " " __TIME__

#undef AI_GESTURE_N_BATCHES
#define AI_GESTURE_N_BATCHES         (1)

static ai_ptr g_gesture_activations_map[1] = AI_C_ARRAY_INIT;
static ai_ptr g_gesture_weights_map[1] = AI_C_ARRAY_INIT;



/**  Array declarations section  **********************************************/
/* Array#0 */
AI_ARRAY_OBJ_DECLARE(
  serving_default_input_layer0_output_array, AI_ARRAY_FORMAT_FLOAT|AI_FMT_FLAG_IS_IO,
  NULL, NULL, 12288, AI_STATIC)

/* Array#1 */
AI_ARRAY_OBJ_DECLARE(
  conversion_0_output_array, AI_ARRAY_FORMAT_S8,
  NULL, NULL, 12289, AI_STATIC)

/* Array#2 */
AI_ARRAY_OBJ_DECLARE(
  conv2d_1_output_array, AI_ARRAY_FORMAT_S8,
  NULL, NULL, 65536, AI_STATIC)

/* Array#3 */
AI_ARRAY_OBJ_DECLARE(
  pool_2_output_array, AI_ARRAY_FORMAT_S8,
  NULL, NULL, 16384, AI_STATIC)

/* Array#4 */
AI_ARRAY_OBJ_DECLARE(
  conv2d_3_pad_before_output_array, AI_ARRAY_FORMAT_S8,
  NULL, NULL, 18496, AI_STATIC)

/* Array#5 */
AI_ARRAY_OBJ_DECLARE(
  conv2d_3_output_array, AI_ARRAY_FORMAT_S8,
  NULL, NULL, 24576, AI_STATIC)

/* Array#6 */
AI_ARRAY_OBJ_DECLARE(
  pool_4_output_array, AI_ARRAY_FORMAT_S8,
  NULL, NULL, 6144, AI_STATIC)

/* Array#7 */
AI_ARRAY_OBJ_DECLARE(
  conv2d_5_pad_before_output_array, AI_ARRAY_FORMAT_S8,
  NULL, NULL, 7776, AI_STATIC)

/* Array#8 */
AI_ARRAY_OBJ_DECLARE(
  conv2d_5_output_array, AI_ARRAY_FORMAT_S8,
  NULL, NULL, 8192, AI_STATIC)

/* Array#9 */
AI_ARRAY_OBJ_DECLARE(
  pool_6_output_array, AI_ARRAY_FORMAT_S8,
  NULL, NULL, 2048, AI_STATIC)

/* Array#10 */
AI_ARRAY_OBJ_DECLARE(
  gemm_11_output_array, AI_ARRAY_FORMAT_S8,
  NULL, NULL, 64, AI_STATIC)

/* Array#11 */
AI_ARRAY_OBJ_DECLARE(
  gemm_12_output_array, AI_ARRAY_FORMAT_S8,
  NULL, NULL, 5, AI_STATIC)

/* Array#12 */
AI_ARRAY_OBJ_DECLARE(
  nl_13_output_array, AI_ARRAY_FORMAT_S8,
  NULL, NULL, 5, AI_STATIC)

/* Array#13 */
AI_ARRAY_OBJ_DECLARE(
  conversion_14_output_array, AI_ARRAY_FORMAT_FLOAT|AI_FMT_FLAG_IS_IO,
  NULL, NULL, 5, AI_STATIC)

/* Array#14 */
AI_ARRAY_OBJ_DECLARE(
  conv2d_1_weights_array, AI_ARRAY_FORMAT_S8,
  NULL, NULL, 432, AI_STATIC)

/* Array#15 */
AI_ARRAY_OBJ_DECLARE(
  conv2d_1_bias_array, AI_ARRAY_FORMAT_S32,
  NULL, NULL, 16, AI_STATIC)

/* Array#16 */
AI_ARRAY_OBJ_DECLARE(
  conv2d_3_weights_array, AI_ARRAY_FORMAT_S8,
  NULL, NULL, 3456, AI_STATIC)

/* Array#17 */
AI_ARRAY_OBJ_DECLARE(
  conv2d_3_bias_array, AI_ARRAY_FORMAT_S32,
  NULL, NULL, 24, AI_STATIC)

/* Array#18 */
AI_ARRAY_OBJ_DECLARE(
  conv2d_5_weights_array, AI_ARRAY_FORMAT_S8,
  NULL, NULL, 6912, AI_STATIC)

/* Array#19 */
AI_ARRAY_OBJ_DECLARE(
  conv2d_5_bias_array, AI_ARRAY_FORMAT_S32,
  NULL, NULL, 32, AI_STATIC)

/* Array#20 */
AI_ARRAY_OBJ_DECLARE(
  gemm_11_weights_array, AI_ARRAY_FORMAT_S8,
  NULL, NULL, 131072, AI_STATIC)

/* Array#21 */
AI_ARRAY_OBJ_DECLARE(
  gemm_11_bias_array, AI_ARRAY_FORMAT_S32,
  NULL, NULL, 64, AI_STATIC)

/* Array#22 */
AI_ARRAY_OBJ_DECLARE(
  gemm_12_weights_array, AI_ARRAY_FORMAT_S8,
  NULL, NULL, 320, AI_STATIC)

/* Array#23 */
AI_ARRAY_OBJ_DECLARE(
  gemm_12_bias_array, AI_ARRAY_FORMAT_S32,
  NULL, NULL, 5, AI_STATIC)

/* Array#24 */
AI_ARRAY_OBJ_DECLARE(
  conv2d_1_scratch0_array, AI_ARRAY_FORMAT_S8,
  NULL, NULL, 1196, AI_STATIC)

/* Array#25 */
AI_ARRAY_OBJ_DECLARE(
  conv2d_3_scratch0_array, AI_ARRAY_FORMAT_S8,
  NULL, NULL, 6032, AI_STATIC)

/* Array#26 */
AI_ARRAY_OBJ_DECLARE(
  conv2d_5_scratch0_array, AI_ARRAY_FORMAT_S8,
  NULL, NULL, 6432, AI_STATIC)

/* Array#27 */
AI_ARRAY_OBJ_DECLARE(
  gemm_11_scratch0_array, AI_ARRAY_FORMAT_S16,
  NULL, NULL, 2368, AI_STATIC)

/* Array#28 */
AI_ARRAY_OBJ_DECLARE(
  gemm_12_scratch0_array, AI_ARRAY_FORMAT_S16,
  NULL, NULL, 89, AI_STATIC)

/* Array#29 */
AI_ARRAY_OBJ_DECLARE(
  nl_13_scratch0_array, AI_ARRAY_FORMAT_S32,
  NULL, NULL, 62, AI_STATIC)

/**  Array metadata declarations section  *************************************/
/* Int quant #0 */
AI_INTQ_INFO_LIST_OBJ_DECLARE(conv2d_1_output_array_intq, AI_STATIC_CONST,
  AI_BUFFER_META_FLAG_SCALE_FLOAT|AI_BUFFER_META_FLAG_ZEROPOINT_S8, 1,
  AI_PACK_INTQ_INFO(
    AI_PACK_INTQ_SCALE(0.02114875055849552f),
    AI_PACK_INTQ_ZP(-128)))

/* Int quant #1 */
AI_INTQ_INFO_LIST_OBJ_DECLARE(conv2d_1_weights_array_intq, AI_STATIC_CONST,
  AI_BUFFER_META_FLAG_SCALE_FLOAT|AI_BUFFER_META_FLAG_ZEROPOINT_S8, 16,
  AI_PACK_INTQ_INFO(
    AI_PACK_INTQ_SCALE(0.0021109613589942455f, 0.0027038156986236572f, 0.0022045052610337734f, 0.001900940085761249f, 0.002309046685695648f, 0.001947773271240294f, 0.0020833327434957027f, 0.002225436270236969f, 0.0017966980813071132f, 0.00247749756090343f, 0.0018235002644360065f, 0.002033183816820383f, 0.00396759994328022f, 0.002051991643384099f, 0.0025274946819990873f, 0.0014128254260867834f),
    AI_PACK_INTQ_ZP(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)))

/* Int quant #2 */
AI_INTQ_INFO_LIST_OBJ_DECLARE(conv2d_3_output_array_intq, AI_STATIC_CONST,
  AI_BUFFER_META_FLAG_SCALE_FLOAT|AI_BUFFER_META_FLAG_ZEROPOINT_S8, 1,
  AI_PACK_INTQ_INFO(
    AI_PACK_INTQ_SCALE(0.029933137819170952f),
    AI_PACK_INTQ_ZP(-128)))

/* Int quant #3 */
AI_INTQ_INFO_LIST_OBJ_DECLARE(conv2d_3_pad_before_output_array_intq, AI_STATIC_CONST,
  AI_BUFFER_META_FLAG_SCALE_FLOAT|AI_BUFFER_META_FLAG_ZEROPOINT_S8, 1,
  AI_PACK_INTQ_INFO(
    AI_PACK_INTQ_SCALE(0.02114875055849552f),
    AI_PACK_INTQ_ZP(-128)))

/* Int quant #4 */
AI_INTQ_INFO_LIST_OBJ_DECLARE(conv2d_3_weights_array_intq, AI_STATIC_CONST,
  AI_BUFFER_META_FLAG_SCALE_FLOAT|AI_BUFFER_META_FLAG_ZEROPOINT_S8, 24,
  AI_PACK_INTQ_INFO(
    AI_PACK_INTQ_SCALE(0.0018781435210257769f, 0.002203844953328371f, 0.0021182133350521326f, 0.0025331887882202864f, 0.0032312741968780756f, 0.003008316271007061f, 0.0022345467004925013f, 0.003500864841043949f, 0.00262173218652606f, 0.002470288658514619f, 0.002733913017436862f, 0.003078187583014369f, 0.003337353467941284f, 0.0033587596844881773f, 0.002448756480589509f, 0.0016790833324193954f, 0.003495961194857955f, 0.0030151347164064646f, 0.003196372650563717f, 0.0027468439657241106f, 0.0020274794660508633f, 0.0018167350208386779f, 0.0021172487176954746f, 0.0030792555771768093f),
    AI_PACK_INTQ_ZP(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)))

/* Int quant #5 */
AI_INTQ_INFO_LIST_OBJ_DECLARE(conv2d_5_output_array_intq, AI_STATIC_CONST,
  AI_BUFFER_META_FLAG_SCALE_FLOAT|AI_BUFFER_META_FLAG_ZEROPOINT_S8, 1,
  AI_PACK_INTQ_INFO(
    AI_PACK_INTQ_SCALE(0.04613778367638588f),
    AI_PACK_INTQ_ZP(-128)))

/* Int quant #6 */
AI_INTQ_INFO_LIST_OBJ_DECLARE(conv2d_5_pad_before_output_array_intq, AI_STATIC_CONST,
  AI_BUFFER_META_FLAG_SCALE_FLOAT|AI_BUFFER_META_FLAG_ZEROPOINT_S8, 1,
  AI_PACK_INTQ_INFO(
    AI_PACK_INTQ_SCALE(0.029933137819170952f),
    AI_PACK_INTQ_ZP(-128)))

/* Int quant #7 */
AI_INTQ_INFO_LIST_OBJ_DECLARE(conv2d_5_weights_array_intq, AI_STATIC_CONST,
  AI_BUFFER_META_FLAG_SCALE_FLOAT|AI_BUFFER_META_FLAG_ZEROPOINT_S8, 32,
  AI_PACK_INTQ_INFO(
    AI_PACK_INTQ_SCALE(0.002070534508675337f, 0.002391878981143236f, 0.002544601447880268f, 0.002780197886750102f, 0.001969496952369809f, 0.0018751189345493913f, 0.0020705002825707197f, 0.00246960436925292f, 0.0028298799879848957f, 0.002390267327427864f, 0.002066456712782383f, 0.002209051977843046f, 0.0024280340876430273f, 0.00280610634945333f, 0.002203795127570629f, 0.0021968402434140444f, 0.002475948538631201f, 0.0017714124405756593f, 0.0021102221217006445f, 0.0018989754607900977f, 0.003065199824050069f, 0.0028338246047496796f, 0.0031303130090236664f, 0.0018955257255584002f, 0.002798772417008877f, 0.0020590757485479116f, 0.0026881354860961437f, 0.003261650213971734f, 0.0030300801154226065f, 0.0024863851722329855f, 0.002641496015712619f, 0.0023984245490282774f),
    AI_PACK_INTQ_ZP(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)))

/* Int quant #8 */
AI_INTQ_INFO_LIST_OBJ_DECLARE(conversion_0_output_array_intq, AI_STATIC_CONST,
  AI_BUFFER_META_FLAG_SCALE_FLOAT|AI_BUFFER_META_FLAG_ZEROPOINT_S8, 1,
  AI_PACK_INTQ_INFO(
    AI_PACK_INTQ_SCALE(0.003921568859368563f),
    AI_PACK_INTQ_ZP(-128)))

/* Int quant #9 */
AI_INTQ_INFO_LIST_OBJ_DECLARE(gemm_11_output_array_intq, AI_STATIC_CONST,
  AI_BUFFER_META_FLAG_SCALE_FLOAT|AI_BUFFER_META_FLAG_ZEROPOINT_S8, 1,
  AI_PACK_INTQ_INFO(
    AI_PACK_INTQ_SCALE(0.1007503792643547f),
    AI_PACK_INTQ_ZP(-128)))

/* Int quant #10 */
AI_INTQ_INFO_LIST_OBJ_DECLARE(gemm_11_weights_array_intq, AI_STATIC_CONST,
  AI_BUFFER_META_FLAG_SCALE_FLOAT|AI_BUFFER_META_FLAG_ZEROPOINT_S8, 64,
  AI_PACK_INTQ_INFO(
    AI_PACK_INTQ_SCALE(0.0018102567410096526f, 0.0021792796906083822f, 0.0006089420639909804f, 0.002131454646587372f, 0.0008833654574118555f, 0.0022038528695702553f, 0.0007313950336538255f, 0.001954176463186741f, 0.0025023501366376877f, 0.0006732525653205812f, 0.0005196928977966309f, 0.000675903691444546f, 0.001958068460226059f, 0.002039751037955284f, 0.002045844215899706f, 0.0020420814398676157f, 0.0006543982890434563f, 0.002435306552797556f, 0.0006144744111225009f, 0.002213739790022373f, 0.0022369264625012875f, 0.0014533055946230888f, 0.002566311042755842f, 0.0007062289514578879f, 0.0009519666200503707f, 0.002274934435263276f, 0.0019249629694968462f, 0.002110350877046585f, 0.0006182444049045444f, 0.0005888058803975582f, 0.0025511090643703938f, 0.002396584488451481f, 0.0005641484167426825f, 0.00245326547883451f, 0.00163612503092736f, 0.002216263208538294f, 0.0020172821823507547f, 0.0023009972646832466f, 0.002562130568549037f, 0.0021225987002253532f, 0.0011871106689795852f, 0.0020895220804959536f, 0.0022006137296557426f, 0.0005936836823821068f, 0.0018265524413436651f, 0.0024217686150223017f, 0.0007267207838594913f, 0.0009342291741631925f, 0.0007573119946755469f, 0.0019873627461493015f, 0.0009502751636318862f, 0.0021292546298354864f, 0.002078024670481682f, 0.002177390269935131f, 0.001919322065077722f, 0.0014138148399069905f, 0.000760300550609827f, 0.0023557369131594896f, 0.0006085162749513984f, 0.00275437138043344f, 0.0015087262727320194f, 0.000638640602119267f, 0.002262203488498926f, 0.0018811613554134965f),
    AI_PACK_INTQ_ZP(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)))

/* Int quant #11 */
AI_INTQ_INFO_LIST_OBJ_DECLARE(gemm_12_output_array_intq, AI_STATIC_CONST,
  AI_BUFFER_META_FLAG_SCALE_FLOAT|AI_BUFFER_META_FLAG_ZEROPOINT_S8, 1,
  AI_PACK_INTQ_INFO(
    AI_PACK_INTQ_SCALE(0.31296828389167786f),
    AI_PACK_INTQ_ZP(-7)))

/* Int quant #12 */
AI_INTQ_INFO_LIST_OBJ_DECLARE(gemm_12_weights_array_intq, AI_STATIC_CONST,
  AI_BUFFER_META_FLAG_SCALE_FLOAT|AI_BUFFER_META_FLAG_ZEROPOINT_S8, 5,
  AI_PACK_INTQ_INFO(
    AI_PACK_INTQ_SCALE(0.004235851112753153f, 0.0036061443388462067f, 0.003952747210860252f, 0.0036068942863494158f, 0.0034376252442598343f),
    AI_PACK_INTQ_ZP(0, 0, 0, 0, 0)))

/* Int quant #13 */
AI_INTQ_INFO_LIST_OBJ_DECLARE(nl_13_output_array_intq, AI_STATIC_CONST,
  AI_BUFFER_META_FLAG_SCALE_FLOAT|AI_BUFFER_META_FLAG_ZEROPOINT_S8, 1,
  AI_PACK_INTQ_INFO(
    AI_PACK_INTQ_SCALE(0.00390625f),
    AI_PACK_INTQ_ZP(-128)))

/* Int quant #14 */
AI_INTQ_INFO_LIST_OBJ_DECLARE(pool_2_output_array_intq, AI_STATIC_CONST,
  AI_BUFFER_META_FLAG_SCALE_FLOAT|AI_BUFFER_META_FLAG_ZEROPOINT_S8, 1,
  AI_PACK_INTQ_INFO(
    AI_PACK_INTQ_SCALE(0.02114875055849552f),
    AI_PACK_INTQ_ZP(-128)))

/* Int quant #15 */
AI_INTQ_INFO_LIST_OBJ_DECLARE(pool_4_output_array_intq, AI_STATIC_CONST,
  AI_BUFFER_META_FLAG_SCALE_FLOAT|AI_BUFFER_META_FLAG_ZEROPOINT_S8, 1,
  AI_PACK_INTQ_INFO(
    AI_PACK_INTQ_SCALE(0.029933137819170952f),
    AI_PACK_INTQ_ZP(-128)))

/* Int quant #16 */
AI_INTQ_INFO_LIST_OBJ_DECLARE(pool_6_output_array_intq, AI_STATIC_CONST,
  AI_BUFFER_META_FLAG_SCALE_FLOAT|AI_BUFFER_META_FLAG_ZEROPOINT_S8, 1,
  AI_PACK_INTQ_INFO(
    AI_PACK_INTQ_SCALE(0.04613778367638588f),
    AI_PACK_INTQ_ZP(-128)))

/**  Tensor declarations section  *********************************************/
/* Tensor #0 */
AI_TENSOR_OBJ_DECLARE(
  conv2d_1_bias, AI_STATIC,
  0, 0x0,
  AI_SHAPE_INIT(4, 1, 16, 1, 1), AI_STRIDE_INIT(4, 4, 4, 64, 64),
  1, &conv2d_1_bias_array, NULL)

/* Tensor #1 */
AI_TENSOR_OBJ_DECLARE(
  conv2d_1_output, AI_STATIC,
  1, 0x1,
  AI_SHAPE_INIT(4, 1, 16, 64, 64), AI_STRIDE_INIT(4, 1, 1, 16, 1024),
  1, &conv2d_1_output_array, &conv2d_1_output_array_intq)

/* Tensor #2 */
AI_TENSOR_OBJ_DECLARE(
  conv2d_1_scratch0, AI_STATIC,
  2, 0x0,
  AI_SHAPE_INIT(4, 1, 1196, 1, 1), AI_STRIDE_INIT(4, 1, 1, 1196, 1196),
  1, &conv2d_1_scratch0_array, NULL)

/* Tensor #3 */
AI_TENSOR_OBJ_DECLARE(
  conv2d_1_weights, AI_STATIC,
  3, 0x1,
  AI_SHAPE_INIT(4, 3, 3, 3, 16), AI_STRIDE_INIT(4, 1, 3, 48, 144),
  1, &conv2d_1_weights_array, &conv2d_1_weights_array_intq)

/* Tensor #4 */
AI_TENSOR_OBJ_DECLARE(
  conv2d_3_bias, AI_STATIC,
  4, 0x0,
  AI_SHAPE_INIT(4, 1, 24, 1, 1), AI_STRIDE_INIT(4, 4, 4, 96, 96),
  1, &conv2d_3_bias_array, NULL)

/* Tensor #5 */
AI_TENSOR_OBJ_DECLARE(
  conv2d_3_output, AI_STATIC,
  5, 0x1,
  AI_SHAPE_INIT(4, 1, 24, 32, 32), AI_STRIDE_INIT(4, 1, 1, 24, 768),
  1, &conv2d_3_output_array, &conv2d_3_output_array_intq)

/* Tensor #6 */
AI_TENSOR_OBJ_DECLARE(
  conv2d_3_pad_before_output, AI_STATIC,
  6, 0x1,
  AI_SHAPE_INIT(4, 1, 16, 34, 34), AI_STRIDE_INIT(4, 1, 1, 16, 544),
  1, &conv2d_3_pad_before_output_array, &conv2d_3_pad_before_output_array_intq)

/* Tensor #7 */
AI_TENSOR_OBJ_DECLARE(
  conv2d_3_scratch0, AI_STATIC,
  7, 0x0,
  AI_SHAPE_INIT(4, 1, 6032, 1, 1), AI_STRIDE_INIT(4, 1, 1, 6032, 6032),
  1, &conv2d_3_scratch0_array, NULL)

/* Tensor #8 */
AI_TENSOR_OBJ_DECLARE(
  conv2d_3_weights, AI_STATIC,
  8, 0x1,
  AI_SHAPE_INIT(4, 16, 3, 3, 24), AI_STRIDE_INIT(4, 1, 16, 384, 1152),
  1, &conv2d_3_weights_array, &conv2d_3_weights_array_intq)

/* Tensor #9 */
AI_TENSOR_OBJ_DECLARE(
  conv2d_5_bias, AI_STATIC,
  9, 0x0,
  AI_SHAPE_INIT(4, 1, 32, 1, 1), AI_STRIDE_INIT(4, 4, 4, 128, 128),
  1, &conv2d_5_bias_array, NULL)

/* Tensor #10 */
AI_TENSOR_OBJ_DECLARE(
  conv2d_5_output, AI_STATIC,
  10, 0x1,
  AI_SHAPE_INIT(4, 1, 32, 16, 16), AI_STRIDE_INIT(4, 1, 1, 32, 512),
  1, &conv2d_5_output_array, &conv2d_5_output_array_intq)

/* Tensor #11 */
AI_TENSOR_OBJ_DECLARE(
  conv2d_5_pad_before_output, AI_STATIC,
  11, 0x1,
  AI_SHAPE_INIT(4, 1, 24, 18, 18), AI_STRIDE_INIT(4, 1, 1, 24, 432),
  1, &conv2d_5_pad_before_output_array, &conv2d_5_pad_before_output_array_intq)

/* Tensor #12 */
AI_TENSOR_OBJ_DECLARE(
  conv2d_5_scratch0, AI_STATIC,
  12, 0x0,
  AI_SHAPE_INIT(4, 1, 6432, 1, 1), AI_STRIDE_INIT(4, 1, 1, 6432, 6432),
  1, &conv2d_5_scratch0_array, NULL)

/* Tensor #13 */
AI_TENSOR_OBJ_DECLARE(
  conv2d_5_weights, AI_STATIC,
  13, 0x1,
  AI_SHAPE_INIT(4, 24, 3, 3, 32), AI_STRIDE_INIT(4, 1, 24, 768, 2304),
  1, &conv2d_5_weights_array, &conv2d_5_weights_array_intq)

/* Tensor #14 */
AI_TENSOR_OBJ_DECLARE(
  conversion_0_output, AI_STATIC,
  14, 0x1,
  AI_SHAPE_INIT(4, 1, 3, 64, 64), AI_STRIDE_INIT(4, 1, 1, 3, 192),
  1, &conversion_0_output_array, &conversion_0_output_array_intq)

/* Tensor #15 */
AI_TENSOR_OBJ_DECLARE(
  conversion_14_output, AI_STATIC,
  15, 0x0,
  AI_SHAPE_INIT(4, 1, 5, 1, 1), AI_STRIDE_INIT(4, 4, 4, 20, 20),
  1, &conversion_14_output_array, NULL)

/* Tensor #16 */
AI_TENSOR_OBJ_DECLARE(
  gemm_11_bias, AI_STATIC,
  16, 0x0,
  AI_SHAPE_INIT(4, 1, 64, 1, 1), AI_STRIDE_INIT(4, 4, 4, 256, 256),
  1, &gemm_11_bias_array, NULL)

/* Tensor #17 */
AI_TENSOR_OBJ_DECLARE(
  gemm_11_output, AI_STATIC,
  17, 0x1,
  AI_SHAPE_INIT(4, 1, 64, 1, 1), AI_STRIDE_INIT(4, 1, 1, 64, 64),
  1, &gemm_11_output_array, &gemm_11_output_array_intq)

/* Tensor #18 */
AI_TENSOR_OBJ_DECLARE(
  gemm_11_scratch0, AI_STATIC,
  18, 0x0,
  AI_SHAPE_INIT(4, 1, 2368, 1, 1), AI_STRIDE_INIT(4, 2, 2, 4736, 4736),
  1, &gemm_11_scratch0_array, NULL)

/* Tensor #19 */
AI_TENSOR_OBJ_DECLARE(
  gemm_11_weights, AI_STATIC,
  19, 0x1,
  AI_SHAPE_INIT(4, 2048, 64, 1, 1), AI_STRIDE_INIT(4, 1, 2048, 131072, 131072),
  1, &gemm_11_weights_array, &gemm_11_weights_array_intq)

/* Tensor #20 */
AI_TENSOR_OBJ_DECLARE(
  gemm_12_bias, AI_STATIC,
  20, 0x0,
  AI_SHAPE_INIT(4, 1, 5, 1, 1), AI_STRIDE_INIT(4, 4, 4, 20, 20),
  1, &gemm_12_bias_array, NULL)

/* Tensor #21 */
AI_TENSOR_OBJ_DECLARE(
  gemm_12_output, AI_STATIC,
  21, 0x1,
  AI_SHAPE_INIT(4, 1, 5, 1, 1), AI_STRIDE_INIT(4, 1, 1, 5, 5),
  1, &gemm_12_output_array, &gemm_12_output_array_intq)

/* Tensor #22 */
AI_TENSOR_OBJ_DECLARE(
  gemm_12_scratch0, AI_STATIC,
  22, 0x0,
  AI_SHAPE_INIT(4, 1, 89, 1, 1), AI_STRIDE_INIT(4, 2, 2, 178, 178),
  1, &gemm_12_scratch0_array, NULL)

/* Tensor #23 */
AI_TENSOR_OBJ_DECLARE(
  gemm_12_weights, AI_STATIC,
  23, 0x1,
  AI_SHAPE_INIT(4, 64, 5, 1, 1), AI_STRIDE_INIT(4, 1, 64, 320, 320),
  1, &gemm_12_weights_array, &gemm_12_weights_array_intq)

/* Tensor #24 */
AI_TENSOR_OBJ_DECLARE(
  nl_13_output, AI_STATIC,
  24, 0x1,
  AI_SHAPE_INIT(4, 1, 5, 1, 1), AI_STRIDE_INIT(4, 1, 1, 5, 5),
  1, &nl_13_output_array, &nl_13_output_array_intq)

/* Tensor #25 */
AI_TENSOR_OBJ_DECLARE(
  nl_13_scratch0, AI_STATIC,
  25, 0x0,
  AI_SHAPE_INIT(4, 1, 62, 1, 1), AI_STRIDE_INIT(4, 4, 4, 248, 248),
  1, &nl_13_scratch0_array, NULL)

/* Tensor #26 */
AI_TENSOR_OBJ_DECLARE(
  pool_2_output, AI_STATIC,
  26, 0x1,
  AI_SHAPE_INIT(4, 1, 16, 32, 32), AI_STRIDE_INIT(4, 1, 1, 16, 512),
  1, &pool_2_output_array, &pool_2_output_array_intq)

/* Tensor #27 */
AI_TENSOR_OBJ_DECLARE(
  pool_4_output, AI_STATIC,
  27, 0x1,
  AI_SHAPE_INIT(4, 1, 24, 16, 16), AI_STRIDE_INIT(4, 1, 1, 24, 384),
  1, &pool_4_output_array, &pool_4_output_array_intq)

/* Tensor #28 */
AI_TENSOR_OBJ_DECLARE(
  pool_6_output, AI_STATIC,
  28, 0x1,
  AI_SHAPE_INIT(4, 1, 32, 8, 8), AI_STRIDE_INIT(4, 1, 1, 32, 256),
  1, &pool_6_output_array, &pool_6_output_array_intq)

/* Tensor #29 */
AI_TENSOR_OBJ_DECLARE(
  pool_6_output0, AI_STATIC,
  29, 0x1,
  AI_SHAPE_INIT(4, 1, 2048, 1, 1), AI_STRIDE_INIT(4, 1, 1, 2048, 2048),
  1, &pool_6_output_array, &pool_6_output_array_intq)

/* Tensor #30 */
AI_TENSOR_OBJ_DECLARE(
  serving_default_input_layer0_output, AI_STATIC,
  30, 0x0,
  AI_SHAPE_INIT(4, 1, 3, 64, 64), AI_STRIDE_INIT(4, 4, 4, 12, 768),
  1, &serving_default_input_layer0_output_array, NULL)



/**  Layer declarations section  **********************************************/


AI_TENSOR_CHAIN_OBJ_DECLARE(
  conversion_14_chain, AI_STATIC_CONST, 4,
  AI_TENSOR_LIST_OBJ_INIT(AI_FLAG_NONE, 1, &nl_13_output),
  AI_TENSOR_LIST_OBJ_INIT(AI_FLAG_NONE, 1, &conversion_14_output),
  AI_TENSOR_LIST_OBJ_EMPTY,
  AI_TENSOR_LIST_OBJ_EMPTY
)

AI_LAYER_OBJ_DECLARE(
  conversion_14_layer, 14,
  NL_TYPE, 0x0, NULL,
  nl, node_convert,
  &conversion_14_chain,
  NULL, &conversion_14_layer, AI_STATIC, 
)


AI_STATIC_CONST ai_i32 nl_13_nl_params_data[] = { 1344188544, 25, -62 };
AI_ARRAY_OBJ_DECLARE(
    nl_13_nl_params, AI_ARRAY_FORMAT_S32,
    nl_13_nl_params_data, nl_13_nl_params_data, 3, AI_STATIC_CONST)
AI_TENSOR_CHAIN_OBJ_DECLARE(
  nl_13_chain, AI_STATIC_CONST, 4,
  AI_TENSOR_LIST_OBJ_INIT(AI_FLAG_NONE, 1, &gemm_12_output),
  AI_TENSOR_LIST_OBJ_INIT(AI_FLAG_NONE, 1, &nl_13_output),
  AI_TENSOR_LIST_OBJ_EMPTY,
  AI_TENSOR_LIST_OBJ_INIT(AI_FLAG_NONE, 1, &nl_13_scratch0)
)

AI_LAYER_OBJ_DECLARE(
  nl_13_layer, 13,
  SM_TYPE, 0x0, NULL,
  sm, forward_sm_integer,
  &nl_13_chain,
  NULL, &conversion_14_layer, AI_STATIC, 
  .nl_params = &nl_13_nl_params, 
  .axis = AI_SHAPE_CHANNEL, 
)

AI_TENSOR_CHAIN_OBJ_DECLARE(
  gemm_12_chain, AI_STATIC_CONST, 4,
  AI_TENSOR_LIST_OBJ_INIT(AI_FLAG_NONE, 1, &gemm_11_output),
  AI_TENSOR_LIST_OBJ_INIT(AI_FLAG_NONE, 1, &gemm_12_output),
  AI_TENSOR_LIST_OBJ_INIT(AI_FLAG_NONE, 2, &gemm_12_weights, &gemm_12_bias),
  AI_TENSOR_LIST_OBJ_INIT(AI_FLAG_NONE, 1, &gemm_12_scratch0)
)

AI_LAYER_OBJ_DECLARE(
  gemm_12_layer, 12,
  DENSE_TYPE, 0x0, NULL,
  dense, forward_dense_integer_SSSA_ch,
  &gemm_12_chain,
  NULL, &nl_13_layer, AI_STATIC, 
)

AI_TENSOR_CHAIN_OBJ_DECLARE(
  gemm_11_chain, AI_STATIC_CONST, 4,
  AI_TENSOR_LIST_OBJ_INIT(AI_FLAG_NONE, 1, &pool_6_output0),
  AI_TENSOR_LIST_OBJ_INIT(AI_FLAG_NONE, 1, &gemm_11_output),
  AI_TENSOR_LIST_OBJ_INIT(AI_FLAG_NONE, 2, &gemm_11_weights, &gemm_11_bias),
  AI_TENSOR_LIST_OBJ_INIT(AI_FLAG_NONE, 1, &gemm_11_scratch0)
)

AI_LAYER_OBJ_DECLARE(
  gemm_11_layer, 11,
  DENSE_TYPE, 0x0, NULL,
  dense, forward_dense_integer_SSSA_ch,
  &gemm_11_chain,
  NULL, &gemm_12_layer, AI_STATIC, 
)

AI_TENSOR_CHAIN_OBJ_DECLARE(
  pool_6_chain, AI_STATIC_CONST, 4,
  AI_TENSOR_LIST_OBJ_INIT(AI_FLAG_NONE, 1, &conv2d_5_output),
  AI_TENSOR_LIST_OBJ_INIT(AI_FLAG_NONE, 1, &pool_6_output),
  AI_TENSOR_LIST_OBJ_EMPTY,
  AI_TENSOR_LIST_OBJ_EMPTY
)

AI_LAYER_OBJ_DECLARE(
  pool_6_layer, 6,
  POOL_TYPE, 0x0, NULL,
  pool, forward_mp_integer_INT8,
  &pool_6_chain,
  NULL, &gemm_11_layer, AI_STATIC, 
  .pool_size = AI_SHAPE_2D_INIT(2, 2), 
  .pool_stride = AI_SHAPE_2D_INIT(2, 2), 
  .pool_pad = AI_SHAPE_INIT(4, 0, 0, 0, 0), 
)

AI_TENSOR_CHAIN_OBJ_DECLARE(
  conv2d_5_chain, AI_STATIC_CONST, 4,
  AI_TENSOR_LIST_OBJ_INIT(AI_FLAG_NONE, 1, &conv2d_5_pad_before_output),
  AI_TENSOR_LIST_OBJ_INIT(AI_FLAG_NONE, 1, &conv2d_5_output),
  AI_TENSOR_LIST_OBJ_INIT(AI_FLAG_NONE, 3, &conv2d_5_weights, &conv2d_5_bias, NULL),
  AI_TENSOR_LIST_OBJ_INIT(AI_FLAG_NONE, 1, &conv2d_5_scratch0)
)

AI_LAYER_OBJ_DECLARE(
  conv2d_5_layer, 5,
  CONV2D_TYPE, 0x0, NULL,
  conv2d, forward_conv2d_deep_3x3_sssa8_ch,
  &conv2d_5_chain,
  NULL, &pool_6_layer, AI_STATIC, 
  .groups = 1, 
  .filter_stride = AI_SHAPE_2D_INIT(1, 1), 
  .dilation = AI_SHAPE_2D_INIT(1, 1), 
  .filter_pad = AI_SHAPE_INIT(4, 0, 0, 0, 0), 
  .in_ch_format = AI_LAYER_FORMAT_CHANNEL_LAST_VALID, 
  .out_ch_format = AI_LAYER_FORMAT_CHANNEL_LAST_VALID, 
)


AI_STATIC_CONST ai_i8 conv2d_5_pad_before_value_data[] = { -128 };
AI_ARRAY_OBJ_DECLARE(
    conv2d_5_pad_before_value, AI_ARRAY_FORMAT_S8,
    conv2d_5_pad_before_value_data, conv2d_5_pad_before_value_data, 1, AI_STATIC_CONST)
AI_TENSOR_CHAIN_OBJ_DECLARE(
  conv2d_5_pad_before_chain, AI_STATIC_CONST, 4,
  AI_TENSOR_LIST_OBJ_INIT(AI_FLAG_NONE, 1, &pool_4_output),
  AI_TENSOR_LIST_OBJ_INIT(AI_FLAG_NONE, 1, &conv2d_5_pad_before_output),
  AI_TENSOR_LIST_OBJ_EMPTY,
  AI_TENSOR_LIST_OBJ_EMPTY
)

AI_LAYER_OBJ_DECLARE(
  conv2d_5_pad_before_layer, 5,
  PAD_TYPE, 0x0, NULL,
  pad, forward_pad,
  &conv2d_5_pad_before_chain,
  NULL, &conv2d_5_layer, AI_STATIC, 
  .value = &conv2d_5_pad_before_value, 
  .mode = AI_PAD_CONSTANT, 
  .pads = AI_SHAPE_INIT(4, 1, 1, 1, 1), 
)

AI_TENSOR_CHAIN_OBJ_DECLARE(
  pool_4_chain, AI_STATIC_CONST, 4,
  AI_TENSOR_LIST_OBJ_INIT(AI_FLAG_NONE, 1, &conv2d_3_output),
  AI_TENSOR_LIST_OBJ_INIT(AI_FLAG_NONE, 1, &pool_4_output),
  AI_TENSOR_LIST_OBJ_EMPTY,
  AI_TENSOR_LIST_OBJ_EMPTY
)

AI_LAYER_OBJ_DECLARE(
  pool_4_layer, 4,
  POOL_TYPE, 0x0, NULL,
  pool, forward_mp_integer_INT8,
  &pool_4_chain,
  NULL, &conv2d_5_pad_before_layer, AI_STATIC, 
  .pool_size = AI_SHAPE_2D_INIT(2, 2), 
  .pool_stride = AI_SHAPE_2D_INIT(2, 2), 
  .pool_pad = AI_SHAPE_INIT(4, 0, 0, 0, 0), 
)

AI_TENSOR_CHAIN_OBJ_DECLARE(
  conv2d_3_chain, AI_STATIC_CONST, 4,
  AI_TENSOR_LIST_OBJ_INIT(AI_FLAG_NONE, 1, &conv2d_3_pad_before_output),
  AI_TENSOR_LIST_OBJ_INIT(AI_FLAG_NONE, 1, &conv2d_3_output),
  AI_TENSOR_LIST_OBJ_INIT(AI_FLAG_NONE, 3, &conv2d_3_weights, &conv2d_3_bias, NULL),
  AI_TENSOR_LIST_OBJ_INIT(AI_FLAG_NONE, 1, &conv2d_3_scratch0)
)

AI_LAYER_OBJ_DECLARE(
  conv2d_3_layer, 3,
  CONV2D_TYPE, 0x0, NULL,
  conv2d, forward_conv2d_deep_3x3_sssa8_ch,
  &conv2d_3_chain,
  NULL, &pool_4_layer, AI_STATIC, 
  .groups = 1, 
  .filter_stride = AI_SHAPE_2D_INIT(1, 1), 
  .dilation = AI_SHAPE_2D_INIT(1, 1), 
  .filter_pad = AI_SHAPE_INIT(4, 0, 0, 0, 0), 
  .in_ch_format = AI_LAYER_FORMAT_CHANNEL_LAST_VALID, 
  .out_ch_format = AI_LAYER_FORMAT_CHANNEL_LAST_VALID, 
)


AI_STATIC_CONST ai_i8 conv2d_3_pad_before_value_data[] = { -128 };
AI_ARRAY_OBJ_DECLARE(
    conv2d_3_pad_before_value, AI_ARRAY_FORMAT_S8,
    conv2d_3_pad_before_value_data, conv2d_3_pad_before_value_data, 1, AI_STATIC_CONST)
AI_TENSOR_CHAIN_OBJ_DECLARE(
  conv2d_3_pad_before_chain, AI_STATIC_CONST, 4,
  AI_TENSOR_LIST_OBJ_INIT(AI_FLAG_NONE, 1, &pool_2_output),
  AI_TENSOR_LIST_OBJ_INIT(AI_FLAG_NONE, 1, &conv2d_3_pad_before_output),
  AI_TENSOR_LIST_OBJ_EMPTY,
  AI_TENSOR_LIST_OBJ_EMPTY
)

AI_LAYER_OBJ_DECLARE(
  conv2d_3_pad_before_layer, 3,
  PAD_TYPE, 0x0, NULL,
  pad, forward_pad,
  &conv2d_3_pad_before_chain,
  NULL, &conv2d_3_layer, AI_STATIC, 
  .value = &conv2d_3_pad_before_value, 
  .mode = AI_PAD_CONSTANT, 
  .pads = AI_SHAPE_INIT(4, 1, 1, 1, 1), 
)

AI_TENSOR_CHAIN_OBJ_DECLARE(
  pool_2_chain, AI_STATIC_CONST, 4,
  AI_TENSOR_LIST_OBJ_INIT(AI_FLAG_NONE, 1, &conv2d_1_output),
  AI_TENSOR_LIST_OBJ_INIT(AI_FLAG_NONE, 1, &pool_2_output),
  AI_TENSOR_LIST_OBJ_EMPTY,
  AI_TENSOR_LIST_OBJ_EMPTY
)

AI_LAYER_OBJ_DECLARE(
  pool_2_layer, 2,
  POOL_TYPE, 0x0, NULL,
  pool, forward_mp_integer_INT8,
  &pool_2_chain,
  NULL, &conv2d_3_pad_before_layer, AI_STATIC, 
  .pool_size = AI_SHAPE_2D_INIT(2, 2), 
  .pool_stride = AI_SHAPE_2D_INIT(2, 2), 
  .pool_pad = AI_SHAPE_INIT(4, 0, 0, 0, 0), 
)

AI_TENSOR_CHAIN_OBJ_DECLARE(
  conv2d_1_chain, AI_STATIC_CONST, 4,
  AI_TENSOR_LIST_OBJ_INIT(AI_FLAG_NONE, 1, &conversion_0_output),
  AI_TENSOR_LIST_OBJ_INIT(AI_FLAG_NONE, 1, &conv2d_1_output),
  AI_TENSOR_LIST_OBJ_INIT(AI_FLAG_NONE, 3, &conv2d_1_weights, &conv2d_1_bias, NULL),
  AI_TENSOR_LIST_OBJ_INIT(AI_FLAG_NONE, 1, &conv2d_1_scratch0)
)

AI_LAYER_OBJ_DECLARE(
  conv2d_1_layer, 1,
  OPTIMIZED_CONV2D_TYPE, 0x0, NULL,
  conv2d, forward_conv2d_rgb_sssa8_ch,
  &conv2d_1_chain,
  NULL, &pool_2_layer, AI_STATIC, 
  .groups = 1, 
  .filter_stride = AI_SHAPE_2D_INIT(1, 1), 
  .dilation = AI_SHAPE_2D_INIT(1, 1), 
  .filter_pad = AI_SHAPE_INIT(4, 1, 1, 1, 1), 
  .in_ch_format = AI_LAYER_FORMAT_CHANNEL_LAST_SAME, 
  .out_ch_format = AI_LAYER_FORMAT_CHANNEL_LAST_VALID, 
)

AI_TENSOR_CHAIN_OBJ_DECLARE(
  conversion_0_chain, AI_STATIC_CONST, 4,
  AI_TENSOR_LIST_OBJ_INIT(AI_FLAG_NONE, 1, &serving_default_input_layer0_output),
  AI_TENSOR_LIST_OBJ_INIT(AI_FLAG_NONE, 1, &conversion_0_output),
  AI_TENSOR_LIST_OBJ_EMPTY,
  AI_TENSOR_LIST_OBJ_EMPTY
)

AI_LAYER_OBJ_DECLARE(
  conversion_0_layer, 0,
  NL_TYPE, 0x0, NULL,
  nl, node_convert,
  &conversion_0_chain,
  NULL, &conv2d_1_layer, AI_STATIC, 
)


#if (AI_TOOLS_API_VERSION < AI_TOOLS_API_VERSION_1_5)

AI_NETWORK_OBJ_DECLARE(
  AI_NET_OBJ_INSTANCE, AI_STATIC,
  AI_BUFFER_INIT(AI_FLAG_NONE,  AI_BUFFER_FORMAT_U8,
    AI_BUFFER_SHAPE_INIT(AI_SHAPE_BCWH, 4, 1, 142756, 1, 1),
    142756, NULL, NULL),
  AI_BUFFER_INIT(AI_FLAG_NONE,  AI_BUFFER_FORMAT_U8,
    AI_BUFFER_SHAPE_INIT(AI_SHAPE_BCWH, 4, 1, 79024, 1, 1),
    79024, NULL, NULL),
  AI_TENSOR_LIST_IO_OBJ_INIT(AI_FLAG_NONE, AI_GESTURE_IN_NUM, &serving_default_input_layer0_output),
  AI_TENSOR_LIST_IO_OBJ_INIT(AI_FLAG_NONE, AI_GESTURE_OUT_NUM, &conversion_14_output),
  &conversion_0_layer, 0xf7e4447e, NULL)

#else

AI_NETWORK_OBJ_DECLARE(
  AI_NET_OBJ_INSTANCE, AI_STATIC,
  AI_BUFFER_ARRAY_OBJ_INIT_STATIC(
  	AI_FLAG_NONE, 1,
    AI_BUFFER_INIT(AI_FLAG_NONE,  AI_BUFFER_FORMAT_U8,
      AI_BUFFER_SHAPE_INIT(AI_SHAPE_BCWH, 4, 1, 142756, 1, 1),
      142756, NULL, NULL)
  ),
  AI_BUFFER_ARRAY_OBJ_INIT_STATIC(
  	AI_FLAG_NONE, 1,
    AI_BUFFER_INIT(AI_FLAG_NONE,  AI_BUFFER_FORMAT_U8,
      AI_BUFFER_SHAPE_INIT(AI_SHAPE_BCWH, 4, 1, 79024, 1, 1),
      79024, NULL, NULL)
  ),
  AI_TENSOR_LIST_IO_OBJ_INIT(AI_FLAG_NONE, AI_GESTURE_IN_NUM, &serving_default_input_layer0_output),
  AI_TENSOR_LIST_IO_OBJ_INIT(AI_FLAG_NONE, AI_GESTURE_OUT_NUM, &conversion_14_output),
  &conversion_0_layer, 0xf7e4447e, NULL)

#endif	/*(AI_TOOLS_API_VERSION < AI_TOOLS_API_VERSION_1_5)*/



/******************************************************************************/
AI_DECLARE_STATIC
ai_bool gesture_configure_activations(
  ai_network* net_ctx, const ai_network_params* params)
{
  AI_ASSERT(net_ctx)

  if (ai_platform_get_activations_map(g_gesture_activations_map, 1, params)) {
    /* Updating activations (byte) offsets */
    
    serving_default_input_layer0_output_array.data = AI_PTR(g_gesture_activations_map[0] + 17580);
    serving_default_input_layer0_output_array.data_start = AI_PTR(g_gesture_activations_map[0] + 17580);
    conversion_0_output_array.data = AI_PTR(g_gesture_activations_map[0] + 66732);
    conversion_0_output_array.data_start = AI_PTR(g_gesture_activations_map[0] + 66732);
    conv2d_1_scratch0_array.data = AI_PTR(g_gesture_activations_map[0] + 65536);
    conv2d_1_scratch0_array.data_start = AI_PTR(g_gesture_activations_map[0] + 65536);
    conv2d_1_output_array.data = AI_PTR(g_gesture_activations_map[0] + 0);
    conv2d_1_output_array.data_start = AI_PTR(g_gesture_activations_map[0] + 0);
    pool_2_output_array.data = AI_PTR(g_gesture_activations_map[0] + 0);
    pool_2_output_array.data_start = AI_PTR(g_gesture_activations_map[0] + 0);
    conv2d_3_pad_before_output_array.data = AI_PTR(g_gesture_activations_map[0] + 16384);
    conv2d_3_pad_before_output_array.data_start = AI_PTR(g_gesture_activations_map[0] + 16384);
    conv2d_3_scratch0_array.data = AI_PTR(g_gesture_activations_map[0] + 0);
    conv2d_3_scratch0_array.data_start = AI_PTR(g_gesture_activations_map[0] + 0);
    conv2d_3_output_array.data = AI_PTR(g_gesture_activations_map[0] + 34880);
    conv2d_3_output_array.data_start = AI_PTR(g_gesture_activations_map[0] + 34880);
    pool_4_output_array.data = AI_PTR(g_gesture_activations_map[0] + 0);
    pool_4_output_array.data_start = AI_PTR(g_gesture_activations_map[0] + 0);
    conv2d_5_pad_before_output_array.data = AI_PTR(g_gesture_activations_map[0] + 6144);
    conv2d_5_pad_before_output_array.data_start = AI_PTR(g_gesture_activations_map[0] + 6144);
    conv2d_5_scratch0_array.data = AI_PTR(g_gesture_activations_map[0] + 13920);
    conv2d_5_scratch0_array.data_start = AI_PTR(g_gesture_activations_map[0] + 13920);
    conv2d_5_output_array.data = AI_PTR(g_gesture_activations_map[0] + 20352);
    conv2d_5_output_array.data_start = AI_PTR(g_gesture_activations_map[0] + 20352);
    pool_6_output_array.data = AI_PTR(g_gesture_activations_map[0] + 0);
    pool_6_output_array.data_start = AI_PTR(g_gesture_activations_map[0] + 0);
    gemm_11_scratch0_array.data = AI_PTR(g_gesture_activations_map[0] + 2048);
    gemm_11_scratch0_array.data_start = AI_PTR(g_gesture_activations_map[0] + 2048);
    gemm_11_output_array.data = AI_PTR(g_gesture_activations_map[0] + 6784);
    gemm_11_output_array.data_start = AI_PTR(g_gesture_activations_map[0] + 6784);
    gemm_12_scratch0_array.data = AI_PTR(g_gesture_activations_map[0] + 0);
    gemm_12_scratch0_array.data_start = AI_PTR(g_gesture_activations_map[0] + 0);
    gemm_12_output_array.data = AI_PTR(g_gesture_activations_map[0] + 180);
    gemm_12_output_array.data_start = AI_PTR(g_gesture_activations_map[0] + 180);
    nl_13_scratch0_array.data = AI_PTR(g_gesture_activations_map[0] + 188);
    nl_13_scratch0_array.data_start = AI_PTR(g_gesture_activations_map[0] + 188);
    nl_13_output_array.data = AI_PTR(g_gesture_activations_map[0] + 0);
    nl_13_output_array.data_start = AI_PTR(g_gesture_activations_map[0] + 0);
    conversion_14_output_array.data = AI_PTR(g_gesture_activations_map[0] + 8);
    conversion_14_output_array.data_start = AI_PTR(g_gesture_activations_map[0] + 8);
    return true;
  }
  AI_ERROR_TRAP(net_ctx, INIT_FAILED, NETWORK_ACTIVATIONS);
  return false;
}




/******************************************************************************/
AI_DECLARE_STATIC
ai_bool gesture_configure_weights(
  ai_network* net_ctx, const ai_network_params* params)
{
  AI_ASSERT(net_ctx)

  if (ai_platform_get_weights_map(g_gesture_weights_map, 1, params)) {
    /* Updating weights (byte) offsets */
    
    conv2d_1_weights_array.format |= AI_FMT_FLAG_CONST;
    conv2d_1_weights_array.data = AI_PTR(g_gesture_weights_map[0] + 0);
    conv2d_1_weights_array.data_start = AI_PTR(g_gesture_weights_map[0] + 0);
    conv2d_1_bias_array.format |= AI_FMT_FLAG_CONST;
    conv2d_1_bias_array.data = AI_PTR(g_gesture_weights_map[0] + 432);
    conv2d_1_bias_array.data_start = AI_PTR(g_gesture_weights_map[0] + 432);
    conv2d_3_weights_array.format |= AI_FMT_FLAG_CONST;
    conv2d_3_weights_array.data = AI_PTR(g_gesture_weights_map[0] + 496);
    conv2d_3_weights_array.data_start = AI_PTR(g_gesture_weights_map[0] + 496);
    conv2d_3_bias_array.format |= AI_FMT_FLAG_CONST;
    conv2d_3_bias_array.data = AI_PTR(g_gesture_weights_map[0] + 3952);
    conv2d_3_bias_array.data_start = AI_PTR(g_gesture_weights_map[0] + 3952);
    conv2d_5_weights_array.format |= AI_FMT_FLAG_CONST;
    conv2d_5_weights_array.data = AI_PTR(g_gesture_weights_map[0] + 4048);
    conv2d_5_weights_array.data_start = AI_PTR(g_gesture_weights_map[0] + 4048);
    conv2d_5_bias_array.format |= AI_FMT_FLAG_CONST;
    conv2d_5_bias_array.data = AI_PTR(g_gesture_weights_map[0] + 10960);
    conv2d_5_bias_array.data_start = AI_PTR(g_gesture_weights_map[0] + 10960);
    gemm_11_weights_array.format |= AI_FMT_FLAG_CONST;
    gemm_11_weights_array.data = AI_PTR(g_gesture_weights_map[0] + 11088);
    gemm_11_weights_array.data_start = AI_PTR(g_gesture_weights_map[0] + 11088);
    gemm_11_bias_array.format |= AI_FMT_FLAG_CONST;
    gemm_11_bias_array.data = AI_PTR(g_gesture_weights_map[0] + 142160);
    gemm_11_bias_array.data_start = AI_PTR(g_gesture_weights_map[0] + 142160);
    gemm_12_weights_array.format |= AI_FMT_FLAG_CONST;
    gemm_12_weights_array.data = AI_PTR(g_gesture_weights_map[0] + 142416);
    gemm_12_weights_array.data_start = AI_PTR(g_gesture_weights_map[0] + 142416);
    gemm_12_bias_array.format |= AI_FMT_FLAG_CONST;
    gemm_12_bias_array.data = AI_PTR(g_gesture_weights_map[0] + 142736);
    gemm_12_bias_array.data_start = AI_PTR(g_gesture_weights_map[0] + 142736);
    return true;
  }
  AI_ERROR_TRAP(net_ctx, INIT_FAILED, NETWORK_WEIGHTS);
  return false;
}


/**  PUBLIC APIs SECTION  *****************************************************/



AI_DEPRECATED
AI_API_ENTRY
ai_bool ai_gesture_get_info(
  ai_handle network, ai_network_report* report)
{
  ai_network* net_ctx = AI_NETWORK_ACQUIRE_CTX(network);

  if (report && net_ctx)
  {
    ai_network_report r = {
      .model_name        = AI_GESTURE_MODEL_NAME,
      .model_signature   = AI_GESTURE_MODEL_SIGNATURE,
      .model_datetime    = AI_TOOLS_DATE_TIME,
      
      .compile_datetime  = AI_TOOLS_COMPILE_TIME,
      
      .runtime_revision  = ai_platform_runtime_get_revision(),
      .runtime_version   = ai_platform_runtime_get_version(),

      .tool_revision     = AI_TOOLS_REVISION_ID,
      .tool_version      = {AI_TOOLS_VERSION_MAJOR, AI_TOOLS_VERSION_MINOR,
                            AI_TOOLS_VERSION_MICRO, 0x0},
      .tool_api_version  = AI_STRUCT_INIT,

      .api_version            = ai_platform_api_get_version(),
      .interface_api_version  = ai_platform_interface_api_get_version(),
      
      .n_macc            = 7332386,
      .n_inputs          = 0,
      .inputs            = NULL,
      .n_outputs         = 0,
      .outputs           = NULL,
      .params            = AI_STRUCT_INIT,
      .activations       = AI_STRUCT_INIT,
      .n_nodes           = 0,
      .signature         = 0xf7e4447e,
    };

    if (!ai_platform_api_get_network_report(network, &r)) return false;

    *report = r;
    return true;
  }
  return false;
}



AI_API_ENTRY
ai_bool ai_gesture_get_report(
  ai_handle network, ai_network_report* report)
{
  ai_network* net_ctx = AI_NETWORK_ACQUIRE_CTX(network);

  if (report && net_ctx)
  {
    ai_network_report r = {
      .model_name        = AI_GESTURE_MODEL_NAME,
      .model_signature   = AI_GESTURE_MODEL_SIGNATURE,
      .model_datetime    = AI_TOOLS_DATE_TIME,
      
      .compile_datetime  = AI_TOOLS_COMPILE_TIME,
      
      .runtime_revision  = ai_platform_runtime_get_revision(),
      .runtime_version   = ai_platform_runtime_get_version(),

      .tool_revision     = AI_TOOLS_REVISION_ID,
      .tool_version      = {AI_TOOLS_VERSION_MAJOR, AI_TOOLS_VERSION_MINOR,
                            AI_TOOLS_VERSION_MICRO, 0x0},
      .tool_api_version  = AI_STRUCT_INIT,

      .api_version            = ai_platform_api_get_version(),
      .interface_api_version  = ai_platform_interface_api_get_version(),
      
      .n_macc            = 7332386,
      .n_inputs          = 0,
      .inputs            = NULL,
      .n_outputs         = 0,
      .outputs           = NULL,
      .map_signature     = AI_MAGIC_SIGNATURE,
      .map_weights       = AI_STRUCT_INIT,
      .map_activations   = AI_STRUCT_INIT,
      .n_nodes           = 0,
      .signature         = 0xf7e4447e,
    };

    if (!ai_platform_api_get_network_report(network, &r)) return false;

    *report = r;
    return true;
  }
  return false;
}


AI_API_ENTRY
ai_error ai_gesture_get_error(ai_handle network)
{
  return ai_platform_network_get_error(network);
}


AI_API_ENTRY
ai_error ai_gesture_create(
  ai_handle* network, const ai_buffer* network_config)
{
  return ai_platform_network_create(
    network, network_config, 
    AI_CONTEXT_OBJ(&AI_NET_OBJ_INSTANCE),
    AI_TOOLS_API_VERSION_MAJOR, AI_TOOLS_API_VERSION_MINOR, AI_TOOLS_API_VERSION_MICRO);
}


AI_API_ENTRY
ai_error ai_gesture_create_and_init(
  ai_handle* network, const ai_handle activations[], const ai_handle weights[])
{
  ai_error err;
  ai_network_params params;

  err = ai_gesture_create(network, AI_GESTURE_DATA_CONFIG);
  if (err.type != AI_ERROR_NONE) {
    return err;
  }
  
  if (ai_gesture_data_params_get(&params) != true) {
    err = ai_gesture_get_error(*network);
    return err;
  }
#if defined(AI_GESTURE_DATA_ACTIVATIONS_COUNT)
  /* set the addresses of the activations buffers */
  for (ai_u16 idx=0; activations && idx<params.map_activations.size; idx++) {
    AI_BUFFER_ARRAY_ITEM_SET_ADDRESS(&params.map_activations, idx, activations[idx]);
  }
#endif
#if defined(AI_GESTURE_DATA_WEIGHTS_COUNT)
  /* set the addresses of the weight buffers */
  for (ai_u16 idx=0; weights && idx<params.map_weights.size; idx++) {
    AI_BUFFER_ARRAY_ITEM_SET_ADDRESS(&params.map_weights, idx, weights[idx]);
  }
#endif
  if (ai_gesture_init(*network, &params) != true) {
    err = ai_gesture_get_error(*network);
  }
  return err;
}


AI_API_ENTRY
ai_buffer* ai_gesture_inputs_get(ai_handle network, ai_u16 *n_buffer)
{
  if (network == AI_HANDLE_NULL) {
    network = (ai_handle)&AI_NET_OBJ_INSTANCE;
    AI_NETWORK_OBJ(network)->magic = AI_MAGIC_CONTEXT_TOKEN;
  }
  return ai_platform_inputs_get(network, n_buffer);
}


AI_API_ENTRY
ai_buffer* ai_gesture_outputs_get(ai_handle network, ai_u16 *n_buffer)
{
  if (network == AI_HANDLE_NULL) {
    network = (ai_handle)&AI_NET_OBJ_INSTANCE;
    AI_NETWORK_OBJ(network)->magic = AI_MAGIC_CONTEXT_TOKEN;
  }
  return ai_platform_outputs_get(network, n_buffer);
}


AI_API_ENTRY
ai_handle ai_gesture_destroy(ai_handle network)
{
  return ai_platform_network_destroy(network);
}


AI_API_ENTRY
ai_bool ai_gesture_init(
  ai_handle network, const ai_network_params* params)
{
  ai_network* net_ctx = AI_NETWORK_OBJ(ai_platform_network_init(network, params));
  ai_bool ok = true;

  if (!net_ctx) return false;
  ok &= gesture_configure_weights(net_ctx, params);
  ok &= gesture_configure_activations(net_ctx, params);

  ok &= ai_platform_network_post_init(network);

  return ok;
}


AI_API_ENTRY
ai_i32 ai_gesture_run(
  ai_handle network, const ai_buffer* input, ai_buffer* output)
{
  return ai_platform_network_process(network, input, output);
}


AI_API_ENTRY
ai_i32 ai_gesture_forward(ai_handle network, const ai_buffer* input)
{
  return ai_platform_network_process(network, input, NULL);
}



#undef AI_GESTURE_MODEL_SIGNATURE
#undef AI_NET_OBJ_INSTANCE
#undef AI_TOOLS_DATE_TIME
#undef AI_TOOLS_COMPILE_TIME

