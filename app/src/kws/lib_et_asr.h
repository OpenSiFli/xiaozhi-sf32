#ifndef _LIB_ET_ASR_H__
#define _LIB_ET_ASR_H__

#ifdef __cplusplus
extern "C"
{
#endif // __cplusplus



#if 0 //ndef __linux__
typedef unsigned char u8_et;
typedef signed char s8_et;
typedef unsigned short u16_et;
typedef signed short s16_et;
typedef unsigned int uint_et;
typedef unsigned long u32_et;
typedef signed long s32_et;
typedef unsigned long long u64_et;
typedef signed long long s64_et;
#else
#include <stdint.h>
#include <stdbool.h>
typedef unsigned char u8_et;
typedef signed char s8_et;
typedef unsigned short u16_et;
typedef signed short s16_et;
typedef unsigned int uint_et;
typedef unsigned int u32_et;
typedef signed int s32_et;
typedef unsigned long long u64_et;
typedef signed long long s64_et;
#endif // __linux__



#define ET_VAD_ONCE_BUF_SIZE    160 //
#define ET_VAD_ONCE_BUF_NUM    6 //
#define ET_ASR_MIC_CH_NUM   1
#define ET_ASR_D_MASTER_MIC_IDX   0
#define ET_ASR_ONE_FRAME_SIZE  (ET_VAD_ONCE_BUF_SIZE * 4) //512//(ET_VAD_ONCE_BUF_SIZE * 4)
#define ET_VAD_ONE_FRAME_SIZE  (ET_ASR_ONE_FRAME_SIZE)
#define ET_VAD_ONE_UINI_BUF_MAX_SIZE (ET_VAD_ONE_FRAME_SIZE * ET_ASR_MIC_CH_NUM)
#define ET_UI_ENROLLED_EMB_LEN_MAX   64

#define ET_VAD_TYPE_CLOSE    0
#define ET_VAD_TYPE_PURE_DB    1
#define ET_VAD_TYPE_HUMAN_VOL    2
//#define ET_VAD_TYPE_HUMAN_FORWARD    3
#define ET_VAD_TYPE_HUMAN_FEATURE    4
#define ET_VAD_TYPE_ONLY_FORWARD    5
#define ET_VAD_TYPE_ONLY_VAD    6

#ifdef __linux__
#define ET_ASR_NEED_FFT_TYPE_EN    0
#define ET_ASR_NEED_UI_HEAP_EN   0
#define ET_ASR_KWS_LIST_BIN_EN    0 //kws_labels.bin

#define ET_ASR_UI_PRINT_STATES_EN   1//1 ,Shi Yong//2, Sheng Cheng//0 , Guan Bi//songjian
#define ET_ASR_UI_VAD_TYPE   ET_VAD_TYPE_HUMAN_VOL //ET_VAD_TYPE_PURE_DB //ET_VAD_TYPE_HUMAN_VOL
#else
#define ET_ASR_NEED_FFT_TYPE_EN    0
#define ET_ASR_NEED_UI_HEAP_EN   0
#define ET_ASR_KWS_LIST_BIN_EN    0 //kws_labels.bin

#define ET_ASR_UI_PRINT_STATES_EN   1//1 ,Shi Yong//2, Sheng Cheng//0 , Guan Bi//songjian
#define ET_ASR_UI_VAD_TYPE   ET_VAD_TYPE_HUMAN_VOL //ET_VAD_TYPE_PURE_DB //ET_VAD_TYPE_HUMAN_VOL
#endif // __linux__



#if ET_ASR_UI_VAD_TYPE == ET_VAD_TYPE_PURE_DB
#define ET_VAD_PRE_DOUBLE_BUFF_EN  6
#elif ET_ASR_UI_VAD_TYPE > 0
#define ET_VAD_PRE_DOUBLE_BUFF_EN  14
#else
#define ET_VAD_PRE_DOUBLE_BUFF_EN  0
#endif //ET_ASR_UI_VAD_TYPE == ET_VAD_TYPE_PURE_DB

typedef void (*et_asr_wakeup_event_cb_t)(int event);
typedef u32_et (*et_asr_time_cb_t)(void);
#if ET_ASR_NEED_FFT_TYPE_EN
typedef void (*et_fft_user_init_cb_t)(void);
typedef void (*et_fft_forward_cb_t)(s32_et *buf);
typedef void (*et_fft_inverse_cb_t)(s32_et* buf);
#endif // ET_ASR_NEED_FFT_TYPE_EN
typedef void (*et_asr_vad_cb_t)(s16_et *ptr, u32_et length, u16_et idx);
typedef void (*et_vad_wordstart_cb_t)(int ret);
typedef void (*et_vad_wordend_cb_t)(int ret, u16_et voice_cnt);

typedef struct {
    u8_et  threshold;           // The threshold of kws
    u8_et  prefix_flag;           // The threshold of sure
    u8_et  major;                  // 1 -->> Major Kws, 0 -->> Shor instruction
//    u8_et  recheck_idx;        // key idx in branch2
    u8_et  label_length;           // The length of key label
    u16_et  kws_value;              // The event id for mcu
    u8_et labels[48];             // The label of key words
} ET_KWS_PARAM;

typedef struct {
//    u8_et vadEn;
//    u8_et sliceType;

    u8_et once_buf_num;

    char* weightFile;
    int weightLen;
    u8_et* thres_list_buf;
    char* c_soft_key;
    #if ET_ASR_KWS_LIST_BIN_EN
    #endif // ET_ASR_KWS_LIST_BIN_EN
    const ET_KWS_PARAM* m_kws_param_buf;
    int m_kws_param_cnt;

#if ET_ASR_NEED_UI_HEAP_EN
    char *et_private_heap;
#endif //ET_ASR_NEED_UI_HEAP_EN
    et_asr_time_cb_t time_cb;
    #if ET_ASR_NEED_FFT_TYPE_EN
    et_fft_user_init_cb_t fft_init_cb;
    et_fft_forward_cb_t forward_cb;
    et_fft_inverse_cb_t inverse_cb;
    #endif // ET_ASR_NEED_FFT_TYPE_EN
} et_asr_kws_cfg_t;

typedef struct {
	u8_et et_soft_vad_en;
	u8_et et_wake_times_thd;
	u8_et et_wake_times_reset_thd;
	u8_et et_hm_db_max_thd;
	u8_et et_hm_max_thd;
	u8_et et_vad_prob_thd;
	u8_et et_hw_db_max_thd;
	u8_et et_hw_rel_db_max_thd;
	u8_et et_bg_noise_base_db;
	u8_et et_bg_noise_rel_db_max_thd;
	u8_et et_bg_noise_hm_thd;

	u16_et et_hm_no_cnt_max;
	//u16_et et_hm_voice_max_num;
	u16_et et_sentence_voiceCnt;
	u16_et et_H_noVoiceCnt;
	u16_et et_sentence_start;
	u16_et et_vad_pre_cnt;
} et_hm_com_cfg_t;


u32_et get_et_asr_time_cnt(void);
#if ET_ASR_NEED_FFT_TYPE_EN
void et_asr_fft_user_init(void);//256samples
void et_asr_fft_forward(s32_et *buf);//FFT
void et_asr_fft_inverse(s32_et* buf);//iFFT
#endif // ET_ASR_NEED_FFT_TYPE_EN
int et_asr_wakeup_first_init(et_asr_kws_cfg_t* cfg, et_asr_wakeup_event_cb_t cb);

int et_asr_wakeup_init(void);
void et_asr_wakeup_exit(void);
u8_et is_et_word_start();
u16_et is_et_sentence_start();

int et_asr_wakeup_buf_write(s16_et *ptr, u32_et samples, u8_et voice_flag, u8_et step, u8_et voice_pos, u8_et num);
int et_asr_buf_write(s16_et *ptr, u32_et samples);
void et_reset_asr_state(void);
void et_start_asr(void);
void et_stop_asr(void);


u8_et is_et_asr_work(void);
u8_et is_et_asr_parm_suc(void);
u8_et is_et_asr_initing(void);
u8_et is_et_asr_exitting(void);
u8_et et_search_kws_list_bin(int row, int col);

et_asr_kws_cfg_t* get_et_asr_kws_cfg_p(void);

void reset_et_vad_states(void);


void set_et_noise_env(u8_et flag);
u8_et get_et_noise_env(void);
u16_et is_et_vad_wake(void);
u16_et is_et_sentence_wake(void);


u8_et is_et_asr_nosafe(void);

u8_et is_et_off_asr_activited(void);
s8_et get_et_left_asr_cnt(void);
void et_open_mem_pools_init(void); //
void et_open_mem_pools_destroy(void);
void* et_open_mem_allocate(int len);
void* et_open_mem_zllocate(int len);
void et_open_mem_free(void* ptr);
const char* get_auth_language_name(void);
const char* get_cur_language_name(void);
#if ET_ASR_KWS_LIST_BIN_EN
u8_et* get_et_kws_labels(int row, u8_et* labels);
#endif // ET_ASR_KWS_LIST_BIN_EN




u8_et get_vad_prob(void);
void reset_vad_vad_prob(void);
int getPcmDB(s16_et* data, int len);
int getMaxDbFrom4(s16_et* data, int len);

u16_et get_et_lib_rhythm2(void);
void reset_et_lib_rhythm2(void);
void et_reset_fbank_begin(void);
et_hm_com_cfg_t* get_et_hm1_cfg_p(void);

/**


void set_et_ui_soft_vad()
{
    et_hm_com_cfg_t*  t_cfg_p = get_et_hm1_cfg_p();
#if ET_ASR_UI_PRINT_STATES_EN == 2
    t_cfg_p->et_soft_vad_en = 0;
#else
    t_cfg_p->et_soft_vad_en = 2;
#endif //ET_ASR_UI_PRINT_STATES_EN
#if ET_ASR_UI_VAD_TYPE == ET_VAD_TYPE_PURE_DB
    t_cfg_p->et_wake_times_thd = 2;
    t_cfg_p->et_wake_times_reset_thd = 6;
    t_cfg_p->et_hm_db_max_thd = 41 * 2;
    t_cfg_p->et_hm_max_thd = 0;
    t_cfg_p->et_vad_prob_thd = 0;
    t_cfg_p->et_hw_db_max_thd = 50 * 2;
    t_cfg_p->et_hw_rel_db_max_thd = 12 * 2;
    t_cfg_p->et_bg_noise_base_db = 39 * 2;
    t_cfg_p->et_bg_noise_rel_db_max_thd = 7 * 2;
    t_cfg_p->et_hm_no_cnt_max = 24;
    //t_cfg_p->et_hm_voice_max_num = 270 * 30;
#else
    t_cfg_p->et_wake_times_thd = 4;
    t_cfg_p->et_wake_times_reset_thd = 6;
#if 1
    t_cfg_p->et_hm_db_max_thd = 41 * 2; //45 * 2 //48 * 2
#else
    t_cfg_p->et_hm_db_max_thd = 1;
#endif //1
    t_cfg_p->et_hm_max_thd = 105;
    t_cfg_p->et_vad_prob_thd = 0;
    t_cfg_p->et_hw_db_max_thd = 55 * 2;
    t_cfg_p->et_bg_noise_base_db = 39 * 2;
    t_cfg_p->et_hw_rel_db_max_thd = 12 * 2;
    t_cfg_p->et_bg_noise_rel_db_max_thd = 7 *2 - 1; //8 * 2;//7 * 2;
    t_cfg_p->et_bg_noise_hm_thd = 9;
    t_cfg_p->et_hm_no_cnt_max = 24;
    //t_cfg_p->et_hm_voice_max_num = 270 * 30;
#endif //
    set_et_soft_vad_config(t_cfg_p);

}



**/

void set_et_soft_vad_config(et_hm_com_cfg_t* cfg_p);
void set_et_hm_vad_bin_addr(char* weightFile);
char* get_et_hm_vad_bin_addr();
u16_et get_et_hm_vad_bin_len();
void set_et_soft_vad_en(u8_et flag);
u8_et is_et_soft_vad_en(void);
s32_et et_entry_level2_vad_cal(s16_et *ptr, u32_et samples);


#ifdef __cplusplus
}
#endif // __cplusplus

#endif // _LIB_ET_ASR_H__
