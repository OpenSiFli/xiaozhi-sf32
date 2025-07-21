/*
 * SPDX-FileCopyrightText: 2024-2025 SiFli Technologies(Nanjing) Co., Ltd
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <rtthread.h>
#include "lwip/api.h"
#include "lwip/dns.h"
#include "lwip/apps/websocket_client.h"
#include "lwip/apps/mqtt_priv.h"
#include "lwip/apps/mqtt.h"
#include "lwip/tcpip.h"
#include "bf0_hal.h"
#include "bts2_global.h"
#include "bts2_app_pan.h"
#include "bts2_app_inc.h"
#include "ble_connection_manager.h"
#include "bt_connection_manager.h"
#include "xiaozhi.h"
#include "xiaozhi_public.h"
#ifdef LWIP_ALTCP_TLS
    #include <lwip/altcp_tls.h>
#endif

#include <webclient.h>
#include <cJSON.h>
#include "bt_env.h"

extern void xiaozhi_ui_update_ble(char *string);
extern void xiaozhi_ui_chat_status(char *string);
extern void xiaozhi_ui_chat_output(char *string);
extern void xiaozhi_ui_update_emoji(char *string);

xiaozhi_context_t g_xz_context;

#ifdef XIAOZHI_USING_MQTT
enum DeviceState mqtt_g_state;

static char message[256];
static const char *hello_message =
    "{"
    "\"type\":\"hello\","
    "\"version\": 3,"
    "\"transport\":\"udp\","
    "\"audio_params\":{"
    "\"format\":\"opus\", \"sample_rate\":16000, \"channels\":1, "
    "\"frame_duration\":60"
    "}}";

static const char *mode_str[] = {"auto", "manual", "realtime"};


void my_mqtt_connection_cb(mqtt_client_t *client, void *arg,
                           mqtt_connection_status_t status)
{
    xiaozhi_context_t *ctx = (xiaozhi_context_t *)arg;
    rt_kprintf("my_mqtt_connection_cb:%d\n", status);
    if (status == MQTT_CONNECT_ACCEPTED)
    {
        rt_sem_release(ctx->sem);
    }
    else
    {
        mqtt_g_state = kDeviceStateFatalError;
        rt_kprintf("MQTT connection failed, status: %d\n", status);
        // TODO: Reset MQTT parameters.
    }
}

static void mqtt_found_callback(const char *name, const ip_addr_t *ipaddr,
                                void *callback_arg)
{
    if (ipaddr != NULL)
    {
        xiaozhi_context_t *ctx = (xiaozhi_context_t *)callback_arg;
        rt_kprintf("DNS lookup succeeded, IP: %s\n", ipaddr_ntoa(ipaddr));
        memcpy(&(ctx->mqtt_addr), ipaddr, sizeof(ip_addr_t));
        rt_sem_release(ctx->sem);
    }
}

void my_mqtt_request_cb(void *arg, err_t err)
{
    xiaozhi_context_t *ctx = (xiaozhi_context_t *)arg;
    rt_kprintf("MQTT Request : %d\n", err);
}

void my_mqtt_request_cb2(void *arg, err_t err)
{
    xiaozhi_context_t *ctx = (xiaozhi_context_t *)arg;
    rt_kprintf("MQTT Request2 : %d\n", err);
    rt_sem_release(ctx->sem);
}

void my_mqtt_incoming_publish_cb(void *arg, const char *topic, u32_t tot_len)
{
    xiaozhi_context_t *ctx = (xiaozhi_context_t *)arg;
    xz_topic_buf_pool_t *topic_buf_pool;
    xz_topic_buf_t *buf;

    rt_kprintf("MQTT incoming topic : %d\n", tot_len);
    rt_kputs(topic);
    rt_kputs("\n");

    topic_buf_pool = &ctx->topic_buf_pool;
    buf = &topic_buf_pool->buf[topic_buf_pool->wr_idx];
    if (buf->buf)
    {
        /* pool full */
        RT_ASSERT(0);
    }

    /* allocate buffer for incoming payload */
    buf->buf = rt_malloc(tot_len);
    RT_ASSERT(buf->buf);
    buf->total_len = tot_len;
    buf->used_len = 0;

    topic_buf_pool->wr_idx = (topic_buf_pool->wr_idx + 1) & 1;
}

void my_mqtt_incoming_data_cb(void *arg, const u8_t *data, u16_t len,
                              u8_t flags)
{
    xiaozhi_context_t *ctx = (xiaozhi_context_t *)arg;
    xz_topic_buf_pool_t *topic_buf_pool;
    xz_topic_buf_t *buf;

    rt_kprintf("MQTT incoming pub data : %d, %x\n", len, flags);
    // rt_kputs(data);

    topic_buf_pool = &ctx->topic_buf_pool;
    buf = &topic_buf_pool->buf[topic_buf_pool->rd_idx];
    RT_ASSERT(buf->buf);

    RT_ASSERT(buf->used_len + len <= buf->total_len);
    memcpy(buf->buf + buf->used_len, data, len);
    buf->used_len += len;
    if (!flags)
    {
        /* wait for last fragment */
        return;
    }

    cJSON *item = NULL;
    cJSON *root = NULL;
    rt_kputs(buf->buf);
    rt_kputs("\r\n");
    root = cJSON_Parse(buf->buf); /*json_data 为MQTT的原始数据*/
    rt_kprintf("buf range: %p~%p\n", buf->buf, buf->buf + buf->total_len);
    rt_free(buf->buf);
    buf->buf = NULL;
    topic_buf_pool->rd_idx = (topic_buf_pool->rd_idx + 1) & 1;
    if (!root)
    {
        rt_kprintf("Error before: [%s]\n", cJSON_GetErrorPtr());
        return;
    }

    char *type = cJSON_GetObjectItem(root, "type")->valuestring;
    rt_kprintf("type addr: %p\n", type);
    if (strcmp(type, "hello") == 0)
    {
        cJSON *udp = cJSON_GetObjectItem(root, "udp");
        char *server = cJSON_GetObjectItem(udp, "server")->valuestring;
        int port = cJSON_GetObjectItem(udp, "port")->valueint;
        char *key = cJSON_GetObjectItem(udp, "key")->valuestring;
        char *nonce = cJSON_GetObjectItem(udp, "nonce")->valuestring;

        ip4addr_aton(server, &(ctx->udp_addr));
        ctx->port = port;
        hex2data(key, ctx->key, 16);
        hex2data(nonce, ctx->nonce, 16);

        cJSON *audio_param = cJSON_GetObjectItem(root, "audio_params");
        char *sample_rate =
            cJSON_GetObjectItem(audio_param, "sample_rate")->valuestring;
        char *duration =
            cJSON_GetObjectItem(audio_param, "duration")->valuestring;
        ctx->sample_rate = atoi(sample_rate);
        ctx->frame_duration = atoi(duration);

        char *session_id = cJSON_GetObjectItem(root, "session_id")->valuestring;
        strncpy(ctx->session_id, session_id, 9);
        mqtt_g_state = kDeviceStateIdle;
        xz_audio_init();

        xiaozhi_ui_chat_output("小智 已连接!");
        xiaozhi_ui_update_ble("open");
        xiaozhi_ui_chat_status("待命中...");
        xiaozhi_ui_update_emoji("neutral");
    }
    else if (strcmp(type, "goodbye") == 0)
    {
        mqtt_g_state = kDeviceStateUnknown;

        xiaozhi_ui_chat_output("等待唤醒...");
        xiaozhi_ui_chat_status("睡眠中...");
        xiaozhi_ui_update_emoji("sleep");
    }
    else if (strcmp(type, "tts") == 0)
    {
        char *state = cJSON_GetObjectItem(root, "state")->valuestring;
        if (strcmp(state, "start") == 0)
        {
            if (mqtt_g_state == kDeviceStateIdle ||
                mqtt_g_state == kDeviceStateListening)
            {
                mqtt_g_state = kDeviceStateSpeaking;
                xz_speaker(1);
            }
        }
        else if (strcmp(state, "stop") == 0)
        {

            mqtt_g_state = kDeviceStateIdle;
            xz_speaker(0);
        }
        else if (strcmp(state, "sentence_start") == 0)
        {
            rt_kputs(cJSON_GetObjectItem(root, "text")->valuestring);
            xiaozhi_ui_chat_output(
                cJSON_GetObjectItem(root, "text")->valuestring);
        }
    }
    else if (strcmp(type, "llm") ==
             0) // {"type":"llm", "text": "😊", "emotion": "smile"}
    {
        rt_kputs(cJSON_GetObjectItem(root, "emotion")->valuestring);
        xiaozhi_ui_update_emoji(
            cJSON_GetObjectItem(root, "emotion")->valuestring);
    }
    else
    {
    }
    cJSON_Delete(root); /*每次调用cJSON_Parse函数后，都要释放内存*/
}

void mqtt_hello(xiaozhi_context_t *ctx)
{
    rt_kprintf("Publish topic:");
    rt_kputs(ctx->publish_topic);

    rt_kprintf("\r\nhello_message:");
    rt_kputs(hello_message);
    rt_kprintf("\r\n");
    LOCK_TCPIP_CORE();
    if (mqtt_client_is_connected(&(ctx->clnt)))
    {
        mqtt_set_inpub_callback(&(ctx->clnt), my_mqtt_incoming_publish_cb,
                                my_mqtt_incoming_data_cb, ctx);
        mqtt_publish(&(ctx->clnt), ctx->publish_topic, hello_message,
                     strlen(hello_message), 0, 0, my_mqtt_request_cb, ctx);
    }
    else
    {
        xiaozhi_ui_chat_status("mqtt断开");
        xiaozhi_ui_chat_output("请重启连接");
    }
    UNLOCK_TCPIP_CORE();
}

void mqtt_listen_start(xiaozhi_context_t *ctx, int mode)
{
    rt_snprintf(message, 256,
                "{\"session_id\":\"%s\",\"type\":\"listen\",\"state\":"
                "\"start\",\"mode\":\"%s\"}",
                ctx->session_id, mode_str[mode]);
    LOCK_TCPIP_CORE();
    if (mqtt_client_is_connected(&(ctx->clnt)))
    {
        mqtt_publish(&(ctx->clnt), ctx->publish_topic, message, strlen(message),
                     0, 0, my_mqtt_request_cb2, ctx);
    }
    else
    {
        xiaozhi_ui_chat_status("mqtt断开");
        xiaozhi_ui_chat_output("请重启连接");
    }
    UNLOCK_TCPIP_CORE();
}

void mqtt_listen_stop(xiaozhi_context_t *ctx)
{
    rt_snprintf(
        message, 256,
        "{\"session_id\":\"%s\",\"type\":\"listen\",\"state\":\"stop\"}",
        ctx->session_id);
    LOCK_TCPIP_CORE();
    if (mqtt_client_is_connected(&(ctx->clnt)))
    {
        mqtt_publish(&(ctx->clnt), ctx->publish_topic, message, strlen(message),
                     0, 0, my_mqtt_request_cb2, ctx);
    }
    else
    {
        xiaozhi_ui_chat_status("mqtt断开");
        xiaozhi_ui_chat_output("请重启连接");
    }
    UNLOCK_TCPIP_CORE();
}

void mqtt_speak_abort(xiaozhi_context_t *ctx, int reason)
{
    rt_snprintf(message, 256, "{\"session_id\":\"%s\",\"type\":\"abort\"",
                ctx->session_id);
    if (reason)
        strcat(message, ",\"reason\":\"wake_word_detected\"}");
    else
        strcat(message, "}");
    LOCK_TCPIP_CORE();
    if (mqtt_client_is_connected(&(ctx->clnt)))
    {
        mqtt_publish(&(ctx->clnt), ctx->publish_topic, message, strlen(message),
                     0, 0, my_mqtt_request_cb2, ctx);
    }
    else
    {
        xiaozhi_ui_chat_status("mqtt断开");
        xiaozhi_ui_chat_output("请重启连接");
    }
    UNLOCK_TCPIP_CORE();
}

void mqtt_wake_word_detected(xiaozhi_context_t *ctx, char *wakeword)
{
    rt_snprintf(message, 256,
                "{\"session_id\":\"%s\",\"type\":\"listen\", "
                "\"state\":\"detected\",\"text\":\"%s\"",
                ctx->session_id, wakeword);
    LOCK_TCPIP_CORE();
    if (mqtt_client_is_connected(&(ctx->clnt)))
    {
        mqtt_publish(&(ctx->clnt), ctx->publish_topic, message, strlen(message),
                     0, 0, my_mqtt_request_cb2, ctx);
    }
    else
    {
        xiaozhi_ui_chat_status("mqtt断开");
        xiaozhi_ui_chat_output("请重启连接");
    }
    UNLOCK_TCPIP_CORE();
}

void mqtt_iot_descriptor(xiaozhi_context_t *ctx, char *descriptors)
{
    rt_snprintf(
        message, 256,
        "{\"session_id\":\"%s\",\"type\":\"iot\", \"descriptor\":\"%s\"",
        ctx->session_id, descriptors);
    LOCK_TCPIP_CORE();
    if (mqtt_client_is_connected(&(ctx->clnt)))
    {
        mqtt_publish(&(ctx->clnt), ctx->publish_topic, message, strlen(message),
                     0, 0, my_mqtt_request_cb2, ctx);
    }
    else
    {
        xiaozhi_ui_chat_status("mqtt断开");
        xiaozhi_ui_chat_output("请重启连接");
    }
    UNLOCK_TCPIP_CORE();
}

mqtt_client_t *mqtt_xiaozhi(xiaozhi_context_t *ctx)
{
    mqtt_client_t *clnt = &ctx->clnt;
    struct mqtt_connect_client_info_t *info = &ctx->info;
    err_t err;

    info->client_id = ctx->client_id;
    info->client_user = ctx->username;
    info->client_pass = ctx->password;
    info->keep_alive = 90;
    LOCK_TCPIP_CORE();
    err = dns_gethostbyname(ctx->endpoint, &ctx->mqtt_addr, mqtt_found_callback,
                            ctx);
    UNLOCK_TCPIP_CORE();
    if (err == ERR_OK)
    {

        rt_kprintf("mqtt_xiaozhi: DNS lookup succeeded, IP: %s\n",
                   ipaddr_ntoa(&(ctx->mqtt_addr)));
        rt_sem_release(ctx->sem);
    }
    if (err != ERR_OK && err != ERR_INPROGRESS)
    {
        rt_kprintf("Coud not find %s, please check PAN connection\n",
                   ctx->endpoint);
        clnt = NULL;
    }
    else if (RT_EOK == rt_sem_take(ctx->sem, 5000))
    {
        mqtt_g_state = kDeviceStateConnecting;
        // TODO free config when finish
        info->tls_config = altcp_tls_create_config_client(NULL, 0);
        LOCK_TCPIP_CORE();
        mqtt_client_connect(&(ctx->clnt), &(ctx->mqtt_addr),
                            LWIP_IANA_PORT_SECURE_MQTT, my_mqtt_connection_cb,
                            ctx, &ctx->info);
        UNLOCK_TCPIP_CORE();
        if (RT_EOK == rt_sem_take(ctx->sem, 10000))
        {
            mqtt_g_state = kDeviceStateIdle;
            LOCK_TCPIP_CORE();
            // ctx->info.tls_config = altcp_tls_create_config_client(NULL, 0);
            UNLOCK_TCPIP_CORE();
            mqtt_hello(ctx);
        }
        else
        {
            rt_kprintf("timeout\n");
            xiaozhi_ui_chat_output("Xiaozhi 连接超时请重启!");
            clnt = NULL;
        }
    }
    else
        clnt = NULL;

    return clnt;
}

int mqtt_http_xiaozhi_data_parse(char *json_data)
{

    uint8_t i, j;
    uint8_t result_array_size = 0;

    cJSON *item = NULL;
    cJSON *root = NULL;

    rt_kputs(json_data);
    root = cJSON_Parse(json_data); /*json_data 为MQTT的原始数据*/
    if (!root)
    {
        rt_kprintf("Error before: [%s]\n", cJSON_GetErrorPtr());
        return -1;
    }

    cJSON *Presult = cJSON_GetObjectItem(root, "mqtt"); /*mqtt的键值对为数组，*/
    result_array_size =
        cJSON_GetArraySize(Presult); /*求results键值对数组中有多少个元素*/
    item = cJSON_GetObjectItem(Presult, "endpoint");
    g_xz_context.endpoint = cJSON_Print(item);
    item = cJSON_GetObjectItem(Presult, "client_id");
    g_xz_context.client_id = cJSON_Print(item);
    item = cJSON_GetObjectItem(Presult, "username");
    g_xz_context.username = cJSON_Print(item);
    item = cJSON_GetObjectItem(Presult, "password");
    g_xz_context.password = cJSON_Print(item);
    item = cJSON_GetObjectItem(Presult, "publish_topic");
    g_xz_context.publish_topic = cJSON_Print(item);

    // Skip the "..." in string
    g_xz_context.endpoint++;
    g_xz_context.endpoint[strlen(g_xz_context.endpoint) - 1] = '\0';
    g_xz_context.client_id++;
    g_xz_context.client_id[strlen(g_xz_context.client_id) - 1] = '\0';
    g_xz_context.username++;
    g_xz_context.username[strlen(g_xz_context.username) - 1] = '\0';
    g_xz_context.password++;
    g_xz_context.password[strlen(g_xz_context.password) - 1] = '\0';
    g_xz_context.publish_topic++;
    g_xz_context.publish_topic[strlen(g_xz_context.publish_topic) - 1] = '\0';

    rt_kprintf("\r\nmqtt:\r\n\t%s\r\n\t%s\r\n\r\n", g_xz_context.endpoint,
               g_xz_context.client_id);
    rt_kprintf("\t%s\r\n\t%s\r\n", g_xz_context.username,
               g_xz_context.password);
    rt_kprintf("\t%s\r\n", g_xz_context.publish_topic);
    mqtt_xiaozhi(&g_xz_context);
    cJSON_Delete(root); /*每次调用cJSON_Parse函数后，都要释放内存*/
    return 0;
}

void xiaozhi_deinit(void)
{
    if (g_xz_context.info.tls_config)
    {
        LOCK_TCPIP_CORE();
        mqtt_disconnect(&(g_xz_context.clnt));
        UNLOCK_TCPIP_CORE();
        if (g_xz_context.info.tls_config)
        {
            LOCK_TCPIP_CORE();
            altcp_tls_free_config(g_xz_context.info.tls_config);
            UNLOCK_TCPIP_CORE();
            g_xz_context.info.tls_config = NULL;
        }
    }
//    if(!aec_is_enable())
//    {
//        xz_mic(0);
//    }
//    else
//    {
//        xz_aec_mic_close(thiz);
//    }
//    xz_speaker_close(thiz);

}

void xiaozhi(int argc, char **argv)
{
    char *my_ota_version;
    uint32_t retry = 10;

    while (retry-- > 0)
    {
        my_ota_version = get_xiaozhi();

        if (g_xz_context.info.tls_config)
        {
            LOCK_TCPIP_CORE();
            mqtt_disconnect(&(g_xz_context.clnt));
            UNLOCK_TCPIP_CORE();
            if (g_xz_context.info.tls_config)
            {
                LOCK_TCPIP_CORE();
                altcp_tls_free_config(g_xz_context.info.tls_config);
                UNLOCK_TCPIP_CORE();
                g_xz_context.info.tls_config = NULL;
            }
        }

        if (my_ota_version)
        {
            if (g_xz_context.sem == NULL)
                g_xz_context.sem = rt_sem_create("xz_sem", 0, RT_IPC_FLAG_FIFO);
            mqtt_http_xiaozhi_data_parse(my_ota_version);
            rt_free(my_ota_version);
            break;
        }
        else
        {
            rt_kprintf("Waiting internet ready(%d)... \r\n", retry);
            rt_thread_mdelay(1000);
        }
    }
}
MSH_CMD_EXPORT(xiaozhi, Get Xiaozhi)
#endif

/************************ (C) COPYRIGHT Sifli Technology *******END OF FILE****/
