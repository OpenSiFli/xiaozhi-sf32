/*
 * SPDX-FileCopyrightText: 2024-2025 SiFli Technologies(Nanjing) Co., Ltd
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "rtthread.h"
#include "bf0_hal.h"
#include "drv_io.h"
#include "stdio.h"
#include "string.h"
#include "xiaozhi2.h"
#include "./iot/iot_c_api.h"
#ifdef BSP_USING_PM
    #include "gui_app_pm.h"
#endif // BSP_USING_PM
#include "xiaozhi_public.h"
#include "bf0_pm.h"
#include <drivers/rt_drv_encoder.h>
extern void xiaozhi_ui_update_ble(char *string);
extern void xiaozhi_ui_update_emoji(char *string);
extern void xiaozhi_ui_chat_status(char *string);
extern void xiaozhi_ui_chat_output(char *string);

extern void xiaozhi_ui_task(void *args);
extern void xiaozhi(int argc, char **argv);
extern void xiaozhi2(int argc, char **argv);
extern void reconnect_websocket();
extern xiaozhi_ws_t g_xz_ws;
extern rt_mailbox_t g_button_event_mb;
rt_mailbox_t g_battery_mb;
/* Common functions for RT-Thread based platform
 * -----------------------------------------------*/
/**
 * @brief  Initialize board default configuration.
 * @param  None
 * @retval None
 */
void HAL_MspInit(void)
{
    //__asm("B .");        /*For debugging purpose*/
    BSP_IO_Init();
#ifdef BSP_USING_BOARD_SF32LB52_XTY_AI
    HAL_PIN_Set(PAD_PA38, GPTIM1_CH1, PIN_PULLUP, 1);
    HAL_PIN_Set(PAD_PA40, GPTIM1_CH2, PIN_PULLUP, 1);
#endif
}
/* User code start from here
 * --------------------------------------------------------*/
#include "bts2_app_inc.h"
#include "ble_connection_manager.h"
#include "bt_connection_manager.h"
#include "bt_env.h"
#include "ulog.h"

#define BT_APP_READY 0
#define BT_APP_CONNECT_PAN 1
#define BT_APP_CONNECT_PAN_SUCCESS 2
#define WEBSOCKET_RECONNECT 3
#define KEEP_FIRST_PAN_RECONNECT 5
#define XZ_CONFIG_UPDATE        6
#define BT_APP_PHONE_DISCONNECTED 7    // 手机主动断开
#define BT_APP_ABNORMAL_DISCONNECT 8   // 异常断开
#define BT_APP_RECONNECT_TIMEOUT 9    // 重连超时
#define BT_APP_RECONNECT 10 // 重连
#define PAN_TIMER_MS 3000

bt_app_t g_bt_app_env;
rt_mailbox_t g_bt_app_mb;
BOOL g_pan_connected = FALSE;
BOOL first_pan_connected = FALSE;
int first_reconnect_attempts = 0;

static rt_timer_t s_reconnect_timer = NULL;
static rt_timer_t s_sleep_timer = NULL;
static int reconnect_attempts = 0;
#define MAX_RECONNECT_ATTEMPTS 30  // 30次尝试，每次1秒，共30秒
static uint8_t g_sleep_enter_flag = 0;    // 进入睡眠标志位

#ifdef BSP_USING_BOARD_SF32LB52_XTY_AI
static rt_timer_t s_pulse_encoder_timer = NULL;
static struct rt_device *s_encoder_device;

static int pulse_encoder_init(void)
{
    s_encoder_device = rt_device_find("encoder1");
    if (s_encoder_device == RT_NULL)
    {
        LOG_E("Failed to find encoder device\n");
        return -RT_ERROR;
    }
    struct rt_encoder_configuration config;
    config.channel = GPT_CHANNEL_ALL;

    rt_err_t result =
        rt_device_control((struct rt_device *)s_encoder_device,
                          PULSE_ENCODER_CMD_ENABLE, (void *)&config); // 使能

    if (result != RT_EOK)
    {
        rt_kprintf("Failed to enable encoder\n");
        return -RT_ERROR;
    }
    return RT_EOK;
}

static void pulse_encoder_timeout_handle(void *parameter)
{
    static int32_t last_count = 0;
    rt_err_t result;
    struct rt_encoder_configuration config_count;
    config_count.get_count = 0;
    result =
        rt_device_control((struct rt_device *)s_encoder_device,
                          PULSE_ENCODER_CMD_GET_COUNT, (void *)&config_count);
    if (result != RT_EOK)
    {
        LOG_E("Failed to get encoder count\n");
        return;
    }

    int32_t current_count = config_count.get_count;
    int32_t delta_count = current_count - last_count;
    last_count = current_count;
    // delta_count
    // 是以4为单位的增量，不能被4整除不算一次脉冲，需要先算出有多少增量
    // 注意正数是顺时针转动，负数是逆时针转动
    if (delta_count % 4 != 0)
    {
        LOG_W("Encoder count is not a multiple of 4, delta_count: %d\n",
              delta_count);
        return;
    }
    int32_t pulses = delta_count / 4; // 每次脉冲是4个单位
    if (pulses != 0)
    {
        LOG_I("Pulse encoder count: %d\n", pulses);
        int current_volume =
            audio_server_get_private_volume(AUDIO_TYPE_LOCAL_MUSIC);
        int new_volume = current_volume + pulses;
        if (new_volume < 0)
        {
            new_volume = 0; // 最小音量为0
        }
        else if (new_volume > 15)
        {
            new_volume = 15; // 最大音量为15
        }

        audio_server_set_private_volume(AUDIO_TYPE_LOCAL_MUSIC, new_volume);
    }
}
#endif

static void battery_level_task(void *parameter)
{
    g_battery_mb = rt_mb_create("battery_level", 1, RT_IPC_FLAG_FIFO);
    if (g_battery_mb == NULL)
    {
        rt_kprintf("Failed to create mailbox g_battery_mb\n");
        return;
    }
    while (1)
    {
        rt_device_t battery_device = rt_device_find("bat1");
        rt_adc_cmd_read_arg_t read_arg;
        read_arg.channel = 7; // 电池电量在通道7
        rt_err_t result =
            rt_adc_enable((rt_adc_device_t)battery_device, read_arg.channel);
        if (result != RT_EOK)
        {
            LOG_E("Failed to enable ADC for battery read\n");
            return;
        }
        rt_uint32_t battery_level =
            rt_adc_read((rt_adc_device_t)battery_device, read_arg.channel);
        rt_adc_disable((rt_adc_device_t)battery_device, read_arg.channel);

        // 获取到的是电池电压，单位是mV
        // 假设电池电压范围是3.6V到4.2V，对应的电量范围是0%到100%
        uint32_t battery_percentage = 0;
        if (battery_level < 3600)
        {
            battery_percentage = 0; // 小于3.6V，电量为0
        }
        else if (battery_level > 4200)
        {
            battery_percentage = 100; // 大于4.2V，电量为100
        }
        else
        {
            // 线性插值计算电量百分比
            battery_percentage = ((battery_level - 3600) * 100) / (4200 - 3600);
        }

        rt_mb_send(g_battery_mb, battery_percentage);
        rt_thread_mdelay(5000);
    }
}

void bt_app_connect_pan_timeout_handle(void *parameter)
{
    LOG_I("bt_app_connect_pan_timeout_handle %x, %d", g_bt_app_mb,
          g_bt_app_env.bt_connected);
    if ((g_bt_app_mb != NULL) && (g_bt_app_env.bt_connected))
        rt_mb_send(g_bt_app_mb, BT_APP_CONNECT_PAN);
    return;
}

#if defined(BSP_USING_SPI_NAND) && defined(RT_USING_DFS)
    #include "dfs_file.h"
    #include "dfs_posix.h"
    #include "drv_flash.h"
    #define NAND_MTD_NAME "root"
int mnt_init(void)
{
    // TODO: how to get base address
    register_nand_device(FS_REGION_START_ADDR & (0xFC000000),
                         FS_REGION_START_ADDR -
                             (FS_REGION_START_ADDR & (0xFC000000)),
                         FS_REGION_SIZE, NAND_MTD_NAME);
    if (dfs_mount(NAND_MTD_NAME, "/", "elm", 0, 0) == 0) // fs exist
    {
        rt_kprintf("mount fs on flash to root success\n");
    }
    else
    {
        // auto mkfs, remove it if you want to mkfs manual
        rt_kprintf("mount fs on flash to root fail\n");
        if (dfs_mkfs("elm", NAND_MTD_NAME) == 0)
        {
            rt_kprintf("make elm fs on flash sucess, mount again\n");
            if (dfs_mount(NAND_MTD_NAME, "/", "elm", 0, 0) == 0)
                rt_kprintf("mount fs on flash success\n");
            else
                rt_kprintf("mount to fs on flash fail\n");
        }
        else
            rt_kprintf("dfs_mkfs elm flash fail\n");
    }
    return RT_EOK;
}
INIT_ENV_EXPORT(mnt_init);
#endif
static void sleep_timer_timeout_handle(void *parameter)
{
    rt_kprintf("30 seconds timeout, set sleep enter flag\n");
    g_sleep_enter_flag = 1;  // 设置进入睡眠标志位
}
static void start_sleep_timer(void)
{
    if (s_sleep_timer == RT_NULL) {
        s_sleep_timer = rt_timer_create("sleep_timer", 
                                        sleep_timer_timeout_handle,
                                        RT_NULL, 
                                        rt_tick_from_millisecond(30000),  // 30秒
                                        RT_TIMER_FLAG_ONE_SHOT | RT_TIMER_FLAG_SOFT_TIMER);
    } else {
        rt_timer_stop(s_sleep_timer);
        rt_timer_control(s_sleep_timer, RT_TIMER_CTRL_SET_TIME, (void *)&(rt_tick_t){rt_tick_from_millisecond(30000)});
    }
    
    if (s_sleep_timer != RT_NULL) {
        rt_timer_start(s_sleep_timer);
        rt_kprintf("Sleep timer started, will trigger after 30 seconds\n");
    } else {
        rt_kprintf("Failed to create sleep timer\n");
    }
}
static void reconnect_timeout_handle(void *parameter)
{
    rt_mb_send(g_bt_app_mb, BT_APP_RECONNECT);
}

static void start_reconnect_timer(void)
{
    if (s_reconnect_timer == NULL) {
        s_reconnect_timer = rt_timer_create(
            "reconnect", reconnect_timeout_handle, NULL,
            rt_tick_from_millisecond(10000), // 1秒间隔
            RT_TIMER_FLAG_PERIODIC);
    }
    
    if (s_reconnect_timer) {
        reconnect_attempts = 0;
        rt_timer_start(s_reconnect_timer);
        LOG_I("Start reconnect timer");
    }
}



void keep_First_pan_connection()
{
    static int first_reconnect_attempts = 0;
    const int max_reconnect_attempts = 3;
    const int reconnect_interval_ms = 4000; // 4秒

    LOG_I("Keep_first_Attempting to reconnect PAN, attempt %d",
          first_reconnect_attempts + 1);
    xiaozhi_ui_chat_status("connecting pan...");
    xiaozhi_ui_chat_output("正在重连PAN...");
    if (first_reconnect_attempts < max_reconnect_attempts)
    {
        if (g_bt_app_env.pan_connect_timer)
        {
            rt_timer_stop(g_bt_app_env.pan_connect_timer);
        }
        bt_interface_conn_ext((char *)&g_bt_app_env.bd_addr, BT_PROFILE_HID);
    }
    else
    {
        LOG_W("Failed to keep_first_reconnect PAN after %d attempts",
              max_reconnect_attempts);
        xiaozhi_ui_chat_status("无法连接PAN");
        xiaozhi_ui_chat_output("请确保设备开启了共享网络,重新发起连接");
        xiaozhi_ui_update_emoji("thinking");

        return;
    }
    first_reconnect_attempts++;
    rt_thread_mdelay(reconnect_interval_ms);
    // 检查是否连接成功
    if (g_pan_connected)
    {
        LOG_I("PAN reconnected successfully%d\n", g_pan_connected);
        return;
    }
}
void pan_reconnect(void)
{
    static int reconnect_attempts = 0;
    const int max_reconnect_attempts = 3;
    const int reconnect_interval_ms = 4000; // 4秒
    while (reconnect_attempts < max_reconnect_attempts)
    {
        LOG_I("Attempting to reconnect PAN, attempt %d",
              reconnect_attempts + 1);
        xiaozhi_ui_chat_status("connecting pan...");
        xiaozhi_ui_chat_output("正在重连PAN...");
        if (g_bt_app_env.pan_connect_timer)
        {
            rt_timer_stop(g_bt_app_env.pan_connect_timer);
        }
        bt_interface_conn_ext((char *)&g_bt_app_env.bd_addr, BT_PROFILE_HID);
        reconnect_attempts++;
        rt_thread_mdelay(reconnect_interval_ms);
        // 检查是否连接成功
        if (g_pan_connected)
        {
            LOG_I("PAN reconnected successfully%d\n", g_pan_connected);
            reconnect_attempts = 0;
            return;
        }
    }

    LOG_W("Failed to reconnect PAN after %d attempts", max_reconnect_attempts);
    xiaozhi_ui_chat_status("无法连接PAN");
    xiaozhi_ui_chat_output("请确保设备开启了共享网络,重新发起连接");
    xiaozhi_ui_update_emoji("thinking");
    reconnect_attempts = 0;
}
static int bt_app_interface_event_handle(uint16_t type, uint16_t event_id,
                                         uint8_t *data, uint16_t data_len)
{
    if (type == BT_NOTIFY_COMMON)
    {
        int pan_conn = 0;

        switch (event_id)
        {
        case BT_NOTIFY_COMMON_BT_STACK_READY:
        {
            rt_mb_send(g_bt_app_mb, BT_APP_READY);
        }
        break;
        case BT_NOTIFY_COMMON_ACL_DISCONNECTED:
        {
            bt_notify_device_base_info_t *info =
                (bt_notify_device_base_info_t *)data;
            LOG_I("disconnected(0x%.2x:%.2x:%.2x:%.2x:%.2x:%.2x) res %d",
                  info->mac.addr[5], info->mac.addr[4], info->mac.addr[3],
                  info->mac.addr[2], info->mac.addr[1], info->mac.addr[0],
                  info->res);
            g_bt_app_env.bt_connected = FALSE;

            //  memset(&g_bt_app_env.bd_addr, 0xFF,
            //  sizeof(g_bt_app_env.bd_addr));
                 if (info->res == BT_NOTIFY_COMMON_SCO_DISCONNECTED) 
                {
                
                    LOG_I("Phone actively disconnected, prepare to enter sleep mode after 30 seconds");
                    rt_mb_send(g_bt_app_mb, BT_APP_PHONE_DISCONNECTED);
                }
                else 
                {
                    LOG_I("Abnormal disconnection, start reconnect attempts");
                    rt_mb_send(g_bt_app_mb, BT_APP_ABNORMAL_DISCONNECT);
                }
            
            if (g_bt_app_env.pan_connect_timer)
                rt_timer_stop(g_bt_app_env.pan_connect_timer);
        }
        break;
        case BT_NOTIFY_COMMON_ENCRYPTION:
        {
            bt_notify_device_mac_t *mac = (bt_notify_device_mac_t *)data;
            LOG_I("Encryption competed");
            g_bt_app_env.bd_addr = *mac;
            pan_conn = 1;
        }
        break;
        case BT_NOTIFY_COMMON_PAIR_IND:
        {
            bt_notify_device_base_info_t *info =
                (bt_notify_device_base_info_t *)data;
            LOG_I("Pairing completed %d", info->res);
            if (info->res == BTS2_SUCC)
            {
                g_bt_app_env.bd_addr = info->mac;
                pan_conn = 1;
            }
        }
        break;
        case BT_NOTIFY_COMMON_KEY_MISSING:
        {
            bt_notify_device_base_info_t *info =
                (bt_notify_device_base_info_t *)data;
            LOG_I("Key missing %d", info->res);
            memset(&g_bt_app_env.bd_addr, 0xFF, sizeof(g_bt_app_env.bd_addr));
            bt_cm_delete_bonded_devs_and_linkkey(info->mac.addr);
        }
        break;
        default:
            break;
        }

        if (pan_conn)
        {
            LOG_I("bd addr 0x%.2x:%.2x:%.2x:%.2x:%.2x:%.2x\n",
                  g_bt_app_env.bd_addr.addr[5], g_bt_app_env.bd_addr.addr[4],
                  g_bt_app_env.bd_addr.addr[3], g_bt_app_env.bd_addr.addr[2],
                  g_bt_app_env.bd_addr.addr[1], g_bt_app_env.bd_addr.addr[0]);
            g_bt_app_env.bt_connected = TRUE;
            // Trigger PAN connection after PAN_TIMER_MS period to avoid SDP
            // confliction.
            if (!g_bt_app_env.pan_connect_timer)
                g_bt_app_env.pan_connect_timer = rt_timer_create(
                    "connect_pan", bt_app_connect_pan_timeout_handle,
                    (void *)&g_bt_app_env,
                    rt_tick_from_millisecond(PAN_TIMER_MS),
                    RT_TIMER_FLAG_SOFT_TIMER);
            else
                rt_timer_stop(g_bt_app_env.pan_connect_timer);
            rt_timer_start(g_bt_app_env.pan_connect_timer);
        }
    }
    else if (type == BT_NOTIFY_PAN)
    {
        switch (event_id)
        {
        case BT_NOTIFY_PAN_PROFILE_CONNECTED:
        {
            xiaozhi_ui_chat_output("PAN连接成功");
            xiaozhi_ui_update_ble("open");
            LOG_I("pan connect successed \n");
            if ((g_bt_app_env.pan_connect_timer))
            {
                rt_timer_stop(g_bt_app_env.pan_connect_timer);
            }
            rt_mb_send(g_bt_app_mb, BT_APP_CONNECT_PAN_SUCCESS);
            g_pan_connected = TRUE; // 更新PAN连接状态
        }
        break;
        case BT_NOTIFY_PAN_PROFILE_DISCONNECTED:
        {
            xiaozhi_ui_chat_status("PAN断开...");
            xiaozhi_ui_chat_output("PAN断开,尝试唤醒键重新连接");
            xiaozhi_ui_update_ble("close");
            LOG_I("pan disconnect with remote device\n");
            g_pan_connected = FALSE; // 更新PAN连接状态
            if (first_pan_connected ==
                FALSE) // Check if the pan has ever been connected
            {
                rt_mb_send(g_bt_app_mb, KEEP_FIRST_PAN_RECONNECT);
            }
        }
        break;
        default:
            break;
        }
    }
    else if (type == BT_NOTIFY_HID)
    {
        switch (event_id)
        {
        case BT_NOTIFY_HID_PROFILE_CONNECTED:
        {
            LOG_I("HID connected\n");
            if (!g_pan_connected)
            {
                if (g_bt_app_env.pan_connect_timer)
                {
                    rt_timer_stop(g_bt_app_env.pan_connect_timer);
                }
                bt_interface_conn_ext((char *)&g_bt_app_env.bd_addr,
                                      BT_PROFILE_PAN);
            }
        }
        break;
        case BT_NOTIFY_HID_PROFILE_DISCONNECTED:
        {
            LOG_I("HID disconnected\n");
        }
        break;
        default:
            break;
        }
    }

    return 0;
}

uint32_t bt_get_class_of_device()
{
    return (uint32_t)BT_SRVCLS_NETWORK | BT_DEVCLS_PERIPHERAL |
           BT_PERIPHERAL_REMCONTROL;
}

static void check_poweron_reason(void)
{
    switch (SystemPowerOnModeGet())
    {
    case PM_REBOOT_BOOT:
    case PM_COLD_BOOT:
    {
        // power on as normal
        break;
    }
    case PM_HIBERNATE_BOOT:
    case PM_SHUTDOWN_BOOT:
    {
        if (PMUC_WSR_RTC & pm_get_wakeup_src())
        {
            // RTC唤醒
            NVIC_EnableIRQ(RTC_IRQn);
            // power on as normal
        }
#ifdef BSP_USING_CHARGER
        else if ((PMUC_WSR_PIN0 << (pm_get_charger_pin_wakeup())) & pm_get_wakeup_src())
        {
        }
#endif
        else if (PMUC_WSR_PIN_ALL & pm_get_wakeup_src())
        {
            rt_thread_mdelay(2500); // 延时2.5秒
            int val = rt_pin_read(43);
            rt_kprintf("Power key(PA43) level after 2.5s: %d\n", val);
            if (val != BSP_KEY2_ACTIVE_HIGH)
            {
                // 按键已松开，认为是误触发，直接关机
                rt_kprintf("Not long press, shutdown now.\n");
                PowerDownCustom();
                while (1) {};
            }
            else
            {
                // 长按，正常开机
                rt_kprintf("Long press detected, power on as normal.\n");
            }
        }
        else if (0 == pm_get_wakeup_src())
        {
            RT_ASSERT(0);
        }
        break;
    }
    default:
    {
        RT_ASSERT(0);
    }
    }
}

int main(void)
{
    check_poweron_reason();
    // 初始化邮箱
    g_button_event_mb = rt_mb_create("btn_evt", 8, RT_IPC_FLAG_FIFO);
    if (g_button_event_mb == NULL)
    {
        rt_kprintf("Failed to create mailbox g_button_event_mb\n");
        return 0;
    }

    audio_server_set_private_volume(AUDIO_TYPE_LOCAL_MUSIC, VOL_DEFAULE_LEVEL); // 设置音量 
    xz_set_lcd_brightness(LCD_BRIGHTNESS_DEFAULT);
    iot_initialize(); // Initialize iot
#ifdef BSP_USING_BOARD_SF32LB52_LCHSPI_ULP
    unsigned int *addr2 = (unsigned int *)0x50003088; // 21
    *addr2 = 0x00000200;
    unsigned int *addr = (unsigned int *)0x500030B0; // 31
    *addr = 0x00000200;

    // senser
    HAL_PIN_Set(PAD_PA30, GPIO_A30, PIN_PULLDOWN, 1);
    BSP_GPIO_Set(30, 0, 1);
    HAL_PIN_Set(PAD_PA39, GPIO_A39, PIN_PULLDOWN, 1);
    HAL_PIN_Set(PAD_PA40, GPIO_A40, PIN_PULLDOWN, 1);

    // rt_pm_request(PM_SLEEP_MODE_IDLE);
#endif
    // Create  xiaozhi UI
    rt_thread_t tid =
        rt_thread_create("xz_ui", xiaozhi_ui_task, NULL, 6144, 30, 10);
    rt_thread_startup(tid);

    // Connect BT PAN
    g_bt_app_mb = rt_mb_create("bt_app", 8, RT_IPC_FLAG_FIFO);
#ifdef BSP_BT_CONNECTION_MANAGER
    bt_cm_set_profile_target(BT_CM_HID, BT_LINK_PHONE, 1);
#endif // BSP_BT_CONNECTION_MANAGER

    bt_interface_register_bt_event_notify_callback(
        bt_app_interface_event_handle);

    sifli_ble_enable();

    rt_thread_t battery_thread =
        rt_thread_create("battery", battery_level_task, NULL, 1024*2, 20, 10);
    rt_thread_startup(battery_thread);

#ifdef BSP_USING_BOARD_SF32LB52_XTY_AI
    if (pulse_encoder_init() != RT_EOK)
    {
        rt_kprintf("Pulse encoder initialization failed.\n");
        return -RT_ERROR;
    }

    s_pulse_encoder_timer =
        rt_timer_create("pulse_encoder", pulse_encoder_timeout_handle, NULL,
                        rt_tick_from_millisecond(200), RT_TIMER_FLAG_PERIODIC);
    if (s_pulse_encoder_timer)
    {
        rt_kprintf("Pulse encoder timer created successfully.\n");
        rt_timer_start(s_pulse_encoder_timer);
    }
    else
    {
        rt_kprintf("Failed to create pulse encoder timer.\n");
        return -1;
    }
#endif
    while (1)
    {

        uint32_t value;

        // handle pan connect event
        rt_mb_recv(g_bt_app_mb, (rt_uint32_t *)&value, RT_WAITING_FOREVER);

        if (value == BT_APP_CONNECT_PAN)
        {
            if (g_bt_app_env.bt_connected)
            {
                bt_interface_conn_ext((char *)&g_bt_app_env.bd_addr,
                                      BT_PROFILE_PAN);
            }
        }
        else if (value == BT_APP_READY)
        {
            LOG_I("BT/BLE stack and profile ready");

#ifdef BT_NAME_MAC_ENABLE
            char local_name[32];
            bd_addr_t addr;
            ble_get_public_address(&addr);
            sprintf(local_name, "%s-%02x:%02x:%02x:%02x:%02x:%02x",
                    BLUETOOTH_NAME, addr.addr[0], addr.addr[1], addr.addr[2],
                    addr.addr[3], addr.addr[4], addr.addr[5]);
#else
            const char *local_name = BLUETOOTH_NAME;
#endif
            bt_interface_set_local_name(strlen(local_name), (void *)local_name);
        }
        else if (value == BT_APP_CONNECT_PAN_SUCCESS)
        {
            rt_kputs("BT_APP_CONNECT_PAN_SUCCESS\r\n");
            xiaozhi_ui_chat_output("PAN连接成功,开始连接小智...");
            xiaozhi_ui_update_ble("open");
            xiaozhi_ui_chat_status("正在连接小智...");
            xiaozhi_ui_update_emoji("neutral");

            rt_thread_mdelay(2000);
#ifdef XIAOZHI_USING_MQTT
            xiaozhi(0, NULL);
            rt_kprintf("Select MQTT Version\n");
#else
            xiaozhi2(0, NULL); // Start Xiaozhi
#endif
        }
        else if (value == KEEP_FIRST_PAN_RECONNECT)
        {
            keep_First_pan_connection(); // Ensure that the first pan connection
                                         // is successful
        }
#ifdef XIAOZHI_USING_MQTT

#else
        else if (value == PAN_RECONNECT) // Press to wake up pan reconnection
        {
            rt_kprintf("PAN_RECONNECT\r\n");
            if (g_pan_connected) // If the pan is already connected, there is no
                                 // need to reconnect the pan
            {
                if (g_xz_ws.is_connected) // Check whether the websocket is
                                          // connected
                {
                    rt_kprintf("g_xz_ws.is_connected = %d\n",
                               g_xz_ws.is_connected);
                    rt_kprintf("ws_connected\n");
                }
                else
                {

                    rt_kprintf("PAN_CONNECTED\r\n");

                    reconnect_websocket(); // 重连websocket
                }
            }
            else // pan is not connected. You need to connect pan and then press
                 // the button to reconnect websocket
            {

                pan_reconnect(); // Triple reconnection of pan
            }
        }
        else if(value == BT_APP_PHONE_DISCONNECTED)
        {
            rt_kprintf("Phone actively disconnected, enter sleep mode after 30 seconds\n");
            start_sleep_timer();
            //睡眠
        }
        else if(value == BT_APP_ABNORMAL_DISCONNECT)
        {
            rt_kprintf("Abnormal disconnection, start reconnect attempts\n");
            rt_thread_mdelay(3000);
            reconnect_attempts = 0;
            start_reconnect_timer();
        }
        else if(value == BT_APP_RECONNECT_TIMEOUT)
        {
            rt_kprintf("Reconnect timeout, enter sleep mode\n");
            g_sleep_enter_flag = 1;
            //睡眠
        }
        else if(value == BT_APP_RECONNECT)
        {
            if (g_bt_app_env.bt_connected) 
            {
                // 已经重新连接成功，停止定时器
                if (s_reconnect_timer) {
                    rt_timer_stop(s_reconnect_timer);
                }
                reconnect_attempts = 0;
                LOG_I("Reconnect successful, stop reconnect timer");
            }
            else
            {
                reconnect_attempts++;
                LOG_I("Reconnect attempt %d/%d", reconnect_attempts, MAX_RECONNECT_ATTEMPTS);
            }

            if (reconnect_attempts <= MAX_RECONNECT_ATTEMPTS) 
            {
                bt_interface_conn_ext((char *)&g_bt_app_env.bd_addr, BT_PROFILE_HID);
            }
            else
            {
                LOG_I("Reconnect timeout, send timeout event");
                rt_mb_send(g_bt_app_mb, BT_APP_RECONNECT_TIMEOUT);
                reconnect_attempts = 0;
                 if (s_reconnect_timer) {
                        rt_timer_stop(s_reconnect_timer);
                    }
            }
        }
        else
        {
            rt_kprintf("WEBSOCKET_DISCONNECT\r\n");
            xiaozhi_ui_chat_output("请重启");
        }
#endif
    }
    return 0;
}

static void pan_cmd(int argc, char **argv)
{
    if (strcmp(argv[1], "del_bond") == 0)
    {
#ifdef BSP_BT_CONNECTION_MANAGER
        bt_cm_delete_bonded_devs();
        LOG_D("Delete bond");
#endif // BSP_BT_CONNECTION_MANAGER
    }
    // only valid after connection setup but phone didn't enable pernal hop
    else if (strcmp(argv[1], "conn_pan") == 0)
        bt_app_connect_pan_timeout_handle(NULL);
}
MSH_CMD_EXPORT(pan_cmd, Connect PAN to last paired device);
