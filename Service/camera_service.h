#ifndef CAMERA_SERVICE_H
#define CAMERA_SERVICE_H
#include <stdint.h>
#include "error_code.h"
ErrorCode camera_service_init(void);
ErrorCode camera_service_start(void);
ErrorCode camera_service_stop(void);
ErrorCode camera_service_deinit(void);
int camera_service_frame_ready(void);
ErrorCode camera_service_get_rgb888(float *buf);
void camera_service_invalidate_cache(void);
void camera_service_ack_frame(void);
#endif
