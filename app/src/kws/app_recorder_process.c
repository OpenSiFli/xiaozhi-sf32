#include <rtthread.h>
#include <string.h>
#include <stdlib.h>
#include "audio_server.h"
#include "bsp_et_asr_ctrl.h"
#include "lib_et_asr.h"
#include "xiaozhi_client_public.h"
#include "app_recorder_process.h"


#define DBG_TAG           "kws"
#define DBG_LVL           AUDIO_DBG_LVL

#include "log.h"

#undef app_malloc
#undef app_free
#undef printf
#undef LOG_I

#define LOG_I       rt_kprintf
#define printf      rt_kprintf
#define app_malloc  rt_malloc
#define app_free    rt_free

#define ET_ASR_RECORD_BUF_SIZE   640

#define KWS_THREAD_NAME "kws_thread"

#define RECORD_DATA_MAX_SIZE    (ET_ASR_RECORD_BUF_SIZE * 2)

#define KWS_QUEUE_NUM       3

#define KWS_EVT_START       (1 << 0)
#define KWS_EVT_STOP        (1 << 1)
#define KWS_EVT_DECODE      (1 << 2)
#define KWS_EVT_ALL        (KWS_EVT_START|KWS_EVT_STOP|KWS_EVT_DECODE)

#if defined(__CC_ARM) || defined(__CLANG_ARM)
L2_RET_BSS_SECT_BEGIN(g_kws_queue_buffer)
static uint8_t g_kws_queue_buffer[KWS_QUEUE_NUM][RECORD_DATA_MAX_SIZE];
L2_RET_BSS_SECT_END
#else
static uint8_t g_kws_queue_buffer[KWS_QUEUE_NUM][RECORD_DATA_MAX_SIZE] L2_RET_BSS_SECT(g_kws_queue_buffer);
#endif

#define KWS_THREAD_STACK_SIZE (3072)

#if defined(__CC_ARM) || defined(__CLANG_ARM)
L2_RET_BSS_SECT_BEGIN(kws_thread_stack) //6000地址
static uint32_t kws_thread_stack[KWS_THREAD_STACK_SIZE / sizeof(uint32_t)];
L2_RET_BSS_SECT_END
#else
static uint32_t kws_thread_stack[KWS_THREAD_STACK_SIZE / sizeof(uint32_t)] L2_RET_BSS_SECT(kws_thread_stack);
#endif

static struct rt_thread kws_thread;

typedef struct
{
    rt_slist_t  node;
    uint8_t     *data;
} kws_queue_t;

typedef struct
{
    audio_client_t  client;
    rt_thread_t     record_data_thread;
    rt_event_t      event;
    kws_queue_t     queue[KWS_QUEUE_NUM];
    uint16_t        record_offset;
    rt_slist_t      recording;
    rt_slist_t      recorded;
    uint8_t         is_inited;
    uint8_t         is_exit;
} kws_data_t;

static kws_data_t instance;



uint8_t youjie_offline_using_type_get(void)
{
//0   仅离线语音识别
//1   离线识别和vad都使用
//2   仅vad静音检测
    return 1;
}

#ifdef __GNUC__
void __2printf(const char *fmt, ...)
{
}
#endif
void et_asr_up_vad_callback(s16_et *ptr, u32_et samples, u16_et idx)
{
    // zai xian shi bie

}

void et_asr_up_wordstart(int ret)
{
#if ET_LOG_DEBUG_EN
//    printf("wordstart,ret%d\n", ret);
#endif //ET_LOG_DEBUG_EN
    if(ret == 3)
    {
        //rt_kprintf("AUDIO_IOCTL_ENABLE_CPU_LOW_SPEE 1 \n");
        audio_ioctl(instance.client, AUDIO_IOCTL_ENABLE_CPU_LOW_SPEED, (void *)0); //退出低速模式
		//clk_set("sys",PLL_240M_LIMIT);
        printf("sentence_start\n");
    }
}

void et_asr_up_wordend(int ret, u16_et voice_cnt)
{
#if ET_LOG_DEBUG_EN
    //printf("wordend,ret:%d\n", ret);
#endif //ET_LOG_DEBUG_EN
    if(ret == 250 || ret == 251)
    {
        // voice_cnt * 40 = Lu Yin Shi Chang
        //printf("sentence_end\n");
		//clk_set("sys",PLL_32M_LIMIT);
		//clk_set("sys",PLL_24M_LIMIT);
		//clk_set("sys",PLL_48M_LIMIT);
    }
    //rt_kprintf("AUDIO_IOCTL_ENABLE_CPU_LOW_SPEE 2 \n");
    //audio_ioctl(instance.client, AUDIO_IOCTL_ENABLE_CPU_LOW_SPEED, (void *)1);  //进入低速模式
}


static int app_audio_record_callback(audio_server_callback_cmt_t cmd, void *callback_userdata, uint32_t reserved)
{
    //LOG_I("callback\n");

    kws_data_t *thiz = (kws_data_t *)callback_userdata;

    if (cmd == as_callback_cmd_data_coming)
    {
        audio_server_coming_data_t *p = (audio_server_coming_data_t *)reserved;

        RT_ASSERT(p->data_len == 320);

        rt_slist_t *idle;
        rt_enter_critical();
        idle = rt_slist_first(&thiz->recording);
        rt_exit_critical();
        if (!idle)
        {
            //LOG_I("drop mic data\n");
            return 0;
        }
        kws_queue_t *queue = rt_container_of(idle, kws_queue_t, node);
        RT_ASSERT(thiz->record_offset + p->data_len <= RECORD_DATA_MAX_SIZE);
        memcpy(queue->data + thiz->record_offset, p->data, p->data_len);
        thiz->record_offset += p->data_len;
        if (thiz->record_offset >= RECORD_DATA_MAX_SIZE)
        {
            thiz->record_offset = 0;
            rt_enter_critical();
            rt_slist_remove(&thiz->recording, idle);
            rt_slist_append(&thiz->recorded, idle);
            rt_exit_critical();
             rt_event_send(thiz->event, KWS_EVT_DECODE);
        }
    }
    return 0;
}

static void record_data_thread_callback(void *parameter)
{
    kws_data_t *thiz = (kws_data_t *)parameter;

    int kws_ret = 0;

    audio_parameter_t pa = {0};
    pa.write_bits_per_sample = 16;
    pa.write_channnel_num = 1;
    pa.write_samplerate = 16000;
    pa.read_bits_per_sample = 16;
    pa.read_channnel_num = 1;
    pa.read_samplerate = 16000;
    pa.read_cache_size = 0;
    pa.write_cache_size = 0;

    thiz->client = audio_open(AUDIO_TYPE_LOCAL_RECORD, AUDIO_RX, &pa, app_audio_record_callback, thiz);
    RT_ASSERT(thiz->client);

    while (!thiz->is_exit)
    {
        rt_uint32_t evt;
        rt_event_recv(thiz->event, KWS_EVT_ALL, RT_EVENT_FLAG_OR | RT_EVENT_FLAG_CLEAR, RT_WAITING_FOREVER, &evt);
        if (evt & KWS_EVT_STOP)
        {
            rt_slist_t *list;
            LOG_I("kws evt stop\n");
            //rt_kprintf("AUDIO_IOCTL_ENABLE_CPU_LOW_SPEE 3 \n");
            audio_ioctl(thiz->client, AUDIO_IOCTL_ENABLE_CPU_LOW_SPEED, (void *)0);
            audio_close(thiz->client);
            thiz->client = NULL;

            /*audio设备已关闭，不会有新数据来，旧数据清掉，防止下次有垃圾数据*/
            rt_enter_critical();
            while (1)
            {
                list = rt_slist_first(&thiz->recorded);
                if (!list)
                {
                    break;
                }
                rt_slist_remove(&thiz->recorded, list);
                rt_slist_append(&thiz->recording, list);
            }
            thiz->record_offset = 0;
            rt_exit_critical();            
            continue;
        }
        if (evt & KWS_EVT_START)
        {
            LOG_I("kws evt start\n");
            thiz->client = audio_open(AUDIO_TYPE_LOCAL_RECORD, AUDIO_RX, &pa, app_audio_record_callback, thiz);
            RT_ASSERT(thiz->client);            
            continue;
        }
        if (!(evt & KWS_EVT_DECODE))
        {
            LOG_I("kws evt unknow\n");
            continue;
        }
        char *text;
        rt_slist_t *recorded;
        rt_enter_critical();
        recorded = rt_slist_first(&thiz->recorded);
        rt_exit_critical();
        if (!recorded)
        {
            LOG_I("should never coming here");
            continue;
        }
        kws_queue_t *queue = rt_container_of(recorded, kws_queue_t, node);
        //uint32_t cj_start_time = et_asr_get_time_cnt();
        et_asr_wakeup_buf_write((short*)(short *)queue->data, ET_ASR_RECORD_BUF_SIZE, 1, 2, 0, 1);	
        rt_enter_critical();
        rt_slist_remove(&thiz->recorded, recorded);
        rt_slist_append(&thiz->recording, recorded);
        recorded = rt_slist_first(&thiz->recorded);
        rt_exit_critical();

        if (recorded)
        {
             rt_event_send(thiz->event, KWS_EVT_DECODE);
        }

        //uint32_t cj_cost_time = et_asr_get_time_cnt() - cj_start_time;
        //printf("cost:%d\n", cj_cost_time);
#if 0//defined(WK_WANSON_ASR_SUPPORT_LIB)
        rs = Wanson_ASR_Recog((short *)&send_buffer[i * RECORD_DATA_MAX_SIZE], ET_ASR_RECORD_BUF_SIZE, (const char **)&text, &score);
#endif
    }

LOG_I("---shouldl not coming here now\n");
// RT_ASSERT(0);
return;
    //rt_kprintf("AUDIO_IOCTL_ENABLE_CPU_LOW_SPEE 4 \n");
    audio_ioctl(thiz->client, AUDIO_IOCTL_ENABLE_CPU_LOW_SPEED, (void *)0);
    audio_close(thiz->client);
    thiz->client = NULL;

    for (int i = 0; i < KWS_QUEUE_NUM; i++)
    {
        app_free(thiz->queue[i].data);
        thiz->queue[i].data = NULL;
    }
    thiz->is_exit = 2;
    LOG_I("kws exit");
}


void kws_start(kws_data_t *thiz)
{
    if (thiz->is_inited)
    {
        return;
    }
    thiz->is_inited = 1;
    thiz->is_exit = 0;
    g_kws_running = 1;
    
    et_bsp_ctrl_main_init();
    rt_slist_init(&thiz->recording);
    rt_slist_init(&thiz->recorded);
    for (int i = 0; i < KWS_QUEUE_NUM; i++)
    {
        thiz->queue[i].data = g_kws_queue_buffer[i]; // 使用L2区静态buffer
        rt_slist_append(&thiz->recording, &thiz->queue[i].node);
    }

    thiz->event = rt_event_create("kws", RT_IPC_FLAG_FIFO);

    rt_err_t result = rt_thread_init(&kws_thread,
                                     KWS_THREAD_NAME,
                                     record_data_thread_callback,
                                     thiz,
                                     &kws_thread_stack[0],
                                     KWS_THREAD_STACK_SIZE,
                                     RT_THREAD_PRIORITY_LOW,
                                     RT_THREAD_TICK_DEFAULT);
    if (result == RT_EOK)
    {
        thiz->record_data_thread = &kws_thread;
        rt_thread_startup(thiz->record_data_thread);
    }
    else
    {
        LOG_I("Failed to init kws thread\n");
        thiz->record_data_thread = RT_NULL;
    }
    
    RT_ASSERT(thiz->record_data_thread);
}

void kws_stop(kws_data_t *thiz)
{
    thiz->is_exit = 1;
    rt_event_send(thiz->event, KWS_EVT_STOP);
    while (rt_thread_find(KWS_THREAD_NAME))
    {
        LOG_I("wait thread %s exit", KWS_THREAD_NAME);
        rt_thread_mdelay(100);
    }

    rt_event_delete(thiz->event);
    thiz->event = NULL;
    thiz->is_exit = 0;
    thiz->is_inited = 0;
    g_kws_running = 0;
    et_bsp_ctrl_exit();
}

void kws_demo_stop()
{
    LOG_I("kws_demo_stop\n");
    
    // 检查是否已经初始化
    if (!instance.is_inited) {
        LOG_I("KWS system is not running\n");
        return;
    }
    
    kws_stop(&instance);

    LOG_I("KWS system completely stopped\n");
}
extern void et_bsp_ctrl_main_init();

void kws_demo()
{
    LOG_I("kws_demo\n");
    kws_start(&instance);
}

