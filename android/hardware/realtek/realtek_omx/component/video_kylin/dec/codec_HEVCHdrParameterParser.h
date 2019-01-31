#include <port.h>
#include <OSAL_RTK.h>
#include <string.h> // memset, memcpy
#include "util.h"
#include "dbgtrace.h"
#include <InbandAPI.h>
#include <sys/mman.h>

//RTK VE1 header file
#include "vpuconfig.h"
#include "vpuapi.h"
#include "vpuapifunc.h"
#include "coda9/coda9_regdefine.h"
#include "coda9/coda9_vpuconfig.h"
#include "wave/common/common_regdefine.h"
#include "wave/common/common_vpuconfig.h"
#include "wave/wave4/wave4_regdefine.h"
#include "vpuerror.h"

typedef struct HdrParameterParser
{
    DecHandle handle;
    CodStd bitstreamFormat;
    int coreIdx;

    h265_mastering_display_colour_volume_t m_mastering;
    h265_vui_param_t m_vui;

    vpu_buffer_t m_vpu_buffer;
    Uint8 *mp_work_buffer;

    int m_query_type;
} HdrParameterParser;


    void HdrAllocate(HdrParameterParser *pHdr, DecHandle handle, CodStd bitstreamFormat, int coreIdx);
    void HdrRelease(HdrParameterParser *pHdr);

    void HdrParseHeaderPicRun(HdrParameterParser *pHdr, DecOutputInfo outputInfo);
    void HdrParseHeaderSeqInit(HdrParameterParser *pHdr, DecInitialInfo initialInfo);

    void HdrInformSequenceChange(HdrParameterParser *pHdr);

    void HdrGetMasteringDisplay(HdrParameterParser *pHdr, private_rtk_v1_data *p_state);
    void HdrGetVui(HdrParameterParser *pHdr, private_rtk_v1_data *p_state);

    void HdrGetLumaChromaOffsetTableAddr(HdrParameterParser *pHdr, int myIndex, int width, int height, unsigned int *pLumaOffTblAddr, unsigned int *pChromeOffTblAddr);