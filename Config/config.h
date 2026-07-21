#ifndef CONFIG_H
#define CONFIG_H
#include <stdint.h>
#include "error_code.h"
#define CONFIG_DEFAULT_VOLUME 20
#define CONFIG_DEFAULT_THRESHOLD 70
#define CONFIG_DEFAULT_WINDOW_SIZE 5
typedef struct { uint8_t v,t,w,ver; uint8_t _r[28]; } SystemConfig;
ErrorCode config_init(void);
ErrorCode config_save(void);
uint8_t config_get_volume(void);
uint8_t config_get_threshold(void);
uint8_t config_get_window_size(void);
void config_set_volume(uint8_t v);
void config_set_threshold(uint8_t t);
void config_set_window_size(uint8_t w);
#endif
