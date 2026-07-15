#ifndef __MENU_H__
#define __MENU_H__

#include <stdint.h>

typedef enum {
    MENU_RECOGNIZE = 0,
    MENU_MAIN,
    MENU_GESTURE,
    MENU_SETTINGS,
    MENU_VOLUME,
    MENU_THRESHOLD,
    MENU_BRIGHT,
    MENU_STATUS,
    MENU_ABOUT
} menu_page_t;

void  menu_init(void);
void  menu_process_key(uint8_t key_raw);
int   menu_is_recognize(void);
void  menu_enter(menu_page_t page);
void  menu_set_fps(uint32_t fps);
void  menu_set_stats(uint32_t fps, uint32_t inf_ms, uint32_t free_heap);

int   menu_get_volume(void);
int   menu_get_threshold(void);
int   menu_get_brightness(void);

#endif
