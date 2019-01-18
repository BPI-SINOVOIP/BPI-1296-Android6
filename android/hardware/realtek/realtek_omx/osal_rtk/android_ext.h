#ifndef _OMX_ANDROID_EXT_H
#define _OMX_ANDROID_EXT_H

#ifdef __cplusplus
extern "C" {
#endif
//#include <hardware/libhardware/include/hardware/gralloc.h>
#if defined(__LINARO_SDK__) && !defined(__LINUX_MEDIA_NAS__)
#include <stddef.h>   //for size_t
// system/core/include/cutils/native_handle.h
typedef struct native_handle
{
    int version;        /* sizeof(native_handle_t) */
    int numFds;         /* number of file-descriptors at &data[0] */
    int numInts;        /* number of ints at &data[numFds] */
    int data[0];        /* numFds + numInts ints */
} native_handle_t;

// system/core/include/system/window.h
typedef const native_handle_t* buffer_handle_t;

#else
#include <hardware/gralloc.h>
#include <hardware/hardware.h>
#endif
#include <OMX_Types.h>
#include <OMX_Component.h>
#include <OMX_Core.h>
#include <OMX_Index.h>
// cannot include because C++ header ?
//
//
//#include "media/hardware/HardwareAPI.h"

#define OMX_google_android_index_enableAndroidNativeBuffers  0x7FFF0000
#define OMX_google_android_index_getAndroidNativeBufferUsage 0x7FFF0001
#define OMX_google_android_index_useAndroidNativeBuffer      0x7FFF0002
#define OMX_IndexConfigRTKThumbnailMode                      0x7FFF0003
#define OMX_IndexConfigRTKExtractorMode                      0x7FFF0004
//#define OMX_IndexConfigRTKSetBSRingMode                      0x7FFF0005
#define OMX_google_android_index_storeMetaDataInBuffers      0x7FFF0006
#define OMX_google_android_index_prependSPSPPSToIDRFrames    0x7FFF0007
#define OMX_google_android_index_configureVideoTunnelMode    0x7FFF0008
#define OMX_google_android_index_describeColorFormat         0x7FFF0009

#define OMX_realtek_android_index_notifyBufferAllocDone      0x7FFF0100
#define OMX_realtek_android_index_encodeToInterlace          0x7FFF0101
#define OMX_realtek_android_index_lowDelayMode               0x7FFF0102
#define OMX_realtek_android_index_setSearchIFrm              0x7FFF0103
#define OMX_realtek_android_index_enableDeInterlace          0x7FFF0104
#define OMX_realtek_android_index_drmReturnIonInformation    0x7FFF0105
#define OMX_realtek_android_index_notifyVeScaling            0x7FFF0106
#define OMX_realtek_android_index_skipNonRefFrame            0x7FFF0107
#define OMX_realtek_android_index_getFrameRate               0x7FFF0108
#define OMX_realtek_android_index_getIsProg                  0x7FFF0109
#define OMX_realtek_android_index_getIsHdr                   0x7FFF010A
#define OMX_realtek_android_index_notifyMVC                  0x7FFF010B

#define OMX_realtek_android_index_notifyProtectedMode        0x7FFF0110
#define OMX_realtek_android_index_configureAudioTunnelMode   0x7FFF0111
#define OMX_realtek_android_index_configureVideoTunnelMode   0x7FFF0112
#define OMX_realtek_android_index_setVideoTunnelModeState    0x7FFF0113
#define OMX_realtek_android_index_setVideoEncRotAngle        0x7FFF0114
#define OMX_realtek_android_index_setErrFrameDropThreshold   0x7FFF0116
#define OMX_realtek_android_index_setHevcPerformance         0x7FFF0117

#define OMX_Glinux_SetSearchIFrm                             0x7FFF0115

#define OMX_realtek_android_index_setAudioTransSampleRate    0x7FFF0200
#define OMX_realtek_android_index_setAudioTransChannels      0x7FFF0201

/* For omx get physical Address */
#define OMX_IndexParamVideoInputPhysical 0x06600000

#define LOCK_MODE_TO_GET_VIRTUAL_ADDRESS 0
#define LOCK_MODE_TO_GET_PHYSICAL_ADDRESS 1
#define LOCK_MODE_TO_GET_YCbCr_VIRTUAL_ADDRESS  2

enum {
    RTK_TUNNEL_STARTDEQUEUE = 0,
    RTK_TUNNEL_STOPDEQUEUE,
    RTK_TUNNEL_GETALLQUEUE,
};

// A pointer to this struct is passed to the OMX_SetParameter when the extension
// index for the 'OMX.google.android.index.enableAndroidNativeBuffers' extension
// is given.
//
// When Android native buffer use is disabled for a port (the default state),
// the OMX node should operate as normal, and expect UseBuffer calls to set its
// buffers.  This is the mode that will be used when CPU access to the buffer is
// required.
//
// When Android native buffer use has been enabled for a given port, the video
// color format for the port is to be interpreted as an Android pixel format
// rather than an OMX color format.  The node should then expect to receive
// UseAndroidNativeBuffer calls (via OMX_SetParameter) rather than UseBuffer
// calls for that port.
typedef enum {

    /*
     * kMetadataBufferTypeCameraSource is used to indicate that
     * the source of the metadata buffer is the camera component.
     */
    kMetadataBufferTypeCameraSource  = 0,

    /*
     * kMetadataBufferTypeGrallocSource is used to indicate that
     * the payload of the metadata buffers can be interpreted as
     * a buffer_handle_t.
     * So in this case,the metadata that the encoder receives
     * will have a byte stream that consists of two parts:
     * 1. First, there is an integer indicating that it is a GRAlloc
     * source (kMetadataBufferTypeGrallocSource)
     * 2. This is followed by the buffer_handle_t that is a handle to the
     * GRalloc buffer. The encoder needs to interpret this GRalloc handle
     * and encode the frames.
     * --------------------------------------------------------------
     * |  kMetadataBufferTypeGrallocSource | sizeof(buffer_handle_t) |
     * --------------------------------------------------------------
     */
    kMetadataBufferTypeGrallocSource = 1,

    // Add more here...

} MetadataBufferType;

struct EnableAndroidNativeBuffersParams {
    OMX_U32 nSize;
    OMX_VERSIONTYPE nVersion;
    OMX_U32 nPortIndex;
    OMX_BOOL enable;
};

// A pointer to this struct is passed to OMX_SetParameter() when the extension
// index "OMX.google.android.index.storeMetaDataInBuffers"
// is given.
//
// When meta data is stored in the video buffers passed between OMX clients
// and OMX components, interpretation of the buffer data is up to the
// buffer receiver, and the data may or may not be the actual video data, but
// some information helpful for the receiver to locate the actual data.
// The buffer receiver thus needs to know how to interpret what is stored
// in these buffers, with mechanisms pre-determined externally. How to
// interpret the meta data is outside of the scope of this method.
//
// Currently, this is specifically used to pass meta data from video source
// (camera component, for instance) to video encoder to avoid memcpying of
// input video frame data. To do this, bStoreMetaDta is set to OMX_TRUE.
// If bStoreMetaData is set to false, real YUV frame data will be stored
// in the buffers. In addition, if no OMX_SetParameter() call is made
// with the corresponding extension index, real YUV data is stored
// in the buffers.
struct StoreMetaDataInBuffersParams {
    OMX_U32 nSize;
    OMX_VERSIONTYPE nVersion;
    OMX_U32 nPortIndex;
    OMX_BOOL bStoreMetaData;
};

// A pointer to this struct is passed to OMX_SetParameter when the extension
// index for the 'OMX.google.android.index.useAndroidNativeBuffer' extension is
// given.  This call will only be performed if a prior call was made with the
// 'OMX.google.android.index.enableAndroidNativeBuffers' extension index,
// enabling use of Android native buffers.
struct UseAndroidNativeBufferParams {
    OMX_U32 nSize;
    OMX_VERSIONTYPE nVersion;
    OMX_U32 nPortIndex;
    OMX_PTR pAppPrivate;
    OMX_BUFFERHEADERTYPE **bufferHeader;
//    const sp<ANativeWindowBuffer>& nativeBuffer;
};

// A pointer to this struct is passed to OMX_GetParameter when the extension
// index for the 'OMX.google.android.index.getAndroidNativeBufferUsage'
// extension is given.  The usage bits returned from this query will be used to
// allocate the Gralloc buffers that get passed to the useAndroidNativeBuffer
// command.
struct GetAndroidNativeBufferUsageParams {
    OMX_U32 nSize;              // IN
    OMX_VERSIONTYPE nVersion;   // IN
    OMX_U32 nPortIndex;         // IN
    OMX_U32 nUsage;             // OUT
};

// Meta data buffer layout used to transport output frames to the decoder for
// dynamic buffer handling.
struct VideoCodecMetaData {
  MetadataBufferType eType;
  buffer_handle_t pHandle;
  OMX_U32 nAspectH;
  OMX_U32 nAspectL;
  OMX_U32 pVuiFrameRate;
};

// A pointer to this struct is passed to OMX_SetParameter when the extension
// index for the 'OMX.google.android.index.prependSPSPPSToIDRFrames' extension
// is given.
// A successful result indicates that future IDR frames will be prefixed by
// SPS/PPS.
struct PrependSPSPPSToIDRFramesParams {
    OMX_U32 nSize;
    OMX_VERSIONTYPE nVersion;
    OMX_BOOL bEnable;
};

// Structure describing a media image (frame)
// Currently only supporting YUV
    typedef enum MediaImageType {
        MEDIA_IMAGE_TYPE_UNKNOWN = 0,
        MEDIA_IMAGE_TYPE_YUV,
    }MediaImageType;

    typedef enum MediaImagePlaneIndex {
        Y = 0,
        U,
        V,
        MAX_NUM_PLANES
    }MediaImagePlaneIndex;

    typedef struct MediaImagePlaneInfo {
        size_t mOffset;             // offset of first pixel of the plane in bytes
                                    // from buffer offset
        size_t mColInc;             // column increment in bytes
        size_t mRowInc;             // row increment in bytes
        size_t mHorizSubsampling;   // subsampling compared to the largest plane
        size_t mVertSubsampling;    // subsampling compared to the largest plane
    }MediaImagePlaneInfo;

typedef struct MediaImage {
    MediaImageType mType;
    size_t mNumPlanes;              // number of planes
    size_t mWidth;                  // width of largest plane (unpadded, as in nFrameWidth)
    size_t mHeight;                 // height of largest plane (unpadded, as in nFrameHeight)
    size_t mBitDepth;               // useable bit depth
    MediaImagePlaneInfo mPlane[MAX_NUM_PLANES];
}MediaImage;

// A pointer to this struct is passed to OMX_GetParameter when the extension
// index for the 'OMX.google.android.index.describeColorFormat'
// extension is given.  This method can be called from any component state
// other than invalid.  The color-format, frame width/height, and stride/
// slice-height parameters are ones that are associated with a raw video
// port (input or output), but the stride/slice height parameters may be
// incorrect. bUsingNativeBuffers is OMX_TRUE if native android buffers will
// be used (while specifying this color format).
//
// The component shall fill out the MediaImage structure that
// corresponds to the described raw video format, and the potentially corrected
// stride and slice-height info.
//
// The behavior is slightly different if bUsingNativeBuffers is OMX_TRUE,
// though most implementations can ignore this difference. When using native buffers,
// the component may change the configured color format to an optimized format.
// Additionally, when allocating these buffers for flexible usecase, the framework
// will set the SW_READ/WRITE_OFTEN usage flags. In this case (if bUsingNativeBuffers
// is OMX_TRUE), the component shall fill out the MediaImage information for the
// scenario when these SW-readable/writable buffers are locked using gralloc_lock.
// Note, that these buffers may also be locked using gralloc_lock_ycbcr, which must
// be supported for vendor-specific formats.
//
// For non-YUV packed planar/semiplanar image formats, or if bUsingNativeBuffers
// is OMX_TRUE and the component does not support this color format with native
// buffers, the component shall set mNumPlanes to 0, and mType to MEDIA_IMAGE_TYPE_UNKNOWN.
typedef struct DescribeColorFormatParams {
    OMX_U32 nSize;
    OMX_VERSIONTYPE nVersion;
    // input: parameters from OMX_VIDEO_PORTDEFINITIONTYPE
    OMX_COLOR_FORMATTYPE eColorFormat;
    OMX_U32 nFrameWidth;
    OMX_U32 nFrameHeight;
    OMX_U32 nStride;
    OMX_U32 nSliceHeight;
    OMX_BOOL bUsingNativeBuffers;

    // output: fill out the MediaImage fields
    MediaImage sMediaImage;
}DescribeColorFormatParams;

// A pointer to this struct is passed to OMX_SetParameter or OMX_GetParameter
// when the extension index for the
// 'OMX.google.android.index.configureVideoTunnelMode' extension is  given.
// If the extension is supported then tunneled playback mode should be supported
// by the codec. If bTunneled is set to OMX_TRUE then the video decoder should
// operate in "tunneled" mode and output its decoded frames directly to the
// sink. In this case nAudioHwSync is the HW SYNC ID of the audio HAL Output
// stream to sync the video with. If bTunneled is set to OMX_FALSE, "tunneled"
// mode should be disabled and nAudioHwSync should be ignored.
// OMX_GetParameter is used to query tunneling configuration. bTunneled should
// return whether decoder is operating in tunneled mode, and if it is,
// pSidebandWindow should contain the codec allocated sideband window handle.
struct ConfigureVideoTunnelModeParams {
    OMX_U32 nSize;              // IN
    OMX_VERSIONTYPE nVersion;   // IN
    OMX_U32 nPortIndex;         // IN
    OMX_BOOL bTunneled;         // IN/OUT
    OMX_U32 nAudioHwSync;       // IN
    OMX_PTR pSidebandWindow;    // OUT
};

OMX_U32 lockAndroidNativeBuf(buffer_handle_t handle, OMX_U32 Width, OMX_U32 Height, OMX_U32 mode, void *pAddrs[]);
OMX_U32 unlockAndroidNativeBuf(buffer_handle_t handle);

#ifdef __cplusplus
}
#endif

#endif //_OMX_ANDROID_EXT_H
