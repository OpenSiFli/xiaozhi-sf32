#ifndef XIAOZHI_WEATHER_H
#define XIAOZHI_WEATHER_H
#include "time.h"


#ifdef __cplusplus
extern "C" {
#endif

typedef struct date_time
{
    int year;     /* eg: 2024 */
    int month;    /* eg:  1 (1-12) */
    int day;      /* eg: 31 (1~xx) */
    int hour;     /* eg:  8 (0~23) */
    int minute;   /* eg: 30 (0-59) */
    int second;   /* eg:  0 (0~59) */
    int weekday;  // 0=Sunday, 1=Monday, ...
    char weekday_str[10];
    char date_str[64];    // 增大缓冲区大小
    char time_str[32];    // 增大缓冲区大小
} date_time_t;


// 天气信息结构体
typedef struct {
    char location[64];      // 城市名称
    char text[32];          // 天气现象文字，如"多云"
    char code[8];           // 天气现象代码，如"4"
    int temperature;        // 温度，摄氏度
    time_t last_update;     // 最后更新时间
} weather_info_t;



//初始化时间管理模块 天气模块
int xiaozhi_time_weather_init(void);
int xiaozhi_ntp_sync(void);
int xiaozhi_weather_get(weather_info_t *weather_info);
void update_xiaozhi_ui_time(void *parameter);
void update_xiaozhi_ui_weather(void *parameter);
void xiaozhi_time_weather(void);
void time_ui_update_callback(void);
void weather_ui_update_callback(void);
int xiaozhi_time_get_current(date_time_t *time_info);


#ifdef __cplusplus
}
#endif

#endif // XIAOZHI_WEATHER_H