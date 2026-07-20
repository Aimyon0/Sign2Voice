/**
 * @file    gesture_app.c
 * @brief   Gesture recognition pipeline — extracted from freertos.c.
 *
 * Data flow:
 *   camera frame → gesture_app_on_frame() → frameQueue msg
 *   → info task runs AI → resultQueue mail
 *   → gesture_app_on_result() → sliding-window → UI update + MP3 trigger
 */

#include "gesture_app.h"
#include "menu.h"
#include "ai_service.h"
#include "ui_service.h"
#include "audio_service.h"
#include "camera_service.h"

#include <string.h>
#include <stdio.h>

/* ---- Sliding-window state ---- */
static int   sw_last    = -1;
static int   sw_cnt     = 0;
static int   last_play  = -1;
static int   g_busy     = 0;  /* READY=0, INFER=1 */

/* ---- Public API ---- */

void gesture_app_init(void)
{
    sw_last   = -1;
    sw_cnt    = 0;
    last_play = -1;
    g_busy    = 0;
}

void gesture_app_on_frame(void)
{
    if (menu_is_recognize() && !g_busy) {
        /* Signal the inference task via frameQueue.
         * frameQueue is an osMessageQ declared in freertos.c.
         * We access it through an extern → see gesture_app_on_frame
         * is called from the display task which owns the queue handle. */
    }
}

int gesture_app_on_result(int cls, float prob, uint32_t inf_ms)
{
    if (!menu_is_recognize()) return 0;

    int sw_n = menu_get_brightness();  /* window size: 3 / 5 / 7 */

    /* Low-confidence frames are skipped */
    if (prob >= (menu_get_threshold() / 100.0f)) {
        if (cls == sw_last) sw_cnt++;
        else               { sw_last = cls; sw_cnt = 1; }
    }

    if (sw_cnt >= sw_n) {
        /* ---- Gesture confirmed ---- */
        if (cls >= 0 && cls < ai_service_get_class_count()) {
            ui_service_set_result(ai_service_get_class_name(cls));
        }
        char c[16];
        snprintf(c, sizeof(c), "%.0f%%", prob * 100.0f);
        ui_service_set_confidence(c);
        ui_service_set_status("Detected");
        sw_cnt = 0;
        sw_last = -1;

        /* Map class to audio track and play */
        int track = audio_service_play_class(cls);
        if (track > 0 && track != last_play) {
            last_play = track;
        } else if (track < 0) {
            last_play = -1;
        }
        return 1;
    }

    return 0;
}

void gesture_app_reset(void)
{
    sw_last   = -1;
    sw_cnt    = 0;
    last_play = -1;
}

int  gesture_app_is_busy(void)       { return g_busy; }
void gesture_app_set_busy(int busy)  { g_busy = busy; }
