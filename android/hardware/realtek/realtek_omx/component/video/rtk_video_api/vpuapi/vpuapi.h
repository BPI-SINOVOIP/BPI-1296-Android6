//--=========================================================================--
//  This file is a part of VPU Reference API project
//-----------------------------------------------------------------------------
//
//       This confidential and proprietary software may be used only
//     as authorized by a licensing agreement from Realtek.
//     In the event of publication, the following notice is applicable:
//
//            (C) COPYRIGHT 2006 - 2013  Realtek.
//                      ALL RIGHTS RESERVED
//
//       The entire notice above must be reproduced on all authorized
//       copies.
//
//--=========================================================================--

#ifndef VPUAPI_H_INCLUDED
#define VPUAPI_H_INCLUDED

#include "vpuconfig.h"

#include "../vdi/vdi.h"
#include "../vdi/vdi_osal.h"
#include "rtk_test.h"

//------------------------------------------------------------------------------
// for RTK usage
//------------------------------------------------------------------------------
//////////////////
// Main Control //
//////////////////
//#define CHRIS_HACK
//#define RTK_HACK_FRAME_BUFFER
#define _TRACE_EN 0
#define RTK_DUMP_BS 0

///////////
// Macro //
///////////
#if _TRACE_EN
#define DBG_PRINT printf
#else
#define DBG_PRINT
#endif
#define _DBG() printf("[¡¹¡¹ %s %d]\n", __FUNCTION__, __LINE__);

extern RTK_VIDEO *gpRTK;
extern YUV_DATA *gpYUV;
int RTK_hack_init();
int RTK_hack_frame_buffer(vpu_buffer_t *vbFrame);
int RTK_decode_one_frame(int displayIndex);
int RTK_hack_frame_buffer_v2(long nTotalSize, long nNumFrame, long sizeLuma, long sizeChroma, long cbcrInterleave, long fieldMap, long sizeFb);
int RTK_unmmap();

//------------------------------------------------------------------------------
// common struct and definition
//------------------------------------------------------------------------------
typedef enum {
    STD_AVC,
    STD_VC1,
    STD_MPEG2,
    STD_MPEG4,
    STD_H263,
    STD_DIV3,
    STD_RV,
    STD_AVS,
    STD_THO = 9,
    STD_VP3,
    STD_VP8,
    STD_HEVC,
    STD_MAX
} CodStd;

/************************************************************************/
/* Limitations                                                          */
/************************************************************************/
#define MAX_FRAMEBUFFER_COUNT               31  /* The 32nd framebuffer is reserved for WTL */

/************************************************************************/
/* PROFILE & LEVEL                                                      */
/************************************************************************/
/* HEVC */
#define HEVC_PROFILE_MAIN                   1
#define HEVC_PROFILE_MAIN10                 2
#define HEVC_PROFILE_STILLPICTURE           3
/* Tier */
#define HEVC_TIER_MAIN                      0
#define HEVC_TIER_HIGH                      1
/* Level */
#define HEVC_LEVEL(_Major, _Minor)          (_Major*10+_Minor)

/* H.264 */
#define H264_PROFILE_BASELINE               66
#define H264_PROFILE_MAIN                   77
#define H264_PROFILE_EXTENDED               88
#define H264_PROFILE_HIGH                   100
#define H264_PROFILE_HIGH10                 110
#define H264_PROFILE_HIGH10_INTRA           120
#define H264_PROFILE_HIGH422                122
#define H264_PROFILE_HIGH444                244
#define H264_PROFILE_CAVLC_444_INTRA        44

/* SEI & VUI mask */
#define SEI_PAYLOAD_BUFFERING_PERIOD        (1<<0)
#define SEI_PAYLOAD_PIC_TIMING              (1<<1)
#define SEI_PAYLOAD_PAN_SCAN_RECT           (1<<2)
#define SEI_PAYLOAD_FILLER_PAYLOAD          (1<<3)
#define SEI_PAYLOAD_USER_DATA_ITU_T_T35     (1<<4)
#define SEI_PAYLOAD_USER_DATA               (1<<5)              // user_data_unregistered()
#define SEI_PAYLOAD_RECOVERY_POINT          (1<<6)
#define SEI_PAYLOAD_SCENE_INFO              (1<<7)
#define SEI_PAYLOAD_FULL_FRAME_SNAPSHOT     (1<<8)
#define SEI_PAYLOAD_PROGRESSIVE_RSS         (1<<9)             // progressive_refinement_segment_start()
#define SEI_PAYLOAD_PROGRESSIVE_RSE         (1<<10)            // progressive_refinement_segment_end()
#define SEI_PAYLOAD_FILM_GRAIN_CHAR         (1<<11)            // film_grain_characteristics()
#define SEI_PAYLOAD_POST_FILTER_HINT        (1<<12)
#define SEI_PAYLOAD_TONE_MAPPING_INFO       (1<<13)
#define SEI_PAYLOAD_FRAME_PACKING_ARR       (1<<14)            // frame_packing_arrangement
#define SEI_PAYLOAD_DISPLAY_ORIENTATION     (1<<15)
#define SEI_PAYLOAD_SOP_DESCRIPTION         (1<<16)
#define SEI_PAYLOAD_ACTIVE_PARAMETER_SETS   (1<<17)
#define SEI_PAYLOAD_SUB_PIC_TIMING          (1<<18)
#define SEI_PAYLOAD_T10_INDEX               (1<<19)
#define SEI_PAYLOAD_DECODED_PICTURE_HASH    (1<<20)
#define VUI_INFORMATION                     (1<<31)

/************************************************************************/
/* Error codes                                                          */
/************************************************************************/
typedef enum {
    RETCODE_SUCCESS,                        /* 0  */
    RETCODE_FAILURE,
    RETCODE_INVALID_HANDLE,
    RETCODE_INVALID_PARAM,
    RETCODE_INVALID_COMMAND,
    RETCODE_ROTATOR_OUTPUT_NOT_SET,         /* 5  */
    RETCODE_ROTATOR_STRIDE_NOT_SET,
    RETCODE_FRAME_NOT_COMPLETE,
    RETCODE_INVALID_FRAME_BUFFER,
    RETCODE_INSUFFICIENT_FRAME_BUFFERS,
    RETCODE_INVALID_STRIDE,                 /* 10 */
    RETCODE_WRONG_CALL_SEQUENCE,
    RETCODE_CALLED_BEFORE,
    RETCODE_NOT_INITIALIZED,
    RETCODE_USERDATA_BUF_NOT_SET,
    RETCODE_MEMORY_ACCESS_VIOLATION,        /* 15 */
    RETCODE_VPU_RESPONSE_TIMEOUT,
    RETCODE_INSUFFICIENT_RESOURCE,
    RETCODE_NOT_FOUND_BITCODE_PATH,
    RETCODE_NOT_SUPPORTED_FEATURE,
    RETCODE_NOT_FOUND_VPU_DEVICE,           /* 20 */
    RETCODE_NOT_HAS_RESOURCE,
} RetCode;


/************************************************************************/
/* Utility macros                                                       */
/************************************************************************/
#define VPU_ALIGN4(_x)              ((_x+0x03)&~0x03)
#define VPU_ALIGN8(_x)              ((_x+0x07)&~0x07)
#define VPU_ALIGN16(_x)             ((_x+0x0f)&~0x0f)
#define VPU_ALIGN32(_X)             ((_X+0x1f)&~0x1f)
#define VPU_ALIGN64(_X)             ((_X+0x3f)&~0x3f)

/************************************************************************/
/*                                                                      */
/************************************************************************/
#define INTERRUPT_TIMEOUT_VALUE     (Uint32)-1

/**
 * \brief       parameters of DEC_SET_SEQ_CHANGE_MASK
 */
#define SEQ_CHANGE_ENABLE_PROFILE   (1<<5)
#define SEQ_CHANGE_ENABLE_SIZE      (1<<16)
#define SEQ_CHANGE_ENABLE_DPB_COUNT (1<<19)
#define SEQ_CHANGE_ENABLE_ALL       (SEQ_CHANGE_ENABLE_PROFILE|SEQ_CHANGE_ENABLE_SIZE|SEQ_CHANGE_ENABLE_DPB_COUNT)



typedef enum {
    ENABLE_ROTATION,
    DISABLE_ROTATION,
    ENABLE_MIRRORING,
    DISABLE_MIRRORING,
    SET_MIRROR_DIRECTION,
    SET_ROTATION_ANGLE,
    SET_ROTATOR_OUTPUT,
    SET_ROTATOR_STRIDE,
    DEC_GET_SEQ_INFO,
    DEC_SET_SPS_RBSP,
    DEC_SET_PPS_RBSP,
    DEC_SET_SEQ_CHANGE_MASK,
    ENABLE_DERING,
    DISABLE_DERING,
    SET_SEC_AXI,
    SET_DRAM_CONFIG,    //coda960 only
    GET_DRAM_CONFIG,    //coda960 only
    ENABLE_REP_USERDATA,
    DISABLE_REP_USERDATA,
    SET_ADDR_REP_USERDATA,
    SET_VIRT_ADDR_REP_USERDATA,
    SET_SIZE_REP_USERDATA,
    SET_USERDATA_REPORT_MODE,
    SET_CACHE_CONFIG,
    GET_TILEDMAP_CONFIG,
    SET_LOW_DELAY_CONFIG,
    SET_DECODE_FLUSH,
    DEC_SET_FRAME_DELAY,
    DEC_SET_WTL_FRAME_NUM,
    DEC_SET_WTL_FRAME_FORMAT,
    DEC_GET_FIELD_PIC_TYPE,
    DEC_GET_DISPLAY_OUTPUT_INFO,
    DEC_ENABLE_REORDER,
    DEC_DISABLE_REORDER,
    DEC_FREE_FRAME_BUFFER,
    DEC_GET_FRAMEBUF_INFO,               /* Get all informations related to framebuffer: see DecGetFrameBufInfo struct */
    DEC_RESET_FRAMEBUF_INFO,             /* reset addresses of framebuffer */
    ENABLE_DEC_THUMBNAIL_MODE,
    DEC_SET_DISPLAY_FLAG,
    DEC_SET_REPEAT_EMPTY_INTERRUPT,
    ENC_ADD_PPS,
    ENC_SET_ACTIVE_PPS,
    ENC_GET_ACTIVE_PPS,
    //vpu put header stream to bitstream buffer
    ENC_PUT_VIDEO_HEADER,
    ENC_PUT_MP4_HEADER,
    ENC_PUT_AVC_HEADER,
    //host generate header bitstream
    ENC_GET_VIDEO_HEADER,
    ENC_SET_INTRA_MB_REFRESH_NUMBER,
    ENC_ENABLE_HEC,
    ENC_DISABLE_HEC,
    ENC_SET_SLICE_INFO,
    ENC_SET_GOP_NUMBER,
    ENC_SET_INTRA_QP,
    ENC_SET_BITRATE,
    ENC_SET_FRAME_RATE,
    ENC_CONFIG_SUB_FRAME_SYNC,
    ENC_SET_SUB_FRAME_SYNC,
    GET_LOW_DELAY_OUTPUT,
    ENABLE_LOGGING,
    DISABLE_LOGGING,
    CMD_END
} CodecCommand;

typedef enum {
    CBCR_SEPARATED = 0,
    CBCR_INTERLEAVED,
    CRCB_INTERLEAVED
} CbCrInterLeave;

typedef enum {
    CBCR_ORDER_NORMAL,
    CBCR_ORDER_REVERSED
} CbCrOrder;

typedef enum {
    MIRDIR_NONE,
    MIRDIR_VER,
    MIRDIR_HOR,
    MIRDIR_HOR_VER
} MirrorDirection;

typedef enum {
    FORMAT_420                          = 0,    /* 8bit */
    FORMAT_422                          = 1,    /* 8bit */
    FORMAT_224                          = 2,    /* 8bit */
    FORMAT_444                          = 3,    /* 8bit */
    FORMAT_400                          = 4,    /* 8bit */
                                                /* Little Endian Perspective     */
                                                /*     | addr 0  | addr 1  |     */
    FORMAT_420_P10_16BIT_MSB,                   /* lsb | 000xxxx |xxxxxxxx | msb */
    FORMAT_420_P10_16BIT_LSB,                   /* lsb | xxxxxxx |xxxxx000 | msb */
    FORMAT_420_P10_32BIT_MSB,                   /* lsb |00xxxxxxxxxxxxxxxxxxxxxxxxxxx| msb */
    FORMAT_420_P10_32BIT_LSB,                   /* lsb |xxxxxxxxxxxxxxxxxxxxxxxxxxx00| msb */
} FrameBufferFormat;

typedef enum {
    INT_BIT_INIT            = 0,
    INT_BIT_SEQ_INIT        = 1,
    INT_BIT_SEQ_END         = 2,
    INT_BIT_PIC_RUN         = 3,
    INT_BIT_FRAMEBUF_SET    = 4,
    INT_BIT_ENC_HEADER      = 5,
    INT_BIT_DEC_PARA_SET    = 7,
    INT_BIT_DEC_BUF_FLUSH   = 8,
    INT_BIT_USERDATA        = 9,
    INT_BIT_DEC_FIELD       = 10,
    INT_BIT_DEC_MB_ROWS     = 13,
    INT_BIT_BIT_BUF_EMPTY   = 14,
    INT_BIT_BIT_BUF_FULL    = 15
} InterruptBit;

/* For backward compatibility */
typedef InterruptBit Coda9InterruptBit;

typedef enum {
    INT_WAVE_INIT           = 0,
    INT_WAVE_DEC_PIC_HDR    = 1,
    INT_WAVE_FINI_SEQ       = 2,
    INT_WAVE_DEC_PIC        = 3,
    INT_WAVE_SET_FRAMEBUF   = 4,
    INT_WAVE_FLUSH_DECODER  = 5,
    INT_WAVE_GET_FW_VERSION = 9,
    INT_WAVE_QUERY_DECODER  = 10,
    INT_WAVE_SLEEP_VPU      = 11,
    INT_WAVE_WAKEUP_VPU     = 12,
    INT_WAVE_CHANGE_INST    = 13,
    INT_WAVE_CREATE_INSTANCE= 14,
    INT_WAVE_BIT_BUF_EMPTY  = 15,
} Wave4InterruptBit;


typedef enum {
    PIC_TYPE_I            = 0,
    PIC_TYPE_P            = 1,
    PIC_TYPE_B            = 2,
    PIC_TYPE_VC1_BI       = 2,
    PIC_TYPE_VC1_B        = 3,
    PIC_TYPE_D            = 3,    // D picture in mpeg2, and is only composed of DC codfficients
    PIC_TYPE_S            = 3,    // S picture in mpeg4, and is an acronym of Sprite. and used for GMC
    PIC_TYPE_VC1_P_SKIP   = 4,
    PIC_TYPE_MP4_P_SKIP_NOT_CODED = 4, // Not Coded P Picture at mpeg4 packed mode
    PIC_TYPE_IDR          = 5,    // H.264 IDR frame
    PIC_TYPE_MAX
} PicType;

typedef enum {
    PAIRED_FIELD          = 0,
    TOP_FIELD_MISSING     = 1,
    BOT_FIELD_MISSING     = 2,
    TOP_BOT_FIELD_MISSING = 3,
} AvcNpfFieldInfo;

typedef enum {
    FF_NONE                 = 0,
    FF_FRAME                = 1,
    FF_FIELD                = 2,
} FrameFlag;

/*!
 * \brief   BS_MODE_INTERRUPT
 *          BS_MODE_ROLLBACK
 *          BS_MODE_PIC_END
 *          BS_MODE_NAL        When this mode is set, caller should fill bitstream in NAL unit into bitstream buffer
 *                             This mode is supported only WAVE4.
 */
typedef enum {
    BS_MODE_INTERRUPT,
    BS_MODE_ROLLBACK,
    BS_MODE_PIC_END,
    BS_MODE_NAL,
} BitStreamMode;

typedef enum {
    SW_RESET_SAFETY,
    SW_RESET_FORCE,
    SW_RESET_ON_BOOT
} SWResetMode;

enum {
    PRODUCT_ID_980,
    PRODUCT_ID_960  = 1,
    PRODUCT_ID_950  = 1,        /*!<< same with CODA960 */
    PRODUCT_ID_7503,
    PRODUCT_ID_320,
    PRODUCT_ID_410,
    PRODUCT_ID_412,
    PRODUCT_ID_NONE,
};



typedef struct {
    // gdi2.0
    int  xy2axiLumMap[32];
    int  xy2axiChrMap[32];
    int  xy2axiConfig;

    // gdi1.0
    int  xy2caMap[16];
    int  xy2baMap[16];
    int  xy2raMap[16];
    int  rbc2axiMap[32];
    int  xy2rbcConfig;
    int  tiledBaseAddr;

    // common
    int  mapType;
    int  productId;
    int  tbSeparateMap;
    int  topBotSplit;
    int  tiledMap;
    int  convLinear;
} TiledMapConfig;


typedef struct {
    int  rasBit;
    int  casBit;
    int  bankBit;
    int  busBit;
} DRAMConfig;

typedef enum {
    LINEAR_FRAME_MAP            = 0,
    TILED_FRAME_V_MAP           = 1,
    TILED_FRAME_H_MAP           = 2,
    TILED_FIELD_V_MAP           = 3,
    TILED_MIXED_V_MAP           = 4,
    TILED_FRAME_MB_RASTER_MAP   = 5,
    TILED_FIELD_MB_RASTER_MAP   = 6,
    TILED_FRAME_NO_BANK_MAP     = 7,    // coda980 only
    TILED_FIELD_NO_BANK_MAP     = 8,    // coda980 only
    LINEAR_FIELD_MAP            = 9,    // coda980 only
    CODA_TILED_MAP_TYPE_MAX     = 10,
    COMPRESSED_FRAME_MAP        = 10,   // WAVE4 only
    COMPRESSED_FRAME_MAP_10BIT  = 11,   // WAVE4 only
    TILED_MAP_TYPE_MAX
} TiledMapType;

typedef struct {
    PhysicalAddress bufY;
    PhysicalAddress bufCb;
    PhysicalAddress bufCr;
    PhysicalAddress bufYBot;        // coda980 only
    PhysicalAddress bufCbBot;       // coda980 only
    PhysicalAddress bufCrBot;       // coda980 only
    int             cbcrInterleave;
    int             nv21;           // WAVE410: cbcrinterleave MUST be enabled.
    int             endian;
    int             myIndex;
    int             mapType;
    int             stride;
    int             height;
    int             lumaBitDepth;
    int             chromaBitDepth;
    FrameBufferFormat   format;
    int             sourceLBurstEn;
} FrameBuffer;

typedef enum {
    FB_TYPE_CODEC,
    FB_TYPE_PPU,
} FramebufferAllocType;

typedef struct {
    int mapType;
    int cbcrInterleave;
    int nv21;
    FrameBufferFormat format;
    int stride;
    int height;
    int lumaBitDepth;
    int chromaBitDepth;
    int endian;
    int num;
    int type;
} FrameBufferAllocInfo;


typedef struct {
    Uint32 left;
    Uint32 top;
    Uint32 right;
    Uint32 bottom;
} VpuRect;

//Theora specific display information
typedef struct {
    int frameWidth;
    int frameHeight;
    int picWidth;
    int picHeight;
    int picOffsetX;
    int picOffsetY;
} ThoScaleInfo;

// VP8 specific display information
typedef struct {
    unsigned hScaleFactor   : 2;
    unsigned vScaleFactor   : 2;
    unsigned picWidth       : 14;
    unsigned picHeight      : 14;
} Vp8ScaleInfo;



typedef struct {
    int lowDelayEn;
    int numRows;
} LowDelayInfo;

typedef struct {
    union {
        struct {
            int useBitEnable;
            int useIpEnable;
            int useDbkYEnable;
            int useDbkCEnable;
            int useOvlEnable;
            int useBtpEnable;
        } coda9;
        struct {
            int useBitEnable;
            int useIpEnable;
            int useLfRowEnable;
        } wave4;
    } u;
} SecAxiUse;

/* For MaverickCache1 */
typedef struct CacheSizeCfg {
    unsigned BufferSize     : 8;
    unsigned PageSizeX      : 4;
    unsigned PageSizeY      : 4;
    unsigned CacheSizeX     : 4;
    unsigned CacheSizeY     : 4;
    unsigned Reserved       : 8;
} CacheSizeCfg;

typedef struct {
    struct {
        union {
            Uint32 word;
            CacheSizeCfg cfg;
        } luma;
        union {
            Uint32 word;
            CacheSizeCfg cfg;
        } chroma;
        unsigned Bypass         : 1;
        unsigned DualConf       : 1;
        unsigned PageMerge      : 2;
    } type1;
    struct {
        unsigned int CacheMode;
    } type2;
} MaverickCacheConfig;

typedef struct {
    int      topFieldFirst;
    PicType  firstFieldType;
    PicType  secondFieldType;
} PicStruct;

typedef struct {
    Uint32 * paraSet;
    int     size;
} DecParamSet;

#ifdef SUPPORT_MEM_PROTECT
typedef enum {
    WPROT_DEC_TEMP,
    WPROT_DEC_WORK,
    WPROT_DEC_PIC_SAVE,
    WPROT_DEC_USER      = WPROT_DEC_PIC_SAVE,
    WPROT_DEC_REPORT,
    WPROT_DEC_FRAME,
    WPROT_DEC_SEC_AXI,
    WPROT_DEC_EXT_1,
    WPROT_DEC_EXT_2,
    WPROT_DEC_EXT_3,
    WPROT_DEC_EXT_4,
    WPROT_DEC_MAX = 10
} WriteMemProtectDecType;

typedef enum {
    WPROT_ENC_TEMP,
    WPROT_ENC_WORK,
    WPROT_ENC_BITSTREAM,
    WPROT_ENC_SCRATCH,
    WPROT_ENC_FRAME,
    WPROT_ENC_SEC_AXI,
    WPROT_ENC_EXT_1,
    WPROT_ENC_EXT_2,
    WPROT_ENC_EXT_3,
    WPROT_ENC_EXT_4,
    WPROT_ENC_MAX = 10
} WriteMemProtectEncType;
typedef struct {
    int enable;
    int isSecondary;
    PhysicalAddress startAddress;
    PhysicalAddress endAddress;
} WriteMemProtectRegion;

typedef struct {
    Uint32                numOfRegion;
    WriteMemProtectRegion decRegion[WPROT_DEC_MAX];
    WriteMemProtectRegion encRegion[WPROT_ENC_MAX];
} WriteMemProtectCfg;
#endif

struct CodecInst;

//------------------------------------------------------------------------------
// decode struct and definition
//------------------------------------------------------------------------------
#define VPU_HANDLE_TO_DECINFO(_handle)          (&(((CodecInst*)_handle)->CodecInfo.decInfo))

typedef struct CodecInst* DecHandle;

typedef struct {
    int fixedFrameRateFlag;
    int timingInfoPresent;
    int chromaLocBotField;
    int chromaLocTopField;
    int chromaLocInfoPresent;
    int colorPrimaries;
    int colorDescPresent;
    int isExtSAR;
    int vidFullRange;
    int vidFormat;
    int vidSigTypePresent;
    int vuiParamPresent;
    int vuiPicStructPresent;
    int vuiPicStruct;
} AvcVuiInfo;

typedef struct {
    int barLeft;
    int barRight;
    int barTop;
    int barBottom;
} MP2BarDataInfo;    

typedef struct {
    Uint32	offsetNum;

    Int16	horizontalOffset1;
    Int16	horizontalOffset2;
    Int16	horizontalOffset3;

    Int16	verticalOffset1;
    Int16	verticalOffset2;
    Int16	verticalOffset3;

} MP2PicDispExtInfo;

typedef struct {
    CodStd          bitstreamFormat;
    PhysicalAddress bitstreamBuffer;
    int             bitstreamBufferSize;
    int             mp4DeblkEnable;
    int             avcExtension;
    int             mp4Class;
    int             tiled2LinearEnable;
    int             tiled2LinearMode;
    int             wtlEnable;
    int             wtlMode;
    int             cbcrInterleave; // 0: Separated YCbCr, 1: NV12
    int             nv21;               /*!<< WAVE410: NV21 format */
    int             cbcrOrder;
    int             bwbEnable;
    int             frameEndian;
    int             streamEndian;
    int             bitstreamMode;
    Uint32          coreIdx;
    vpu_buffer_t    vbWork;
} DecOpenParam;

typedef struct {
    Int32           picWidth;
    Int32           picHeight;
    Int32           fRateNumerator;
    Int32           fRateDenominator;
    VpuRect            picCropRect;
    Int32           mp4DataPartitionEnable;
    Int32           mp4ReversibleVlcEnable;
    Int32           mp4ShortVideoHeader;
    Int32           h263AnnexJEnable;
    Int32           minFrameBufferCount;
    Int32           frameBufDelay;              /*!<< -1 when HEVC */
    Int32           normalSliceSize;
    Int32           worstSliceSize;

    // Report Information
    Int32           maxSubLayers;               /*!<< number of sub-layer for HEVC */
    Int32           profile;
    Int32           level;
    Int32           tier;                       /*!<< 0: Main Tier, 1: High Tier for HEVC */
    Int32           interlace;
    Int32           constraint_set_flag[4];
    Int32           direct8x8Flag;
    Int32           vc1Psf;
    Int32           isExtSAR;                   /*!<< H.264/265 */
    Int32           maxNumRefFrme;
    Int32           maxNumRefFrmFlag;
    Int32           aspectRateInfo;
    Int32           bitRate;
    ThoScaleInfo    thoScaleInfo;
    Vp8ScaleInfo    vp8ScaleInfo;
    Int32           mp2LowDelay;
    Int32           mp2DispVerSize;
    Int32           mp2DispHorSize;
    Int32           chromaFormat;

    Int32           userDataNum;
    Int32           userDataSize;
    Int32           userDataBufFull;
    //VUI information
    Int32           chromaFormatIDC;            /*!<< HEVC */
    Int32           lumaBitdepth;
    Int32           chromaBitdepth;
    Int32           seqInitErrReason;
    PhysicalAddress rdPtr;
    PhysicalAddress wrPtr;
    AvcVuiInfo      avcVuiInfo;
    Uint32          sequenceNo;
    MP2BarDataInfo  mp2BardataInfo;
} DecInitialInfo;


// Report Information

typedef struct {
    Int32 iframeSearchEnable;           /*!<< CODA9 features. For backward compatibility */
    Int32 skipframeMode;
#ifdef SUPPORT_SET_CRA_AS_BLA
	BOOL craAsBlaFlag;
#endif
    union {
        Int32 mp2PicFlush;
        Int32 rvDbkMode;
    } DecStdParam;
} DecParam;

// Report Information
typedef struct {
    int userDataNum;        // User Data
    int userDataSize;
    int userDataBufFull;
    int activeFormat;
} DecOutputExtData;
// VP8 specific header information
typedef struct {
    unsigned showFrame      : 1;
    unsigned versionNumber  : 3;
    unsigned refIdxLast     : 8;
    unsigned refIdxAltr     : 8;
    unsigned refIdxGold     : 8;
} Vp8PicInfo;

// MVC specific picture information
typedef struct {
    int viewIdxDisplay;
    int viewIdxDecoded;
} MvcPicInfo;

// AVC specific SEI information (frame packing arrangement SEI)
typedef struct {
    unsigned exist;
    unsigned framePackingArrangementId;
    unsigned framePackingArrangementCancelFlag;
    unsigned quincunxSamplingFlag;
    unsigned spatialFlippingFlag;
    unsigned frame0FlippedFlag;
    unsigned fieldViewsFlag;
    unsigned currentFrameIsFrame0Flag;
    unsigned frame0SelfContainedFlag;
    unsigned frame1SelfContainedFlag;
    unsigned framePackingArrangementExtensionFlag;
    unsigned framePackingArrangementType;
    unsigned contentInterpretationType;
    unsigned frame0GridPositionX;
    unsigned frame0GridPositionY;
    unsigned frame1GridPositionX;
    unsigned frame1GridPositionY;
    unsigned framePackingArrangementRepetitionPeriod;
} AvcFpaSei;

// AVC specific HRD information
typedef struct {
    int cpbMinus1;
    int vclHrdParamFlag;
    int nalHrdParamFlag;
} AvcHrdInfo;

// AVC specific Recovery Point information
/**
* @brief    This is a data structure for AVC specific picture information. Only AVC decoder
returns this structure after decoding a frame.
*/
typedef struct {
/**
@verbatim
This is a flag to indicate whether AVC RP SEI exists or not.

@* 0 : AVC RP SEI does not exist.
@* 1 : AVC RP SEI exists.
@endverbatim
*/
    unsigned exist;
    int recoveryFrameCnt;       /**< recovery_frame_cnt */
    int exactMatchFlag;         /**< exact_match_flag */
    int brokenLinkFlag;         /**< broken_link_flag */
    int changingSliceGroupIdc;  /**< changing_slice_group_idc */
} AvcRpSei;

typedef struct {
    int displayPOC;                 /*!<< when indexFrameDisplay is -1, this value will be -1 */
    int decodedPOC;                 /*!<< when indexFrameDecoded is -1, this value will be -1 */
} H265Info;

typedef struct {
    int indexFrameDisplay;
    int indexFrameDisplayForTiled;  /*!<< In case of WTL mode, This index indicates framebuffer index of tiled or compressed framebuffer */
    int indexFrameDecoded;
    int indexFrameDecodedForTiled;  /*!<< In case of WTL mode, This index indicates framebuffer index of tiled or compressed framebuffer */
    int nalType;                    /*!<< WAVE410 ONLY, See Table 7-1 in H.265 specifictaion */
    int picType;
    int picTypeFirst;
    int numOfErrMBs;
    int numOfTotMBs;
    int notSufficientSliceBuffer;
    int notSufficientPsBuffer;
    int decodingSuccess;
    int interlacedFrame;
    int chunkReuseRequired;
    VpuRect rcDisplay;
    int dispPicWidth;
    int dispPicHeight;
    VpuRect rcDecoded;
    int decPicWidth;
    int decPicHeight;
    int aspectRateInfo;
    int fRateNumerator;
    int fRateDenominator;
    Vp8ScaleInfo vp8ScaleInfo;
    Vp8PicInfo vp8PicInfo;
    MvcPicInfo mvcPicInfo;
    AvcFpaSei avcFpaSei;
    AvcHrdInfo avcHrdInfo;
    AvcVuiInfo avcVuiInfo;
    MP2BarDataInfo mp2BardataInfo;
    MP2PicDispExtInfo mp2PicDispExtInfo;
    AvcRpSei avcRpSei;
    H265Info h265Info;
    int avcNpfFieldInfo;
    int avcPocPic;
    int avcPocTop;
    int avcPocBot;
    int wprotErrReason;
    PhysicalAddress wprotErrAddress;
    // Report Information
    int pictureStructure;
    int topFieldFirst;
    int repeatFirstField;
    int progressiveFrame;
    int fieldSequence;
    int frameDct;
    int nalRefIdc;
    int decFrameInfo;
    int picStrPresent;
    int picTimingStruct;
    int progressiveSequence;
    int mp4TimeIncrement;
    int mp4ModuloTimeBase;
    DecOutputExtData decOutputExtData;
    int consumedByte;
    int rdPtr;
    int wrPtr;
    int bytePosFrameStart;
    int bytePosFrameEnd;
    FrameBuffer dispFrame;
    int mp2DispVerSize;
    int mp2DispHorSize;
    int mp2NpfFieldInfo;
    int frameDisplayFlag;
    int sequenceChanged;        // CODA9: [0]   1 - sequence changed
    // WAVE4: [5]   1 - H.265 profile changed
    //        [16]  1 - resolution changed
    //        [19]  1 - number of DPB changed
    int streamEndFlag;
    int frameCycle;
    int errorReason;
    int errorReasonExt;
    BOOL sequenceNo;
    int rvTr;			/**< This variable reports RV timestamp for Ref frame. */
    int rvTrB;			/**< This variable reports RV timestamp for B frame. */
/**
@verbatim
This field is valid only for VC-1 decoding. Field information of display frame index
is returned on `indexFrameDisplay`. Refer to <<vpuapi_h_AvcNpfFieldInfo>>.

@* 0 : Paired fields
@* 1 : Bottom (top-field missing)
@* 2 : Top (bottom-field missing)
@endverbatim
*/
    int vc1NpfFieldInfo;
} DecOutputInfo;

/**
 * \brief           Data type of DEC_GET_FRAME_BUFFER
 */
typedef struct {
    vpu_buffer_t  vbFrame;
    vpu_buffer_t  vbWTL;
    vpu_buffer_t  vbFbcYTbl;     /*!<< FBC Luma offset table   */
    vpu_buffer_t  vbFbcCTbl;     /*!<< FBC Chroma offset table */
    FrameBuffer   framebufPool[64];
    vpu_buffer_t  vbSlice;       // AVC, VP8 only
    vpu_buffer_t  vbMV;
} DecGetFramebufInfo;


#define MAX_ENC_PPS_NUM	2


//------------------------------------------------------------------------------
// encode struct and definition
//------------------------------------------------------------------------------

typedef struct CodecInst EncInst;
typedef EncInst * EncHandle;

typedef struct {
    int mp4DataPartitionEnable;
    int mp4ReversibleVlcEnable;
    int mp4IntraDcVlcThr;
    int mp4HecEnable;
    int mp4Verid;
} EncMp4Param;

typedef struct {
    int h263AnnexIEnable;
    int h263AnnexJEnable;
    int h263AnnexKEnable;
    int h263AnnexTEnable;
} EncH263Param;




typedef struct {
    int ppsId;
    int entropyCodingMode;  // 0 : CAVLC, 1 : CABAC, 2: CAVLC/CABAC select according to PicType
    int cabacInitIdc;
    int transform8x8Mode;   // 0 : disable(BP), 1 : enable(HP)
} AvcPpsParam;

typedef struct {
    int constrainedIntraPredFlag;
    int disableDeblk;
    int deblkFilterOffsetAlpha;
    int deblkFilterOffsetBeta;
    int chromaQpOffset;
    int audEnable;
    int frameCroppingFlag;
    int frameCropLeft;
    int frameCropRight;
    int frameCropTop;
    int frameCropBottom;


    int         mvcExtension;       /*!<< MVC extension */
    int         interviewEn;        /*!<< MVC extension */
    int         parasetRefreshEn;   /*!<< MVC extension */
    int         prefixNalEn;        /*!<< MVC extension */

    int         level;
    int         profile;            /*!<< 0 : BP, 1: MP 2: HP, CODA980 & WAVE320 */
    // [START] CODA980 & WAVE320
    int         fieldFlag;
    int         fieldRefMode;
    int         chromaFormat400;            /*!<< 0 : 420, 1 : 400 (MONO mode allowed only on HP) */
    int         ppsNum;
    AvcPpsParam ppsParam[MAX_ENC_PPS_NUM];
    // [END] CODA980 & WAVE320
} EncAvcParam;

typedef struct {
    int sliceMode;
    int sliceSizeMode;
    int sliceSize;
} EncSliceMode;

typedef enum {
    WIRED_BASE_SUB_FRAME_SYNC,
    REGISTER_BASE_SUB_FRAME_SYNC
} SubFrameSyncMode;

typedef struct {
    Uint32 subFrameSyncMode;
    Uint32 subFrameSyncOn;
    Uint32 sourceBufNumber;
    Uint32 sourceBufIndexBase;
} EncSubFrameSyncConfig;

typedef struct {
    Uint32 ipuEndOfRow;
    Uint32 ipuNewFrame;
    Uint32 ipuCurFrameIndex;
    int ipuCurRowIdx;
} EncSubFrameSyncState;

#ifdef RC_CHANGE_PARAMETER_DEF
/**
* @brief    This data structure is used when HOST wants to apply new RC parameters. 
*/
typedef struct {
    char pchChangeCfgFileName[1024]; /**< Encoder configuration file name */
    int  ChangeFrameNum;        /**< A frame number to start applying parameter change  */
    int  paraEnable;            /**< An enable flag for each parameter change */
    
    int  NewGopNum;             /**< A new encode GOP number */
    //int  NewIntraQpEn;
    int  NewIntraQp;            /**< A new quantized step parameter of intra frame picture for encoding process.*/
    int  NewBitrate;            /**< A new target bit rate in kilo bit per seconds (kbps) */
    int  NewFrameRate;          /**< A new frame rate */
    int  NewIntraRefresh;       /**< A new intra MB refresh number */
} ParamChange;
#endif /* RC_CHANGE_PARAMETER_DEF */
typedef struct {
    PhysicalAddress bitstreamBuffer;
    Uint32          bitstreamBufferSize;
    CodStd          bitstreamFormat;
    int             ringBufferEnable;
    int             picWidth;
    int             picHeight;
    int             linear2TiledEnable;
    int             linear2TiledMode;
    int             frameRateInfo;
    // [START] CODA980 & WAVE320
    int             MESearchRangeX;
    int             MESearchRangeY;
    int             rcGopIQpOffsetEn;
    int             rcGopIQpOffset;
    int             MESearchRange;
    // [END] CODA980 & WAVE320
    int             bitRate;
    int             initialDelay;
    double          qCompress;
    double          CBR_Decay;
    int             vbvBufferSize;
    int             frameSkipDisable;
    int             gopSize;
    int             idrInterval;
    int             meBlkMode;
    EncSliceMode    sliceMode;
    int             ConsIntraRefreshEnable;
    int             intraRefresh;
    int             ConscIntraRefreshEnable;
    int             userQpMax;

    //h.264 only
    int             maxIntraSize;
    int             userMaxDeltaQp;
    int             userQpMin;
    int             userMinDeltaQp;
    int             rcEnable;           // rate control enable (0:constant QP, 1:CBR, 2:ABR)
    int             MEUseZeroPmv;       // 0: PMV_ENABLE, 1: PMV_DISABLE
    int             intraCostWeight;    // Additional weight of Intra Cost for mode decision to reduce Intra MB density

    //mp4 only
    int             rcIntraQp;
    int             userGamma;
    int             rcIntervalMode;     // 0:normal, 1:frame_level, 2:slice_level, 3: user defined Mb_level
    int             mbInterval;         // use when RcintervalMode is 3

    union {
        EncMp4Param     mp4Param;
        EncH263Param    h263Param;
        EncAvcParam     avcParam;
    } EncStdParam;

    // Maverick-II Cache Configuration
    int             frameCacheBypass;
    int             frameCacheBurst;
    int             frameCacheMerge;
    int             frameCacheWayShape;
    int             cbcrInterleave;
    int             cbcrOrder;
    int             frameEndian;
    int             streamEndian;
    int             bwbEnable;
    int             lineBufIntEn;
#ifdef RC_CHANGE_PARAMETER_DEF
	ParamChange paramChange;
#endif /* RC_CHANGE_PARAMETER_DEF */

    Uint32          coreIdx;
} EncOpenParam;


typedef struct {
    int             minFrameBufferCount;
} EncInitialInfo;

/*
 * This EncSetROI structure is valid on CODA980 and WAVE320
 */
#define MAX_ROI_NUMBER  10
typedef struct {
    int mode;
    int number;
    VpuRect region[MAX_ROI_NUMBER];
    int qp[MAX_ROI_NUMBER];
} EncSetROI;

typedef struct {
    FrameBuffer*    sourceFrame;
    int             forceIPicture;
    int             skipPicture;
    int             quantParam;
    PhysicalAddress picStreamBufferAddr;
    int             picStreamBufferSize;
    int             fieldRun;               /*!<< CODA980 & WAVE320 */
    EncSetROI       setROI;                 /*!<< CODA980 & WAVE320 */
#ifdef SUPPORT_AVC_FRAME_RC
    int slice_type;
#endif
} EncParam;

// Report Information
typedef struct {
    int enable;
    int type;
    int sz;
    PhysicalAddress addr;
} EncReportInfo;

typedef struct {
    PhysicalAddress bitstreamBuffer;
    Uint32 bitstreamSize;
    int bitstreamWrapAround;
    int picType;
    int numOfSlices;
    int reconFrameIndex;
    FrameBuffer reconFrame;
    int rdPtr;
    int wrPtr;
    // Report Information
    EncReportInfo mbInfo;
    EncReportInfo mvInfo;
    EncReportInfo sliceInfo;
} EncOutputInfo;

typedef struct {
    PhysicalAddress paraSet;
    int size;
} EncParamSet;


typedef struct {
    PhysicalAddress buf;
    BYTE *pBuf;
    int size;
    int headerType;
} EncHeaderParam;

typedef enum {
    VOL_HEADER,
    VOS_HEADER,
    VIS_HEADER
} Mp4HeaderType;

typedef enum {
    SPS_RBSP,
    PPS_RBSP,
    SPS_RBSP_MVC,
    PPS_RBSP_MVC,
} AvcHeaderType;




#ifdef __cplusplus
extern "C" {
#endif

    Int32  VPU_IsBusy(
        Uint32 coreIdx
    );

    Int32 VPU_WaitInterrupt(
        Uint32 coreIdx,
        int timeout
    );

    /*!
     * \b It returns current program counter of VPU.
     *
     * \return      program counter
     * \return      0 when VPU is not running.
     * \return      -1 Invalid parameters.
     */
    Int32  VPU_IsInit(
        Uint32 coreIdx
    );

    RetCode VPU_Init(
        Uint32 coreIdx
    );

    RetCode VPU_InitWithBitcode(
        Uint32 coreIdx,
        const Uint16 *bitcode,
        Uint32 sizeInWord
    );

    RetCode VPU_DeInit(
        Uint32 coreIdx
    );

    /*!
     * \return      number of instances
     * \return      -1  invalid parameter
     */
    int VPU_GetOpenInstanceNum(
        Uint32 coreIdx
    );

    RetCode VPU_GetVersionInfo(
        Uint32 coreIdx,
        Uint32* versionInfo,
        Uint32* revision,
        Uint32* productId
    );

    void VPU_ClearInterrupt(
        Uint32 coreIdx
    );

    RetCode VPU_SWReset(
        Uint32      coreIdx,
        SWResetMode resetMode,
        void*       pendingInst
    );

    RetCode VPU_HWReset(
        Uint32  coreIdx
    );

    RetCode VPU_SleepWake(
        Uint32  coreIdx,
        int     iSleepWake
    );

    int VPU_GetMvColBufSize(
        CodStd codStd,
        int width,
        int height,
        int num
    );

    RetCode VPU_GetFBCOffsetTableSize(
        CodStd  codStd,
        int     width,
        int     height,
        int*     ysize,
        int*     csize
    );

    /**
     *  \brief  It returns size of framebuffer with given parameters.
     *
     *  \param  coreIdx     [in]    VPU core index number
     *  \param  width       [in]    picture width in pixel
     *  \param  height      [in]    picture height in pixel
     *  \param  mapType     [in]    map type of framebuffer. see TiledMapType data type.
     *  \param  format      [in]    YUV400, 420, ... see FrameBufferFormat data type.
     *  \param  interleave  [in]    CbCr interleaved format Also known as NV12.
     *                              This parameter is ignored but we keep this parameter for backward compatibility.
     *  \param  pDramCfg    [in]    RAS, CAS bits of memory of target system.
     *                              If the product is NOT CODA960 then this parameter will be ignored.
     *
     *  \return     -1 if given parameters are invalid.
     *  \return     size of framebuffer in bytes.
     */
    int VPU_GetFrameBufSize(
        int         coreIdx,
        int         width,
        int         height,
        int         mapType,
        int         format,
        int         interleave,
        DRAMConfig* pDramCfg
    );

    /**
     *  \brief      returns product ID.
     *
     *  \return     PRODUCT_ID_410, PRODUCT_ID_980,...
     *  \return     -1 on failure.
     */
    int VPU_GetProductId(
        int coreIdx
    );

// function for decode
    RetCode VPU_DecOpen(
        DecHandle *pHandle,
        DecOpenParam *pop
    );

    RetCode VPU_DecClose(
        DecHandle handle
    );

    /**
     *  \brief  This function is used to stop waiting bitstream for sequence header.
     *
     *  \param  handle  [in]    decoder handler
     *  \param  escape  [in]    If escape is set to 1 then VPU will try to parse sequence header
     *                          given bitstream.
     *
     *  \return RETCODE_SUCCESS
     *  \return RETCODE_INVALID_HANDLE for invalid handle or bitstream mode is not *INTERRUPT MODE*
     *
     *  *DEPRECATED*
     *  It is followed by VPU_DecCompleteSeqInit() or VPU_DecGetInitialInfo().
     *  INTERRUPT MODE in rtk API is one of styles of consuming bitstream buffer by VPU.
     *  When bitstream mode is set to INTERRUPT MODE the VPU will wait until the bitstream buffer
     *  is filled enough.
     */
    RetCode VPU_DecSetEscSeqInit(
        DecHandle   handle,
        int         escape
    );

    /**
     * *DEPRECATED*
     */
    RetCode VPU_DecGetInitialInfo(
        DecHandle       handle,
        DecInitialInfo* info
    );

    /**
     *  \brief  Starts decoding sequence header.
     *
     *  \param  handle  [in]    decoder handler
     */
    RetCode VPU_DecIssueSeqInit(
        DecHandle handle
    );

    /**
     *  \brief  Gets sequence header informations like width or height of picture, number of framebuffers
     *          for decoder to decode a stream, etc
     *
     *  \param handle   [in]    decoder handler
     *  \param info     [out]   pointer to DecInitialInfo structure to receives sequence header informations.
     */
    RetCode VPU_DecCompleteSeqInit(
        DecHandle       handle,
        DecInitialInfo* info
    );

    RetCode VPU_DecRegisterFrameBuffer(
        DecHandle    handle,
        FrameBuffer* bufArray,
        int          num,
        int          stride,
        int          height,
        int          mapType
    );

    RetCode VPU_DecGetFrameBuffer(
        DecHandle   handle,
        int         frameIdx,
        FrameBuffer* frameBuf
    );

    RetCode VPU_DecGetBitstreamBuffer(
        DecHandle       handle,
        PhysicalAddress *prdPrt,
        PhysicalAddress *pwrPtr,
        Uint32          *size
    );

    /**
     * \brief   Updates write-pointer with given size
     * \param   handle      decoder handle
     * \param   size        size in byte
     *                      If size is equal to 0 then set end-of-stream flag.
     *                      If size is equal to -1 then clear end-of-stream flag.
     * \details In this function, the parameter size is not checked out
     *          You can get available size of the bitstream buffer by calling VPU_DecGetBitstreamBuffer()
     * \see     VPU_DecGetBitstreamBuffer()
     */
    RetCode VPU_DecUpdateBitstreamBuffer(
        DecHandle   handle,
        int         size
    );

    /**
     * \
     */
    RetCode VPU_DecStartOneFrame(
        DecHandle handle,
        DecParam *param
    );

    RetCode VPU_DecGetOutputInfo(
        DecHandle       handle,
        DecOutputInfo*  info
    );

    RetCode VPU_DecFourKSetting(
        DecHandle handle
    );

    /**
     * \brief   Flush remaining framebuffers.
     *          In case of CODA9, It doesn't return retIndex.
     *          In case of WAVE4, It returns retIndex containing indexes of framebuffers in display order.
     */
    RetCode VPU_DecFrameBufferFlush(
        DecHandle       handle,
        DecOutputInfo*  pRemainingInfo,
        Uint32*         pRetNum
    );

    RetCode VPU_DecSetRdPtr(
        DecHandle       handle,
        PhysicalAddress addr,
        int             updateWrPtr
    );

    RetCode VPU_DecClrDispFlag(
        DecHandle   handle,
        int         index
    );

    RetCode VPU_DecGiveCommand(
        DecHandle       handle,
        CodecCommand    cmd,
        void*           parameter
    );

    RetCode VPU_DecAllocateFrameBuffer(
        DecHandle               handle,
        FrameBufferAllocInfo    info,
        FrameBuffer*            frameBuffer
    );

// function for encode
    RetCode VPU_EncOpen(
        EncHandle*      handle,
        EncOpenParam*   encOpParam
    );

    RetCode VPU_EncClose(
        EncHandle handle
    );

    RetCode VPU_EncGetInitialInfo(
        EncHandle       handle,
        EncInitialInfo* encInitInfo
    );

    RetCode VPU_EncRegisterFrameBuffer(
        EncHandle       handle,
        FrameBuffer*    bufArray,
        int             num,
        int             stride,
        int             height,
        int             mapType
    );

    RetCode VPU_EncGetFrameBuffer(
        EncHandle       handle,
        int             frameIdx,
        FrameBuffer*    frameBuf
    );

    RetCode VPU_EncGetBitstreamBuffer(
        EncHandle handle,
        PhysicalAddress *prdPrt,
        PhysicalAddress *pwrPtr,
        int * size
    );

    RetCode VPU_EncUpdateBitstreamBuffer(
        EncHandle handle,
        int size
    );

    RetCode VPU_EncStartOneFrame(
        EncHandle handle,
        EncParam * param
    );

    RetCode VPU_EncGetOutputInfo(
        EncHandle handle,
        EncOutputInfo * info
    );

    RetCode VPU_EncGiveCommand(
        EncHandle handle,
        CodecCommand cmd,
        void * parameter
    );

    void VPU_EncSetHostParaAddr(
        PhysicalAddress baseAddr,
        PhysicalAddress paraAddr
    );

    RetCode VPU_EncAllocateFrameBuffer(
        EncHandle handle,
        FrameBufferAllocInfo info,
        FrameBuffer *frameBuffer
    );


#ifdef __cplusplus
}
#endif

#endif

