#include "mcp_api.h"
#include "mcp_server.h"
#include "cJSON.h"
#include <rtthread.h>
#include <string.h>
#include "rgbled_mcp.h"


extern "C" 
{
    //注册IoT设备(可多个设备)
    void McpServer_ParseMessage(const char* message)
    {
        McpServer::GetInstance().ParseMessage(message);                
    }

    void MCP_RGBLED_CLOSE()
    {
#ifdef BSP_USING_BOARD_SF32LB52_NANO_52J
        rt_pin_write(32, PIN_HIGH);
#else
        if (RGBLEDTool::is_color_cycling_)
        {
            GetRGBLEDController().SetColor(0x000000);
            RGBLEDTool::is_color_cycling_ = false;
            GetRGBLEDController().SetColor(0x000000);  // 关灯
            rt_thread_mdelay(100);
        }
#endif

    }  




} // extern "C"