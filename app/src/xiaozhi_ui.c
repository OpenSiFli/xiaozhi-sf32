#include "rtthread.h"
#include "bf0_hal.h"
#include "drv_io.h"
#include "littlevgl2rtt.h"
#include "lv_tiny_ttf.h"
#include "string.h"

#include "bf0_pm.h"
#include "gui_app_pm.h"
#include "drv_gpio.h"
#include "lv_timer.h"
#include "lv_display.h"
#include "lv_obj_pos.h"
#include "ulog.h"
#include "drv_flash.h"
#include "xiaozhi2.h"
#define IDLE_TIME_LIMIT (30000)
#define SHOW_TEXT_LEN 150
#define LCD_DEVICE_NAME "lcd"
#define TOUCH_NAME "touch"
static struct rt_semaphore update_ui_sema;
/*Create style with the new font*/
static lv_style_t style;
static lv_style_t style_battery;

extern const unsigned char droid_sans_fallback_font[];
extern const int droid_sans_fallback_font_size;
static float g_scale = 1.0f;
#define SCALE_DPX(val) LV_DPX((val) * g_scale)


extern const lv_image_dsc_t neutral;
extern const lv_image_dsc_t happy;
extern const lv_image_dsc_t laughing;
extern const lv_image_dsc_t funny;
extern const lv_image_dsc_t sad;
extern const lv_image_dsc_t angry;
extern const lv_image_dsc_t crying;
extern const lv_image_dsc_t loving;
extern const lv_image_dsc_t embarrassed;
extern const lv_image_dsc_t surprised;
extern const lv_image_dsc_t shocked;
extern const lv_image_dsc_t thinking;
extern const lv_image_dsc_t winking;
extern const lv_image_dsc_t cool;
extern const lv_image_dsc_t relaxed;
extern const lv_image_dsc_t delicious;
extern const lv_image_dsc_t kissy;
extern const lv_image_dsc_t confident;
extern const lv_image_dsc_t sleepy;
extern const lv_image_dsc_t silly;
extern const lv_image_dsc_t confused;

extern const lv_image_dsc_t ble; // ble
extern const lv_image_dsc_t ble_close;

static lv_obj_t *global_label1;
static lv_obj_t *global_label2;

static lv_obj_t *global_img;

static lv_obj_t *global_img_ble;

static rt_timer_t g_split_text_timer = RT_NULL;
static char g_second_part[512];
static lv_obj_t *g_label_for_second_part = NULL;
// xiaozhi2
extern rt_mailbox_t g_button_event_mb;
extern xiaozhi_ws_t g_xz_ws;
extern void ws_send_speak_abort(void *ws, char *session_id, int reason);
extern void ws_send_listen_start(void *ws, char *session_id,
                                 enum ListeningMode mode);
extern void ws_send_listen_stop(void *ws, char *session_id);

//默认oled电池图标尺寸
#define OUTLINE_W    58
#define OUTLINE_H    33

//LCD_USING_ST7789电池图标尺寸
#define OUTLINE_W_ST7789    40
#define OUTLINE_H_ST7789    20

// 全局变量存储当前电池电量
static int g_battery_level = 100; // 默认为满电
static lv_obj_t *g_battery_fill = NULL; // 电池填充对象
static lv_obj_t *g_battery_label = NULL; // 电量标签



#define BASE_WIDTH  390
#define BASE_HEIGHT 450

// 获取当前屏幕尺寸并计算缩放因子
static float get_scale_factor(void)
{
    lv_disp_t *disp = lv_disp_get_default();
    lv_coord_t scr_width = lv_disp_get_hor_res(disp);
    lv_coord_t scr_height = lv_disp_get_ver_res(disp);

    float scale_x = (float)scr_width / BASE_WIDTH;
    float scale_y = (float)scr_height / BASE_HEIGHT;

    return (scale_x < scale_y) ? scale_x : scale_y;
}

rt_err_t xiaozhi_ui_obj_init()
{

    LV_IMAGE_DECLARE(neutral);
    LV_IMAGE_DECLARE(happy);
    LV_IMAGE_DECLARE(laughing);
    LV_IMAGE_DECLARE(funny);
    LV_IMAGE_DECLARE(sad);
    LV_IMAGE_DECLARE(angry);
    LV_IMAGE_DECLARE(crying);
    LV_IMAGE_DECLARE(loving);
    LV_IMAGE_DECLARE(embarrassed);
    LV_IMAGE_DECLARE(surprised);
    LV_IMAGE_DECLARE(shocked);
    LV_IMAGE_DECLARE(thinking);
    LV_IMAGE_DECLARE(winking);
    LV_IMAGE_DECLARE(cool);
    LV_IMAGE_DECLARE(relaxed);
    LV_IMAGE_DECLARE(delicious);
    LV_IMAGE_DECLARE(kissy);
    LV_IMAGE_DECLARE(confident);
    LV_IMAGE_DECLARE(sleepy);
    LV_IMAGE_DECLARE(silly);
    LV_IMAGE_DECLARE(confused);

    LV_IMAGE_DECLARE(ble);
    LV_IMAGE_DECLARE(ble_close);

    // 获取屏幕分辨率
    lv_coord_t scr_width = lv_disp_get_hor_res(NULL);
    lv_coord_t scr_height = lv_disp_get_ver_res(NULL);

    // 创建主容器 - Flex Column，垂直排列
    lv_obj_t *main_container = lv_obj_create(lv_screen_active());
    lv_obj_remove_flag(main_container, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_size(main_container, scr_width, scr_height);

    // 清除主容器的 padding 和 margin
    lv_obj_set_style_pad_all(main_container, 0, 0);
    lv_obj_set_style_margin_all(main_container, 0, 0);

    lv_obj_set_style_bg_opa(main_container, LV_OPA_0, 0);
    lv_obj_set_style_border_width(main_container, 0, 0);

    lv_obj_set_flex_flow(main_container, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(main_container,
                          LV_FLEX_ALIGN_START,   // 主轴靠上对齐
                          LV_FLEX_ALIGN_CENTER,  // 交叉轴居中
                          LV_FLEX_ALIGN_CENTER); // 对齐方式统一

    // 顶部状态栏容器（Flex Row）
    lv_obj_t *header_row = lv_obj_create(main_container);
    lv_obj_remove_flag(header_row, LV_OBJ_FLAG_SCROLLABLE); // 关闭滚动条
    lv_obj_set_size(header_row, scr_width, SCALE_DPX(40));  // 固定高度为 40dp

    // 清除 header_row 的内边距和外边距
    lv_obj_set_style_pad_all(header_row, 0, 0);
    lv_obj_set_style_margin_all(header_row, 0, 0);
    // 设置 header_row 的背景透明和边框宽度为 0
    lv_obj_set_style_bg_opa(header_row, LV_OPA_0, 0);
    lv_obj_set_style_border_width(header_row, 0, 0);
    lv_obj_set_flex_flow(header_row, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(header_row, LV_FLEX_ALIGN_SPACE_BETWEEN,
                          LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);

    // 插入一个空白对象作为左边距
    lv_obj_t *spacer = lv_obj_create(header_row);
    lv_obj_remove_flag(spacer, LV_OBJ_FLAG_SCROLLABLE); // 关闭滚动条
    lv_obj_set_style_bg_opa(spacer, LV_OPA_0, 0);
    lv_obj_set_style_border_width(spacer, 0, 0);
    lv_obj_set_size(spacer, SCALE_DPX(40), LV_SIZE_CONTENT); // 宽度为 40dp

    // BLE 图标 - 左上角
    global_img_ble = lv_img_create(header_row);
    lv_img_set_src(global_img_ble, &ble);
    lv_obj_set_size(global_img_ble, SCALE_DPX(24), SCALE_DPX(24)); // 24dp 图标
    lv_img_set_zoom(global_img_ble,
                    (int)(LV_SCALE_NONE * g_scale)); // 根据缩放因子缩放

    // Top Label - 居中显示
    global_label1 = lv_label_create(header_row);
    lv_label_set_long_mode(global_label1, LV_LABEL_LONG_WRAP);
    lv_obj_add_style(global_label1, &style, 0);
    lv_obj_set_width(global_label1, LV_PCT(80));
    lv_obj_set_style_text_align(global_label1, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_align_to(global_label1, header_row, LV_ALIGN_CENTER, 0, 0);

    // 电池图标 - 放在 header_row 容器中，与 BLE 图标对称
    lv_obj_t* battery_outline = lv_obj_create(header_row);
    lv_obj_set_style_border_width(battery_outline, 2, 0);// 边框宽度
    lv_obj_set_style_pad_all(battery_outline, 0, 0);// 内边距
    lv_obj_set_style_radius(battery_outline, 8, 0);// 圆角半径
    lv_obj_clear_flag(battery_outline, LV_OBJ_FLAG_SCROLLABLE);
    #ifdef LCD_USING_ST7789
    lv_obj_set_size(battery_outline, OUTLINE_W_ST7789, OUTLINE_H_ST7789);
    #else// LCD_USING_ST7789
    lv_obj_set_size(battery_outline, OUTLINE_W * g_scale, OUTLINE_H * g_scale);
    rt_kprintf("Battery outline sizedefualt: %d x %d\n", OUTLINE_W * g_scale, OUTLINE_H * g_scale);
    #endif //defualt

    g_battery_fill = lv_obj_create(battery_outline);
    lv_obj_set_style_outline_width(g_battery_fill, 0, 0);
    lv_obj_set_style_outline_pad(g_battery_fill, 0, 0);
    lv_obj_set_style_border_width(g_battery_fill, 0, 0);
    lv_obj_set_style_bg_color(g_battery_fill, lv_color_hex(0xff00), 0); // 初始绿色

    #ifdef LCD_USING_ST7789
    lv_obj_set_size(g_battery_fill, OUTLINE_W_ST7789 - 4, OUTLINE_H_ST7789 - 4);
    #else// LCD_USING_ST7789
    lv_obj_set_size(g_battery_fill, OUTLINE_W * g_scale - 4, OUTLINE_H * g_scale - 4);
    #endif//defualt

    lv_obj_set_style_border_width(g_battery_fill, 0, 0);
    lv_obj_set_style_radius(g_battery_fill, 8, 0);
    lv_obj_align(g_battery_fill, LV_ALIGN_LEFT_MID, 0, 0);
    lv_obj_clear_flag(g_battery_fill, LV_OBJ_FLAG_SCROLLABLE);

    g_battery_label = lv_label_create(battery_outline);
    // lv_obj_add_style(g_battery_label, &style_battery, 0);
    lv_label_set_text_fmt(g_battery_label, "%d%%", g_battery_level);
    lv_obj_align(g_battery_label, LV_ALIGN_CENTER, 0, 0);


    // 插入右侧空白对象用于对称布局
    lv_obj_t *spacer_right = lv_obj_create(header_row);
    lv_obj_remove_flag(spacer_right, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_style_bg_opa(spacer_right, LV_OPA_0, 0);
    lv_obj_set_style_border_width(spacer_right, 0, 0);
    lv_obj_set_size(spacer_right, SCALE_DPX(40),
                    LV_SIZE_CONTENT); // 宽度为 40dp

    // Emoji 图标 - 屏幕中心向上偏移（以图标中心为基准）
    global_img = lv_img_create(main_container);
    lv_img_set_src(global_img, &neutral);
    lv_obj_set_size(global_img, SCALE_DPX(80), SCALE_DPX(80)); // 固定大小 80dp
    lv_img_set_zoom(global_img,
                    (int)(LV_SCALE_NONE * g_scale)); // 根据缩放因子缩放
    lv_obj_align(global_img, LV_ALIGN_CENTER, 0,
                 -SCALE_DPX(40)); // 向上偏移 40dp

    // Output Label - 紧贴 emoji 下方
    global_label2 = lv_label_create(main_container);
    lv_label_set_long_mode(global_label2, LV_LABEL_LONG_WRAP);
    lv_obj_add_style(global_label2, &style, 0);
    lv_obj_set_width(global_label2, LV_PCT(90));
    lv_obj_set_style_text_align(global_label2, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_align_to(global_label2, global_img, LV_ALIGN_OUT_BOTTOM_MID, 0,
                    SCALE_DPX(10));

    lv_obj_set_style_bg_color(global_label1, lv_color_hex(0xff0000),
                              LV_STATE_DEFAULT);

    // rt_kprintf("Screen res: %d x %d\n", scr_width, scr_height);

    return RT_EOK;
}

void xiaozhi_ui_chat_status(char *string) // top text
{
    rt_sem_take(&update_ui_sema, RT_WAITING_FOREVER);

    if (string)
    {
        lv_label_set_text(global_label1, string);
    }

    rt_sem_release(&update_ui_sema);
}

void xiaozhi_ui_chat_output(char *string)
{
    rt_sem_take(&update_ui_sema, RT_WAITING_FOREVER);

    if (string)
    {
        lv_label_set_text(global_label2, string);
#ifdef BSP_USING_PM
        lv_display_trigger_activity(NULL);
#endif // BSP_USING_PM
    }

    rt_sem_release(&update_ui_sema);
}

static void switch_to_second_part(void *parameter)
{
    if (g_label_for_second_part && strlen(g_second_part) > 0)
    {
        rt_sem_take(&update_ui_sema, RT_WAITING_FOREVER);

        int len = strlen(g_second_part);
        if (len > SHOW_TEXT_LEN)
        {
            // 再次分割文本
            char first_part[SHOW_TEXT_LEN + 1];
            char remaining[512];

            // 查找合适的截断点
            int cut_pos = SHOW_TEXT_LEN;
            while (cut_pos > 0 &&
                   ((unsigned char)g_second_part[cut_pos] & 0xC0) == 0x80)
            {
                cut_pos--;
            }

            strncpy(first_part, g_second_part, cut_pos);
            first_part[cut_pos] = '\0';

            strncpy(remaining, g_second_part + cut_pos, sizeof(remaining) - 1);
            remaining[sizeof(remaining) - 1] = '\0';

            // 显示当前部分
            lv_label_set_text(g_label_for_second_part, first_part);

            // 保存剩余部分
            strncpy(g_second_part, remaining, sizeof(g_second_part) - 1);
            g_second_part[sizeof(g_second_part) - 1] = '\0';

            // 重置定时器以显示下一部分
            rt_timer_control(g_split_text_timer, RT_TIMER_CTRL_SET_TIME,
                             &(rt_tick_t){rt_tick_from_millisecond(9000)});
            rt_timer_start(g_split_text_timer);
        }
        else
        {
            // 最后一部分，直接显示
            lv_label_set_text(g_label_for_second_part, g_second_part);
            memset(g_second_part, 0, sizeof(g_second_part));
            g_label_for_second_part = NULL;
        }

        rt_sem_release(&update_ui_sema);
    }
}
void xiaozhi_ui_tts_output(char *string)
{
    rt_sem_take(&update_ui_sema, RT_WAITING_FOREVER);

    if (string)
    {
        int len = strlen(string);
        rt_kprintf("len == %d\n", len);

        if (len > SHOW_TEXT_LEN)
        {
            // 查看 SHOW_TEXT_LEN 是否落在一个多字节字符中间
            int cut_pos = SHOW_TEXT_LEN;

            // 向前调整到完整的 UTF-8 字符起点
            while (cut_pos > 0 &&
                   ((unsigned char)string[cut_pos] & 0xC0) == 0x80)
            {
                cut_pos--;
            }

            if (cut_pos == 0) // 找不到合适的截断点，直接截断
                cut_pos = SHOW_TEXT_LEN;

            // 截取第一部分
            char first_part[SHOW_TEXT_LEN + 1];
            strncpy(first_part, string, cut_pos);
            first_part[cut_pos] = '\0'; // 确保字符串结束

            // 剩余部分从 cut_pos 开始
            strncpy(g_second_part, string + cut_pos, sizeof(g_second_part) - 1);
            g_second_part[sizeof(g_second_part) - 1] = '\0'; // 确保结尾
            g_label_for_second_part = global_label2;

            lv_label_set_text(global_label2, first_part);
#ifdef BSP_USING_PM
            lv_display_trigger_activity(NULL);
#endif // BSP_USING_PM

            // 创建定时器
            if (!g_split_text_timer)
            {
                g_split_text_timer = rt_timer_create(
                    "next_text", switch_to_second_part, NULL,
                    rt_tick_from_millisecond(9000), // 9秒后显示下一部分
                    RT_TIMER_FLAG_ONE_SHOT | RT_TIMER_FLAG_SOFT_TIMER);
            }
            else
            {
                rt_timer_stop(g_split_text_timer);
            }
            rt_timer_start(g_split_text_timer);
        }
        else
        {
            lv_label_set_text(global_label2, string);
#ifdef BSP_USING_PM
            lv_display_trigger_activity(NULL);
#endif // BSP_USING_PM
        }
    }

    rt_sem_release(&update_ui_sema);
}

void xiaozhi_ui_update_emoji(char *string) // emoji
{

    rt_sem_take(&update_ui_sema, RT_WAITING_FOREVER);

    if (string)
    {
        if (strcmp(string, "neutral") == 0)
        {
            lv_img_set_src(global_img, &neutral);
        }
        else if (strcmp(string, "happy") == 0)
        {
            lv_img_set_src(global_img, &happy);
        }
        else if (strcmp(string, "laughing") == 0)
        {
            lv_img_set_src(global_img, &laughing);
        }
        else if (strcmp(string, "funny") == 0)
        {
            lv_img_set_src(global_img, &funny);
        }
        else if (strcmp(string, "sad") == 0)
        {
            lv_img_set_src(global_img, &sad);
        }
        else if (strcmp(string, "angry") == 0)
        {
            lv_img_set_src(global_img, &angry);
        }
        else if (strcmp(string, "crying") == 0)
        {
            lv_img_set_src(global_img, &crying);
        }
        else if (strcmp(string, "loving") == 0)
        {
            lv_img_set_src(global_img, &loving);
        }
        else if (strcmp(string, "embarrassed") == 0)
        {
            lv_img_set_src(global_img, &embarrassed);
        }
        else if (strcmp(string, "surprised") == 0)
        {
            lv_img_set_src(global_img, &surprised);
        }
        else if (strcmp(string, "shocked") == 0)
        {
            lv_img_set_src(global_img, &shocked);
        }
        else if (strcmp(string, "thinking") == 0)
        {
            lv_img_set_src(global_img, &thinking);
        }
        else if (strcmp(string, "winking") == 0)
        {
            lv_img_set_src(global_img, &winking);
        }
        else if (strcmp(string, "cool") == 0)
        {
            lv_img_set_src(global_img, &cool);
        }
        else if (strcmp(string, "relaxed") == 0)
        {
            lv_img_set_src(global_img, &relaxed);
        }
        else if (strcmp(string, "delicious") == 0)
        {
            lv_img_set_src(global_img, &delicious);
        }
        else if (strcmp(string, "kissy") == 0)
        {
            lv_img_set_src(global_img, &kissy);
        }
        else if (strcmp(string, "confident") == 0)
        {
            lv_img_set_src(global_img, &confident);
        }
        else if (strcmp(string, "sleepy") == 0)
        {
            lv_img_set_src(global_img, &sleepy);
        }
        else if (strcmp(string, "silly") == 0)
        {
            lv_img_set_src(global_img, &silly);
        }
        else if (strcmp(string, "confused") == 0)
        {
            lv_img_set_src(global_img, &confused);
        }
        else
        {
            lv_img_set_src(global_img, &neutral); // common emoji is neutral
        }
    }

    rt_sem_release(&update_ui_sema);
}

void xiaozhi_ui_update_ble(char *string) // ble
{
    rt_sem_take(&update_ui_sema, RT_WAITING_FOREVER);

    if (string)
    {
        if (strcmp(string, "open") == 0)
        {
            lv_img_set_src(global_img_ble, &ble);
        }
        else if (strcmp(string, "close") == 0)
        {
            lv_img_set_src(global_img_ble, &ble_close);
        }
    }

    rt_sem_release(&update_ui_sema);
}
extern const unsigned char droid_sans_fallback_font[];
extern const int droid_sans_fallback_font_size;

static rt_device_t lcd_device;
static void pm_event_handler(gui_pm_event_type_t event)
{
    LOG_I("in pm_event_handle");
    switch (event)
    {
    case GUI_PM_EVT_SUSPEND:
    {
        LOG_I("in GUI_PM_EVT_SUSPEND");
        lv_timer_enable(false);
        break;
    }
    case GUI_PM_EVT_RESUME:
    {
        lv_timer_enable(true);
        break;
    }
    default:
    {
        RT_ASSERT(0);
    }
    }
}
void pm_ui_init()
{

    int8_t wakeup_pin;
    uint16_t gpio_pin;
    GPIO_TypeDef *gpio;

    gpio = GET_GPIO_INSTANCE(34);
    gpio_pin = GET_GPIOx_PIN(34);

    wakeup_pin = HAL_HPAON_QueryWakeupPin(gpio, gpio_pin);
    RT_ASSERT(wakeup_pin >= 0);

    lcd_device = rt_device_find(LCD_DEVICE_NAME);
    if (lcd_device == RT_NULL)
    {
        LOG_I("lcd_device!=NULL!");
        RT_ASSERT(0);
    }
#ifdef BSP_USING_PM
    pm_enable_pin_wakeup(wakeup_pin, AON_PIN_MODE_DOUBLE_EDGE);
    gui_ctx_init();
    gui_pm_init(lcd_device, pm_event_handler);

#endif
}
void xiaozhi_update_battery_level(int level)
{
    // 确保电量在 0 到 100 之间
    g_battery_level = level;
    rt_kprintf("Battery level updated: %d\n", g_battery_level);   
    if (g_battery_fill)
    {
        #ifdef LCD_USING_ST7789
        int  width = (OUTLINE_W_ST7789 - 4) * g_battery_level / 100; // 计算填充宽度
        #else // LCD_USING_ST7789
        int  width = (OUTLINE_W * g_scale - 4) * g_battery_level / 100; // 计算填充宽度
        #endif 

        if (width < 2 && g_battery_level > 0) width = 2; // 最小宽度为 2
        lv_obj_set_width(g_battery_fill, width);// 根据电量设置填充宽度

        // 更新颜色
        if (g_battery_level <= 20)
        {
            lv_obj_set_style_bg_color(g_battery_fill, lv_color_hex(0xff0000), 0); // 红色
        }
        else
        {
            lv_obj_set_style_bg_color(g_battery_fill, lv_color_hex(0xff00), 0);   // 绿色
        }
    }

    if (g_battery_label)
    {
        rt_kprintf("Updating battery label: %d\n", g_battery_level);
        lv_label_set_text_fmt(g_battery_label, "%d%%", g_battery_level);// 更新电量标签
    }
}
void battery_timer_callback(void *parameter)
{
    // int level = read_battery_level_from_hardware(); // 硬件获取
    int level = 20; // 模拟电池电量为 20%
    rt_kprintf("Battery level: %d\n", level);
    rt_sem_take(&update_ui_sema, RT_WAITING_FOREVER);
    xiaozhi_update_battery_level(level);
    rt_sem_release(&update_ui_sema);
}

void xiaozhi_ui_task(void *args)
{
    rt_err_t ret = RT_EOK;
    rt_uint32_t ms;
    static rt_device_t touch_device;
    static rt_timer_t battery_timer = RT_NULL;

    // 启动定时器，每 10 秒更新一次电池电量
    battery_timer = rt_timer_create("battery", 
                                    battery_timer_callback, 
                                    RT_NULL, 
                                    rt_tick_from_millisecond(10000), 
                                    RT_TIMER_FLAG_PERIODIC | RT_TIMER_FLAG_SOFT_TIMER);

    if (battery_timer)
    {
        rt_kprintf("Battery timer created successfully.\n");
        rt_timer_start(battery_timer);
    }


    
    rt_sem_init(&update_ui_sema, "update_ui", 1, RT_IPC_FLAG_FIFO);

    /* init littlevGL */
    ret = littlevgl2rtt_init("lcd");
    if (ret != RT_EOK)
    {
        return;
    }

    // touch_device = rt_device_find(TOUCH_NAME);
    // if(touch_device==RT_NULL)
    // {
    //     LOG_I("touch_device!=NULL!");
    //     RT_ASSERT(0);
    // }
    // rt_device_control(touch_device, RTGRAPHIC_CTRL_POWEROFF, NULL);

#ifdef BSP_USING_PM
    pm_ui_init();
#endif

    float scale = get_scale_factor();
    g_scale = scale; // 保存全局缩放因子
    rt_kprintf("Scale factor: %.2f\n", scale);
    const int base_font_size = 30;
    const int adjusted_font_size = (int)(base_font_size * scale);

    const int base_font_size_battery = 14;
    const int adjusted_font_size_battery = (int)(base_font_size_battery * scale);


    lv_style_init(&style);
    lv_font_t *font = lv_tiny_ttf_create_data(droid_sans_fallback_font,
                                              droid_sans_fallback_font_size,
                                              adjusted_font_size);
    lv_style_set_text_font(&style, font);
    lv_style_set_text_align(&style, LV_TEXT_ALIGN_CENTER);
    lv_style_set_text_color(&style, lv_color_hex(0xFFFFFF));
    lv_obj_set_style_bg_color(lv_screen_active(), lv_color_hex(0x000000),
                              LV_PART_MAIN | LV_STATE_DEFAULT); // SET BG COLOR

    lv_style_init(&style_battery);
    lv_font_t *font_battery = lv_tiny_ttf_create_data(droid_sans_fallback_font, droid_sans_fallback_font_size, adjusted_font_size_battery);
    lv_style_set_text_font(&style_battery, font_battery);
    lv_style_set_text_align(&style_battery, LV_TEXT_ALIGN_CENTER);
    lv_style_set_text_color(&style_battery, lv_color_hex(0x000000)); // 黑色
    




    ret = xiaozhi_ui_obj_init();
    if (ret != RT_EOK)
    {
        return;
    }

    xiaozhi_ui_update_ble("close");
    xiaozhi_ui_chat_status("连接中...");
    xiaozhi_ui_chat_output("等待连接...");
    xiaozhi_ui_update_emoji("neutral");

    while (1)
    {
        rt_uint32_t btn_event;
        if (rt_mb_recv(g_button_event_mb, &btn_event, 0) == RT_EOK)
        {
            rt_kprintf("button event: %d\n", btn_event);
            switch (btn_event)
            {
            case BUTTON_EVENT_PRESSED:
                // if (g_state == kDeviceStateSpeaking)
                {
                    ws_send_speak_abort(&g_xz_ws.clnt, g_xz_ws.session_id,
                                        kAbortReasonWakeWordDetected);
                    xz_speaker(0); // 关闭扬声器
                }
                ws_send_listen_start(&g_xz_ws.clnt, g_xz_ws.session_id,
                                     kListeningModeManualStop);
                xiaozhi_ui_chat_status("聆听中...");
                xz_mic(1);
                break;

            case BUTTON_EVENT_RELEASED:
                xiaozhi_ui_chat_status("待命中...");
                ws_send_listen_stop(&g_xz_ws.clnt, g_xz_ws.session_id);
                xz_mic(0);
                break;

            default:
                break;
            }
        }

        if (RT_EOK == rt_sem_trytake(&update_ui_sema))
        {
            ms = lv_task_handler();

            char *current_text = lv_label_get_text(global_label1);
            /*
                    if (current_text)
                    {
                        rt_kprintf("Label text: %s\n", current_text);
                    }
            */
#ifdef BSP_USING_PM
            if (strcmp(current_text, "聆听中...") == 0)
            {
                lv_display_trigger_activity(NULL);
            }
            if (lv_display_get_inactive_time(NULL) > IDLE_TIME_LIMIT)
            {
                LOG_I("10s no action \n");
                gui_pm_fsm(GUI_PM_ACTION_SLEEP);
            }

            if (gui_is_force_close())
            {
                LOG_I("in force_close");
                bool lcd_drawing;
                rt_device_control(lcd_device, RTGRAPHIC_CTRL_GET_BUSY,
                                  &lcd_drawing);
                if (!lcd_drawing)
                {
                    LOG_I("no input:%d", lv_display_get_inactive_time(NULL));
                    gui_suspend();
                    LOG_I("ui resume");
                    /* force screen to redraw */
                    lv_obj_invalidate(lv_screen_active());
                    /* reset activity timer */
                    lv_display_trigger_activity(NULL);
                }
            }
#endif // BSP_USING_PM

            rt_thread_mdelay(ms);
            rt_sem_release(&update_ui_sema);
        }
    }
}
