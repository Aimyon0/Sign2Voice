/**
  ******************************************************************************
  * @file    gesture.c
  * @author  AST Embedded Analytics Research Platform
  * @date    2026-07-15T21:11:18+0800
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

#include "ai_lite_inspect.h"
#include "ai_platform_interface.h"
#include "layers.h"
#include "core_convert.h"
#include "gesture.h"
#include "gesture_details.h"
#include "stai_events.h"

#include "lite_operators.h"

#include "ai_lite_inspect.h"
/*****************************************************************************/
#define STAI_INTERNAL_API_MAJOR               (1)
#define STAI_INTERNAL_API_MINOR               (0)
#define STAI_INTERNAL_API_MICRO               (0)

#define STAI_MAGIC                            (0xB1C00100)

/*****************************************************************************/
#define _STAI_CONCAT_ARG(a, b)     a ## b
#define STAI_CONCAT(a, b)         _STAI_CONCAT_ARG(a, b)

/*!  STAI_CAST SECTION                       *********************************/
#define STAI_CAST(type, expr) \
  ((type)(expr))


/*****************************************************************************/
#define STAI_SIZE(_size) \
  ((stai_size)(_size))

/*****************************************************************************/
#define STAI_INIT_BUFFER(_flags, _size, _address) \
  { \
    .size = (_size), \
    .address = (uintptr_t)(_address), \
    .flags = (_flags), \
  }

#define STAI_INIT_TENSOR(_name, _flags, _fmt, _size_bytes, _shape, _scale, _zeropoint) \
  { \
    .size_bytes = (_size_bytes), \
    .flags = (_flags), \
    .format = (stai_format)(_fmt), \
    .shape = STAI_PACK(_shape), \
    .scale = STAI_PACK(_scale), \
    .zeropoint = STAI_PACK(_zeropoint), \
    .name = (_name) \
  }

#define STAI_INIT_ARRAY(_size, _ptr) \
  { .size = STAI_SIZE(_size), .data = STAI_PACK(_ptr) }


#define STAI_CAST_ARRAY(_type, _size, _ptr) \
  { .size = STAI_SIZE(_size), .data = (_type)STAI_PACK(_ptr) }


#define STAI_DECLARE_ARRAY(_type, _size, ...) \
  { .size = STAI_SIZE(_size), .data = (_type[_size]) { STAI_PACK(__VA_ARGS__) } }


#define STAI_EMPTY_ARRAY() \
  { .size = 0, .data = NULL }


#define STAI_INIT_VERSION(_major, _minor, _micro) \
  { .major = (_major), .minor = (_minor), .micro = (_micro), .reserved = 0x0 }

/*****************************************************************************/
/**  Getters and setters  **/

#define STAI_GET_ARRAY_SIZE(nd_array) \
  (nd_array.size)


#define STAI_GET_ARRAY_ELEM(nd_array, pos) \
  (nd_array.data[(pos)])

#define _STAI_SET_ERROR(net_ctx, cond, value, exit) { \
  if (!(net_ctx)) { return STAI_ERROR_NETWORK_INVALID_CONTEXT_HANDLE; } \
  if (((uintptr_t)net_ctx) & (_STAI_CONTEXT_ALIGNMENT-1)) { return STAI_ERROR_NETWORK_INVALID_CONTEXT_ALIGNMENT; } \
  if (((value) >= STAI_ERROR_GENERIC) && (cond)) { \
    if ((net_ctx)->_return_code == STAI_SUCCESS) { \
      (net_ctx)->_return_code = (value); \
    } \
    return (exit); \
  } \
}

/*****************************************************************************/
/* TODO REMOVE THESE TWO MACROS */
#define STAI_EVENT_NODE_START_CB
#define STAI_EVENT_NODE_STOP_CB

#ifdef STAI_EVENT_NODE_START_CB
#ifndef _STAI_GESTURE_EVENT_NODE_START_CB
  #define _STAI_GESTURE_EVENT_NODE_START_CB(_node_id, _buffers_size, ...) \
  if (net_ctx->_callback) { \
    const stai_event_node_start_stop _start_event = { \
      .node_id=(_node_id), \
      .buffers={ \
        .size=(_buffers_size), \
        .data=(stai_ptr const*)(const stai_ptr[_buffers_size])STAI_PACK(__VA_ARGS__) \
      } \
    }; \
    net_ctx->_callback(net_ctx->_callback_cookie, STAI_EVENT_NODE_START, (const void*)&_start_event); \
  }
#endif
#else
  #define _STAI_GESTURE_EVENT_NODE_START_CB(_node_id, _buffers_size, ...) \
    do { /* _STAI_GESTURE_EVENT_NODE_START_CB() */ } while(0);
#endif      /* STAI_EVENT_NODE_START_CB */

#ifdef STAI_EVENT_NODE_STOP_CB
#ifndef _STAI_GESTURE_EVENT_NODE_STOP_CB
  #define _STAI_GESTURE_EVENT_NODE_STOP_CB(_node_id, _buffers_size, ...) \
  if (net_ctx->_callback) { \
    const stai_event_node_start_stop _stop_event = { \
      .node_id=(_node_id), \
      .buffers={ \
        .size=(_buffers_size), \
        .data=(stai_ptr const*)(stai_ptr[_buffers_size])STAI_PACK(__VA_ARGS__) \
      } \
    }; \
    net_ctx->_callback(net_ctx->_callback_cookie, STAI_EVENT_NODE_STOP, (const void*)&_stop_event); \
  }
#endif
#else
  #define _STAI_GESTURE_EVENT_NODE_STOP_CB(_node_id, _buffers_size, ...) \
    do { /* _STAI_GESTURE_EVENT_NODE_STOP_CB() */ } while(0);
#endif      /* STAI_EVENT_NODE_STOP_CB */


/*****************************************************************************/
#define _STAI_GESTURE_MODEL_SIGNATURE     "0x5f3c978ced3b7aff0142c989c3b31656"
#define _STAI_GESTURE_DATETIME            "2026-07-15T21:11:18+0800"
#define _STAI_GESTURE_COMPILE_DATETIME    __DATE__ " " __TIME__

#define _STAI_CONTEXT_ALIGNMENT        STAI_GESTURE_CONTEXT_ALIGNMENT

/*****************************************************************************/
#define g_gesture_activations_1     (NULL)




#if defined(HAVE_GESTURE_INFO)
/*****************************************************************************/
static const stai_network_info g_gesture_info = {
  .model_signature = _STAI_GESTURE_MODEL_SIGNATURE,
  .c_compile_datetime = _STAI_GESTURE_COMPILE_DATETIME,
  .c_model_name = STAI_GESTURE_MODEL_NAME,
  .c_model_datetime = _STAI_GESTURE_DATETIME,
  .c_model_signature = 0x0,
  .runtime_version = STAI_INIT_VERSION(12, 0, 1),
  .tool_version = STAI_INIT_VERSION(4, 0, 1),
  .api_version = STAI_INIT_VERSION(1, 0, 0),
  .n_macc = STAI_GESTURE_MACC_NUM,
  .n_nodes = STAI_GESTURE_NODES_NUM,
  .flags = STAI_GESTURE_FLAGS,
  .n_inputs = STAI_GESTURE_IN_NUM,
  .n_outputs = STAI_GESTURE_OUT_NUM,
  .n_activations = STAI_GESTURE_ACTIVATIONS_NUM,
  .n_weights = STAI_GESTURE_WEIGHTS_NUM,
  .n_states = STAI_GESTURE_STATES_NUM,
  .inputs = (stai_tensor[STAI_GESTURE_IN_NUM]) {
    STAI_INIT_TENSOR(
      STAI_GESTURE_IN_1_NAME,
      STAI_GESTURE_IN_1_FLAGS,
      STAI_GESTURE_IN_1_FORMAT,
      STAI_GESTURE_IN_1_SIZE_BYTES,
      STAI_DECLARE_ARRAY(int32_t, 4, 1, 64, 64, 3),
      STAI_EMPTY_ARRAY(),
      STAI_EMPTY_ARRAY()),
    },
    .outputs = (stai_tensor[STAI_GESTURE_OUT_NUM]) {
    STAI_INIT_TENSOR(
      STAI_GESTURE_OUT_1_NAME,
      STAI_GESTURE_OUT_1_FLAGS,
      STAI_GESTURE_OUT_1_FORMAT,
      STAI_GESTURE_OUT_1_SIZE_BYTES,
      STAI_DECLARE_ARRAY(int32_t, 2, 1, 5),
      STAI_EMPTY_ARRAY(),
      STAI_EMPTY_ARRAY()),
    },
  .activations = (stai_tensor[STAI_GESTURE_ACTIVATIONS_NUM]) {
    STAI_INIT_TENSOR(
      (NULL),
      STAI_GESTURE_ACTIVATION_1_FLAGS,
      STAI_FORMAT_U8,
      STAI_GESTURE_ACTIVATION_1_SIZE_BYTES,
      STAI_DECLARE_ARRAY(int32_t, 1, 85056),
      STAI_EMPTY_ARRAY(),
      STAI_EMPTY_ARRAY()),
    },
  .weights = (stai_tensor[STAI_GESTURE_WEIGHTS_NUM]) {
    STAI_INIT_TENSOR(
      (NULL),
      STAI_GESTURE_WEIGHT_1_FLAGS,
      STAI_FORMAT_U8,
      STAI_GESTURE_WEIGHT_1_SIZE_BYTES,
      STAI_DECLARE_ARRAY(int32_t, 1, 142756),
      STAI_EMPTY_ARRAY(),
      STAI_EMPTY_ARRAY()),
    },

  .states = NULL
};
#endif

#define _STAI_CONTEXT_ACQUIRE(_net_ctx, _net_handle) \
  _stai_gesture_context* _net_ctx = (_stai_gesture_context*)(_net_handle); \
  STAI_ASSERT(_net_ctx != NULL) \
  _STAI_SET_ERROR(_net_ctx, _net_ctx->_magic != STAI_MAGIC, \
                  STAI_ERROR_NETWORK_INVALID_CONTEXT_HANDLE, _net_ctx->_return_code)


/*****************************************************************************/
static
void _stai_gesture_check(_stai_gesture_context* net_ctx)
{
  stai_size idx;

// Check activations status
  for (idx=0; idx<STAI_GESTURE_ACTIVATIONS_NUM; idx++) {
    if (net_ctx->_activations[idx] == NULL) break;
  }
  net_ctx->_flags |= (idx == STAI_GESTURE_ACTIVATIONS_NUM) ? STAI_FLAG_ACTIVATIONS : STAI_FLAG_NONE;
// Check inputs status
  for (idx=0; idx<STAI_GESTURE_IN_NUM; idx++) {
    if (net_ctx->_inputs[idx] == NULL) break;
  }
  net_ctx->_flags |= (idx == STAI_GESTURE_IN_NUM) ? STAI_FLAG_INPUTS : STAI_FLAG_NONE;

  // Check outputs status
  for (idx=0; idx<STAI_GESTURE_OUT_NUM; idx++) {
    if (net_ctx->_outputs[idx] == NULL) break;
  }
  net_ctx->_flags |= (idx == STAI_GESTURE_OUT_NUM) ? STAI_FLAG_OUTPUTS : STAI_FLAG_NONE;

// Check weights status
  for (idx=0; idx<STAI_GESTURE_WEIGHTS_NUM; idx++) {
    if (net_ctx->_weights[idx] == NULL) break;
  }
  net_ctx->_flags |= (idx == STAI_GESTURE_WEIGHTS_NUM) ? STAI_FLAG_WEIGHTS : STAI_FLAG_NONE;
STAI_PRINT("  [_stai_network_check] flags: 0x%08x\n", net_ctx->_flags)
}


/*****************************************************************************/
STAI_API_ENTRY
stai_return_code stai_gesture_init(
  stai_network* network)
{
  /* Memory where to store internal context is provided by applications as a raw byte buffer */
  _stai_gesture_context* net_ctx = (_stai_gesture_context*)(network);
  net_ctx->_return_code = STAI_SUCCESS;
  STAI_PRINT("[Entering Network Init] network(%p) context_size(%d)\n", net_ctx, (int32_t)sizeof(_stai_gesture_context))

  _STAI_SET_ERROR(net_ctx, STAI_GESTURE_CONTEXT_SIZE != sizeof(_stai_gesture_context),
                 STAI_ERROR_NETWORK_INVALID_CONTEXT_SIZE, net_ctx->_return_code)

  {
    const _stai_gesture_context _gesture_context = {
      ._magic = STAI_MAGIC,
      ._signature = STAI_GESTURE_MODEL_SIGNATURE,
      ._flags = STAI_GESTURE_FLAGS,
      ._return_code = STAI_SUCCESS,
      ._callback = NULL,
      ._callback_cookie = NULL,
      ._activations = {
      (stai_ptr)g_gesture_activations_1
      },
      ._weights = {
      NULL
      },
      ._inputs = {
    NULL},
      ._outputs = {
    NULL},
    };

    // Deep copy of internal context to opaque buffer provided by app
    *net_ctx = _gesture_context;

    _stai_gesture_check(net_ctx);
  }

  return net_ctx->_return_code;
}


STAI_API_ENTRY
stai_return_code stai_gesture_deinit(
  stai_network* network)
{
  _STAI_CONTEXT_ACQUIRE(net_ctx, network)

  /*  Reset flags to initial state  */
  net_ctx->_flags = STAI_GESTURE_FLAGS;
  return net_ctx->_return_code;
}

/*****************************************************************************/



/* Int quant #0 */
AI_INTQ_INFO_LIST_OBJ_DECLARE(pool_6_output_array_intq, AI_STATIC,
  AI_BUFFER_META_FLAG_SCALE_FLOAT|AI_BUFFER_META_FLAG_ZEROPOINT_S8, 1,
  AI_PACK_INTQ_INFO(
    AI_PACK_INTQ_SCALE(0.04613778367638588f),
    AI_PACK_INTQ_ZP(-128)))

/* Int quant #1 */
AI_INTQ_INFO_LIST_OBJ_DECLARE(gemm_11_output_array_intq, AI_STATIC,
  AI_BUFFER_META_FLAG_SCALE_FLOAT|AI_BUFFER_META_FLAG_ZEROPOINT_S8, 1,
  AI_PACK_INTQ_INFO(
    AI_PACK_INTQ_SCALE(0.1007503792643547f),
    AI_PACK_INTQ_ZP(-128)))

/* Int quant #2 */
AI_INTQ_INFO_LIST_OBJ_DECLARE(gemm_11_weights_array_intq, AI_STATIC,
  AI_BUFFER_META_FLAG_SCALE_FLOAT|AI_BUFFER_META_FLAG_ZEROPOINT_S8, 64,
  AI_PACK_INTQ_INFO(
    AI_PACK_INTQ_SCALE(0.0018102567410096526f, 0.0021792796906083822f, 0.0006089420639909804f, 0.002131454646587372f, 0.0008833654574118555f, 0.0022038528695702553f, 0.0007313950336538255f, 0.001954176463186741f, 0.0025023501366376877f, 0.0006732525653205812f, 0.0005196928977966309f, 0.000675903691444546f, 0.001958068460226059f, 0.002039751037955284f, 0.002045844215899706f, 0.0020420814398676157f, 0.0006543982890434563f, 0.002435306552797556f, 0.0006144744111225009f, 0.002213739790022373f, 0.0022369264625012875f, 0.0014533055946230888f, 0.002566311042755842f, 0.0007062289514578879f, 0.0009519666200503707f, 0.002274934435263276f, 0.0019249629694968462f, 0.002110350877046585f, 0.0006182444049045444f, 0.0005888058803975582f, 0.0025511090643703938f, 0.002396584488451481f, 0.0005641484167426825f, 0.00245326547883451f, 0.00163612503092736f, 0.002216263208538294f, 0.0020172821823507547f, 0.0023009972646832466f, 0.002562130568549037f, 0.0021225987002253532f, 0.0011871106689795852f, 0.0020895220804959536f, 0.0022006137296557426f, 0.0005936836823821068f, 0.0018265524413436651f, 0.0024217686150223017f, 0.0007267207838594913f, 0.0009342291741631925f, 0.0007573119946755469f, 0.0019873627461493015f, 0.0009502751636318862f, 0.0021292546298354864f, 0.002078024670481682f, 0.002177390269935131f, 0.001919322065077722f, 0.0014138148399069905f, 0.000760300550609827f, 0.0023557369131594896f, 0.0006085162749513984f, 0.00275437138043344f, 0.0015087262727320194f, 0.000638640602119267f, 0.002262203488498926f, 0.0018811613554134965f),
    AI_PACK_INTQ_ZP(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)))

/* Int quant #3 */
AI_INTQ_INFO_LIST_OBJ_DECLARE(gemm_12_output_array_intq, AI_STATIC,
  AI_BUFFER_META_FLAG_SCALE_FLOAT|AI_BUFFER_META_FLAG_ZEROPOINT_S8, 1,
  AI_PACK_INTQ_INFO(
    AI_PACK_INTQ_SCALE(0.31296828389167786f),
    AI_PACK_INTQ_ZP(-7)))

/* Int quant #4 */
AI_INTQ_INFO_LIST_OBJ_DECLARE(gemm_12_weights_array_intq, AI_STATIC,
  AI_BUFFER_META_FLAG_SCALE_FLOAT|AI_BUFFER_META_FLAG_ZEROPOINT_S8, 5,
  AI_PACK_INTQ_INFO(
    AI_PACK_INTQ_SCALE(0.004235851112753153f, 0.0036061443388462067f, 0.003952747210860252f, 0.0036068942863494158f, 0.0034376252442598343f),
    AI_PACK_INTQ_ZP(0, 0, 0, 0, 0)))



/* Array#0 */
AI_ARRAY_OBJ_DECLARE(
  pool_6_output_array, AI_ARRAY_FORMAT_S8,
  NULL, NULL, 2048, AI_STATIC)

/* Array#1 */
AI_ARRAY_OBJ_DECLARE(
  gemm_11_output_array, AI_ARRAY_FORMAT_S8,
  NULL, NULL, 64, AI_STATIC)

/* Array#2 */
AI_ARRAY_OBJ_DECLARE(
  gemm_11_weights_array, AI_ARRAY_FORMAT_S8,
  NULL, NULL, 131072, AI_STATIC)

/* Array#3 */
AI_ARRAY_OBJ_DECLARE(
  gemm_11_bias_array, AI_ARRAY_FORMAT_S32,
  NULL, NULL, 64, AI_STATIC)

/* Array#4 */
AI_ARRAY_OBJ_DECLARE(
  gemm_11_scratch0_array, AI_ARRAY_FORMAT_S16,
  NULL, NULL, 2368, AI_STATIC)

/* Array#5 */
AI_ARRAY_OBJ_DECLARE(
  gemm_12_output_array, AI_ARRAY_FORMAT_S8,
  NULL, NULL, 5, AI_STATIC)

/* Array#6 */
AI_ARRAY_OBJ_DECLARE(
  gemm_12_weights_array, AI_ARRAY_FORMAT_S8,
  NULL, NULL, 320, AI_STATIC)

/* Array#7 */
AI_ARRAY_OBJ_DECLARE(
  gemm_12_bias_array, AI_ARRAY_FORMAT_S32,
  NULL, NULL, 5, AI_STATIC)

/* Array#8 */
AI_ARRAY_OBJ_DECLARE(
  gemm_12_scratch0_array, AI_ARRAY_FORMAT_S16,
  NULL, NULL, 89, AI_STATIC)



/* Tensor #0 */
AI_TENSOR_OBJ_DECLARE(
  gemm_11_bias, AI_STATIC,
  16, 0x0,
  AI_SHAPE_INIT(4, 1, 64, 1, 1), AI_STRIDE_INIT(4, 4, 4, 256, 256),
  1, &gemm_11_bias_array, NULL)

/* Tensor #1 */
AI_TENSOR_OBJ_DECLARE(
  gemm_11_output, AI_STATIC,
  17, 0x1,
  AI_SHAPE_INIT(4, 1, 64, 1, 1), AI_STRIDE_INIT(4, 1, 1, 64, 64),
  1, &gemm_11_output_array, &gemm_11_output_array_intq)

/* Tensor #2 */
AI_TENSOR_OBJ_DECLARE(
  gemm_11_scratch0, AI_STATIC,
  18, 0x0,
  AI_SHAPE_INIT(4, 1, 2368, 1, 1), AI_STRIDE_INIT(4, 2, 2, 4736, 4736),
  1, &gemm_11_scratch0_array, NULL)

/* Tensor #3 */
AI_TENSOR_OBJ_DECLARE(
  gemm_11_weights, AI_STATIC,
  19, 0x1,
  AI_SHAPE_INIT(4, 2048, 64, 1, 1), AI_STRIDE_INIT(4, 1, 2048, 131072, 131072),
  1, &gemm_11_weights_array, &gemm_11_weights_array_intq)

/* Tensor #4 */
AI_TENSOR_OBJ_DECLARE(
  pool_6_output0, AI_STATIC,
  29, 0x1,
  AI_SHAPE_INIT(4, 1, 2048, 1, 1), AI_STRIDE_INIT(4, 1, 1, 2048, 2048),
  1, &pool_6_output_array, &pool_6_output_array_intq)

/* Tensor #5 */
AI_TENSOR_OBJ_DECLARE(
  gemm_12_bias, AI_STATIC,
  20, 0x0,
  AI_SHAPE_INIT(4, 1, 5, 1, 1), AI_STRIDE_INIT(4, 4, 4, 20, 20),
  1, &gemm_12_bias_array, NULL)

/* Tensor #6 */
AI_TENSOR_OBJ_DECLARE(
  gemm_12_output, AI_STATIC,
  21, 0x1,
  AI_SHAPE_INIT(4, 1, 5, 1, 1), AI_STRIDE_INIT(4, 1, 1, 5, 5),
  1, &gemm_12_output_array, &gemm_12_output_array_intq)

/* Tensor #7 */
AI_TENSOR_OBJ_DECLARE(
  gemm_12_scratch0, AI_STATIC,
  22, 0x0,
  AI_SHAPE_INIT(4, 1, 89, 1, 1), AI_STRIDE_INIT(4, 2, 2, 178, 178),
  1, &gemm_12_scratch0_array, NULL)

/* Tensor #8 */
AI_TENSOR_OBJ_DECLARE(
  gemm_12_weights, AI_STATIC,
  23, 0x1,
  AI_SHAPE_INIT(4, 64, 5, 1, 1), AI_STRIDE_INIT(4, 1, 64, 320, 320),
  1, &gemm_12_weights_array, &gemm_12_weights_array_intq)


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
  NULL, &gemm_11_layer, AI_STATIC, 
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
  NULL, &gemm_12_layer, AI_STATIC, 
)
/**  Hybrid layers declarations section  *************************************/
void forward_lite_dense_integer_SSSA_ch_gemm_11(_stai_gesture_context* net_ctx)
{
  pool_6_output_array.data = AI_PTR(net_ctx->_activations[0] + 7228);
  pool_6_output_array.data_start = AI_PTR(net_ctx->_activations[0] + 7228);
  gemm_11_weights_array.data = AI_PTR(net_ctx->_weights[0] + 11088);
  gemm_11_weights_array.data_start = AI_PTR(net_ctx->_weights[0] + 11088);
  gemm_11_bias_array.data = AI_PTR(net_ctx->_weights[0] + 142160);
  gemm_11_bias_array.data_start = AI_PTR(net_ctx->_weights[0] + 142160);
  gemm_11_scratch0_array.data = AI_PTR(net_ctx->_activations[0] + 0);
  gemm_11_scratch0_array.data_start = AI_PTR(net_ctx->_activations[0] + 0);
  gemm_11_output_array.data = AI_PTR(net_ctx->_activations[0] + 9276);
  gemm_11_output_array.data_start = AI_PTR(net_ctx->_activations[0] + 9276);
  _STAI_GESTURE_EVENT_NODE_START_CB(11, 1, { pool_6_output0.data->data});
  forward_dense_integer_SSSA_ch(&gemm_11_layer);
  _STAI_GESTURE_EVENT_NODE_STOP_CB(11, 1, { gemm_11_output.data->data});
}
void forward_lite_dense_integer_SSSA_ch_gemm_12(_stai_gesture_context* net_ctx)
{
  gemm_11_output_array.data = AI_PTR(net_ctx->_activations[0] + 9276);
  gemm_11_output_array.data_start = AI_PTR(net_ctx->_activations[0] + 9276);
  gemm_12_weights_array.data = AI_PTR(net_ctx->_weights[0] + 142416);
  gemm_12_weights_array.data_start = AI_PTR(net_ctx->_weights[0] + 142416);
  gemm_12_bias_array.data = AI_PTR(net_ctx->_weights[0] + 142736);
  gemm_12_bias_array.data_start = AI_PTR(net_ctx->_weights[0] + 142736);
  gemm_12_scratch0_array.data = AI_PTR(net_ctx->_activations[0] + 0);
  gemm_12_scratch0_array.data_start = AI_PTR(net_ctx->_activations[0] + 0);
  gemm_12_output_array.data = AI_PTR(net_ctx->_activations[0] + 7228);
  gemm_12_output_array.data_start = AI_PTR(net_ctx->_activations[0] + 7228);
  _STAI_GESTURE_EVENT_NODE_START_CB(12, 1, { gemm_11_output.data->data});
  forward_dense_integer_SSSA_ch(&gemm_12_layer);
  _STAI_GESTURE_EVENT_NODE_STOP_CB(12, 1, { gemm_12_output.data->data});
}

/*****************************************************************************/


static const ai_u32 conversion_0_t_out_0_shape_h_w_ch_d_prod_const_u32 = 12288;
static const ai_float conversion_0_t_out_0_fmt_scale_const_f32 = 0.003921568859368563f;
static const ai_i8 conversion_0_t_out_0_fmt_zero_const_s8 = -128;

static const ai_u16 conv2d_1_t_in_0_shape_w_const_u16 = 64;
static const ai_u16 conv2d_1_t_out_0_shape_ch_const_u16 = 16;
static const ai_u16 conv2d_1_t_weight_0_shape_w_const_u16 = 3;
static const ai_i32 conv2d_1_l_pad_W_0_const_s32 = 1;
static const ai_u16 conv2d_1_l_stride_0_const_u16 = 1;
static const ai_i8 conv2d_1_t_in_0_fmt_zero_const_s8 = -128;
static const ai_i8 conv2d_1_t_out_0_fmt_zero_const_s8 = -128;
static const ai_float conv2d_1_t_in_0_fmt_scale_const_f32 = 0.003921568859368563f;
static const ai_float conv2d_1_t_out_0_fmt_scale_const_f32 = 0.02114875055849552f;
static const ai_float conv2d_1_t_weight_0_fmt_scale_const_f32[] = LITE_ARRAY_VALUES(0.0021109613589942455f, 0.0027038156986236572f, 0.0022045052610337734f, 0.001900940085761249f, 0.002309046685695648f, 0.001947773271240294f, 0.0020833327434957027f, 0.002225436270236969f, 0.0017966980813071132f, 0.00247749756090343f, 0.0018235002644360065f, 0.002033183816820383f, 0.00396759994328022f, 0.002051991643384099f, 0.0025274946819990873f, 0.0014128254260867834f);
static const ai_layer_format_type conv2d_1_l_out_ch_format_const_layer_format_type = AI_LAYER_FORMAT_CHANNEL_LAST_VALID;
static const ai_u16 conv2d_1_t_out_0_shape_w_const_u16 = 64;

static const ai_u16 pool_2_t_in_0_shape_w_const_u16 = 64;
static const ai_u16 pool_2_t_in_0_shape_h_const_u16 = 64;
static const ai_u16 pool_2_t_in_0_shape_ch_const_u16 = 16;
static const ai_u16 pool_2_l_pool_size_1_const_u16 = 2;
static const ai_u16 pool_2_l_pool_size_0_const_u16 = 2;
static const ai_u16 pool_2_l_legacy_pool_pad_1_const_u16 = 0;
static const ai_u16 pool_2_l_legacy_pool_pad_0_const_u16 = 0;
static const ai_u16 pool_2_l_pool_stride_1_const_u16 = 2;
static const ai_u16 pool_2_l_pool_stride_0_const_u16 = 2;
static const ai_u16 pool_2_t_out_0_shape_w_const_u16 = 32;
static const ai_u16 pool_2_t_out_0_shape_h_const_u16 = 32;
static const ai_i8 pool_2_t_in_0_fmt_zero_const_s8 = -128;
static const ai_i8 pool_2_t_out_0_fmt_zero_const_s8 = -128;

static const ai_i8 conv2d_3_pad_before_v_pad_constant_value_const_s8[] = LITE_ARRAY_VALUES(-128);
static const ai_i16 conv2d_3_pad_before_t_in_0_fmt_bitsize_const_s16 = 8;
static const ai_u32 conv2d_3_pad_before_t_in_0_shape_h_const_u32 = 32;

static const ai_u16 conv2d_3_t_in_0_shape_w_const_u16 = 34;
static const ai_u16 conv2d_3_t_in_0_shape_h_const_u16 = 34;
static const ai_u16 conv2d_3_t_in_0_shape_ch_const_u16 = 16;
static const ai_u16 conv2d_3_t_out_0_shape_ch_const_u16 = 24;
static const ai_i8 conv2d_3_t_in_0_fmt_zero_const_s8 = -128;
static const ai_i8 conv2d_3_t_out_0_fmt_zero_const_s8 = -128;
static const ai_float conv2d_3_t_in_0_fmt_scale_const_f32 = 0.02114875055849552f;
static const ai_float conv2d_3_t_out_0_fmt_scale_const_f32 = 0.029933137819170952f;
static const ai_float conv2d_3_t_weight_0_fmt_scale_const_f32[] = LITE_ARRAY_VALUES(0.0018781435210257769f, 0.002203844953328371f, 0.0021182133350521326f, 0.0025331887882202864f, 0.0032312741968780756f, 0.003008316271007061f, 0.0022345467004925013f, 0.003500864841043949f, 0.00262173218652606f, 0.002470288658514619f, 0.002733913017436862f, 0.003078187583014369f, 0.003337353467941284f, 0.0033587596844881773f, 0.002448756480589509f, 0.0016790833324193954f, 0.003495961194857955f, 0.0030151347164064646f, 0.003196372650563717f, 0.0027468439657241106f, 0.0020274794660508633f, 0.0018167350208386779f, 0.0021172487176954746f, 0.0030792555771768093f);
static const ai_layer_format_type conv2d_3_l_out_ch_format_const_layer_format_type = AI_LAYER_FORMAT_CHANNEL_LAST_VALID;
static const ai_u16 conv2d_3_t_out_0_shape_w_const_u16 = 32;
static const ai_u16 conv2d_3_t_out_0_shape_h_const_u16 = 32;

static const ai_u16 pool_4_t_in_0_shape_w_const_u16 = 32;
static const ai_u16 pool_4_t_in_0_shape_h_const_u16 = 32;
static const ai_u16 pool_4_t_in_0_shape_ch_const_u16 = 24;
static const ai_u16 pool_4_l_pool_size_1_const_u16 = 2;
static const ai_u16 pool_4_l_pool_size_0_const_u16 = 2;
static const ai_u16 pool_4_l_legacy_pool_pad_1_const_u16 = 0;
static const ai_u16 pool_4_l_legacy_pool_pad_0_const_u16 = 0;
static const ai_u16 pool_4_l_pool_stride_1_const_u16 = 2;
static const ai_u16 pool_4_l_pool_stride_0_const_u16 = 2;
static const ai_u16 pool_4_t_out_0_shape_w_const_u16 = 16;
static const ai_u16 pool_4_t_out_0_shape_h_const_u16 = 16;
static const ai_i8 pool_4_t_in_0_fmt_zero_const_s8 = -128;
static const ai_i8 pool_4_t_out_0_fmt_zero_const_s8 = -128;

static const ai_i8 conv2d_5_pad_before_v_pad_constant_value_const_s8[] = LITE_ARRAY_VALUES(-128);
static const ai_i16 conv2d_5_pad_before_t_in_0_fmt_bitsize_const_s16 = 8;
static const ai_u32 conv2d_5_pad_before_t_in_0_shape_h_const_u32 = 16;

static const ai_u16 conv2d_5_t_in_0_shape_w_const_u16 = 18;
static const ai_u16 conv2d_5_t_in_0_shape_h_const_u16 = 18;
static const ai_u16 conv2d_5_t_in_0_shape_ch_const_u16 = 24;
static const ai_u16 conv2d_5_t_out_0_shape_ch_const_u16 = 32;
static const ai_i8 conv2d_5_t_in_0_fmt_zero_const_s8 = -128;
static const ai_i8 conv2d_5_t_out_0_fmt_zero_const_s8 = -128;
static const ai_float conv2d_5_t_in_0_fmt_scale_const_f32 = 0.029933137819170952f;
static const ai_float conv2d_5_t_out_0_fmt_scale_const_f32 = 0.04613778367638588f;
static const ai_float conv2d_5_t_weight_0_fmt_scale_const_f32[] = LITE_ARRAY_VALUES(0.002070534508675337f, 0.002391878981143236f, 0.002544601447880268f, 0.002780197886750102f, 0.001969496952369809f, 0.0018751189345493913f, 0.0020705002825707197f, 0.00246960436925292f, 0.0028298799879848957f, 0.002390267327427864f, 0.002066456712782383f, 0.002209051977843046f, 0.0024280340876430273f, 0.00280610634945333f, 0.002203795127570629f, 0.0021968402434140444f, 0.002475948538631201f, 0.0017714124405756593f, 0.0021102221217006445f, 0.0018989754607900977f, 0.003065199824050069f, 0.0028338246047496796f, 0.0031303130090236664f, 0.0018955257255584002f, 0.002798772417008877f, 0.0020590757485479116f, 0.0026881354860961437f, 0.003261650213971734f, 0.0030300801154226065f, 0.0024863851722329855f, 0.002641496015712619f, 0.0023984245490282774f);
static const ai_layer_format_type conv2d_5_l_out_ch_format_const_layer_format_type = AI_LAYER_FORMAT_CHANNEL_LAST_VALID;
static const ai_u16 conv2d_5_t_out_0_shape_w_const_u16 = 16;
static const ai_u16 conv2d_5_t_out_0_shape_h_const_u16 = 16;

static const ai_u16 pool_6_t_in_0_shape_w_const_u16 = 16;
static const ai_u16 pool_6_t_in_0_shape_h_const_u16 = 16;
static const ai_u16 pool_6_t_in_0_shape_ch_const_u16 = 32;
static const ai_u16 pool_6_l_pool_size_1_const_u16 = 2;
static const ai_u16 pool_6_l_pool_size_0_const_u16 = 2;
static const ai_u16 pool_6_l_legacy_pool_pad_1_const_u16 = 0;
static const ai_u16 pool_6_l_legacy_pool_pad_0_const_u16 = 0;
static const ai_u16 pool_6_l_pool_stride_1_const_u16 = 2;
static const ai_u16 pool_6_l_pool_stride_0_const_u16 = 2;
static const ai_u16 pool_6_t_out_0_shape_w_const_u16 = 8;
static const ai_u16 pool_6_t_out_0_shape_h_const_u16 = 8;
static const ai_i8 pool_6_t_in_0_fmt_zero_const_s8 = -128;
static const ai_i8 pool_6_t_out_0_fmt_zero_const_s8 = -128;



static const ai_u32 nl_13_t_in_0_shape_ch_prod_const_u32 = 5;

static const ai_u32 conversion_14_t_out_0_shape_h_w_ch_d_prod_const_u32 = 5;
static const ai_float conversion_14_t_in_0_fmt_scale_const_f32 = 0.00390625f;
static const ai_i8 conversion_14_t_in_0_fmt_zero_const_s8 = -128;
STAI_API_ENTRY
stai_return_code stai_gesture_run(
  stai_network* network,
  const stai_run_mode mode)
{
   STAI_UNUSED(mode)
  _STAI_CONTEXT_ACQUIRE(net_ctx, network)

  _STAI_SET_ERROR(net_ctx, (net_ctx->_flags & STAI_FLAG_ACTIVATIONS) != STAI_FLAG_ACTIVATIONS,
        STAI_ERROR_NETWORK_INVALID_ACTIVATIONS_PTR, net_ctx->_return_code)

  _STAI_SET_ERROR(net_ctx, (net_ctx->_flags & STAI_FLAG_INPUTS) != STAI_FLAG_INPUTS,
                  STAI_ERROR_NETWORK_INVALID_IN_PTR, net_ctx->_return_code)
  _STAI_SET_ERROR(net_ctx, (net_ctx->_flags & STAI_FLAG_OUTPUTS) != STAI_FLAG_OUTPUTS,
                  STAI_ERROR_NETWORK_INVALID_OUT_PTR, net_ctx->_return_code)

  _STAI_SET_ERROR(net_ctx, (net_ctx->_flags & STAI_FLAG_WEIGHTS) != STAI_FLAG_WEIGHTS,
                  STAI_ERROR_NETWORK_INVALID_WEIGHTS_PTR, net_ctx->_return_code)


  /* LITE_KERNEL_SECTION BEGIN conversion_0 */
  {
      const ai_float* conversion_0_t_in_0_ptr_const_f32 = (ai_float*)(net_ctx->_inputs[0] + 0);
    ai_i8* conversion_0_t_out_0_ptr_s8 = (ai_i8*)(net_ctx->_activations[0] + 72764);
  
  _STAI_GESTURE_EVENT_NODE_START_CB(0, 1, {(stai_ptr) conversion_0_t_in_0_ptr_const_f32});
    
  forward_lite_node_convert_integer_if32os8(conversion_0_t_in_0_ptr_const_f32, conversion_0_t_out_0_ptr_s8, conversion_0_t_out_0_shape_h_w_ch_d_prod_const_u32, conversion_0_t_out_0_fmt_scale_const_f32, conversion_0_t_out_0_fmt_zero_const_s8);
    
  _STAI_GESTURE_EVENT_NODE_STOP_CB(0, 1, {(stai_ptr) conversion_0_t_out_0_ptr_s8});
  }
  /* LITE_KERNEL_SECTION END conversion_0 */
  /* LITE_KERNEL_SECTION BEGIN conv2d_1 */
  {
      const ai_i8* conv2d_1_t_in_0_ptr_const_s8 = (ai_i8*)(net_ctx->_activations[0] + 72764);
    const ai_i8* conv2d_1_t_weight_0_ptr_const_s8 = (ai_i8*)(net_ctx->_weights[0] + 0);
    const ai_i32* conv2d_1_t_weight_1_ptr_const_s32 = (ai_i32*)(net_ctx->_weights[0] + 432);
    ai_i8* conv2d_1_t_out_0_ptr_s8 = (ai_i8*)(net_ctx->_activations[0] + 7228);
    ai_i16* conv2d_1_t_scratch_0_ptr_s16 = (ai_i16*)(net_ctx->_activations[0] + 0);
  
  _STAI_GESTURE_EVENT_NODE_START_CB(1, 1, {(stai_ptr) conv2d_1_t_in_0_ptr_const_s8});
    
  forward_lite_conv2d_rgb_sssa8_ch(conv2d_1_t_in_0_ptr_const_s8, conv2d_1_t_in_0_shape_w_const_u16, conv2d_1_t_weight_0_ptr_const_s8, conv2d_1_t_out_0_shape_ch_const_u16, conv2d_1_t_weight_0_shape_w_const_u16, conv2d_1_l_pad_W_0_const_s32, conv2d_1_l_stride_0_const_u16, conv2d_1_t_weight_1_ptr_const_s32, conv2d_1_t_in_0_fmt_zero_const_s8, conv2d_1_t_out_0_fmt_zero_const_s8, conv2d_1_t_in_0_fmt_scale_const_f32, conv2d_1_t_out_0_fmt_scale_const_f32, conv2d_1_t_weight_0_fmt_scale_const_f32, conv2d_1_l_out_ch_format_const_layer_format_type, conv2d_1_t_out_0_ptr_s8, conv2d_1_t_out_0_shape_w_const_u16, 1196, conv2d_1_t_scratch_0_ptr_s16);
    
  _STAI_GESTURE_EVENT_NODE_STOP_CB(1, 1, {(stai_ptr) conv2d_1_t_out_0_ptr_s8});
  }
  /* LITE_KERNEL_SECTION END conv2d_1 */
  /* LITE_KERNEL_SECTION BEGIN pool_2 */
  {
      const ai_i8* pool_2_t_in_0_ptr_const_s8 = (ai_i8*)(net_ctx->_activations[0] + 7228);
    ai_i8* pool_2_t_out_0_ptr_s8 = (ai_i8*)(net_ctx->_activations[0] + 7228);
  
  _STAI_GESTURE_EVENT_NODE_START_CB(2, 1, {(stai_ptr) pool_2_t_in_0_ptr_const_s8});
    
  forward_lite_maxpool_is8os8_scalepos(pool_2_t_in_0_ptr_const_s8, pool_2_t_out_0_ptr_s8, pool_2_t_in_0_shape_w_const_u16, pool_2_t_in_0_shape_h_const_u16, pool_2_t_in_0_shape_ch_const_u16, pool_2_l_pool_size_1_const_u16, pool_2_l_pool_size_0_const_u16, pool_2_l_legacy_pool_pad_1_const_u16, pool_2_l_legacy_pool_pad_0_const_u16, pool_2_l_pool_stride_1_const_u16, pool_2_l_pool_stride_0_const_u16, pool_2_t_out_0_shape_w_const_u16, pool_2_t_out_0_shape_h_const_u16, 1.0f, pool_2_t_in_0_fmt_zero_const_s8, pool_2_t_out_0_fmt_zero_const_s8);
    
  _STAI_GESTURE_EVENT_NODE_STOP_CB(2, 1, {(stai_ptr) pool_2_t_out_0_ptr_s8});
  }
  /* LITE_KERNEL_SECTION END pool_2 */
  /* LITE_KERNEL_SECTION BEGIN conv2d_3_pad_before */
  {
      const ai_ptr conv2d_3_pad_before_t_in_0_ptr_const_ptr = (ai_ptr)(net_ctx->_activations[0] + 7228);
    ai_ptr conv2d_3_pad_before_t_out_0_ptr_ptr = (ai_ptr)(net_ctx->_activations[0] + 66556);
  
  _STAI_GESTURE_EVENT_NODE_START_CB(3, 1, {(stai_ptr) conv2d_3_pad_before_t_in_0_ptr_const_ptr});
    
  forward_lite_pad_constant(conv2d_3_pad_before_t_in_0_ptr_const_ptr, conv2d_3_pad_before_t_out_0_ptr_ptr, (ai_handle)(conv2d_3_pad_before_v_pad_constant_value_const_s8), conv2d_3_pad_before_t_in_0_fmt_bitsize_const_s16, conv2d_3_pad_before_t_in_0_shape_h_const_u32, (ai_i32)(1), (ai_i32)(512), (ai_i32)(544), (ai_i32)(544), (ai_i32)(16), (ai_i32)(16));
    
  _STAI_GESTURE_EVENT_NODE_STOP_CB(3, 1, {(stai_ptr) conv2d_3_pad_before_t_out_0_ptr_ptr});
  }
  /* LITE_KERNEL_SECTION END conv2d_3_pad_before */
  /* LITE_KERNEL_SECTION BEGIN conv2d_3 */
  {
      const ai_i8* conv2d_3_t_in_0_ptr_const_s8 = (ai_i8*)(net_ctx->_activations[0] + 66556);
    const ai_i8* conv2d_3_t_weight_0_ptr_const_s8 = (ai_i8*)(net_ctx->_weights[0] + 496);
    const ai_i32* conv2d_3_t_weight_1_ptr_const_s32 = (ai_i32*)(net_ctx->_weights[0] + 3952);
    ai_i8* conv2d_3_t_out_0_ptr_s8 = (ai_i8*)(net_ctx->_activations[0] + 7228);
    ai_i16* conv2d_3_t_scratch_0_ptr_s16 = (ai_i16*)(net_ctx->_activations[0] + 1196);
  
  _STAI_GESTURE_EVENT_NODE_START_CB(3, 1, {(stai_ptr) conv2d_3_t_in_0_ptr_const_s8});
    
  forward_lite_conv2d_deep_3x3_sssa8_ch(conv2d_3_t_in_0_ptr_const_s8, conv2d_3_t_in_0_shape_w_const_u16, conv2d_3_t_in_0_shape_h_const_u16, conv2d_3_t_in_0_shape_ch_const_u16, conv2d_3_t_weight_0_ptr_const_s8, conv2d_3_t_out_0_shape_ch_const_u16, conv2d_3_t_weight_1_ptr_const_s32, conv2d_3_t_in_0_fmt_zero_const_s8, conv2d_3_t_out_0_fmt_zero_const_s8, conv2d_3_t_in_0_fmt_scale_const_f32, conv2d_3_t_out_0_fmt_scale_const_f32, conv2d_3_t_weight_0_fmt_scale_const_f32, conv2d_3_l_out_ch_format_const_layer_format_type, conv2d_3_t_out_0_ptr_s8, conv2d_3_t_out_0_shape_w_const_u16, conv2d_3_t_out_0_shape_h_const_u16, 1, 6032, conv2d_3_t_scratch_0_ptr_s16);
    
  _STAI_GESTURE_EVENT_NODE_STOP_CB(3, 1, {(stai_ptr) conv2d_3_t_out_0_ptr_s8});
  }
  /* LITE_KERNEL_SECTION END conv2d_3 */
  /* LITE_KERNEL_SECTION BEGIN pool_4 */
  {
      const ai_i8* pool_4_t_in_0_ptr_const_s8 = (ai_i8*)(net_ctx->_activations[0] + 7228);
    ai_i8* pool_4_t_out_0_ptr_s8 = (ai_i8*)(net_ctx->_activations[0] + 31804);
  
  _STAI_GESTURE_EVENT_NODE_START_CB(4, 1, {(stai_ptr) pool_4_t_in_0_ptr_const_s8});
    
  forward_lite_maxpool_is8os8_scalepos(pool_4_t_in_0_ptr_const_s8, pool_4_t_out_0_ptr_s8, pool_4_t_in_0_shape_w_const_u16, pool_4_t_in_0_shape_h_const_u16, pool_4_t_in_0_shape_ch_const_u16, pool_4_l_pool_size_1_const_u16, pool_4_l_pool_size_0_const_u16, pool_4_l_legacy_pool_pad_1_const_u16, pool_4_l_legacy_pool_pad_0_const_u16, pool_4_l_pool_stride_1_const_u16, pool_4_l_pool_stride_0_const_u16, pool_4_t_out_0_shape_w_const_u16, pool_4_t_out_0_shape_h_const_u16, 1.0f, pool_4_t_in_0_fmt_zero_const_s8, pool_4_t_out_0_fmt_zero_const_s8);
    
  _STAI_GESTURE_EVENT_NODE_STOP_CB(4, 1, {(stai_ptr) pool_4_t_out_0_ptr_s8});
  }
  /* LITE_KERNEL_SECTION END pool_4 */
  /* LITE_KERNEL_SECTION BEGIN conv2d_5_pad_before */
  {
      const ai_ptr conv2d_5_pad_before_t_in_0_ptr_const_ptr = (ai_ptr)(net_ctx->_activations[0] + 31804);
    ai_ptr conv2d_5_pad_before_t_out_0_ptr_ptr = (ai_ptr)(net_ctx->_activations[0] + 7228);
  
  _STAI_GESTURE_EVENT_NODE_START_CB(5, 1, {(stai_ptr) conv2d_5_pad_before_t_in_0_ptr_const_ptr});
    
  forward_lite_pad_constant(conv2d_5_pad_before_t_in_0_ptr_const_ptr, conv2d_5_pad_before_t_out_0_ptr_ptr, (ai_handle)(conv2d_5_pad_before_v_pad_constant_value_const_s8), conv2d_5_pad_before_t_in_0_fmt_bitsize_const_s16, conv2d_5_pad_before_t_in_0_shape_h_const_u32, (ai_i32)(1), (ai_i32)(384), (ai_i32)(432), (ai_i32)(432), (ai_i32)(24), (ai_i32)(24));
    
  _STAI_GESTURE_EVENT_NODE_STOP_CB(5, 1, {(stai_ptr) conv2d_5_pad_before_t_out_0_ptr_ptr});
  }
  /* LITE_KERNEL_SECTION END conv2d_5_pad_before */
  /* LITE_KERNEL_SECTION BEGIN conv2d_5 */
  {
      const ai_i8* conv2d_5_t_in_0_ptr_const_s8 = (ai_i8*)(net_ctx->_activations[0] + 7228);
    const ai_i8* conv2d_5_t_weight_0_ptr_const_s8 = (ai_i8*)(net_ctx->_weights[0] + 4048);
    const ai_i32* conv2d_5_t_weight_1_ptr_const_s32 = (ai_i32*)(net_ctx->_weights[0] + 10960);
    ai_i8* conv2d_5_t_out_0_ptr_s8 = (ai_i8*)(net_ctx->_activations[0] + 15004);
    ai_i16* conv2d_5_t_scratch_0_ptr_s16 = (ai_i16*)(net_ctx->_activations[0] + 0);
  
  _STAI_GESTURE_EVENT_NODE_START_CB(5, 1, {(stai_ptr) conv2d_5_t_in_0_ptr_const_s8});
    
  forward_lite_conv2d_deep_3x3_sssa8_ch(conv2d_5_t_in_0_ptr_const_s8, conv2d_5_t_in_0_shape_w_const_u16, conv2d_5_t_in_0_shape_h_const_u16, conv2d_5_t_in_0_shape_ch_const_u16, conv2d_5_t_weight_0_ptr_const_s8, conv2d_5_t_out_0_shape_ch_const_u16, conv2d_5_t_weight_1_ptr_const_s32, conv2d_5_t_in_0_fmt_zero_const_s8, conv2d_5_t_out_0_fmt_zero_const_s8, conv2d_5_t_in_0_fmt_scale_const_f32, conv2d_5_t_out_0_fmt_scale_const_f32, conv2d_5_t_weight_0_fmt_scale_const_f32, conv2d_5_l_out_ch_format_const_layer_format_type, conv2d_5_t_out_0_ptr_s8, conv2d_5_t_out_0_shape_w_const_u16, conv2d_5_t_out_0_shape_h_const_u16, 1, 6432, conv2d_5_t_scratch_0_ptr_s16);
    
  _STAI_GESTURE_EVENT_NODE_STOP_CB(5, 1, {(stai_ptr) conv2d_5_t_out_0_ptr_s8});
  }
  /* LITE_KERNEL_SECTION END conv2d_5 */
  /* LITE_KERNEL_SECTION BEGIN pool_6 */
  {
      const ai_i8* pool_6_t_in_0_ptr_const_s8 = (ai_i8*)(net_ctx->_activations[0] + 15004);
    ai_i8* pool_6_t_out_0_ptr_s8 = (ai_i8*)(net_ctx->_activations[0] + 7228);
  
  _STAI_GESTURE_EVENT_NODE_START_CB(6, 1, {(stai_ptr) pool_6_t_in_0_ptr_const_s8});
    
  forward_lite_maxpool_is8os8_scalepos(pool_6_t_in_0_ptr_const_s8, pool_6_t_out_0_ptr_s8, pool_6_t_in_0_shape_w_const_u16, pool_6_t_in_0_shape_h_const_u16, pool_6_t_in_0_shape_ch_const_u16, pool_6_l_pool_size_1_const_u16, pool_6_l_pool_size_0_const_u16, pool_6_l_legacy_pool_pad_1_const_u16, pool_6_l_legacy_pool_pad_0_const_u16, pool_6_l_pool_stride_1_const_u16, pool_6_l_pool_stride_0_const_u16, pool_6_t_out_0_shape_w_const_u16, pool_6_t_out_0_shape_h_const_u16, 1.0f, pool_6_t_in_0_fmt_zero_const_s8, pool_6_t_out_0_fmt_zero_const_s8);
    
  _STAI_GESTURE_EVENT_NODE_STOP_CB(6, 1, {(stai_ptr) pool_6_t_out_0_ptr_s8});
  }
  /* LITE_KERNEL_SECTION END pool_6 */
  /* LITE_KERNEL_SECTION BEGIN gemm_11 */
  {
    
  forward_lite_dense_integer_SSSA_ch_gemm_11(net_ctx);
  }
  /* LITE_KERNEL_SECTION END gemm_11 */
  /* LITE_KERNEL_SECTION BEGIN gemm_12 */
  {
    
  forward_lite_dense_integer_SSSA_ch_gemm_12(net_ctx);
  }
  /* LITE_KERNEL_SECTION END gemm_12 */
  /* LITE_KERNEL_SECTION BEGIN nl_13 */
  {
      ai_i8* nl_13_t_out_0_ptr_s8 = (ai_i8*)(net_ctx->_activations[0] + 7236);
    const ai_i8* nl_13_t_in_0_ptr_const_s8 = (ai_i8*)(net_ctx->_activations[0] + 7228);
    ai_i32* nl_13_t_scratch_0_ptr_s32 = (ai_i32*)(net_ctx->_activations[0] + 0);
  
  _STAI_GESTURE_EVENT_NODE_START_CB(13, 1, {(stai_ptr) nl_13_t_in_0_ptr_const_s8});
    
  forward_lite_nl_softmax_is8os8(nl_13_t_out_0_ptr_s8, nl_13_t_in_0_ptr_const_s8, nl_13_t_in_0_shape_ch_prod_const_u32, 1, 5, 1344188544, 25, -62, nl_13_t_scratch_0_ptr_s32);
    
  _STAI_GESTURE_EVENT_NODE_STOP_CB(13, 1, {(stai_ptr) nl_13_t_out_0_ptr_s8});
  }
  /* LITE_KERNEL_SECTION END nl_13 */
  /* LITE_KERNEL_SECTION BEGIN conversion_14 */
  {
      const ai_i8* conversion_14_t_in_0_ptr_const_s8 = (ai_i8*)(net_ctx->_activations[0] + 7236);
    ai_float* conversion_14_t_out_0_ptr_f32 = (ai_float*)(net_ctx->_outputs[0] + 0);
  
  _STAI_GESTURE_EVENT_NODE_START_CB(14, 1, {(stai_ptr) conversion_14_t_in_0_ptr_const_s8});
    
  forward_lite_node_convert_integer_is8of32(conversion_14_t_in_0_ptr_const_s8, conversion_14_t_out_0_ptr_f32, conversion_14_t_out_0_shape_h_w_ch_d_prod_const_u32, conversion_14_t_in_0_fmt_scale_const_f32, conversion_14_t_in_0_fmt_zero_const_s8);
    
  _STAI_GESTURE_EVENT_NODE_STOP_CB(14, 1, {(stai_ptr) conversion_14_t_out_0_ptr_f32});
  }
  /* LITE_KERNEL_SECTION END conversion_14 */
  return net_ctx->_return_code;
}

/*****************************************************************************/
/*  Getters APIs Section  */
STAI_API_ENTRY
stai_size stai_gesture_get_context_size()
{
  return (stai_size)STAI_GESTURE_CONTEXT_SIZE;
}

#if defined(HAVE_GESTURE_INFO)
STAI_API_ENTRY
stai_return_code stai_gesture_get_info(
  stai_network* network,
  stai_network_info* info)
{
  _STAI_CONTEXT_ACQUIRE(net_ctx, network)
  _STAI_SET_ERROR(net_ctx, info==NULL, STAI_ERROR_NETWORK_INVALID_INFO, net_ctx->_return_code)

  // Copy of network info struct
  *info = g_gesture_info;

  return STAI_SUCCESS;
}
#endif


STAI_API_ENTRY
stai_return_code stai_gesture_get_activations(
  stai_network* network, stai_ptr* activations, stai_size* n_activations)
{
  _STAI_CONTEXT_ACQUIRE(net_ctx, network)

  _STAI_SET_ERROR(net_ctx, !n_activations, STAI_ERROR_NETWORK_INVALID_API_ARGUMENTS, net_ctx->_return_code)
  *n_activations = STAI_GESTURE_ACTIVATIONS_NUM;
for (stai_size idx=0; activations && (idx<STAI_GESTURE_ACTIVATIONS_NUM); idx++) {
    // get address of the activations buffers
    activations[idx] = net_ctx->_activations[idx];
  }return net_ctx->_return_code;
}


STAI_API_ENTRY
stai_return_code stai_gesture_get_weights(
  stai_network* network, stai_ptr* weights, stai_size* n_weights)
{
  _STAI_CONTEXT_ACQUIRE(net_ctx, network)
  _STAI_SET_ERROR(net_ctx, !n_weights, STAI_ERROR_NETWORK_INVALID_API_ARGUMENTS, net_ctx->_return_code)
  *n_weights = STAI_GESTURE_WEIGHTS_NUM;
for (stai_size idx=0; weights && (idx<STAI_GESTURE_WEIGHTS_NUM); idx++) {
    // get address of the weights buffers
    weights[idx] = net_ctx->_weights[idx];
  }return net_ctx->_return_code;
}


STAI_API_ENTRY
stai_return_code stai_gesture_get_inputs(
  stai_network* network, stai_ptr* inputs, stai_size* n_inputs)
{
  _STAI_CONTEXT_ACQUIRE(net_ctx, network)
  _STAI_SET_ERROR(net_ctx, !n_inputs, STAI_ERROR_NETWORK_INVALID_API_ARGUMENTS, net_ctx->_return_code)
  *n_inputs = STAI_GESTURE_IN_NUM;
  for (stai_size idx=0; inputs && (idx<STAI_GESTURE_IN_NUM); idx++) {
    inputs[idx] = net_ctx->_inputs[idx];
  }
  return net_ctx->_return_code;
}


STAI_API_ENTRY
stai_return_code stai_gesture_get_outputs(
  stai_network* network, stai_ptr* outputs, stai_size* n_outputs)
{
  _STAI_CONTEXT_ACQUIRE(net_ctx, network)
  _STAI_SET_ERROR(net_ctx, !n_outputs, STAI_ERROR_NETWORK_INVALID_API_ARGUMENTS, net_ctx->_return_code)
  *n_outputs = STAI_GESTURE_OUT_NUM;
  for (stai_size idx=0; outputs && (idx<STAI_GESTURE_OUT_NUM); idx++) {
    outputs[idx] = net_ctx->_outputs[idx];
  }
  return net_ctx->_return_code;
}


STAI_API_ENTRY
stai_return_code stai_gesture_get_error(
  stai_network* network)
{
  _STAI_CONTEXT_ACQUIRE(net_ctx, network)

  /* return 1st generated error or STAI_SUCCESS if no errors so far */
  return net_ctx->_return_code;
}


STAI_API_ENTRY
stai_return_code stai_gesture_get_states(
  stai_network* network, stai_ptr* states, stai_size* n_states)
{
  _STAI_CONTEXT_ACQUIRE(net_ctx, network)
  _STAI_SET_ERROR(net_ctx, !n_states, STAI_ERROR_NETWORK_INVALID_API_ARGUMENTS, net_ctx->_return_code)
  /* get the number of internals states (supporting multi-heap also for internal states) */
  *n_states = STAI_GESTURE_STATES_NUM;

  STAI_UNUSED(states)
return net_ctx->_return_code;
}


/*****************************************************************************/
/*  Setters APIs Section  */

STAI_API_ENTRY
stai_return_code stai_gesture_set_activations(
  stai_network* network,
  const stai_ptr* activations,
  const stai_size n_activations)
{
  _STAI_CONTEXT_ACQUIRE(net_ctx, network)
const uintptr_t _activations_alignment[] = STAI_GESTURE_ACTIVATIONS_ALIGNMENTS;
  STAI_PRINT("  [stai_gesture_set_activations] network(%p) activations[%d]: %p\n\n", net_ctx, n_activations, activations)
  _STAI_SET_ERROR(net_ctx, !activations,
                  STAI_ERROR_NETWORK_INVALID_API_ARGUMENTS, net_ctx->_return_code)
  _STAI_SET_ERROR(net_ctx, n_activations!=STAI_GESTURE_ACTIVATIONS_NUM,
                  STAI_ERROR_NETWORK_INVALID_ACTIVATIONS_NUM, net_ctx->_return_code)

  for (stai_size idx=0; activations && idx<STAI_GESTURE_ACTIVATIONS_NUM; idx++) {
    STAI_PRINT("  activation[%d]: %p\n", idx, activations[idx])
    _STAI_SET_ERROR(net_ctx, activations[idx]==NULL,
                    STAI_ERROR_NETWORK_INVALID_ACTIVATIONS_PTR, net_ctx->_return_code)
    _STAI_SET_ERROR(net_ctx, ((uintptr_t)activations[idx]) & (_activations_alignment[idx]-1),
                    STAI_ERROR_INVALID_BUFFER_ALIGNMENT, net_ctx->_return_code)
    net_ctx->_activations[idx] = activations[idx];
  }
  net_ctx->_inputs[0] = activations[0] + 23612;

  net_ctx->_outputs[0] = activations[0] + 7244;
_stai_gesture_check(net_ctx);
  return net_ctx->_return_code;
}


STAI_API_ENTRY
stai_return_code stai_gesture_set_weights(
  stai_network* network,
  const stai_ptr* weights,
  const stai_size n_weights)
{
  _STAI_CONTEXT_ACQUIRE(net_ctx, network)
const uintptr_t _weights_alignment[] = STAI_GESTURE_WEIGHTS_ALIGNMENTS;
  _STAI_SET_ERROR(net_ctx, !weights,
                  STAI_ERROR_NETWORK_INVALID_API_ARGUMENTS, net_ctx->_return_code)
  _STAI_SET_ERROR(net_ctx, n_weights!=STAI_GESTURE_WEIGHTS_NUM,
                  STAI_ERROR_NETWORK_INVALID_WEIGHTS_NUM, net_ctx->_return_code)
  for (stai_size idx=0; weights && idx<STAI_GESTURE_WEIGHTS_NUM; idx++) {
    STAI_PRINT("  weight[%d]: %p\n", idx, weights[idx])
    _STAI_SET_ERROR(net_ctx, weights[idx]==NULL,
                    STAI_ERROR_NETWORK_INVALID_WEIGHTS_PTR, net_ctx->_return_code)
    _STAI_SET_ERROR(net_ctx, ((uintptr_t)weights[idx]) & (_weights_alignment[idx]-1),
                    STAI_ERROR_INVALID_BUFFER_ALIGNMENT, net_ctx->_return_code)
    net_ctx->_weights[idx] = weights[idx];
  }_stai_gesture_check(net_ctx);
  return net_ctx->_return_code;
}


STAI_API_ENTRY
stai_return_code stai_gesture_set_inputs(
  stai_network* network,
  const stai_ptr* inputs,
  const stai_size n_inputs)
{
  const uintptr_t _inputs_alignment[] = STAI_GESTURE_IN_ALIGNMENTS;
  _STAI_CONTEXT_ACQUIRE(net_ctx, network)
  _STAI_SET_ERROR(net_ctx, !inputs,
                  STAI_ERROR_NETWORK_INVALID_API_ARGUMENTS, net_ctx->_return_code)
  _STAI_SET_ERROR(net_ctx, n_inputs!=STAI_GESTURE_IN_NUM,
                  STAI_ERROR_NETWORK_INVALID_IN_NUM, net_ctx->_return_code)

  for (stai_size idx=0; inputs && idx<STAI_GESTURE_IN_NUM; idx++) {
    STAI_PRINT("  input[%d]: %p\n", idx, inputs[idx])
    _STAI_SET_ERROR(net_ctx, inputs[idx]==NULL,
                    STAI_ERROR_NETWORK_INVALID_IN_PTR, net_ctx->_return_code)
    _STAI_SET_ERROR(net_ctx, ((uintptr_t)inputs[idx]) & (_inputs_alignment[idx]-1),
                    STAI_ERROR_INVALID_BUFFER_ALIGNMENT, net_ctx->_return_code)
    net_ctx->_inputs[idx] = inputs[idx];
  }

  _stai_gesture_check(net_ctx);
  return net_ctx->_return_code;
}


STAI_API_ENTRY
stai_return_code stai_gesture_set_outputs(
  stai_network* network,
  const stai_ptr* outputs,
  const stai_size n_outputs)
{
  const uintptr_t _outputs_alignment[] = STAI_GESTURE_OUT_ALIGNMENTS;
  _STAI_CONTEXT_ACQUIRE(net_ctx, network)
  _STAI_SET_ERROR(net_ctx, !outputs,
                  STAI_ERROR_NETWORK_INVALID_API_ARGUMENTS, net_ctx->_return_code)
  _STAI_SET_ERROR(net_ctx, n_outputs!=STAI_GESTURE_OUT_NUM,
                  STAI_ERROR_NETWORK_INVALID_OUT_NUM, net_ctx->_return_code)

  for (stai_size idx=0; outputs && idx<n_outputs; idx++) {
    STAI_PRINT("  output[%d]: %p\n", idx, outputs[idx])
    _STAI_SET_ERROR(net_ctx, outputs[idx]==NULL,
                    STAI_ERROR_NETWORK_INVALID_OUT_PTR, net_ctx->_return_code)
    _STAI_SET_ERROR(net_ctx, ((uintptr_t)outputs[idx]) & (_outputs_alignment[idx]-1),
                    STAI_ERROR_INVALID_BUFFER_ALIGNMENT, net_ctx->_return_code)
    net_ctx->_outputs[idx] = outputs[idx];
  }

  _stai_gesture_check(net_ctx);
  return net_ctx->_return_code;
}


STAI_API_ENTRY
stai_return_code stai_gesture_set_states(
  stai_network* network,
  const stai_ptr* states,
  const stai_size n_states)
{
  _STAI_CONTEXT_ACQUIRE(net_ctx, network)

  STAI_UNUSED(states)
  STAI_UNUSED(n_states)
_stai_gesture_check(net_ctx);
  return net_ctx->_return_code;
}

STAI_API_ENTRY
stai_return_code stai_gesture_set_callback(
  stai_network* network, const stai_event_cb cb, void* cb_cookie)
{
  _STAI_CONTEXT_ACQUIRE(net_ctx, network)
  STAI_PRINT("  set_callback %p cb %p cookie %p\n", net_ctx, cb, cb_cookie)
  // _STAI_SET_ERROR(net_ctx, cb==NULL, STAI_ERROR_NETWORK_INVALID_CALLBACK, net_ctx->_return_code)
  net_ctx->_callback = cb;
  net_ctx->_callback_cookie = cb_cookie;
  return net_ctx->_return_code;
}

#undef _STAI_SET_ERROR
#undef _STAI_CONTEXT_ALIGNMENT
#undef _STAI_CONTEXT_ACQUIRE
#undef _STAI_GESTURE_EVENT_NODE_START_CB
#undef _STAI_GESTURE_EVENT_NODE_STOP_CB
#undef _STAI_GESTURE_MODEL_SIGNATURE
#undef _STAI_GESTURE_DATETIME
#undef _STAI_GESTURE_COMPILE_DATETIME

