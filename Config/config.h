#ifndef CONFIG_H
#define CONFIG_H
#include <stdint.h>
#include "error_code.h"
#define CONFIG_VERSION 1
#define CONFIG_DEFAULT_VOLUME 20
#define CONFIG_DEFAULT_THRESHOLD 70
#define CONFIG_DEFAULT_WINDOW_SIZE 5

typedef struct {
    uint8_t volume; uint8_t threshold; uint8_t window_size;
    uint8_t version; uint8_t _reserved[28];
} SystemConfig;

ErrorCode config_init(void);
ErrorCode config_load(void);
ErrorCode config_save(void);
void config_reset(void);
uint8_t config_get_volume(void);
uint8_t config_get_threshold(void);
uint8_t config_get_window_size(void);
void config_set_volume(uint8_t v);
void config_set_threshold(uint8_t t);
void config_set_window_size(uint8_t w);
#endif
