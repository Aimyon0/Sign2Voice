/**
 * @file    ui_service.h
 * @brief   LVGL display service.
 *          Initialises LVGL, creates the transparent overlay, refreshes labels.
 */

#ifndef UI_SERVICE_H
#define UI_SERVICE_H

#ifdef __cplusplus
extern "C" {
#endif

/* ---- Init ---- */

/** Call after lv_init() / lv_port_disp_init() / lv_port_indev_init().
 *  Creates the top-bar + bottom-panel overlay. */
int  ui_service_init(void);

/* ---- Periodic ---- */

/** Call from the display task (e.g. every 2 ms) to drive LVGL. */
void ui_service_tick(void);

/** Full update: read internal labels and push to screen. */
void ui_service_update(void);

/* ---- Setters (thread-safe via LVGL task lock if needed) ---- */

void ui_service_set_fps(const char *s);
void ui_service_set_result(const char *s);
void ui_service_set_confidence(const char *s);
void ui_service_set_status(const char *s);

#ifdef __cplusplus
}
#endif

#endif /* UI_SERVICE_H */
