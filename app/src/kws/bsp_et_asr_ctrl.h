#ifndef __BSP_ET_ASR_CTRL_H
#define __BSP_ET_ASR_CTRL_H
#include <stdio.h>
#include <stdint.h>

#include <lib_et_asr.h>

#if 1//ET_WAKEUP_EN

void et_bsp_ctrl_main_init();
void et_bsp_ctrl_exit();
unsigned int et_asr_get_time_cnt();

void et_record_q_data_push(int16_t *data, int len);
void et_asr_recognition_process();

void et_open_record();
void et_close_record();
void et_asr_file_test();
#endif // ET_WAKEUP_EN

#endif
