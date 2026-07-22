#include "audio_service.h"
#include "mp3.h"
#include "log.h"
static const int cls2track[5]={1,2,-1,3,4};
static int g_volume=20;
static int g_last_play=-1;
ErrorCode audio_service_init(void) { MP3_Init(); MP3_SetVolume(g_volume); g_last_play=-1; return ERR_OK; }
ErrorCode audio_service_start(void) { return ERR_OK; }
ErrorCode audio_service_stop(void) { MP3_Stop(); return ERR_OK; }
ErrorCode audio_service_deinit(void) { return ERR_OK; }
ErrorCode audio_service_set_volume(int vol) {
    if(vol<0)vol=0; if(vol>30)vol=30; g_volume=vol; MP3_SetVolume(vol); return ERR_OK;
}
int audio_service_play_class(int cls) {
    if(cls<0||cls>=5) return -1;
    int track=cls2track[cls];
    if(track>0 && track!=g_last_play){ MP3_PlayIndex(track); g_last_play=track; }
    else if(track<0) g_last_play=-1;
    return track;
}
void audio_service_reset(void) { g_last_play=-1; }
