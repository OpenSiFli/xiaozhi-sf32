#ifndef _ET_ASR_DEFAULT_STATE_H
#define _ET_ASR_DEFAULT_STATE_H

#include "lib_et_asr.h"

#if ET_ASR_UI_PRINT_STATES_EN == 1
signed char* get_et_d_states_buf_row_p(int row);
unsigned char* get_et_d_states_dec_bits_buf_row_p(int row);
#endif //ET_ASR_UI_PRINT_STATES_EN

#endif //_ET_ASR_DEFAULT_STATE_H
