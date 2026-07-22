#include "gesture_app.h"
#include "menu.h"
#include "config.h"
#include "ai_service.h"
#include "ui_service.h"
#include "audio_service.h"
#include "log.h"
#include <stdio.h>
static int sw_last=-1, sw_cnt=0, g_busy=0;
static uint32_t g_cooldown=0;
#define COOLDOWN_MS 2000
void gesture_app_init(void) { sw_last=-1; sw_cnt=0; g_busy=0; g_cooldown=0; audio_service_reset(); }
void gesture_app_on_frame(void) { if(menu_is_recognize()&&!g_busy){} }
int gesture_app_on_result(int cls, float prob, uint32_t inf_ms) {
    if(!menu_is_recognize()) return 0;
    if(cls<0||cls>=ai_service_get_class_count()) return 0;
    if(g_cooldown > HAL_GetTick()) return 0;
    int sw_n=config_get_window_size();
    float thr=config_get_threshold()/100.0f;
    if(prob>=thr){if(cls==sw_last)sw_cnt++; else{sw_last=cls;sw_cnt=1;}}
    if(sw_cnt>=sw_n){
        ui_service_set_result(ai_service_get_class_name(cls));
        char c[16]; snprintf(c,sizeof(c),"%.0f%%",prob*100.0f);
        ui_service_set_confidence(c); ui_service_set_status("Detected");
        sw_cnt=0; sw_last=-1;
        audio_service_play_class(cls);
        g_cooldown=HAL_GetTick()+COOLDOWN_MS;
        return 1;
    }
    return 0;
}
void gesture_app_reset(void) { sw_last=-1; sw_cnt=0; audio_service_reset(); }
int gesture_app_is_busy(void) { return g_busy; }
void gesture_app_set_busy(int busy) { g_busy=busy; }
