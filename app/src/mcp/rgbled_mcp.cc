#include "rtthread.h"
#include "rgbled_mcp.h"
#include "drv_io.h"
#include "stdio.h"
#include "string.h"
#include "drivers/rt_drv_pwm.h"
#include "bf0_hal.h"
#include "../board/board_hardware.h" 
#define LED_PIN 32 
RGBLEDController& GetRGBLEDController() {
    static RGBLEDController instance;
    return instance;
}

bool RGBLEDTool::is_color_cycling_ = false;
void RGBLEDTool::ColorCycleThreadEntry(void* param) 
{
    open_led();    
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
            // 配置PA32为GPIO输出模式并输出低电平（点亮）
            set_mode_led(); 
            rt_thread_t thread = rt_thread_create("rgb_cycle",
                            ColorCycleThreadEntry, 
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
            close_led();
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
