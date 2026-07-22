#ifndef AUDIO_SERVICE_H
#define AUDIO_SERVICE_H
#include "error_code.h"
ErrorCode audio_service_init(void);
ErrorCode audio_service_start(void);
ErrorCode audio_service_stop(void);
ErrorCode audio_service_deinit(void);
ErrorCode audio_service_set_volume(int vol);
int audio_service_play_class(int cls);
void audio_service_reset(void);
#endif
