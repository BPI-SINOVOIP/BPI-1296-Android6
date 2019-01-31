//--=========================================================================--
//  This file is a part of VPU Reference API project
//-----------------------------------------------------------------------------
//
//       This confidential and proprietary software may be used only
//     as authorized by a licensing agreement from Realtek.
//     In the event of publication, the following notice is applicable:
//
//            (C) COPYRIGHT 2006 - 2011  Realtek.
//                      ALL RIGHTS RESERVED
//
//       The entire notice above must be reproduced on all authorized
//       copies.
//
//--=========================================================================--
#ifndef __WAVE410_FUNCTION_H__
#define __WAVE410_FUNCTION_H__

#include "vpuapi.h"
#include "product.h"


#define WAVE4_FBC_LUMA_TABLE_SIZE(_w, _h)   ((_h+15)/16*(_w+255)/256*128)
#define WAVE4_FBC_CHROMA_TABLE_SIZE(_w, _h) ((_h+15)/16*(_w/2+255)/256*128)
#define WAVE4_MVCOL_BUF_SIZE(_w, _h)        ((_w+63)/64* (_h+63)/64*256+ 64)

typedef enum {
    DECODE_PICTURE_HEADER,
    DECODE_SEQUENCE_HEADER,
} DecPicOption;

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

    extern Int32 Wave4VpuGetProductId(
        Uint32      coreIdx
    );

    extern RetCode Wave4VpuGetVersion(
        Uint32  coreIdx,
        Uint32* versionInfo,
        Uint32* revision
    );

    extern RetCode Wave4VpuInit(
        Uint32      coreIdx,
        void*       firmware,
        Uint32      size
    );

    extern RetCode Wave4VpuDeinit(
        Uint32      coreIdx
    );

    extern Uint32 Wave4VpuIsInit(
        Uint32      coreIdx
    );

    extern Int32 Wave4VpuIsBusy(
        Uint32      coreIdx
    );

    extern Int32 Wave4VpuWaitInterrupt(
        Uint32      coreIdx,
        Int32       timeout
    );

    extern RetCode Wave4VpuClearInterrupt(
        Uint32      coreIdx,
        Uint32      flags
    );

    extern RetCode Wave4VpuReset(
        Uint32      coreIdx,
        SWResetMode resetMode
    );

    extern RetCode Wave4VpuSleepWake(
        Uint32      coreIdx, 
        int         iSleepWake,
        const Uint16* code, 
        Uint32 size
    );

    extern VpuAttr* Wave4VpuGetAttributes(
        Uint32 coreIdx
    );

    extern RetCode Wave4VpuBuildUpDecParam(
        CodecInst*      instance,
        DecOpenParam*   param
    );

    extern RetCode Wave4VpuDecInitSeq(
        CodecInst*  instance
    );

    extern RetCode Wave4VpuDecFiniSeq(
        CodecInst*  instance
    );

    extern RetCode Wave4VpuDecRegisterFramebuffer(
        CodecInst*      instance,
        FrameBuffer*    fb,
        TiledMapType    mapType,
        Uint32          count
    );

    extern RetCode Wave4VpuDecode(
        CodecInst*      instance,
        DecParam*       option
    );

    extern RetCode Wave4VpuDecGetResult(
        CodecInst*      instance,
        DecOutputInfo*  result
    );

    extern RetCode Wave4VpuDecSetBitstreamFlag(
        CodecInst*  instance,
        BOOL        running,
        BOOL        eos
    );

    extern RetCode Wave4VpuDecGetSeqInfo(
        CodecInst*      instance,
        DecInitialInfo* info
    );

    extern RetCode Wave4VpuDecFlush(
        CodecInst*          instance,
        FramebufferIndex*   framebufferIndexes,
        Uint32              size
    );

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __WAVE410_FUNCTION_H__ */
