#ifndef CODEC_VDEC_H
#define CODEC_VDEC_H

#ifdef __cplusplus
extern "C"
{
#endif

#include "codec.h"
#include "ion/ion.h"
#ifdef ENABLE_TEE_DRM_FLOW
#include "tee_client_api.h"
#include "tee_api.h"
#endif

    typedef struct PRIVATE_STRUCT
    {
        OMX_BOOL bIsRTKExtractor;
        OMX_BOOL bUseOutputBeDecoded;
        OMX_U32 nFrameWidth;
        OMX_U32 nFrameHeight;
        OMX_VIDEO_WMVFORMATTYPE pWMVFormat;
        OMX_BOOL bIsRV8;
        OMX_BOOL bUseProtectInputBuffer;
        OMX_BOOL bLowDelayMode;
        OMX_BOOL bSetSearchIFrm;
        OMX_U32 nEnableDeInterlace;
    } PRIVATE_STRUCT;

// create codec instance
    CODEC_PROTOTYPE *RTKHwDecOmx_decoder_create_video(OMX_VIDEO_CODINGTYPE eCompressionFormat, PRIVATE_STRUCT pRtkInfo);

#ifdef __cplusplus
}
#endif
#endif                       // CODEC_VDEC_H

