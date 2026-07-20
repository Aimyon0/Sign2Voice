/**
 * @file    gesture_app.h
 * @brief   Gesture recognition application pipeline.
 *          Sliding-window filter + AI inference trigger + audio feedback.
 */

#ifndef GESTURE_APP_H
#define GESTURE_APP_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/** One-time init. Resets internal sliding-window state. */
void gesture_app_init(void);

/** Called when a new camera frame is ready.
 *  Triggers AI inference if state machine is idle. */
void gesture_app_on_frame(void);

/** Called by the display task to process a completed inference result.
 *  Applies sliding-window consensus, updates UI, triggers MP3.
 *  @param cls     predicted class (0..4)
 *  @param prob    confidence [0, 1]
 *  @param inf_ms  inference latency in ms
 *  @return        1 if a gesture was confirmed, 0 otherwise */
int gesture_app_on_result(int cls, float prob, uint32_t inf_ms);

/** Reset sliding-window state (e.g. when returning from menu). */
void gesture_app_reset(void);

/** Non-zero when the inference pipeline is busy. */
int  gesture_app_is_busy(void);
void gesture_app_set_busy(int busy);

#ifdef __cplusplus
}
#endif

#endif /* GESTURE_APP_H */
