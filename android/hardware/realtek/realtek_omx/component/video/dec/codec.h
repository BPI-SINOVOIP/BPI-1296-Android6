#ifndef RTK_DECODER_H
#define RTK_DECODER_H

#ifdef __cplusplus
extern "C"
{
#endif

#include <OMX_Types.h>
#include <OMX_Core.h>
#include <OMX_Component.h>
#include <OSAL.h>

#include "dbgmacros.h"

#define USE_VIDEO_FREEZE_CONCEALMENT 0

#define COMPONENT_NAME_VIDEO      "OMX.realtek.video.decoder"
#define COMPONENT_NAME_IMAGE      "OMX.realtek.image.decoder"
#define USE_ION_ALLOC_FB


    /* Maximum video resolutions for different HW congfigurations*/
#define MAX_VIDEO_RESOLUTION_D1    720 * 576
#define MAX_VIDEO_RESOLUTION_720P  1280 * 720
#define MAX_VIDEO_RESOLUTION_1080P 1920 * 1088
#define MAX_VIDEO_RESOLUTION_4K2K  4096 * 2160
#define MAX_VIDEO_RESOLUTION_4K4K  4096 * 4096
    /* Set maximum video resolution */
#define MAX_VIDEO_RESOLUTION MAX_VIDEO_RESOLUTION_1080P

    /* Maximum image resolutions supported by different HW versions */
#define MAX_IMAGE_RESOLUTION_16MPIX   4672 * 3504
#define MAX_IMAGE_RESOLUTION_67MPIX   8176 * 8176
#define MAX_IMAGE_RESOLUTION_256MPIX  16383 * 16383
    /* Set maximum image resolution */
#define MAX_IMAGE_RESOLUTION MAX_IMAGE_RESOLUTION_67MPIX

#if ! defined(__LINARO_SDK__)
#define ENABLE_OUTPUT_BE_DECODE_BUFFER
#endif

    typedef enum AVC_FORMAT
    {
        AVCFORMAT_AVC,
        AVCFORMAT_MVC
    }
    AVC_FORMAT;

    typedef enum MPEG4_FORMAT
    {
        MPEG4FORMAT_MPEG4 = 0,
        MPEG4FORMAT_DIVX5,
        MPEG4FORMAT_XVID,
        MPEG4FORMAT_DIVX4 = 5,
        MPEG4FORMAT_SORENSON = 256
    } MPEG4_FORMAT;

#ifdef ENABLE_OUTPUT_BE_DECODE_BUFFER
    typedef enum BUFFER_CMD
    {
        BUFFERCMD_ADD_BUF = 0,
        BUFFERCMD_REGISTER_BUF,
        BUFFERCMD_RESET_BUF,
        BUFFERCMD_CLEAR_BUF
    } BUFFER_CMD;
#endif

    typedef struct CLASS_FORMAT
    {
        AVC_FORMAT avc_class;
        MPEG4_FORMAT mp4_class;
    } CLASS_FORMAT;

    typedef struct STREAM_BUFFER
    {
        OMX_U8 *bus_data;
        OSAL_BUS_WIDTH bus_address; // use this for HW
        OMX_U32 streamlen;
        OMX_U32 sliceInfoNum;
        OMX_U8 *pSliceInfo;
        OMX_U32 allocsize;
        OMX_BOOL bEOS;
        OMX_U32 nFlags;
    } STREAM_BUFFER;

    typedef struct STREAM_INFO
    {
        OMX_COLOR_FORMATTYPE format;    // stream color format
        OMX_U32 framesize;   // framesize in bytes
        OMX_U32 width;       // picture display width
        OMX_U32 height;      // picture display height
        OMX_U32 sliceheight; // picture slice height
        OMX_U32 stride;      // picture scan line width
        OMX_U32 interlaced;  // is sequence interlaced
        OMX_U32 imageSize;   // size of image in memory
        OMX_BOOL isThumbnail;
        OMX_BOOL crop_available;       // crop information
        OMX_U32 crop_width;
        OMX_U32 crop_height;
        OMX_U32 crop_left;
        OMX_U32 crop_top;
        OMX_U32 reg_num_buffer;
        OMX_U32 nMinOutputUndequeueCount;
    } STREAM_INFO;

    typedef struct FRAME
    {
        OMX_U8 *fb_bus_data; // pointer to DMA accesible output buffer.
        OMX_U32 fb_bus_address; // memory buffer address on the bus
        OMX_U32 fb_size;     // buffer size
        OMX_U32 size;        // output frame size in bytes
        OMX_U32 MB_err_count;   // decoding macroblock error count
        OMX_BOOL isIntra;
        OMX_BOOL isGoldenOrAlternate;
        OMX_TICKS nTimeStamp;
        OMX_U32 outputBufferType;
        OMX_BOOL isEosFrame;
        private_rtk_v1_data priv_data;
    } FRAME;

    typedef enum ROTATION
    {
        ROTATE_NONE,
        ROTATE_RIGHT_90 = 90,
        ROTATE_LEFT_90 = -90,
        ROTATE_180 = 180,
        ROTATE_FLIP_VERTICAL,
        ROTATE_FLIP_HORIZONTAL
    } ROTATION;

    /* Opencore specific */
    /* OMX COMPONENT CAPABILITY RELATED MEMBERS */
    typedef struct
            PV_OMXComponentCapabilityFlagsType
    {
        OMX_BOOL iIsOMXComponentMultiThreaded;
        OMX_BOOL iOMXComponentSupportsExternalOutputBufferAlloc;
        OMX_BOOL iOMXComponentSupportsExternalInputBufferAlloc;
        OMX_BOOL iOMXComponentSupportsMovableInputBuffers;
        OMX_BOOL iOMXComponentSupportsPartialFrames;
        OMX_BOOL iOMXComponentUsesNALStartCode;
        OMX_BOOL iOMXComponentCanHandleIncompleteFrames;
        OMX_BOOL iOMXComponentUsesFullAVCFrames;
    } PV_OMXComponentCapabilityFlagsType;

    typedef enum CODEC_STATE
    {
        CODEC_NEED_MORE,
        CODEC_HAS_FRAME,
        CODEC_HAS_INFO,
        CODEC_OK,
        CODEC_WAIT_SEEK,
        CODEC_NOT_HAS_RESOURCE,
        CODEC_DECODE_FINISH,
        CODEC_ERROR_HW_TIMEOUT = -1,
        CODEC_ERROR_HW_BUS_ERROR = -2,
        CODEC_ERROR_SYS = -3,
        CODEC_ERROR_DWL = -4,
        CODEC_ERROR_UNSPECIFIED = -5,
        CODEC_ERROR_STREAM = -6,
        CODEC_ERROR_INVALID_ARGUMENT = -7,
        CODEC_ERROR_NOT_INITIALIZED = -8,
        CODEC_ERROR_INITFAIL = -9,
        CODEC_ERROR_HW_RESERVED = -10,
        CODEC_ERROR_MEMFAIL = -11,
        CODEC_ERROR_STREAM_NOT_SUPPORTED = -12,
        CODEC_ERROR_FORMAT_NOT_SUPPORTED = -13
    } CODEC_STATE;

    typedef enum
    {
        INTERLEAVED_TOP_FIELD = 0,  /* top    field data stored in even lines of a frame buffer */
        INTERLEAVED_BOT_FIELD,      /* bottom field data stored in odd  lines of a frame buffer */
        CONSECUTIVE_TOP_FIELD,      /* top    field data stored consecutlively in all lines of a field buffer */
        CONSECUTIVE_BOT_FIELD,      /* bottom field data stored consecutlively in all lines of a field buffer */
        CONSECUTIVE_FRAME,          /* progressive frame data stored consecutlively in all lines of a frame buffer */
        INTERLEAVED_TOP_FIELD_422,  /* top    field data stored in even lines of a frame buffer */
        INTERLEAVED_BOT_FIELD_422,  /* bottom field data stored in odd  lines of a frame buffer */
        CONSECUTIVE_TOP_FIELD_422,  /* top    field data stored consecutlively in all lines of a field buffer */
        CONSECUTIVE_BOT_FIELD_422,  /* bottom field data stored consecutlively in all lines of a field buffer */
        CONSECUTIVE_FRAME_422,      /* progressive frame with 4:2:2 chroma */
        TOP_BOTTOM_FRAME,           /* top field in the 0~height/2-1, bottom field in the height/2~height-1 in the frame */
        INTERLEAVED_TOP_BOT_FIELD,  /* one frame buffer contains one top and one bot field, top field first */
        INTERLEAVED_BOT_TOP_FIELD,  /* one frame buffer contains one bot and one top field, bot field first */

        MPEG2_PIC_MODE_NOT_PROG     /* yllin: for MPEG2 check pic mode usage */

    } VP_PICTURE_MODE_t ;

    typedef struct CODEC_PROTOTYPE CODEC_PROTOTYPE;

// internal CODEC interface, which wraps up Hantro API
    struct CODEC_PROTOTYPE
    {
        //
        // Destroy the codec instance
        //
        void (*destroy) (CODEC_PROTOTYPE *);
        //
        // Decode n bytes of data given in the stream buffer object.
        // On return consumed should indicate how many bytes were consumed from the buffer.
        //
        // The function should return one of following:
        //
        //    CODEC_NEED_MORE  - nothing happened, codec needs more data.
        //    CODEC_HAS_INFO   - headers were parsed and information about stream is ready.
        //    CODEC_HAS_FRAME  - codec has one or more headers ready
        //    less than zero   - one of the enumerated error values
        //
        // Parameters:
        //
        //    CODEC_PROTOTYPE  - this codec instance
        //    STREAM_BUFFER    - data to be decoded
        //    OMX_U32          - pointer to an integer that should on return indicate how many bytes were used from the input buffer
        //    FRAME            - where to store any frames that are ready immediately
        CODEC_STATE(*decode) (CODEC_PROTOTYPE *, STREAM_BUFFER *, OMX_U32 *,
                              FRAME *);

        //
        // Get info about the current stream. On return stream information is stored in STREAM_INFO object.
        //
        // The function should return one of the following:
        //
        //   CODEC_OK         - got information succesfully
        //   less than zero   - one of the enumerated error values
        //
        CODEC_STATE(*getinfo) (CODEC_PROTOTYPE *, STREAM_INFO *);

        //
        // Get a frame from the decoder. On return the FRAME object contains the frame data. If codec does internal
        // buffering this means that codec needs to copy the data into the specied buffer. Otherwise it might be
        // possible for a codec implementation to store the frame directly into the frame's buffer.
        //
        // The function should return one of the following:
        //
        //  CODEC_OK         - everything OK but no frame was available.
        //  CODEC_HAS_FRAME  - got frame ok.
        //  less than zero   - one of the enumerated error values
        //
        // Parameters:
        //
        //  CODEC_PROTOTYPE  - this codec instance
        //  FRAME            - location where frame data is to be written
        //  OMX_BOOL         - end of stream (EOS) flag
        //
        CODEC_STATE(*getframe) (CODEC_PROTOTYPE *, FRAME *, OMX_BOOL);

        //
        // Scan for complete frames in the stream buffer. First should be made to
        // give an offset to the first frame within the buffer starting from the start
        // of the buffer. Respectively last should be made to give an offset to the
        // start of the last frame in the buffer.
        //
        // Note that this does not tell whether the last frame is complete or not. Just that
        // complete frames possibly exists between first and last offsets.
        //
        // The function should return one of the following.
        //
        //   -1             - no frames found. Value of first and last undefined.
        //    1             - frames were found.
        //
        // Parameters:
        //
        //  CODEC_PROTOTYPE - this codec instance
        //  STREAM_BUFFER   - frame data pointer
        //  OMX_U32         - first offset pointer
        //  OMX_U32         - last offset pointer
        //
        OMX_S32 (*scanframe) (CODEC_PROTOTYPE *, STREAM_BUFFER *, OMX_U32 * first,
                              OMX_U32 * last);
        //
        // Flush the input and output data
        //
        void (*flush) (CODEC_PROTOTYPE *, OMX_U32);
#ifdef ENABLE_OUTPUT_BE_DECODE_BUFFER
        //
        // Command for buffer handling
        //
        void (*bufcmd) (CODEC_PROTOTYPE *, OMX_U32, OMX_U8 *, OMX_U32, BUFFER_CMD);
#endif
    };

#ifdef __cplusplus
}
#endif
#endif                       // RTK_DECODER_H
