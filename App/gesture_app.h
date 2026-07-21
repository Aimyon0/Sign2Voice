#ifndef GESTURE_APP_H
#define GESTURE_APP_H
#include <stdint.h>
#include "error_code.h"
void gesture_app_init(void);
void gesture_app_on_frame(void);
int gesture_app_on_result(int cls, float prob, uint32_t inf_ms);
void gesture_app_reset(void);
int gesture_app_is_busy(void);
void gesture_app_set_busy(int busy);
#endif
