#ifndef XIAOZHI_PUBLIC_H
#define XIAOZHI_PUBLIC_H


#include <rtthread.h>
#include "lwip/api.h"
#include "lwip/dns.h"
#include "lwip/apps/mqtt_priv.h"
#include "lwip/apps/mqtt.h"
#include "lwip/udp.h"
#include "lwip/tcpip.h"
#include "xiaozhi.h"
#include "bf0_hal.h"
#include "button.h"
#include "os_adaptor.h"
#include "opus_types.h"
#include "opus_multistream.h"
#include "os_support.h"
#include "audio_server.h"
#include "mem_section.h"
#ifdef PKG_XIAOZHI_USING_AEC
    #include "webrtc/common_audio/vad/include/webrtc_vad.h"
    #include "sifli_resample.h"
#endif
#include "bts2_app_inc.h"
#include "ble_connection_manager.h"
#include "bt_connection_manager.h"
#include "bt_env.h"
#include "cJSON.h"


 #define PAN_RECONNECT 4
 #define XZ_DOWNLINK_QUEUE_NUM 128
 #define XZ_MIC_FRAME_LEN (320 * 6) // 60ms for 16k samplerate
 #define XZ_SPK_FRAME_LEN (480 * 6) // 60ms for 24k samplerate, speaker frame len
 
// 可复用函数
char *get_mac_address(void);
void hash_run(uint8_t algo, uint8_t *raw_data, uint32_t raw_data_len,
              uint8_t *result, uint32_t result_len);
void hex_2_asc(uint8_t n, char *str);
char *get_client_id();
int check_internet_access();
char *get_xiaozhi();
char *my_json_string(cJSON *json, char *key);


typedef struct
{
    rt_slist_t node;
    uint8_t *data;
    uint16_t data_len;
    uint16_t size;
} xz_decode_queue_t;

typedef struct
{
    struct rt_thread thread;
    uint8_t encode_in[XZ_MIC_FRAME_LEN];
    uint8_t encode_out[XZ_MIC_FRAME_LEN];
    bool inited;
    bool is_rx_enable;
    bool is_tx_enable;
    bool is_exit;
    uint8_t is_websocket; // 0: mqtt, 1: websocket
    rt_event_t event;
    OpusEncoder *encoder;
    OpusDecoder *decoder;
    audio_client_t speaker;
    audio_client_t mic;
#if PKG_XIAOZHI_USING_AEC
    sifli_resample_t *resample;
    VadInst *handle;
    int voice_state;
    uint32_t voice_start_times;
    uint32_t voice_stop_times;
#endif
    uint32_t mic_rx_count;
    struct rt_ringbuffer *rb_opus_encode_input;
    xz_decode_queue_t downlink_queue[XZ_DOWNLINK_QUEUE_NUM];
    uint16_t downlink_decode_out[XZ_SPK_FRAME_LEN / 2];
    rt_slist_t downlink_decode_busy;
    rt_slist_t downlink_decode_idle;
    bool vad_enabled;
} xz_audio_t;

void xz_aec_mic_close(xz_audio_t *thiz);
void xz_aec_mic_open(xz_audio_t *thiz);

#endif // XIAOZHI_PUBLIC_H