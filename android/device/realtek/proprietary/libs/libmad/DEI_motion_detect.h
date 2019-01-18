//  $Id$
/*************************************************************************************************
DEI_MD_NEW engine

Realtek 2013/11/20
by Tracey Chen
**************************************************************************************************/


#ifndef __DEI_MD_NEW_H
#define __DEI_MD_NEW_H

#include <stdio.h>
#include <stdlib.h>
//#include "./IMGpointer.h"
//#include "./common.h"

//using namespace std;

#define reg_expand_enable (0)		//bool
#define reg_expand_range (3)		//int
#define reg_light_comb_enable (0)	//bool
#define reg_light_comb_TB_thd (15)	//int
#define reg_light_comb_PN_thd0 (5)	//int
#define reg_light_comb_PN_thd1 (30)	//int
#define reg_light_comb_TW_thd (15)	//int
#define reg_light_comb_BW_thd (15)	//int
#define reg_light_comb_thd (15)		//int
#define reg_mf_l_th (5)			//int
#define reg_mf_h_th (32)		//int
#define reg_chk_l_th (0)		
#define reg_chk_h_th (80)
#define reg_chk_enable_cnt_th (3)
#define reg_chk_disable_cnt_th (10)
#define reg_combing_check_enable (1)	//bool
#define reg_use_wb_diff (1)		//bool
#define reg_combing_thd (10)
#define reg_nc_combing_thd (16)
#define reg_final_combing_thd (16)
#define reg_mf_lpf_enable (1)		//bool
#define reg_use_lpf_data (0)		//bool
#define reg_mf_select (1)		//bool
#define reg_wb_chk_result (0)		//bool
#define reg_chk_use_max_mf (0)		//bool
#define reg_wb_max_diff (0)		//bool
#define reg_r_value (8)
#define reg_escape_thd (64)
#define      reg_LPF3x5_T_C0		(4)
#define      reg_LPF3x5_T_C1		(4)
#define      reg_LPF3x5_T_C2		(4)
#define      reg_LPF3x5_T_C3		(4)
#define      reg_LPF3x5_T_C4		(4)
#define      reg_LPF3x5_C_C0		(4)
#define      reg_LPF3x5_C_C1		(2)
#define      reg_LPF3x5_C_C2		(3)
#define      reg_LPF3x5_C_C3		(2)
#define      reg_LPF3x5_C_C4		(4)
#define      reg_LPF3x5_B_C0		(4)
#define      reg_LPF3x5_B_C1		(4)
#define      reg_LPF3x5_B_C2		(4)
#define      reg_LPF3x5_B_C3		(4)
#define      reg_LPF3x5_B_C4		(4)
#define      reg_LPF1x5_C0		(4)
#define      reg_LPF1x5_C1		(1)
#define      reg_LPF1x5_C2		(2)
#define      reg_LPF1x5_C3		(1)
#define      reg_LPF1x5_C4		(4)
#ifdef __cplusplus
extern "C" {
#endif
//#########################################
typedef struct _DEI_MD_NEW
{
    unsigned char P[1920+12+4], N[1920+12+4], CT[1920+12+4], CB[1920+12+4];
    unsigned char diff[1920+12+4];
//    int lpf3x5_diff[1920+12];
//int lpf3x5_diff[8];
    unsigned char fm_TOP[1920+12+4], fm_BOT[1920+12+4];
    signed char b_combing[1920+12+4];
//    int blend_ratio[1920+12];
    unsigned char blend_ratio[1920+12+4];
#if reg_expand_enable == 1      //it's 0 now
    int expand_blend_ratio[1920+12+4];
#endif
    int chk_area_max_diff, chk_area_max_blend_ratio;
    int bottom_field_first_line;
    int top_field_last_line;

    int chk_enable_cnt, chk_disable_cnt;
    int chk;
} DEI_MD_NEW;
    //----------------------
//public:
    //DEI_MD_NEW(const string prefix);

#ifdef __cplusplus
}//extern "C"
#endif

#endif



