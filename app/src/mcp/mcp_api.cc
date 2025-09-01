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
#if defined(BSP_USING_BOARD_SF32LB52_NANO_52J) || defined(BSP_USING_BOARD_SF32LB52_XTY_AI_THT)
        rt_pin_write(32, PIN_HIGH);
#else
        if (RGBLEDTool::is_color_cycling_)
        {
            RGBLEDTool::is_color_cycling_ = false;
            rt_thread_mdelay(100);
            GetRGBLEDController().SetColor(0x000000);  // 关灯
        }
#endif

    }  




} // extern "C"