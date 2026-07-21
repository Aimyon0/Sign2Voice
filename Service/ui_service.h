#ifndef UI_SERVICE_H
#define UI_SERVICE_H
#include "error_code.h"
ErrorCode ui_service_init(void);
ErrorCode ui_service_start(void);
ErrorCode ui_service_stop(void);
ErrorCode ui_service_deinit(void);
void ui_service_tick(void);
void ui_service_update(void);
void ui_service_set_fps(const char *s);
void ui_service_set_result(const char *s);
void ui_service_set_confidence(const char *s);
void ui_service_set_status(const char *s);
#endif
