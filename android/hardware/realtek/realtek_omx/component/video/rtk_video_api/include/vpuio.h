//------------------------------------------------------------------------------
// File: vpuio.h
//
// Copyright (c) 2006, Realtek.  All rights reserved.
//------------------------------------------------------------------------------

#ifndef _VPU_IO_H_
#define _VPU_IO_H_

#include "vputypes.h"

typedef struct {
    osal_file_t fileHandle;
    Uint8*      buf;
    Uint32      size;
    Uint32      point;
    Uint32      count;
    Uint32      loopCount;
} BufInfo;

typedef struct {
    unsigned char*    y_addr;
    unsigned char*    cb_addr;
    unsigned char*    cr_addr;
} FRAME_ADDR;

#if defined (__cplusplus)
extern "C" {
#endif

    /* Decode Function */
    int FillSdramBurst(Uint32 core_idx, BufInfo * pBufInfo, Uint32 targetAddr, vpu_buffer_t *pVbStream, Uint32 size, int checkeos, int *streameos, int endian );
    void StoreYuvImageBurstFormat(Uint32 core_idx, FrameBuffer *fbSrc, TiledMapConfig mapCfg, Uint8 *pDst, VpuRect cropRect, BOOL enableCrop, int format, int endian);
    /* Encode Function */
    int ProcessEncodedBitstreamBurst(Uint32 core_idx,
                                     osal_file_t fp,
                                     PhysicalAddress targetAddr,
                                     PhysicalAddress bsBufStartAddr,
                                     PhysicalAddress bsBufEndAddr,
                                     unsigned int size,
                                     int endian);
    int  LoadTiledImageYuvBurst(Uint32 core_idx, BYTE *pYuv, int picWidth, int picHeight, FrameBuffer *fb, TiledMapConfig mapCfg, int stride, int interLeave, int format, int endian);
    void LoadYuvImageBurstFormat(Uint32 core_idx, Uint8 * src, int picWidth, int picHeight,
                                 FrameBuffer *fb, int stride,
                                 int interLeave, int format, int endian );
    void read_encoded_bitstream_burst(Uint32 core_idx, unsigned char * buffer, int targetAddr,
                                      PhysicalAddress bsBufStartAddr, PhysicalAddress bsBufEndAddr,
                                      int size, int endian );

#if defined (__cplusplus)
}
#endif

#endif //#ifndef _VPU_IO_H_
