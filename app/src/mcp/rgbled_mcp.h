#ifndef RGBLED_MCP_H
#define RGBLED_MCP_H

#include "mcp_server.h"
#include "rtthread.h"
#include "drv_io.h"
#include "stdio.h"
#include "string.h"
#include "drivers/rt_drv_pwm.h"
#include "bf0_hal.h"

// 定义颜色常量
#define RGBLED_NAME    "rgbled"

struct rt_color
{
    const char *color_name;
    uint32_t color;
};

static struct rt_color rgb_color_arry[] =
{
    {"black", 0x000000},
    {"blue", 0x0000ff},
    {"green", 0x00ff00},
    {"cyan",  0x00ffff},
    {"red", 0xff0000},
    {"purple", 0xff00ff},
    {"yellow", 0xffff00},
    {"white", 0xffffff}
};

// RGB LED控制类
class RGBLEDController {
public:
    RGBLEDController() {
        Init();
    }

    void SetColor(uint32_t color) {
        struct rt_rgbled_configuration configuration;
        configuration.color_rgb = color;
        rt_kprintf("Setting RGB LED color: 0x%06X\n", color);
        rt_device_control(rgbled_device_, PWM_CMD_SET_COLOR, &configuration);
    }

private:
    rt_device_t rgbled_device_;

    void Init() {
#ifdef SF32LB52X
        HAL_PIN_Set(PAD_PA32, GPTIM2_CH1, PIN_NOPULL, 1);   // RGB LED 52x  pwm3_cc1
#elif defined SF32LB58X
        HAL_PIN_Set(PAD_PB39, GPTIM3_CH4, PIN_NOPULL, 0);//58x          pwm4_cc4
#elif defined SF32LB56X
        HAL_PIN_Set(PAD_PB09, GPTIM3_CH4, PIN_NOPULL, 0);//566
#endif
        /*rgbled poweron*/
#ifdef SF32LB52X
        HAL_PMU_ConfigPeriLdo(PMU_PERI_LDO3_3V3, true, true);
#endif
        rgbled_device_ = rt_device_find(RGBLED_NAME); //find rgb
        if (!rgbled_device_) {
            RT_ASSERT(0);
        }
    }
};

class RGBLEDTool {
public:
    static void RegisterRGBLEDTool(McpServer* server);
    static bool is_color_cycling_;
    static void ColorCycleThreadEntry(void* param);
    static bool IsLightOn();  //灯光状态
};

RGBLEDController& GetRGBLEDController();

#endif // RGBLED_MCP_H