/**
 * @file lv_port_indev.h
 * LVGL v9.x Input Device Port for STM32H743
 *
 * Currently provides a skeleton / placeholder for future touch support.
 * When a touch controller (e.g. XPT2046, FT5x06, GT911) is added,
 * implement the read callback and call lv_port_indev_init().
 */

#ifndef LV_PORT_INDEV_H
#define LV_PORT_INDEV_H

#ifdef __cplusplus
extern "C" {
#endif

#include "lvgl.h"

/**
 * @brief  Initialize LVGL input device(s).
 * @note   Currently a skeleton — no physical touch driver is present.
 *         Call once after lv_port_disp_init() when touch hardware is ready.
 * @return lv_indev_t* pointer to the created input device, or NULL.
 */
lv_indev_t *lv_port_indev_init(void);

#ifdef __cplusplus
}
#endif

#endif /* LV_PORT_INDEV_H */
