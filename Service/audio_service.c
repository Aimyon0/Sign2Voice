/**
 * @file    audio_service.c
 * @brief   Audio service — wraps MP3-TF-16P (YX5200) playback.
 */

#include "audio_service.h"
#include "mp3.h"

/* Gesture class → audio track index.
 *   class 0 = fist   → track 1
 *   class 1 = like   → track 2
 *   class 2 = no_ges → no audio  (-1)
 *   class 3 = ok     → track 3
 *   class 4 = palm   → track 4   */
static const int cls2track[5] = {1, 2, -1, 3, 4};

static int g_volume = 20;

/* ---- Public API ---- */

int audio_service_init(void)
{
    MP3_Init();
    MP3_SetVolume(g_volume);
    return 0;
}

int audio_service_set_volume(int vol)
{
    if (vol < 0)  vol = 0;
    if (vol > 30) vol = 30;
    g_volume = vol;
    MP3_SetVolume(vol);
    return 0;
}

int audio_service_play_class(int cls)
{
    if (cls < 0 || cls >= 5) return -1;
    int track = cls2track[cls];
    if (track > 0) {
        MP3_PlayIndex(track);
    }
    return track;
}
