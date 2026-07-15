/**
 * @file ui_overlay.h
 * Sign2Voice UI Overlay — top bar + bottom panel for LVGL
 *
 * Layout (320×240 landscape):
 *  ┌──────────────────────────────────┐ y=0
 *  │ Sign2Voice              FPS 28  │ h=22  top_bar
 *  ├──────────────────────────────────┤ y=22
 *  │                                  │
 *  │       CAMERA PREVIEW             │ h=150 (DMA direct, not LVGL)
 *  │                                  │
 *  ├──────────────────────────────────┤ y=172
 *  │ Recognized                       │
 *  │         8                        │ h=68  bottom_panel
 *  │ Confidence: 92%                  │
 *  │ Status: Ready                    │
 *  └──────────────────────────────────┘ y=240
 *
 * Usage:
 *   1. ui_overlay_init()   — creates all widgets (once).
 *   2. ui_overlay_update() — reads global state and refreshes labels.
 *
 * Extension to Sign2Voice:
 *   - The result label (`ui_label_result`) accepts any string.
 *   - When switching from MNIST digit → gesture word, just call
 *     ui_overlay_set_result("HELLO") instead of ui_overlay_set_result("8").
 *   - No widget tree changes needed.
 */

#ifndef UI_OVERLAY_H
#define UI_OVERLAY_H

#ifdef __cplusplus
extern "C" {
#endif

#include "lvgl.h"

/*------------------
 *  Layout constants
 *------------------*/
#define UI_TOP_H       20     /* Top bar height (px) */
#define UI_CAMERA_Y0   20     /* Camera band starts here */
#define UI_CAMERA_H    220    /* Camera band height (px) — bigger in portrait */
#define UI_BOTTOM_H    80     /* Bottom panel height = 320-20-220 */
#define UI_SCREEN_W    240
#define UI_SCREEN_H    320

/*------------------
 *  Global LVGL objects
 *------------------*/
extern lv_obj_t *ui_top_bar;
extern lv_obj_t *ui_bottom_panel;
extern lv_obj_t *ui_label_title;
extern lv_obj_t *ui_label_fps;
extern lv_obj_t *ui_label_result;
extern lv_obj_t *ui_label_confidence;
extern lv_obj_t *ui_label_status;

/*------------------
 *  Public API
 *------------------*/

/** One-time init: create all widgets. Call after lv_port_disp_init(). */
void ui_overlay_init(void);

/** Refresh all labels from global state. Call from LVGL timer or RTOS task. */
void ui_overlay_update(void);

/* Per-field setters (called from camera/AI task) */

/** Set FPS display. s must be a static or global buffer (LVGL doesn't copy). */
void ui_overlay_set_fps(const char *s);

/** Set the recognised result. Accepts "8", "HELLO", "THANK YOU", etc. */
void ui_overlay_set_result(const char *s);

/** Set confidence string, e.g. "92%". */
void ui_overlay_set_confidence(const char *s);

/** Set status: "Ready" / "Inferencing..." / "Detected" */
void ui_overlay_set_status(const char *s);

#ifdef __cplusplus
}
#endif

#endif /* UI_OVERLAY_H */
