//------------------------------------------------------------------------------
// File: vpuhelper.h
//
// Copyright (c) 2006, Realtek.  All rights reserved.
//------------------------------------------------------------------------------

#ifndef _VPU_HELPER_H_
#define _VPU_HELPER_H_

#include "vpuapi.h"
#include "vpuapifunc.h"

#include "vpuio.h"
#include "vpurun.h"
#ifdef SUPPORT_FFMPEG_DEMUX
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#endif
// Decoder Log file name to report information
#define FN_PIC_INFO "dec_pic_disp_info.log"
#define FN_SEQ_INFO "dec_seq_disp_info.log"
#define FN_PIC_TYPE "dec_pic_type.log"
#define FN_USER_DATA "dec_user_data.log"
#define FN_SEQ_USER_DATA "dec_seq_user_data.log"

// Encoder Log file name to report information
#define FN_ENC_QP_DATA "enc_qp.log"
#define FN_ENC_SLICE_BND_DATA "enc_slice_bnd.log"
#define FN_ENC_MV_DATA "enc_mv.log"
#define FN_ENC_SLICE_DATA "enc_slice.log"

#define	USER_DATA_INFO_OFFSET			(8*17)
#ifdef RC_CHANGE_PARAMETER_DEF
typedef enum {
    EN_PARA_CHANGE_NONE =0,
    EN_GOP_NUM          =1,
    EN_RC_INTRA_QP      =2,
    EN_RC_BIT_RATE      =4,
    EN_RC_FRAME_RATE    =8,
    EN_INTRA_REFRESH    =16
} PARA_CHANGE_ENABLE;
#endif

typedef struct VpuReportContext_t VpuReportContext_t;
typedef struct VpuReportConfig_t VpuReportConfig_t;

struct VpuReportConfig_t {
    PhysicalAddress userDataBufAddr;
    int userDataEnable;
    int userDataReportMode; // (0 : interrupt mode, 1 interrupt disable mode)
    int userDataBufSize;

};

struct VpuReportContext_t {
    VpuReportConfig_t cfg;
    osal_file_t fpUserDataLogfile;
    osal_file_t fpSeqUserDataLogfile;
    int decIndex;
};


typedef struct {
    char SrcFileName[256];
    int NumFrame;
    int PicX;
    int PicY;
    int FrameRate;

    // MPEG4 ONLY
    int VerId;
    int DataPartEn;
    int RevVlcEn;
    int ShortVideoHeader;
    int AnnexI;
    int AnnexJ;
    int AnnexK;
    int AnnexT;
    int IntraDcVlcThr;
    int VopQuant;

    // H.264 ONLY
    int ConstIntraPredFlag;
    int DisableDeblk;
    int DeblkOffsetA;
    int DeblkOffsetB;
    int ChromaQpOffset;
    int PicQpY;
    // H.264 VUI information
    int         VuiPresFlag;
    int         VideoSignalTypePresFlag;
    char		VideoFormat;
    char		VideoFullRangeFlag;
    int         ColourDescripPresFlag;
    char		ColourPrimaries;
    char		TransferCharacteristics;
    char		MatrixCoeff;
    int         NumReorderFrame;
    int         MaxDecBuffering;
    int         aud_en;
    int         level;
    // COMMON
    int GopPicNum;
    int SliceMode;
    int SliceSizeMode;
    int SliceSizeNum;
    // COMMON - RC
    int RcEnable;
    int RcBitRate;
    int RcInitDelay;
    double RcQcompress;
    double CBR_Decay;
    int RcBufSize;
    int IntraRefreshNum;
    int ConstantIntraQPEnable;
    int MaxQpSetEnable;
    int MaxQp;
    //H.264 only
    int MaxDeltaQpSetEnable;
    int MaxDeltaQp;
    int MinQpSetEnable;
    int MinQp;
    int MinDeltaQpSetEnable;
    int MinDeltaQp;
    int intraCostWeight;

    //MP4 Only
    int RCIntraQP;
    int HecEnable;

    int GammaSetEnable;
    int Gamma;

    // NEW RC Scheme
    int rcIntervalMode;
    int RcMBInterval;
    int skip_pic_nums[MAX_PIC_SKIP_NUM];
    int RcMaxIntraSize;
    int SearchRangeX;
    int SearchRangeY;
    //H.264 ONLY
    int entropyCodingMode;
    int cabacInitIdc;
    int transform8x8Mode;
    int chroma_format_400;
    int field_flag;
    int field_ref_mode;
    int RcGopIQpOffsetEn;
    int RcGopIQpOffset;
#ifdef RC_CHANGE_PARAMETER_DEF
    char pchChangeCfgFileName[1024];
    int  ChangeFrameNum;
    int  paraEnable;

    int  IntraMbRefreshNum;
    int  NewGopNum;
    int  NewIntraQp;
    int  NewBitrate;
    int  NewFrameRate;
    int  NewIntraRefresh;
#endif

    int interviewEn;
    int parasetRefreshEn;
    int prefixNalEn;

    int MeUseZeroPmv;	// will be removed. must be 264 = 0, mpeg4 = 1 263 = 0
    int MeBlkModeEnable; // only api option
    int IDRInterval;
} ENC_CFG;




typedef enum {                                   // start code
    NAL_TYPE_NON_IDR        = 0x01,
    NAL_TYPE_DP_A           = 0x02,
    NAL_TYPE_DP_B           = 0x03,
    NAL_TYPE_DP_C           = 0x04,
    NAL_TYPE_IDR            = 0x05,
    NAL_TYPE_SEI            = 0x06,
    NAL_TYPE_SEQ_PARA       = 0x07,
    NAL_TYPE_PIC_PARA       = 0x08,
    NAL_TYPE_AUD            = 0x09,
    NAL_TYPE_EO_SEQ         = 0x0a,
    NAL_TYPE_EO_STR         = 0x0b,
    NAL_TYPE_FILLER         = 0x0c,
    NAL_TYPE_SEQ_PARA_EXT   = 0x0d,
} NAL_TYPE;

typedef enum {
    UI_CMD_ERROR=-1,
    UI_CMD_KEEP_GOING=0,
    UI_CMD_SKIP_FRAME_LIFECYCLE,
    UI_CMD_RANDOM_ACCESS,
} UI_CMD;


typedef struct {
    DecOutputInfo*  buffer;
    int             size;
    int             count;
    int             front;
    int             rear;
} frame_queue_item_t;


#if defined (__cplusplus)
extern "C" {
#endif

    void CheckVersion(Uint32 core_idx);
    void PrintMemoryAccessViolationReason(Uint32 core_idx, DecOutputInfo *out);
    void PrintSEI(DecOutputInfo *out);
    // Submenu
    int FrameMenu(Uint32 option[]);
    int UI_GetUserCommand(int core_idx, DecHandle handle, DecParam *decParam, int *randomAccessPos);
    int HasStartCode( int bitstreamFormat, int profile );
    int fourCCToCodStd(unsigned int  fourcc);
    int fourCCToMp4Class(unsigned int  fourcc);
    int codecIdToCodStd(int codec_id);
    int codecIdToFourcc(int codec_id);
    int codecIdToMp4Class(int codec_id);


// Maverick Cache II
    int  MaverickCache2Config(MaverickCacheConfig* pCache, int decoder , int interleave, int bypass, int burst, int merge, int mapType, int wayshape);

#ifdef SUPPORT_FFMPEG_DEMUX
    //make from ffmpeg
    int BuildSeqHeader(BYTE *pbHeader, const CodStd codStd, const AVStream *st, int* size);
    int BuildPicHeader(BYTE *pbHeader, const CodStd codStd, const AVStream *st, const AVPacket *pkt, int size);
    int BuildPicData(BYTE *pbChunk, const CodStd codStd, const AVStream *st, const AVPacket *pkt);
#endif
    // Decoder Report Information
    void ConfigSeqReport(Uint32 core_idx, DecHandle handle, CodStd bitstreamFormat);
    void SaveSeqReport(Uint32 core_idx, DecHandle handle, DecInitialInfo *pSeqInfo, CodStd bitstreamFormat);
    void ConfigDecReport(Uint32 core_idx, DecHandle handle, CodStd bitstreamFormat);
    void SaveDecReport(Uint32 core_idx, DecHandle handle, DecOutputInfo *pDecInfo, CodStd bitstreamFormat, int mbNumX);
    void CheckUserDataInterrupt(Uint32 core_idx, DecHandle handle, int decodeIdx, CodStd bitstreamFormat, int int_reason);
    VpuReportContext_t *OpenDecReport(Uint32 core_idx, VpuReportConfig_t *cfg);
    void CloseDecReport(Uint32 core_idx);
    void PrintVpuStatus(Uint32 coreIdx, Uint32 productId);


    // Encoder helper function
    void changeRcParaTest(Uint32 core_idx,
                          EncHandle handle,
                          osal_file_t bsFp,
                          EncParam *pEncParam,
                          EncHeaderParam *pEncHeaderParam,
                          EncConfigParam *pEncConfig,
                          EncOpenParam	*pEncOP);
    int getEncOpenParam(EncOpenParam *pEncOP, EncConfigParam *pEncConfig, ENC_CFG *pEncCfg);
#ifdef RC_CHANGE_PARAMETER_DEF
#ifdef SUPPORT_AVC_FRAME_RC
    int getEncOpenParamChange(EncOpenParam *pEncOP, char *pEncConfig, ENC_CFG *pEncCfg, RC_INFO *rc);
    int ParseChangeParamCfgFile (ENC_CFG *pEncCfg, char *FileName, int picQpY, RC_INFO *rc);
#else
	int getEncOpenParamChange(EncOpenParam *pEncOP, char *pEncConfig, ENC_CFG *pEncCfg);
	int ParseChangeParamCfgFile (ENC_CFG *pEncCfg, char *FileName, int picQpY);
#endif
#endif /* RC_CHANGE_PARAMETER_DEF */
    int getEncOpenParamDefault(EncOpenParam *pEncOP, EncConfigParam *pEncConfig);
    void getAvcEncPPS(EncOpenParam *pEncOP);
    int parseMp4CfgFile(ENC_CFG *pEncCfg, char *FileName);
    int parseAvcCfgFile(ENC_CFG *pEncCfg, char *FileName);
    // Bitstream Buffer Control
    int WriteBsBufFromBufHelper(Uint32 core_idx,
                                DecHandle handle,
                                vpu_buffer_t *pVbStream,
                                BYTE *pChunk,
                                int chunkSize,
                                int endian);
    int GetBitstreamBufferRoom(DecHandle handle);
    int find_next_idr_nonidr_nal(BYTE *data, int len, int *sc_ptr, int *sc_num);
    int read_enc_ringbuffer_data(Uint32 core_idx,
                                 EncHandle handle,
                                 osal_file_t bsFp,
                                 PhysicalAddress bitstreamBuffer,
                                 Uint32 bitstreamBufferSize,
                                 int endian);
    int FillBsResetBufHelper(Uint32 core_idx,
                             unsigned char *buf,
                             PhysicalAddress paBsBufAddr,
                             int bsBufsize,
                             int endian);
    RetCode ReadBsRingBufHelper(Uint32 core_idx,
                                EncHandle handle,
                                osal_file_t bsFp,
                                PhysicalAddress bitstreamBuffer,
                                Uint32 bitstreamBufferSize,
                                int defaultsize,
                                int endian);
    int FillBsRingBufHelper(Uint32 core_idx,
                            EncHandle handle,
                            unsigned char *buf,
                            PhysicalAddress paBsBufStart,
                            PhysicalAddress paBsBufEnd,
                            int defaultsize,
                            int endian);
    // DPB Image Data Control
    int LoadYuvImageHelperFormat(Uint32 core_idx,
                                 osal_file_t yuvFp,
                                 Uint8 *pYuv,
                                 FrameBuffer	*fb,
                                 TiledMapConfig mapCfg,
                                 int picWidth,
                                 int picHeight,
                                 int stride,
                                 int interleave,
                                 int format,
                                 int endian);

    int SaveYuvImageHelperFormat(Uint32 core_idx,
                                 osal_file_t yuvFp,
                                 FrameBuffer *fbSrc,
                                 TiledMapConfig mapCfg,
                                 Uint8 *pYuv,
                                 VpuRect rect,
                                 BOOL enableCrop,
                                 int interLeave,
                                 int format,
                                 int endian);


    // Bitstream Data Control
    int ReadBsResetBufHelper(Uint32 core_idx,
                             osal_file_t streamFp,
                             PhysicalAddress bitstream,
                             int size,
                             int endian);


    int write_bitstream_from_file_helper(Uint32 core_idx, DecHandle handle,
                                         vpu_buffer_t *pVbStream,
                                         BufInfo*      pBitstream,
                                         int size,
                                         int endian);
    int IsSupportInterlaceMode(CodStd bitstreamFormat, DecInitialInfo *pSeqInfo);

    void init_VSYNC_flag();
    void clear_VSYNC_flag();
    int check_VSYNC_flag();
    void set_VSYNC_flag();
    frame_queue_item_t* frame_queue_init(int count);
    void frame_queue_deinit(frame_queue_item_t* queue);
    int frame_queue_enqueue(frame_queue_item_t* queue, DecOutputInfo data);
    int frame_queue_dequeue(frame_queue_item_t* queue, DecOutputInfo* data);
    int frame_queue_dequeue_all(frame_queue_item_t* queue);
    int frame_queue_peekqueue(frame_queue_item_t* queue, DecOutputInfo* data);
    int frame_queue_count(frame_queue_item_t* queue);
    int frame_queue_check_in_queue(frame_queue_item_t* queue, int index);
    /* make container */
#ifdef SUPPORT_FFMPEG_DEMUX
#define MAX_HEADER_CNT 3
#define CONTAINER_HEADER_SIZE (50*MAX_HEADER_CNT)
    int container_init(EncConfigParam encConfig,
                       AVOutputFormat **fmt,
                       AVFormatContext **oc,
                       AVStream **video_st,
                       int gopSize);
    int container_copy_header_from_bitstream_buffer(const Uint32 core_idx,
            const PhysicalAddress bitstream,
            const int size,
            const int endian,
            char *buf,
            int *pos);
    int container_save_header_from_buffer(char *dst, BYTE *src, const int size, int *pos);
    int container_write_es(const Uint32 core_idx,
                           const PhysicalAddress bitstream,
                           const int size,
                           const int endian,
                           AVFormatContext *oc,
                           AVStream *st,
                           char *header_buffer,
                           int *header_pos,
                           int format,
                           int picType);
    int container_deinit(AVFormatContext *oc);
#endif /* SUPPORT_FFMPEG_DEMUX */



#if defined (__cplusplus)
}
#endif

#endif //#ifndef _VPU_HELPER_H_
