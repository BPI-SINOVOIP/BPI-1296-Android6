//  $Id$
/*************************************************************************************************
DEI engine

Realtek 2013/10/27
by Tracey Chen
**************************************************************************************************/


#ifndef __DEI_H
#define __DEI_H

//#########################################
#include <string.h>
#include "IMGpointer.h"
#include "DEI_motion_detect.h"
#include "buffer_2D.h"

//using namespace std;
// #define WEAVE_MODE
//#define WEAVE_FORWARD 0
//#define WEAVE_BACKWARD 1
//#define WEAVE_AVERAGE  2
#define BOB_MODE 3
#define MAD_MODE 4

//reg_YC420_MODE
#define YC_420_MODE_OFF		0
#define YC_420_NORMAL_MODE	1
#define YC_420_AVERAGE_MODE	2
#ifdef __cplusplus
extern "C" {
#endif
//#########################################
typedef struct DEI
{
    BUFFER_2D	   *mf_read, *mf_write;
    int dei_width, dei_height;
    signed char *b_combing_buffer, *b_combing_buffer_top;
    int b_combing_width;
#ifndef USE_NEON
    unsigned short *ratio_array;
#endif

    //internal data .....
    int pre_l, nxt_l, top_l, bot_l;
    int input_width, input_height;
    /*bool*/ int bottom_field_first_line;
    /*bool*/ int top_field_last_line;

    // register setting
    int reg_YC420_MODE;	// 0:disable // 1:normal 420 mode // 2:average 420 mode

    int reg_DI_mode;
    int reg_final_mode;     //might fail this line

    /*bool*/ int reg_HW_escape_enable;
    /*bool*/ int DBG_BLEND_RATIO;
    /*bool*/ int DBG_CHK_AREA;
    /*bool*/ int DBG_COMBING;

//public:
    //----------------------
    BUFFER_2D     mf_top_field, mf_bot_field;
    DEI_MD_NEW    NEW_MOTION_DETECT;
    //----------------------
}DEI;

void DEI_init(DEI *dei, const int width, const int height);
void DEI_uninit(DEI *dei);
/*
void Bob_even_line(IMGpointer2 out, const IMGpointer3 even);
void Bob_odd_line(IMGpointer2 out, const IMGpointer3 odd);
*/
void blending_mode(IMGpointer2 out, const IMGpointer3 cur);

void DEI_process_one_image_odd(DEI *dei, DEI *dei2, const int  cur_field,
                           IMGpointer2 out2,
//                           int& reg_PN_diff_count,
//                           int& reg_NC_diff_count,
//                           int& reg_NC_combing_count,
//                           int& reg_final_combing_count,
                           const IMGpointer3 pre_img3,
                           const IMGpointer3 cur_img3,
                           const IMGpointer3 nxt_img3);

void DEI_process_one_image_even(DEI *dei, DEI *dei2, const int  cur_field,
                           IMGpointer2 out2,
//                           int& reg_PN_diff_count,
//                           int& reg_NC_diff_count,
//                           int& reg_NC_combing_count,
//                           int& reg_final_combing_count,
                           const IMGpointer3 pre_img3,
                           const IMGpointer3 cur_img3,
                           const IMGpointer3 nxt_img3);
//};
//#########################################
#ifdef __cplusplus
}
#endif
#endif


