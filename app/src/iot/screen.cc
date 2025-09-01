#include "thing.h"


// C 接口头文件
extern "C" {
#include <rtthread.h>
extern void xiaozhi_ui_update_brightness(int brightness);
}

// 定义设备名
#define LCD_DEVICE_NAME "lcd"

#define TAG "Screen"

namespace iot {

class Screen : public Thing {
private:
    rt_device_t lcd_device_;  // 类成员变量，避免使用全局静态变量

public:
    Screen() : Thing("Screen", "屏幕"), lcd_device_(nullptr) {
        // 查找 LCD 设备
        lcd_device_ = rt_device_find(LCD_DEVICE_NAME);
        if (lcd_device_ == RT_NULL) {
            rt_kprintf("LCD device not found!");
            RT_ASSERT(0);
        }

        // 属性：当前背光亮度
        properties_.AddNumberProperty("Brightness", "当前背光亮度值(0到100之间)", [this]() -> int {
            uint8_t current_brightness = 0;
            if (lcd_device_) {
                rt_device_control(lcd_device_, RTGRAPHIC_CTRL_GET_BRIGHTNESS, &current_brightness);
            }
            return static_cast<int>(current_brightness);
        });

        // 方法：设置背光亮度
        methods_.AddMethod("SetBrightness", "设置背光亮度", ParameterList({
            Parameter("Brightness", "0到100之间的整数", kValueTypeNumber, true)
        }), [this](const ParameterList& parameters) {
            uint8_t brightness = static_cast<uint8_t>(parameters["Brightness"].number());
            if (brightness > 100) {
                brightness = 100;
            }
            if (brightness < 10 )  brightness = 10;
            if (lcd_device_) {
                rt_device_control(lcd_device_, RTGRAPHIC_CTRL_SET_BRIGHTNESS, &brightness);
                rt_kprintf("Brightness set to: %d", brightness);
                xiaozhi_ui_update_brightness(brightness);
            } else {
                rt_kprintf("LCD device is NULL, cannot set brightness");
            }
        });

        // 新增方法：GetBrightness（获取亮度）
        methods_.AddMethod("GetBrightness", "获取当前背光亮度", ParameterList(),
            [this](const ParameterList&) {
                uint8_t brightness = 0;
                if (lcd_device_) {
                    rt_device_control(lcd_device_, RTGRAPHIC_CTRL_GET_BRIGHTNESS, &brightness);
                }
                return brightness; // 返回实际亮度值
            });
    }
};

} // namespace iot

// 注册为 IoT 设备
DECLARE_THING(Screen);