/**
 * @file    audio_service.h
 * @brief   Audio playback service wrapping the MP3-TF-16P module (YX5200).
 */

#ifndef AUDIO_SERVICE_H
#define AUDIO_SERVICE_H

#ifdef __cplusplus
extern "C" {
#endif

/* ---- Init ---- */

/** Init the MP3 module (USART1, 9600 bps).
 *  Call once before scheduler starts. */
int  audio_service_init(void);

/* ---- Playback ---- */

/** Set volume 0-30. */
int  audio_service_set_volume(int vol);

/** Map gesture class (0-4) to an audio track index and play it.
 *  Returns the track number played, or -1 if the class is muted (no_gesture). */
int  audio_service_play_class(int cls);

#ifdef __cplusplus
}
#endif

#endif /* AUDIO_SERVICE_H */
