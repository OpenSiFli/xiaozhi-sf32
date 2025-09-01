#include "thing.h"

// 添加 C 接口头文件
extern "C" {
#include "audio_server.h"
//#include "../xiaozhi_ui.h"
extern void xiaozhi_ui_update_volume(int volume);
}


#define TAG "Speaker"

namespace iot {

class Speaker : public Thing {
public:
    Speaker() : Thing("Speaker", "扬声器") {
        // 定义属性：volume（当前音量值）
        properties_.AddNumberProperty("volume", "当前音量值(0到15之间)", [this]() -> int {
            return audio_server_get_private_volume(AUDIO_TYPE_LOCAL_MUSIC);
        });

        // 定义方法：SetVolume（设置音量）
        methods_.AddMethod("SetVolume", "设置音量", ParameterList({
            Parameter("volume", "0到15之间的整数", kValueTypeNumber, true)
        }), [this](const ParameterList& parameters) {
            uint8_t volume = static_cast<uint8_t>(parameters["volume"].number());
            if(volume > 15) {
                volume = 15;
            }
            
            audio_server_set_private_volume(AUDIO_TYPE_LOCAL_MUSIC, volume);
            xiaozhi_ui_update_volume(volume);
        });

        // 新增方法：GetVolume（获取音量）
        methods_.AddMethod("GetVolume", "获取当前音量", ParameterList(),
            [this](const ParameterList&) {
                return audio_server_get_private_volume(AUDIO_TYPE_LOCAL_MUSIC); // 直接返回音频服务获取的值
            });

        
    }
};

} // namespace iot

DECLARE_THING(Speaker);