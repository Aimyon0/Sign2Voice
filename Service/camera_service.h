/**
 * @file    camera_service.h
 * @brief   Camera + frame-capture service.
 *          Wraps OV2640, DCMI, DMA and colour-conversion behind a simple API.
 */

#ifndef CAMERA_SERVICE_H
#define CAMERA_SERVICE_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/* ---- Init / Start ---- */

/** Init OV2640 sensor, DCMI peripheral, DMA stream.
 *  Must be called after LCD_Init() and before scheduler starts. */
int  camera_service_init(void);

/** Start continuous capture. Called from the display task. */
int  camera_service_start(void);

/* ---- Frame capture ---- */

/** Non-zero when a new RGB565 frame has been DMA-ed into RGB_DATA[][]. */
int  camera_service_frame_ready(void);

/** Convert the latest RGB565 frame to 64×64 RGB888 float32 [0,1].
 *  Writes into `buf` (must be 64*64*3 floats).
 *  Returns 0 on success. */
int  camera_service_get_rgb888(float *buf);

/** Invalidate D-Cache on the RGB_DATA buffer (ARMv7-M DSB). */
void camera_service_invalidate_cache(void);

/** Acknowledge a captured frame: reset the frame-ready flag.
 *  Call after consuming the frame to enable the next capture. */
void camera_service_ack_frame(void);

#ifdef __cplusplus
}
#endif

#endif /* CAMERA_SERVICE_H */
