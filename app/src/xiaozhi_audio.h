#ifndef XIAOZHI_H
#define XIAOZHI_H

#ifdef __cplusplus
extern "C" {
#endif

void xz_mqtt_button_init(void);
void xz_audio_init(void);
void xz_speaker_open(xz_audio_t *thiz);
void xz_speaker_close(xz_audio_t *thiz);
void xz_mic_open(xz_audio_t *thiz);
void xz_mic_close(xz_audio_t *thiz);
void reinit_audio(void);


#ifdef __cplusplus
}
#endif


#endif