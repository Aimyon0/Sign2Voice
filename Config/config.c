#include "config.h"
#include "log.h"
static SystemConfig g_cfg;
ErrorCode config_init(void) { g_cfg.v=20; g_cfg.t=70; g_cfg.w=5; return ERR_OK; }
ErrorCode config_save(void) { return ERR_OK; }
uint8_t config_get_volume(void) { return g_cfg.v; }
uint8_t config_get_threshold(void) { return g_cfg.t; }
uint8_t config_get_window_size(void) { return g_cfg.w; }
void config_set_volume(uint8_t v) { if(v<=30)g_cfg.v=v; }
void config_set_threshold(uint8_t t) { if(t>=50&&t<=95)g_cfg.t=t; }
void config_set_window_size(uint8_t w) { if(w==3||w==5||w==7)g_cfg.w=w; }
