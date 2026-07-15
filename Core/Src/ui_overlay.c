/**
 * @file ui_overlay.c
 * Sign2Voice UI Overlay — industrial dark theme, static LVGL widgets
 *
 * Design rules:
 *  - NO animations, NO screen-sized objects, NO full-refresh triggers.
 *  - All label updates use lv_label_set_text() which marks only the
 *    label's bounding box dirty → minimal SPI traffic.
 *  - Global string buffers for FPS / result / confidence / status.
 *  - ui_overlay_update() copies from globals to lv_label_set_text().
 *
 * Style:
 *  - Background: #0a0a14  (very dark blue-black)
 *  - Text:       #e0e0e0  (off-white)
 *  - Accent:     #00c8ff  (cyan-blue, for result / highlights)
 *  - Status bar: #141428  (slightly lighter dark)
 */

#include "ui_overlay.h"
#include <stdio.h>
#include <string.h>

/* ================================================================
 *  Global LVGL widget pointers
 * ================================================================ */
lv_obj_t *ui_scr_orig      = NULL;
lv_obj_t *ui_top_bar       = NULL;
lv_obj_t *ui_bottom_panel  = NULL;
lv_obj_t *ui_label_title   = NULL;
lv_obj_t *ui_label_fps     = NULL;
lv_obj_t *ui_label_result  = NULL;
lv_obj_t *ui_label_confidence = NULL;
lv_obj_t *ui_label_status  = NULL;

/* ================================================================
 *  Global state buffers (written by camera/AI task, read by UI)
 * ================================================================ */
static char ui_fps_str[16]        = "FPS --";
static char ui_result_str[32]     = "--";
static char ui_confidence_str[16] = "--%";
static char ui_status_str[20]     = "Ready";

/* ================================================================
 *  Static style pointers (created once, shared across objects)
 * ================================================================ */
static lv_style_t style_bg_dark;
static lv_style_t style_text_white;
static lv_style_t style_text_accent;
static lv_style_t style_text_small;
static lv_style_t style_text_large;

/* Colour palette */
#define COLOR_BG_DARK     lv_color_hex(0x0a0a14)
#define COLOR_BG_PANEL    lv_color_hex(0x0d0d1f)
#define COLOR_TEXT_WHITE  lv_color_hex(0xe0e0e0)
#define COLOR_ACCENT      lv_color_hex(0x00c8ff)
#define COLOR_DIM         lv_color_hex(0x606080)

/* ================================================================
 *  Internal helpers
 * ================================================================ */

/** Initialise all re-usable styles. Called once from ui_overlay_init(). */
static void ui_styles_init(void)
{
    /* ---- Panel background: very dark ---- */
    lv_style_init(&style_bg_dark);
    lv_style_set_bg_color(&style_bg_dark, COLOR_BG_DARK);
    lv_style_set_bg_opa(&style_bg_dark, LV_OPA_COVER);
    lv_style_set_border_width(&style_bg_dark, 0);
    lv_style_set_pad_all(&style_bg_dark, 0);
    lv_style_set_radius(&style_bg_dark, 0);

    /* ---- White text, 14pt ---- */
    lv_style_init(&style_text_white);
    lv_style_set_text_color(&style_text_white, COLOR_TEXT_WHITE);
    lv_style_set_text_font(&style_text_white, &lv_font_montserrat_14);

    /* ---- Accent (cyan) text, 14pt ---- */
    lv_style_init(&style_text_accent);
    lv_style_set_text_color(&style_text_accent, COLOR_ACCENT);
    lv_style_set_text_font(&style_text_accent, &lv_font_montserrat_14);

    /* ---- Small dim text, 12pt (for secondary labels) ---- */
    lv_style_init(&style_text_small);
    lv_style_set_text_color(&style_text_small, COLOR_DIM);
    lv_style_set_text_font(&style_text_small, &lv_font_montserrat_14);

    /* ---- Large result text, 24pt ---- */
    lv_style_init(&style_text_large);
    lv_style_set_text_color(&style_text_large, COLOR_ACCENT);
    lv_style_set_text_font(&style_text_large, &lv_font_montserrat_24);
}

/* ================================================================
 *  Widget creation
 * ================================================================ */

void ui_overlay_init(void)
{
    /* Initialise shared styles */
    ui_styles_init();

    lv_obj_t *scr = lv_screen_active();
    ui_scr_orig = scr;  /* 供菜单系统切回 */

    /* Transparent screen — camera image shows through */
    lv_obj_set_style_bg_opa(scr, LV_OPA_TRANSP, 0);

    /* ============================================================
     *  TOP BAR  (320 × 24)
     * ============================================================ */
    ui_top_bar = lv_obj_create(scr);
    lv_obj_set_size(ui_top_bar, UI_SCREEN_W, UI_TOP_H);
    lv_obj_set_pos(ui_top_bar, 0, 0);
    lv_obj_set_style_bg_color(ui_top_bar, COLOR_BG_DARK, 0);
    lv_obj_set_style_bg_opa(ui_top_bar, LV_OPA_COVER, 0);
    lv_obj_set_style_border_width(ui_top_bar, 0, 0);
    lv_obj_set_style_pad_all(ui_top_bar, 0, 0);
    lv_obj_set_style_radius(ui_top_bar, 0, 0);
    lv_obj_set_scrollbar_mode(ui_top_bar, LV_SCROLLBAR_MODE_OFF);
    lv_obj_clear_flag(ui_top_bar, LV_OBJ_FLAG_SCROLLABLE);

    /* ---- Title: "Sign2Voice" (left) ---- */
    ui_label_title = lv_label_create(ui_top_bar);
    lv_label_set_text(ui_label_title, "Sign2Voice");
    lv_obj_add_style(ui_label_title, &style_text_white, 0);
    lv_obj_align(ui_label_title, LV_ALIGN_LEFT_MID, 8, 0);

    /* ---- FPS (right) ---- */
    ui_label_fps = lv_label_create(ui_top_bar);
    lv_label_set_text(ui_label_fps, "FPS --");
    lv_obj_add_style(ui_label_fps, &style_text_accent, 0);
    lv_obj_align(ui_label_fps, LV_ALIGN_RIGHT_MID, -8, 0);

    /* ============================================================
     *  BOTTOM PANEL  (320 × 96)
     * ============================================================ */
    ui_bottom_panel = lv_obj_create(scr);
    lv_obj_set_size(ui_bottom_panel, UI_SCREEN_W, UI_BOTTOM_H);
    lv_obj_set_pos(ui_bottom_panel, 0, UI_SCREEN_H - UI_BOTTOM_H);
    lv_obj_set_style_bg_color(ui_bottom_panel, COLOR_BG_PANEL, 0);
    lv_obj_set_style_bg_opa(ui_bottom_panel, LV_OPA_COVER, 0);
    lv_obj_set_style_border_width(ui_bottom_panel, 0, 0);
    lv_obj_set_style_pad_all(ui_bottom_panel, 6, 0);
    lv_obj_set_style_radius(ui_bottom_panel, 0, 0);
    lv_obj_clear_flag(ui_bottom_panel, LV_OBJ_FLAG_SCROLLABLE);

    /*
     *  Bottom panel layout (68px high, manual positioning):
     *
     *  "Recognized"          (y=3,  x=8,   14pt dim)
     *  "      8      "       (y=18, x=0,   full width, 24pt accent, centre-aligned)
     *  "Confidence: 92%"     (y=46, x=8,   14pt dim)
     *  "Status: Ready"       (y=56, x=8,   14pt dim)
     *                        total: ~68px
     */

    /* "Recognized" label (static, never changes) */
    lv_obj_t *lbl_recog = lv_label_create(ui_bottom_panel);
    lv_label_set_text(lbl_recog, "Recognized");
    lv_obj_add_style(lbl_recog, &style_text_small, 0);
    lv_obj_set_pos(lbl_recog, 8, 3);

    /* Result (large, centred, changes every inference) */
    ui_label_result = lv_label_create(ui_bottom_panel);
    lv_label_set_text(ui_label_result, "--");
    lv_obj_add_style(ui_label_result, &style_text_large, 0);
    lv_obj_set_pos(ui_label_result, 0, 18);
    lv_obj_set_width(ui_label_result, UI_SCREEN_W);
    lv_obj_set_style_text_align(ui_label_result, LV_TEXT_ALIGN_CENTER, 0);

    /* Confidence */
    ui_label_confidence = lv_label_create(ui_bottom_panel);
    lv_label_set_text(ui_label_confidence, "Confidence: --%");
    lv_obj_add_style(ui_label_confidence, &style_text_small, 0);
    lv_obj_set_pos(ui_label_confidence, 8, 46);

    /* Status */
    ui_label_status = lv_label_create(ui_bottom_panel);
    lv_label_set_text(ui_label_status, "Status: Ready");
    lv_obj_add_style(ui_label_status, &style_text_small, 0);
    lv_obj_set_pos(ui_label_status, 8, 56);

    /*
     *  IMPORTANT: We do NOT create any widget that spans the camera band.
     *  The camera preview area (y=24..223) is completely empty of LVGL objects.
     *  LVGL's dirty-rect tracking will therefore never ask the flush callback
     *  to paint rows in that range.
     */
}

/* ================================================================
 *  Per-field setters (called from camera/AI context)
 * ================================================================ */

void ui_overlay_set_fps(const char *s)
{
    strncpy(ui_fps_str, s, sizeof(ui_fps_str) - 1);
    ui_fps_str[sizeof(ui_fps_str) - 1] = '\0';
}

void ui_overlay_set_result(const char *s)
{
    strncpy(ui_result_str, s, sizeof(ui_result_str) - 1);
    ui_result_str[sizeof(ui_result_str) - 1] = '\0';
}

void ui_overlay_set_confidence(const char *s)
{
    strncpy(ui_confidence_str, s, sizeof(ui_confidence_str) - 1);
    ui_confidence_str[sizeof(ui_confidence_str) - 1] = '\0';
}

void ui_overlay_set_status(const char *s)
{
    strncpy(ui_status_str, s, sizeof(ui_status_str) - 1);
    ui_status_str[sizeof(ui_status_str) - 1] = '\0';
}

/* ================================================================
 *  Periodic update — call from LVGL timer or UI task
 * ================================================================ */

void ui_overlay_update(void)
{
    if (ui_label_fps == NULL) return;  /* not initialised yet */

    lv_label_set_text(ui_label_fps,        ui_fps_str);
    lv_label_set_text(ui_label_result,     ui_result_str);
    lv_label_set_text(ui_label_confidence, ui_confidence_str);
    lv_label_set_text(ui_label_status,     ui_status_str);
}
