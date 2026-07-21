#include "ui_service.h"
#include "lvgl.h"
#include "lv_port_disp.h"
#include "lv_port_indev.h"
#include "lv_port_tick.h"
#include "ui_overlay.h"
#include "log.h"
ErrorCode ui_service_init(void) { lv_init(); lv_port_disp_init(); lv_port_indev_init(); ui_overlay_init(); return ERR_OK; }
ErrorCode ui_service_start(void) { return ERR_OK; }
ErrorCode ui_service_stop(void) { return ERR_OK; }
ErrorCode ui_service_deinit(void) { return ERR_OK; }
void ui_service_tick(void) { lv_timer_handler(); }
void ui_service_update(void) { ui_overlay_update(); }
void ui_service_set_fps(const char *s) { ui_overlay_set_fps(s); }
void ui_service_set_result(const char *s) { ui_overlay_set_result(s); }
void ui_service_set_confidence(const char *s) { ui_overlay_set_confidence(s); }
void ui_service_set_status(const char *s) { ui_overlay_set_status(s); }
