
#ifndef DECODER_PATCH_H
#define DECODER_PATCH_H

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdbool.h>
#include <stdint.h>
    
    int is_et_ui_delay_keyword(int index, uint8_t prefix_flag);
    int is_et_ui_filter_keyword(int index, uint8_t  prefix_flag);
    int find_phoneme_index(const char *phoneme);
    int8_t et_ui_decoder_recheck_result(char* audio_phone_str, int strLen, int16_t left, int16_t right, int chunk_size, int16_t *aipos,int* kwsIndex,uint8_t *alipath,uint8_t *alipos,uint8_t *label_seq,uint8_t *ph_boundary);

#ifdef __cplusplus
}
#endif

#endif // DECODER_PATCH_H
