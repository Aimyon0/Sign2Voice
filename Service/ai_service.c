#include "ai_service.h"
#include "app_x-cube-ai.h"
#include "ai_platform.h"
#include "gesture.h"
#include "gesture_data.h"
#include "main.h"
#include "log.h"
static uint32_t g_inf_ms = 0;
static int g_inited = 0;
ErrorCode ai_service_init(void) { MX_X_CUBE_AI_Init(); g_inited=1; return ERR_OK; }
ErrorCode ai_service_start(void) { return ERR_OK; }
ErrorCode ai_service_stop(void) { return ERR_OK; }
ErrorCode ai_service_deinit(void) { g_inited=0; return ERR_OK; }
ErrorCode ai_service_run(void) {
    if(!g_inited) return ERR_AI;
    uint32_t t0=HAL_GetTick(); MX_X_CUBE_AI_Process(); g_inf_ms=HAL_GetTick()-t0;
    return ERR_OK;
}
int ai_service_get_class(void) { return g_predicted_class; }
float ai_service_get_prob(void) { return g_predicted_prob; }
uint32_t ai_service_get_inf_ms(void) { return g_inf_ms; }
int ai_service_get_class_count(void) { return 5; }
const char* ai_service_get_class_name(int cls) { extern const char* class_names[]; return (cls>=0&&cls<5)?class_names[cls]:"?"; }
float* ai_service_get_input_buf(void) { extern float ai_input_data[64*64*3]; return ai_input_data; }
