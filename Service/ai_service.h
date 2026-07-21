#ifndef AI_SERVICE_H
#define AI_SERVICE_H
#include <stdint.h>
#include "error_code.h"
ErrorCode ai_service_init(void);
ErrorCode ai_service_start(void);
ErrorCode ai_service_stop(void);
ErrorCode ai_service_deinit(void);
ErrorCode ai_service_run(void);
int ai_service_get_class(void);
float ai_service_get_prob(void);
uint32_t ai_service_get_inf_ms(void);
int ai_service_get_class_count(void);
const char* ai_service_get_class_name(int cls);
float* ai_service_get_input_buf(void);
#endif
