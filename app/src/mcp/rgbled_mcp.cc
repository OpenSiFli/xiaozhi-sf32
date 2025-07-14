#include "rtthread.h"
#include "rgbled_mcp.h"
#include "drv_io.h"
#include "stdio.h"
#include "string.h"
#include "drivers/rt_drv_pwm.h"
#include "bf0_hal.h"

RGBLEDController& GetRGBLEDController() {
    static RGBLEDController instance;
    return instance;
}

bool RGBLEDTool::is_color_cycling_ = false;
void RGBLEDTool::ColorCycleThreadEntry(void* param) {
    int index = 0;
    while (is_color_cycling_) {
        uint32_t color = rgb_color_arry[index].color;
        GetRGBLEDController().SetColor(color);
        index = (index + 1) % (sizeof(rgb_color_arry)/sizeof(rgb_color_arry[0]));
        rt_thread_mdelay(500);
    }
}
bool RGBLEDTool::IsLightOn() {
    // 如果正在循环变色，则认为灯是开启状态
    return is_color_cycling_;
}
void RGBLEDTool::RegisterRGBLEDTool(McpServer* server) {
    // 循环变色工具
        server->AddTool(
            "self.led.turn_on_the_light",
            "turn on the light.",
            PropertyList(),
            [](const PropertyList&) -> ReturnValue {
                if (is_color_cycling_) return true;

                is_color_cycling_ = true;

                rt_thread_t thread = rt_thread_create("rgb_cycle",
                    ColorCycleThreadEntry,  // 使用静态函数
                    nullptr,
                    1024,
                    10,
                    10);

                if (thread) rt_thread_startup(thread);
                return true;
        }
    );

    server->AddTool(
        "self.led.turn_off_the_light",
        "turn off the light.",
        PropertyList(),
        [](const PropertyList&) -> ReturnValue {
            is_color_cycling_ = false;
            GetRGBLEDController().SetColor(0x000000);
            return true;
        }
    );

    server->AddTool(
        "self.led.get_light_status",
        "Get the current status of the LED (on or off).",
        PropertyList(),
        [](const PropertyList&) -> ReturnValue {
            return RGBLEDTool::IsLightOn();
        }
    );
}
