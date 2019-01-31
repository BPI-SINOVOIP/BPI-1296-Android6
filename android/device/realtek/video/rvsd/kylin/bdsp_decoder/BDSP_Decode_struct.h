/*********************************************************************************/
/*                                                                               */
/* BDSP_Decode_struct.h                                                         */
/*                                                                               */
/* Purpose: BD subtitling decoder structure definitions                         */
/*                                                                               */
/*                                                                               */
/*                                                                               */
/* Copyright (c) 2009 Realtek Semiconductor Corp. All rights reserved.           */
/*                                                                               */
/* No part of this document may be copied, reproduced,                           */
/* or transmitted in any form without the prior written                          */
/* consent of Realtek Semiconductor Corp. Hsinchu, Taiwan                        */
/*                                                                               */
/* Revision History:                                                             */
/*                                                                               */
/*      Date               By                  Description                       */
/* ============================================================================= */
/*    05/07/09         Bonds Lu                Creation                          */
/*                                                                               */
/*                                                                               */
/*                                                                               */
/*********************************************************************************/
#ifndef _BDSP_DECODE_STRUCT_H_
#define _BDSP_DECODE_STRUCT_H_

//#include "rpc_common.h"
//#include "VP_comp.h"
//#include "VP_Utilities.h"
//#include "VTM_struct.h"
//#include "VBM_struct.h"
//#include "SE_Utils.h"

#include "../sp_common/sp_common.h"
#include "BDSP_Decode_def.h"
#include "rvsd_CommonFunc.h"


#ifdef BDSP_SOFTWARE_DECODE

typedef struct
{
#if SP_LITTLE_ENDIAN
    unsigned int dummy:8 ;
    unsigned int run_length_zero_1_63:6 ;
    unsigned int switch_2:1 ;
    unsigned int switch_1:1 ;
    unsigned int code:8 ;
    unsigned int reserved:8 ;
#else
    unsigned int reserved:8 ;
    unsigned int code:8 ;
    unsigned int switch_1:1 ;
    unsigned int switch_2:1 ;
    unsigned int run_length_zero_1_63:6 ;
    unsigned int dummy:8 ;
#endif
} BDSP_8BIT_CODE_0 ;


typedef struct
{
#if SP_LITTLE_ENDIAN
    unsigned int run_length_zero_64_16K:14 ;
    unsigned int switch_2:1 ;
    unsigned int switch_1:1 ;
    unsigned int zero:8 ;
    unsigned int reserved:8 ;
#else
    unsigned int reserved:8 ;
    unsigned int zero:8 ;
    unsigned int switch_1:1 ;
    unsigned int switch_2:1 ;
    unsigned int run_length_zero_64_16K:14 ;
#endif
} BDSP_8BIT_CODE_1 ;

typedef struct
{
#if SP_LITTLE_ENDIAN
    unsigned int code:8 ;
    unsigned int run_length_3_63:6 ;
    unsigned int switch_2:1 ;
    unsigned int switch_1:1 ;
    unsigned int zero:8 ;
    unsigned int reserved:8 ;
#else
    unsigned int reserved:8 ;
    unsigned int zero:8 ;
    unsigned int switch_1:1 ;
    unsigned int switch_2:1 ;
    unsigned int run_length_3_63:6 ;
    unsigned int code:8 ;
#endif
} BDSP_8BIT_CODE_2 ;

typedef struct
{
#if SP_LITTLE_ENDIAN
    unsigned int code:8 ;
    unsigned int run_length_64_16K:14 ;
    unsigned int switch_2:1 ;
    unsigned int switch_1:1 ;
    unsigned int zero:8 ;
#else
    unsigned int zero:8 ;
    unsigned int switch_1:1 ;
    unsigned int switch_2:1 ;
    unsigned int run_length_64_16K:14 ;
    unsigned int code:8 ;
#endif
} BDSP_8BIT_CODE_3 ;

typedef struct
{
    unsigned short x_offset ;
    unsigned short y_offset ;
    unsigned short pitch ;    /* object pitch */
    unsigned short width ;    /* for end-of-line   detection */
    unsigned short height ;   /* for out-of-object detection */
    unsigned int   pixelBuf ;
    unsigned int   lineBuf ;

} BDSP_TARGET ;

#else

//typedef struct
//{
//  unsigned int dst_pic_idx:2 ;
//  unsigned int reserved:1 ;
//  unsigned int dst_y:12 ;
//  unsigned int dst_x:12 ;
//  unsigned int Err_fill:1 ;
//  unsigned int opcode:4 ;  /* must be b'1001 */
//} BDSP_SECMD_DSTPIC ;    /* WORD 1 */
//typedef struct
//{
//  unsigned int reserved:4 ;
//  unsigned int pxd_addr:28 ;
//} BDSP_SECMD_DECODE_WORD2 ;
//typedef struct
//{
//  unsigned int Err_color:8 ;
//  unsigned int pxd_length:24 ;
//} BDSP_SECMD_DECODE_WORD3 ;
//typedef struct
//{
//  unsigned int height:16 ;
//  unsigned int width:16 ;
//} BDSP_SECMD_DECODE_WORD4 ;

#endif

typedef struct
{
    unsigned char reserved:7 ;
    unsigned char err:1 ;

} BDSP_OBJECT_STATUS ;

typedef struct
{
    unsigned int  object_id:16 ;
    unsigned int  version_number:16 ;

    /* sequence_descriptor */
    unsigned int  first_in_sequence_flag:1 ;
    unsigned int  last_in_sequence_flag:1 ;
    unsigned int  reserved:6 ;

    unsigned int  data_length:24 ;
    unsigned int  width:16 ;
    unsigned int  height:16 ;

    unsigned int  pixelBuf ; /* pixel buffer start address */
    unsigned int  pitch ;
    unsigned int  rleBuf ;   /* RLE buffer start address */
    unsigned int  processedLength ;

    BDSP_OBJECT_STATUS status ;
    unsigned char padding0;
    unsigned short padding1;

} BDSP_OBJECT ;

typedef struct
{
    unsigned char  CLUT_id ;
    unsigned char  version_number ;
    unsigned short padding0;
    /* CLUTs, YUVT8888 format for each entry */
    unsigned int   colors[256] ;

} BDSP_CLUT ;


typedef struct
{
    /* size : 10 bytes */
    unsigned int window_id:8 ;
    unsigned int padding0:24;
    unsigned int horizontal_position:16 ;
    unsigned int vertical_position:16 ;
    unsigned int width:16 ;
    unsigned int height:16 ;
} BDSP_WINDOW ;

typedef struct
{
    /* size : int * 4 = 16 bytes */
    unsigned int object_id_ref:16 ;
    unsigned int window_id_ref:8 ;
    unsigned int object_cropped_flag:1 ;
    unsigned int forced_on_flag:1 ;
    unsigned int reserved:6 ;
    unsigned int horizontal_position:16 ;
    unsigned int vertical_position:16 ;

    unsigned int crop_horizontal_position:16 ;
    unsigned int crop_vertical_position:16 ;
    unsigned int crop_width:16 ;
    unsigned int crop_height:16 ;

} BDSP_COMPOSITION_OBJECT ;


typedef struct
{
    /*===== 4 bytes align here =====*/

    /* video_descriptor */

    unsigned int video_width:16 ;
    unsigned int video_height:16 ;
#if SP_FIXED_BUG
    unsigned int number:16 ;
    unsigned int frame_rate:4 ;
    unsigned int reserved0:4 ;

    /* composition_descriptor */
    unsigned int state:2 ;
    unsigned int reserved1:6 ;
#else
    unsigned int frame_rate:4 ;
    unsigned int reserved0:4 ;

    /* composition_descriptor */
    unsigned int number:16 ;
    unsigned int state:2 ;
    unsigned int reserved1:6 ;
#endif

    unsigned int palette_update_flag: 1 ;
    unsigned int reserved2: 7 ;
    unsigned int palette_id_ref: 8 ;
    unsigned int num_of_objects: 8 ;
#if SP_FIXED_BUG
    unsigned int reserved3: 8;
#endif

    /*===== 8 bytes align here =====*/

    BDSP_COMPOSITION_OBJECT     object [BDSP_MAX_NUM_OBJECTS_PER_PAGE] ;

    /*===== 8 bytes align here =====*/
} BDSP_PAGE ;

typedef struct
{
    unsigned char reserved:3 ;
    unsigned char poll_se:1 ;
    unsigned char render_status:2 ;
    unsigned char svc_acquired:1 ;
    unsigned char decode:1 ;

} BDSP_DECODE_STATUS ;


typedef struct
{
    /*===== 8 bytes align here =====*/

    long long  PTS ;

    BDSP_DECODE_STATUS  status ;

    unsigned char padding0;

    unsigned short windows ;  /* number of active windows in an epoch */
    unsigned short CLUTs ;    /* number of active CLUTs   in an epoch */
    unsigned short objects ;  /* number of active objects   in an epoch */

    /*===== 8 bytes align here =====*/

    /* bitstream pointers */
    unsigned int   bsBase ;
    unsigned int   bsLimit ;
    unsigned int   bsCurr ;
    unsigned int   bsWrite ;
    unsigned long *bsWrPtr ;
    unsigned long *bsRdPtr ;

    /*===== 8 bytes align here =====*/

    VP_INBAND_RING_BUF  S_ICQ ;  /* in-band command queue */

    /*===== 4 bytes align here =====*/

    /* VBM borrow GOB record */
    unsigned int   gobBase ;      /* base address for object buffer */
    unsigned int   gobSize ;

    /*===== 4 bytes align here =====*/

    BDSP_PAGE      page ;
    /*===== 8 bytes align here =====*/
    BDSP_WINDOW   window[BDSP_MAX_NUM_WINDOWS] ;
    /*===== 4 bytes align here =====*/
    BDSP_CLUT     CLUT  [BDSP_MAX_NUM_CLUTS  ] ;
    /*===== 4 bytes align here =====*/
    BDSP_OBJECT   object[BDSP_MAX_NUM_OBJECTS] ;
    /*===== 4 bytes align here =====*/

    /* object buffers */
    unsigned int  objectBufOffset ;

    unsigned int   SE_task_id ;

    /* page's coordinate */
    VP_WINDOW      rectPage ;
    unsigned int   pitch ;

    long long      *bsLinear ;

#if (BDSP_ON_MIPS == 0)
    /* Phoenix */
    unsigned int   bsCurr_C ;
    SP_INFO *pSpInfo;
#endif

    /* region/render buffers */
    unsigned int   renderbufBase ;
    VP_BUFFER     *renderBuf ;
    VP_BUFFER      renderBufPool[BDSP_MAX_NUM_RENDER_BUFS] ;
} BDSP_DECODE ;


/* function prototypes */

unsigned int BDSP_YUVT2RGBA (int, int, int, int) ;

int  BDSP_Reset                       (BDSP_DECODE *) ;
int  BDSP_Init                        (BDSP_DECODE *, unsigned char) ;
int  BDSP_Find_CLUT                   (BDSP_DECODE *, unsigned int) ;
int  BDSP_Find_Object                 (BDSP_DECODE *, unsigned int) ;
int  BDSP_Find_Window                 (BDSP_DECODE *, unsigned int) ;
int  BDSP_ResetVE                     (BDSP_DECODE *, unsigned int, unsigned int, unsigned int, unsigned int) ;
int  BDSP_SetBitstreamPtr             (BDSP_DECODE *, unsigned int, unsigned int, unsigned int) ;
int  BDSP_GetProcessedLength          (BDSP_DECODE *, unsigned int) ;
int  BDSP_GetFullness                 (BDSP_DECODE *) ;
int  BDSP_Scan_InBandCommand          (BDSP_DECODE *) ;
int  BDSP_Decode_Segment              (BDSP_DECODE *) ;
int  BDSP_Decode_Page                 (BDSP_DECODE *, unsigned int) ;
int  BDSP_Decode_Window               (BDSP_DECODE *, unsigned int) ;
int  BDSP_Decode_CLUT                 (BDSP_DECODE *, unsigned int) ;
int  BDSP_Decode_Object               (BDSP_DECODE *, unsigned int) ;
int  BDSP_Decode_RenderPage           (BDSP_DECODE *) ;

int BDSP_Decoder_Create (SP_INFO *, VP_COMPONENT *, int) ;

#endif
