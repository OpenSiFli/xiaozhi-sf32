/*
 * SPDX-FileCopyrightText: 2024-2025 SiFli Technologies(Nanjing) Co., Ltd
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <rtthread.h>
#include "lwip/api.h"
#include "lwip/tcpip.h"
#include "lwip/dns.h"
#include "lwip/apps/websocket_client.h"
#include "lwip/apps/mqtt_priv.h"
#include "lwip/apps/mqtt.h"
#include "xiaozhi.h"
#include "xiaozhi2.h"
#include "bf0_hal.h"
#include "bts2_global.h"
#include "bts2_app_pan.h"
#include <cJSON.h>
#include "button.h"
#include "audio_server.h"
#include <webclient.h>
#include "bt_env.h"
#include "./iot/iot_c_api.h"
#include "./mcp/mcp_api.h"
#ifdef BSP_USING_PM
    #include "gui_app_pm.h"
#endif // BSP_USING_PM
#include "xiaozhi_public.h"
#define MAX_WSOCK_HDR_LEN 4096
extern void xiaozhi_ui_update_ble(char *string);
extern void xiaozhi_ui_chat_status(char *string);
extern void xiaozhi_ui_chat_output(char *string);
extern void xiaozhi_ui_update_emoji(char *string);
extern void xiaozhi_ui_tts_output(char *string);

// IoT 模块相关
extern void iot_initialize();                              // 初始化 IoT 模块
extern void iot_invoke(const uint8_t *data, uint16_t len); // 执行远程命令
extern const char *iot_get_descriptors_json();             // 获取设备描述
extern const char *iot_get_states_json();                  // 获取设备状态

xiaozhi_ws_t g_xz_ws;
rt_mailbox_t g_button_event_mb;

enum DeviceState web_g_state;
static char message[256];
extern BOOL g_pan_connected;
extern xz_audio_t *thiz;
static const char *mode_str[] = {"auto", "manual", "realtime"};

static const char *hello_message =
    "{"
    "\"type\":\"hello\","
    "\"version\": 3,"
#ifdef CONFIG_IOT_PROTOCOL_MCP
    "\"features\":{\"mcp\":true},"
#endif
    "\"transport\":\"websocket\","
    "\"audio_params\":{"
    "\"format\":\"opus\", \"sample_rate\":16000, \"channels\":1, "
    "\"frame_duration\":60"
    "}}";

void parse_helLo(const u8_t *data, u16_t len);

void send_iot_descriptors(void)
{
    const char *desc = iot_get_descriptors_json();
    if (desc == NULL)
    {
        rt_kprintf("Failed to get IoT descriptors\n");
        return;
    }

    char msg[1024];
    snprintf(msg, sizeof(msg),
             "{\"session_id\":\"%s\",\"type\":\"iot\",\"update\":true,"
             "\"descriptors\":%s}",
             g_xz_ws.session_id, desc);

    rt_kprintf("Sending IoT descriptors:\n");
    rt_kprintf(msg);
    rt_kprintf("\n");
    if (g_xz_ws.is_connected == 1)
    {
        wsock_write(&g_xz_ws.clnt, msg, strlen(msg), OPCODE_TEXT);
    }
    else
    {
        rt_kprintf("websocket is not connected\n");
    }
}

void send_iot_states(void)
{
    const char *state = iot_get_states_json();
    if (state == NULL)
    {
        rt_kprintf("Failed to get IoT states\n");
        return;
    }

    char msg[1024];
    snprintf(msg, sizeof(msg),
             "{\"session_id\":\"%s\",\"type\":\"iot\",\"update\":true,"
             "\"states\":%s}",
             g_xz_ws.session_id, state);

    rt_kprintf("Sending IoT states:\n");
    rt_kprintf(msg);
    rt_kprintf("\n");
    if (g_xz_ws.is_connected == 1)
    {
        wsock_write(&g_xz_ws.clnt, msg, strlen(msg), OPCODE_TEXT);
    }
    else
    {
        rt_kprintf("websocket is not connected\n");
    }
}

void ws_send_speak_abort(void *ws, char *session_id, int reason)
{
    rt_kprintf("speak abort\n");
    rt_snprintf(message, 256, "{\"session_id\":\"%s\",\"type\":\"abort\"",
                session_id);
    if (reason)
        strcat(message, ",\"reason\":\"wake_word_detected\"}");
    else
        strcat(message, "}");

    if (g_xz_ws.is_connected == 1)
    {
        wsock_write((wsock_state_t *)ws, message, strlen(message), OPCODE_TEXT);
    }
    else
    {
        rt_kprintf("websocket is not connected\n");
    }
}

void ws_send_listen_start(void *ws, char *session_id, enum ListeningMode mode)
{
    rt_kprintf("listen start\n");
    rt_snprintf(message, 256,
                "{\"session_id\":\"%s\",\"type\":\"listen\",\"state\":"
                "\"start\",\"mode\":\"%s\"}",
                session_id, mode_str[mode]);
    // rt_kputs("\r\n");
    // rt_kputs(message);
    // rt_kputs("\r\n");
    if (g_xz_ws.is_connected == 1)
    {
        wsock_write((wsock_state_t *)ws, message, strlen(message), OPCODE_TEXT);
    }
    else
    {
        rt_kprintf("websocket is not connected\n");
    }
}

void ws_send_listen_stop(void *ws, char *session_id)
{
    rt_kprintf("listen stop\n");
    rt_snprintf(
        message, 256,
        "{\"session_id\":\"%s\",\"type\":\"listen\",\"state\":\"stop\"}",
        session_id);
    if (g_xz_ws.is_connected == 1)
    {
        wsock_write((wsock_state_t *)ws, message, strlen(message), OPCODE_TEXT);
    }
    else
    {
        rt_kprintf("websocket is not connected\n");
    }
}
void ws_send_hello(void *ws)
{
    if (g_xz_ws.is_connected == 1)
    {
        wsock_write((wsock_state_t *)ws, hello_message, strlen(hello_message),
                    OPCODE_TEXT);
    }
    else
    {
        rt_kprintf("websocket is not connected\n");
    }
}
void xz_audio_send_using_websocket(uint8_t *data, int len)
{
    if (g_xz_ws.is_connected == 1)
    {
        err_t err = wsock_write(&g_xz_ws.clnt, data, len, OPCODE_BINARY);
        // rt_kprintf("send audio = %d len=%d\n", err, len);
    }
    //else
    //    rt_kprintf("Websocket disconnected\n");
}

err_t my_wsapp_fn(int code, char *buf, size_t len)
{
    if (code == WS_CONNECT)
    {
        rt_kprintf("websocket connected\n");
        int status = (uint16_t)(uint32_t)buf;
        if (status == 101) // wss setup success
        {
            rt_sem_release(g_xz_ws.sem);
            g_xz_ws.is_connected = 1;
        }
    }
    else if (code == WS_DISCONNECT)
    {
        if (!g_xz_ws.is_connected)
        {
            rt_sem_release(g_xz_ws.sem);
        }
        else
        {
#ifdef BSP_USING_PM
            // 关闭 VAD
            if(thiz->vad_enabled)
            {
                thiz->vad_enabled = false;
                rt_kprintf("web_cloae,so vad_close\n");
            }
#endif      
            MCP_RGBLED_CLOSE();

            xiaozhi_ui_chat_status("休眠中...");
            xiaozhi_ui_chat_output("请按键唤醒");
            xiaozhi_ui_update_emoji("sleepy");
        }
        rt_kprintf("WebSocket closed\n");
        g_xz_ws.is_connected = 0;
    }
    else if (code == WS_TEXT)
    {
        // 打印原始数据
        rt_kprintf("web send to me:\n");
        rt_kprintf("%.*s\n", (int)len, buf); // 打印接收到的文本数据
        parse_helLo(buf, len);
    }
    else
    {
        // Receive Audio Data
        xz_audio_downlink(buf, len, NULL, 0);
    }
    return 0;
}
void xiaozhi2(int argc, char **argv);
void reconnect_websocket()
{

    if (!g_pan_connected)
    {
        xiaozhi_ui_chat_status("请开启网络共享");
        xiaozhi_ui_chat_output("请在手机上开启网络共享后重新发起连接");
        xiaozhi_ui_update_emoji("embarrassed");
        return;
    }
    // Check if the websocket is already connected
    if (g_xz_ws.clnt.pcb != NULL && g_xz_ws.clnt.pcb->state != CLOSED)
    {
        rt_kprintf("WebSocket is not in CLOSED state, cannot reconnect\n");
        return;
    }
    xiaozhi_ui_chat_status("唤醒中...");
    xiaozhi_ui_chat_output("正在唤醒小智...");
    xiaozhi_ui_update_emoji("neutral");

    err_t result;
    uint32_t retry = 10;
    while (retry-- > 0)
    {

        if (g_xz_ws.sem == NULL)
            g_xz_ws.sem = rt_sem_create("xz_ws", 0, RT_IPC_FLAG_FIFO);
        char *Client_Id = get_client_id();
        wsock_init(&g_xz_ws.clnt, 1, 1,
                   my_wsapp_fn); // 初始化websocket,注册回调函数
        result = wsock_connect(
            &g_xz_ws.clnt, MAX_WSOCK_HDR_LEN, XIAOZHI_HOST, XIAOZHI_WSPATH,
            LWIP_IANA_PORT_HTTPS, XIAOZHI_TOKEN, NULL,
            "Protocol-Version: 1\r\nDevice-Id: %s\r\nClient-Id: %s\r\n",
            get_mac_address(), Client_Id);
        rt_kprintf("Web socket connection %d\r\n", result);
        if (result == 0)
        {
            rt_kprintf("result_g_xz_ws.sem = 0%d\n", g_xz_ws.sem->value);

            if (RT_EOK == rt_sem_take(g_xz_ws.sem, 50000))
            {
                rt_kprintf("g_xz_ws.is_connected = %d\n", g_xz_ws.is_connected);
                if (g_xz_ws.is_connected)
                {
                    result = wsock_write(&g_xz_ws.clnt, hello_message,
                                         strlen(hello_message), OPCODE_TEXT);
                    rt_kprintf("Web socket write %d\r\n", result);
                    break;
                }
                else
                {
                    xiaozhi_ui_chat_status("唤醒中...");
                    xiaozhi_ui_chat_output("唤醒小智失败,请重试！");
                    xiaozhi_ui_update_emoji("neutral");
                    rt_kprintf(
                        "result = wsock_write_Web socket disconnected\r\n");
                }
            }
            else
            {
                xiaozhi_ui_chat_output("连接超时,请重试！");
                rt_kprintf("Web socket connected timeout\r\n");
            }
        }
        else
        {
            rt_kprintf("Waiting reconnect ready(%d)... \r\n", retry);
            rt_thread_mdelay(1000);
        }
    }
}
extern rt_mailbox_t g_bt_app_mb;
#define WEBSOCKET_RECONNECT 3
static void xz_button_event_handler(int32_t pin, button_action_t action) {
    static button_action_t last_action = BUTTON_RELEASED;
    if (last_action == action) return;
    last_action = action;

    if (action == BUTTON_PRESSED) {
#ifdef BSP_USING_PM
        gui_pm_fsm(GUI_PM_ACTION_WAKEUP); // 唤醒设备
#endif
        rt_kprintf("pressed\r\n");
        // 1. 检查是否处于睡眠状态（WebSocket未连接）
        if (!g_xz_ws.is_connected) {
            // 先执行唤醒（PAN重连）
#ifdef BSP_USING_PM
            rt_kprintf("web_open,so vad_enabled\n");
            if(!thiz->vad_enabled)
            {
                thiz->vad_enabled = true;
                if(aec_is_enable())
                    xz_aec_mic_open(thiz);
                else
                    xz_mic_open(thiz);
            }
#endif            
            rt_mb_send(g_bt_app_mb, PAN_RECONNECT);
            xiaozhi_ui_chat_status("唤醒中...");
        } 
        else 
        {   
            // 2. 已唤醒，直接进入对话模式
            rt_mb_send(g_button_event_mb, BUTTON_EVENT_PRESSED);
            xiaozhi_ui_chat_status("聆听中...");
        }
    } else if (action == BUTTON_RELEASED) {
#ifdef BSP_USING_PM
        gui_pm_fsm(GUI_PM_ACTION_WAKEUP);
#endif
        rt_kprintf("released\r\n");
        // 仅在已唤醒时发送停止监听
        if (g_xz_ws.is_connected) {
            rt_mb_send(g_button_event_mb, BUTTON_EVENT_RELEASED);
            xiaozhi_ui_chat_status("待命中...");
        }
    }
}
void simulate_button_pressed()
{
    if(vad_is_enable())
        xz_button_event_handler(BSP_KEY1_PIN, BUTTON_PRESSED);
}
void simulate_button_released()
{
    if(vad_is_enable())
        xz_button_event_handler(BSP_KEY1_PIN, BUTTON_RELEASED);
}
static void xz_button_init(void) // Session key
{
    static int initialized = 0;

    if (initialized == 0)
    {
        button_cfg_t cfg;
        cfg.pin = BSP_KEY1_PIN;

        cfg.active_state = BSP_KEY1_ACTIVE_HIGH;
        cfg.mode = PIN_MODE_INPUT;
        cfg.button_handler = xz_button_event_handler; // Session key
        int32_t id = button_init(&cfg);
        RT_ASSERT(id >= 0);
        RT_ASSERT(SF_EOK == button_enable(id));
        initialized = 1;
    }
}
void xz_ws_audio_init()
{
    rt_kprintf("xz_audio_init\n");
    rt_kprintf("exit sniff mode\n");
    bt_interface_exit_sniff_mode(
        (unsigned char *)&g_bt_app_env.bd_addr); // exit sniff mode
    bt_interface_wr_link_policy_setting(
        (unsigned char *)&g_bt_app_env.bd_addr,
        BT_NOTIFY_LINK_POLICY_ROLE_SWITCH); // close role switch
    audio_server_set_private_volume(AUDIO_TYPE_LOCAL_MUSIC, 8); // 设置音量
    xz_audio_decoder_encoder_open(1); // 打开音频解码器和编码器
    xz_button_init();
}
void parse_helLo(const u8_t *data, u16_t len)
{
    cJSON *item = NULL;
    cJSON *root = NULL;
    rt_kprintf(data);
    rt_kprintf("--\r\n");
    root = cJSON_Parse(data); /*json_data 为MQTT的原始数据*/
    if (!root)
    {
        rt_kprintf("Error before: [%s]\n", cJSON_GetErrorPtr());
        return;
    }

    char *type = cJSON_GetObjectItem(root, "type")->valuestring;
    rt_kprintf("type = %s\n", type);
    if (strcmp(type, "hello") == 0)
    {
        char *session_id = cJSON_GetObjectItem(root, "session_id")->valuestring;
        rt_kprintf("session_id = %s\n", session_id);
        cJSON *audio_param = cJSON_GetObjectItem(root, "audio_params");
        char *sample_rate =
            cJSON_GetObjectItem(audio_param, "sample_rate")->valuestring;
        char *duration =
            cJSON_GetObjectItem(audio_param, "duration")->valuestring;
        g_xz_ws.sample_rate = atoi(sample_rate);
        g_xz_ws.frame_duration = atoi(duration);
        strncpy(g_xz_ws.session_id, session_id, 9);
        web_g_state = kDeviceStateIdle;
        xz_ws_audio_init(); // 初始化音频
#ifndef CONFIG_IOT_PROTOCOL_MCP
        send_iot_descriptors(); // 发送iot描述
        send_iot_states();      // 发送iot状态
#endif                          // CONFIG_IOT_PROTOCOL_MCP
        xiaozhi_ui_chat_status("待命中...");
        xiaozhi_ui_chat_output("小智已连接!");
        xiaozhi_ui_update_emoji("neutral");
    }
    else if (strcmp(type, "goodbye") == 0)
    {
        web_g_state = kDeviceStateUnknown;
        rt_kprintf("session ended\n");
        xiaozhi_ui_chat_status("睡眠中...");
        xiaozhi_ui_chat_output("等待唤醒...");
        xiaozhi_ui_update_emoji("sleep");
    }
    else if (strcmp(type, "stt") == 0)
    {
        char *txt = cJSON_GetObjectItem(root, "text")->valuestring;
        xiaozhi_ui_chat_output(txt);
        web_g_state = kDeviceStateSpeaking;
        xz_speaker(1);
    }
    else if (strcmp(type, "tts") == 0)
    {
        char *txt = cJSON_GetObjectItem(root, "text")->valuestring;
        rt_kprintf(txt);
        rt_kprintf("--\r\n");

        char *state = cJSON_GetObjectItem(root, "state")->valuestring;

        if (strcmp(state, "start") == 0)
        {
            if (web_g_state == kDeviceStateIdle ||
                web_g_state == kDeviceStateListening)
            {
                web_g_state = kDeviceStateSpeaking;
                xz_speaker(1); // 打开扬声器
                xiaozhi_ui_chat_status("讲话中...");
            }
        }
        else if (strcmp(state, "stop") == 0)
        {
            web_g_state = kDeviceStateIdle;
            xz_speaker(0); // 关闭扬声器
            xiaozhi_ui_chat_status("待命中...");
        }
        else if (strcmp(state, "sentence_start") == 0)
        {
            char *txt = cJSON_GetObjectItem(root, "text")->valuestring;
            // rt_kputs(txt);
            xiaozhi_ui_tts_output(txt); // 使用专用函数处理 tts 输出
        }
        else
        {
            rt_kprintf("Unkown test: %s\n", state);
        }
    }
    else if (strcmp(type, "llm") == 0)
    {
        rt_kprintf(cJSON_GetObjectItem(root, "emotion")->valuestring);
        xiaozhi_ui_update_emoji(
            cJSON_GetObjectItem(root, "emotion")->valuestring);
    }
    else if (strcmp(type, "iot") == 0)
    {
#ifndef CONFIG_IOT_PROTOCOL_MCP
        rt_kprintf("iot command\n");
        cJSON *commands = cJSON_GetObjectItem(root, "commands");
        // rt_kprintf("commands: %s\n", cJSON_Print(commands));
        for (int i = 0; i < cJSON_GetArraySize(commands); i++)
        {
            // rt_kprintf("command %d: %s\n", i,
            // cJSON_Print(cJSON_GetArrayItem(commands, i)));
            cJSON *cmd = cJSON_GetArrayItem(commands, i);
            // rt_kprintf("cmd: %s\n", cJSON_Print(cmd));
            char *cmd_str = cJSON_PrintUnformatted(cmd);
            // rt_kprintf("cmd_str: %s\n", cmd_str);
            if (cmd_str)
            {
                iot_invoke((uint8_t *)cmd_str, strlen(cmd_str));
                send_iot_states(); // 发送 IoT 状态
                rt_free(cmd_str);
            }
        }
#endif // 定义了MCP就不走IOT
    }
    else if (strcmp(type, "mcp") == 0)
    {
        rt_kprintf("mcp command\n");
        cJSON *payload = cJSON_GetObjectItem(root, "payload");
        if (payload && cJSON_IsObject(payload))
        {
            McpServer_ParseMessage(cJSON_PrintUnformatted(payload));
        }
    }
    else
    {
        rt_kprintf("Unkown type: %s\n", type);
    }

    cJSON_Delete(root); /*每次调用cJSON_Parse函数后，都要释放内存*/
}

static void svr_found_callback(const char *name, const ip_addr_t *ipaddr,
                               void *callback_arg)
{
    if (ipaddr != NULL)
    {
        rt_kprintf("DNS lookup succeeded, IP: %s\n", ipaddr_ntoa(ipaddr));
    }
}

void xiaozhi_ws_connect(void)
{
    if (!g_pan_connected)
    {
        xiaozhi_ui_chat_status("请开启网络共享");
        xiaozhi_ui_chat_output("请在手机上开启网络共享后重新发起连接");
        xiaozhi_ui_update_emoji("embarrassed");
        return;
    }
    // 检查 WebSocket 的 TCP 控制块状态是否为 CLOSED
    if (g_xz_ws.clnt.pcb != NULL && g_xz_ws.clnt.pcb->state != CLOSED)
    {
        rt_kprintf("WebSocket is not in CLOSED state, cannot reconnect\n");
        return;
    }
    err_t err;
    uint32_t retry = 10;
    while (retry-- > 0)
    {

        if (g_xz_ws.sem == NULL)
            g_xz_ws.sem = rt_sem_create("xz_ws", 0, RT_IPC_FLAG_FIFO);

        wsock_init(&g_xz_ws.clnt, 1, 1,
                   my_wsapp_fn); // 初始化websocket,注册回调函数
        char *Client_Id = get_client_id();
        err = wsock_connect(
            &g_xz_ws.clnt, MAX_WSOCK_HDR_LEN, XIAOZHI_HOST, XIAOZHI_WSPATH,
            LWIP_IANA_PORT_HTTPS, XIAOZHI_TOKEN, NULL,
            "Protocol-Version: 1\r\nDevice-Id: %s\r\nClient-Id: %s\r\n",
            get_mac_address(), Client_Id);
        rt_kprintf("Web socket connection %d\r\n", err);
        if (err == 0)
        {
            rt_kprintf("err = 0\n");
            if (RT_EOK == rt_sem_take(g_xz_ws.sem, 50000))
            {
                rt_kprintf("g_xz_ws.is_connected = %d\n", g_xz_ws.is_connected);
                if (g_xz_ws.is_connected)
                {
                    err = wsock_write(&g_xz_ws.clnt, hello_message,
                                      strlen(hello_message), OPCODE_TEXT);
                    rt_kprintf("Web socket write %d\r\n", err);
                    break;
                }
                else
                {
                    rt_kprintf("err = wsock_write_Web socket disconnected\r\n");
                }
            }
            else
            {
                rt_kprintf("Web socket connected timeout\r\n");
            }
        }
        else
        {
            rt_kprintf("Waiting ws_connect ready%d... \r\n", retry);
            rt_thread_mdelay(1000);
        }
    }
}

int web_http_xiaozhi_data_parse(char *json_data)
{
    uint8_t i, j;
    uint8_t result_array_size = 0;
    char *endpoint;
    char *client_id;
    char *username;
    char *password;
    char *publish_topic;
    char *session;
    cJSON *item = NULL;
    cJSON *root = NULL;

    rt_kprintf(json_data);
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
    endpoint = cJSON_Print(item);
    item = cJSON_GetObjectItem(Presult, "client_id");
    client_id = cJSON_Print(item);
    item = cJSON_GetObjectItem(Presult, "username");
    username = cJSON_Print(item);
    item = cJSON_GetObjectItem(Presult, "password");
    password = cJSON_Print(item);
    item = cJSON_GetObjectItem(Presult, "publish_topic");
    publish_topic = cJSON_Print(item);

    // Skip the "..." in string
    endpoint++;
    endpoint[strlen(endpoint) - 1] = '\0';
    client_id++;
    client_id[strlen(client_id) - 1] = '\0';
    username++;
    username[strlen(username) - 1] = '\0';
    password++;
    password[strlen(password) - 1] = '\0';
    publish_topic++;
    publish_topic[strlen(publish_topic) - 1] = '\0';

    rt_kprintf("\r\nmqtt:\r\n\t%s\r\n\t%s\r\n\r\n", endpoint, client_id);
    rt_kprintf("\t%s\r\n\t%s\r\n", username, password);
    rt_kprintf("\t%s\r\n", publish_topic);
    xiaozhi_ws_connect();
    cJSON_Delete(root); /*每次调用cJSON_Parse函数后，都要释放内存*/
    return 0;
}

void xiaozhi2_deinit(void)
{
    wsock_close(&g_xz_ws.clnt, WSOCK_RESULT_LOCAL_ABORT, ERR_OK);
    g_xz_ws.is_connected = 0;
    xz_audio_decoder_encoder_close();
    rt_kprintf("%s\r\n", __func__);
}


void xiaozhi2(int argc, char **argv)
{
    char *my_ota_version;
    uint32_t retry = 10;

    if (!g_pan_connected)
    {
        xiaozhi_ui_chat_status("请开启网络共享");
        xiaozhi_ui_chat_output("请在手机上开启网络共享后重新发起连接");
        xiaozhi_ui_update_emoji("embarrassed");
        return;
    }

    while (retry-- > 0)
    {
        xiaozhi_ui_chat_output("正在网络准备...");
        my_ota_version = get_xiaozhi();
        if (my_ota_version)
        {
            rt_kprintf("my_ota_version = %s\n", my_ota_version);
            web_http_xiaozhi_data_parse(my_ota_version);
            rt_free(my_ota_version);
            break;
        }
        else
        {
            rt_kprintf("Waiting internet ready(%d)... \r\n", retry);
            xiaozhi_ui_chat_status("等待网络...");
            xiaozhi_ui_chat_output("等待网络重新准备...");
            rt_thread_mdelay(1000);
        }
    }
    if (!my_ota_version)
    {
        xiaozhi_ui_chat_output("请检查网络连接后重试");
        return;
    }
}
MSH_CMD_EXPORT(xiaozhi2, Get Xiaozhi)

/************************ (C) COPYRIGHT Sifli Technology *******END OF FILE****/
