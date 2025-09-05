#ifndef XIAOZHI_H
#define XIAOZHI_H

    #define GET_HEADER_BUFSZ 1024             // 头部大小
    #define GET_RESP_BUFSZ 1024               // 响应缓冲区大小
    #define GET_URL_LEN_MAX 256               // 网址最大长度
    #define GET_URI "https://%s/xiaozhi/ota/" // 获取小智版本
    #define XIAOZHI_HOST "api.tenclass.net"
    #define XIAOZHI_WSPATH "/xiaozhi/v1/"
    #define XIAOZHI_TOKEN "Bearer 12345678"

enum ListeningMode
{
    kListeningModeAutoStop,
    kListeningModeManualStop,
    kListeningModeAlwaysOn // 需要 AEC 支持
};

enum AbortReason
{
    kAbortReasonNone,
    kAbortReasonWakeWordDetected
};

enum DeviceState
{
    kDeviceStateUnknown,
    kDeviceStateStarting,
    kDeviceStateWifiConfiguring,
    kDeviceStateIdle,
    kDeviceStateConnecting,
    kDeviceStateListening,
    kDeviceStateSpeaking,
    kDeviceStateUpgrading,
    kDeviceStateActivating,
    kDeviceStateFatalError
};
    #ifdef XIAOZHI_USING_MQTT
extern enum DeviceState mqtt_g_state;
    #else
extern enum DeviceState web_g_state;
    #endif
typedef struct
{
    uint16_t total_len;
    uint16_t used_len;
    uint8_t *buf;
} xz_topic_buf_t;

typedef struct
{
    uint8_t rd_idx;
    uint8_t wr_idx;
    xz_topic_buf_t buf[2];
} xz_topic_buf_pool_t;

/**
 * @brief xiaozhi cntext 数据结构体
 */
typedef struct
{
    char *endpoint;
    char *client_id;
    char *username;
    char *password;
    char *publish_topic;
    char *session;
    ip_addr_t mqtt_addr;
    ip_addr_t udp_addr;
    uint16_t port;
    uint8_t session_id[12];
    uint8_t key[16];
    uint8_t nonce[16];
    uint32_t sample_rate;
    uint32_t frame_duration;

    mqtt_client_t clnt;
    uint32_t local_sequence;
    uint32_t remote_sequence;
    struct mqtt_connect_client_info_t info;
    rt_sem_t sem;
    xz_topic_buf_pool_t topic_buf_pool;
} xiaozhi_context_t;

extern xiaozhi_context_t g_xz_context;
void mqtt_hello(xiaozhi_context_t *ctx);
void mqtt_listen_start(xiaozhi_context_t *ctx, int mode);
void mqtt_listen_stop(xiaozhi_context_t *ctx);
void mqtt_speak_abort(xiaozhi_context_t *ctx, int reason);
void mqtt_wake_word_detected(xiaozhi_context_t *ctx, char *wakeword);
void mqtt_iot_descriptor(xiaozhi_context_t *ctx, char *descriptors);
void xiaozhi(int argc, char **argv);

char *get_mac_address(void);
void xz_audio_init(void);
void xz_mic(int on);
void xz_speaker(int on);
void xz_audio_open(void);
void xz_audio_close(void);
void xz_audio_decoder_encoder_open(uint8_t is_websocket);
void ws_send_listen_start(void *ws, char *session_id, enum ListeningMode mode);
void ws_send_listen_stop(void *ws, char *session_id);
void xz_audio_decoder_encoder_close(void);
void xz_audio_downlink(uint8_t *data, uint32_t size, uint32_t *aes_value,
                       uint8_t need_aes);
void xz_audio_send_using_websocket(uint8_t *data, int len);
void xz_set_lcd_brightness(uint16_t level);
#endif
/************************ (C) COPYRIGHT Sifli Technology *******END OF FILE****/
