/*********************************************************************************/
/*                                                                               */
/* BDSP_Decode_def.h                                                            */
/*                                                                               */
/* Purpose: BD subtitling decoder constant macro definitions                    */
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
#ifndef _BDSP_DECODE_DEF_H_
#define _BDSP_DECODE_DEF_H_

#define BDSP_SOFTWARE_DECODE
#ifndef MAX
#define MAX(a,b) (((a)>(b))?(a):(b))
#endif

#ifndef MIN
#define MIN(a,b) (((a)<(b))?(a):(b))
#endif

#define BDSP_MAX_NUM_RENDER_BUFS              3
#define BDSP_MAX_NUM_PAGE                     8  /* ??? */
#define BDSP_MAX_NUM_WINDOWS                  2
#define BDSP_MAX_NUM_CLUTS                    8
#define BDSP_MAX_NUM_OBJECTS                 64
#define BDSP_MAX_NUM_OBJECTS_PER_PAGE         2

#define BDSP_DEC_RING_BUF_MIN_EMPTINESS     0x4000

#define BDSP_DEC_SUCCESS   0

/* decode status */
#define BDSP_DEC_STATUS_BITSTREAM_UNDERFLOW        1

/* decode render status */
#define BDSP_DEC_RENDER_STATUS_NONE                0
#define BDSP_DEC_RENDER_STATUS_START               1
#define BDSP_DEC_RENDER_STATUS_BITBLIT             2


/* decode errors */
#define BDSP_ERR_UNKNOWN_WINDOW                   -1
#define BDSP_ERR_UNKNOWN_OBJECT                   -2
#define BDSP_ERR_UNKNOWN_CLUT                     -3
#define BDSP_ERR_UNDEFINED_PAGE_STATE             -4
#define BDSP_ERR_INVALID_OBJECT_FIELD             -5
#define BDSP_ERR_INVALID_WINDOW_FIELD             -6
#define BDSP_ERR_INVALID_PAGE_FIELD               -7
#define BDSP_ERR_INVALID_CLUT_ENTRY               -8
#define BDSP_ERR_INVALID_RENDER_PAGE              -9
#define BDSP_ERR_OBJECT_OVERFLOW                  -10
#define BDSP_ERR_WINDOW_OVERFLOW                  -11
#define BDSP_ERR_PIXEL_BUF_OVERFLOW               -12
#define BDSP_ERR_CLUT_OVERFLOW                    -13
#define BDSP_ERR_SE_TASK_BUFFER_OVERFLOW          -14

#define BDSP_TOTAL_OBJECT_BUFFER_SIZE       0x400000   // 3M  for OBJECT_BUFFER, 1M for bsLinear
#define BDSP_MAX_OBJECT_BUFFER_SIZE         0x300000
#define BDSP_MAX_PICTURE_WIDTH                  1920
#define BDSP_MAX_PICTURE_HEIGHT                 1088
#define BDSP_RENDER_BUFFER_SIZE               (BDSP_MAX_PICTURE_WIDTH  * BDSP_MAX_PICTURE_HEIGHT)


/* Segment type (Blue-ray Disc Read-Only Format Table 9-60) */
#define BDSP_SEGMENT_PALETTE_DEFINITION       0x14
#define BDSP_SEGMENT_OBJECT_DEFINITION        0x15
#define BDSP_SEGMENT_PRESENTATION_COMPOSITION 0x16
#define BDSP_SEGMENT_WINDOW_DEFINITION        0x17
#define BDSP_SEGMENT_END_OF_DISPLAY           0x80

/* Composition state (Blue-ray Disc Read-Only Format Table 9-62) */
#define BDSP_COMPOSITION_STATE_NORMAL_CASE        0
#define BDSP_COMPOSITION_STATE_ACQUISITION_POINT  1
#define BDSP_COMPOSITION_STATE_EPOCH_START        2
#define BDSP_COMPOSITION_STATE_EPOCH_CONTINUE     3

/* check range: start inclusive, end exclusive */
#define S_oE(x, start, end) \
(   ((end) == (start) &&  ((x) == (start))) \
 || ((end)  > (start) &&  ((x) >= (start) && (x)  < (end)))  \
 || ((end)  < (start) && !((x)  < (start) && (x) >= (end))) \
)


/* size of bitstream wrap-around copy buffer */
#define BDSP_LINEAR_BS_BUFFER_SIZE    (5 * 64 * 1024)

#endif
