#include "et_keyword.h"
#define ET_UI_LANG1_KWS_WORDS_TOTAL_NUM    10
const static ET_KWS_PARAM et_ui_kws_param_lang1_list[10] = 
{
    {
        .threshold = 130,
        .prefix_flag = 255,
        .major = 1,
        .kws_value = 1,
        .label_length = 8,
        //    x iao zh ix x iao zh ix,130,001,1,255
        .labels = {47, 14, 33, 18, 47, 14, 33, 18} 
    },
    {
        .threshold = 100,
        .prefix_flag = 255,
        .major = 2,
        .kws_value = 0,
        .label_length = 8,
        //    x iao aa ai x iao aa ai,100,000,2,255
        .labels = {47, 14, 57, 53, 47, 14, 57, 53} 
    },
    {
        .threshold = 100,
        .prefix_flag = 255,
        .major = 3,
        .kws_value = 0,
        .label_length = 8,
        //    x iao k ang x iao k ang,100,000,3,255
        .labels = {47, 14, 29, 37, 47, 14, 29, 37} 
    },
    {
        .threshold = 100,
        .prefix_flag = 255,
        .major = 4,
        .kws_value = 0,
        .label_length = 8,
        //    x iao k u x iao k u,100,000,4,255
        .labels = {47, 14, 29, 21, 47, 14, 29, 21} 
    },
    {
        .threshold = 100,
        .prefix_flag = 255,
        .major = 5,
        .kws_value = 0,
        .label_length = 8,
        //    x iao l ing x iao l ing,100,000,5,255
        .labels = {47, 14, 64, 17, 47, 14, 64, 17} 
    },
    {
        .threshold = 100,
        .prefix_flag = 255,
        .major = 6,
        .kws_value = 0,
        .label_length = 8,
        //    x iao n ing x iao n ing,100,000,6,255
        .labels = {47, 14, 51, 17, 47, 14, 51, 17} 
    },
    {
        .threshold = 100,
        .prefix_flag = 255,
        .major = 7,
        .kws_value = 0,
        .label_length = 8,
        //    x iao ii ia x iao ii ia,100,000,7,255
        .labels = {47, 14, 49, 6, 47, 14, 49, 6} 
    },
    {
        .threshold = 100,
        .prefix_flag = 255,
        .major = 8,
        .kws_value = 0,
        .label_length = 8,
        //    n i h ao x iao k ang,100,000,8,255
        .labels = {51, 41, 13, 55, 47, 14, 29, 37} 
    },
    {
        .threshold = 100,
        .prefix_flag = 255,
        .major = 9,
        .kws_value = 0,
        .label_length = 8,
        //    n i h ao x iao l ing,100,000,9,255
        .labels = {51, 41, 13, 55, 47, 14, 64, 17} 
    },
    {
        .threshold = 100,
        .prefix_flag = 255,
        .major = 10,
        .kws_value = 0,
        .label_length = 8,
        //    n i h ao x iao m ang,100,000,10,255
        .labels = {51, 41, 13, 55, 47, 14, 5, 37} 
    }
};

int get_et_ui_kws_param_lang1_list_len()
{
    return ET_UI_LANG1_KWS_WORDS_TOTAL_NUM;
}

const ET_KWS_PARAM* get_et_ui_kws_param_lang1_list_p()
{
    return &et_ui_kws_param_lang1_list[0];
}
