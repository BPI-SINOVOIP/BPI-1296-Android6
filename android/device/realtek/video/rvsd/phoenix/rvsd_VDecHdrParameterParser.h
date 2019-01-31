
#include "rvsd_maincore.h"
#include "rvsd_VDecVPU.h"
#include "rvsd_dbg.h"
#include "rvsd_BitReader.h"

#undef LOG_TAG
#define LOG_TAG "HdrParameterParser"


struct Ts103433Info
{
    int ts103433_spec_version;
    int ts103433_payload_mode;
};

struct ColourVolumeReconstructionInfo
{
    int cv_rec_id;
    bool cv_rec_cancel_flag;
    bool cv_rec_persistence_flag;
    bool cv_rec_target_info_flag;

    int cv_rec_primaries;
    int cv_rec_matrix_coefficients;

    int tone_mapping_input_signal_black_level_offset;
    int tone_mapping_input_signal_white_level_offset;

    int shadow_gain_control;
    int highlight_gain_control;
    int mid_tone_width_adjustment_factor;
    int tone_mapping_output_fine_tuning_num_val;
    int tone_mapping_output_fine_tuning_x[10];
    int tone_mapping_output_fine_tuning_y[10];

    int saturation_gain_num_val;
    int saturation_gain_x[6];
    int saturation_gain_y[6];
};

class HdrParameterParser
{
public:
    HdrParameterParser();
    ~HdrParameterParser();

    void Allocate(_DecConfigParam *p_vpu);
    void Release();

    void ParseHeaderPicRun();
    void ParseHeaderSeqInit();

    void InformSequenceChange();

    void GetMasteringDisplay(YUV_STATE *p_state) const;
    void GetVui(YUV_STATE *p_state) const;

#if defined(RVSD_SUPPORT_TECHNICOLOR)
    void GetTechnicolorData(YUV_STATE *p_state) const;
#endif

private:
    void Reset();
    bool IsCodecValid(_DecConfigParam *p_vpu) const;
    void Parse(Uint32 header);

    void ParseT35UserData(unsigned char *p_payload, int payload_size);
    void ParseTs103433Info(BitReader &reader);
    void ParseCvri(BitReader &reader);

    void PrintCvri() const;
    void PrintTs1034433Info() const;
    void PrintMdcv() const;
    void PrintVui() const;
    void PrintColorRemappingInfo() const;

#if defined(RVSD_SUPPORT_TECHNICOLOR)
    void PrintTechniColorData(YUV_STATE *p_state) const;

    void GetCvri(YUV_STATE *p_state) const;
    void GetColorRemappingInfo(YUV_STATE *p_state) const;
#endif

    static const int m_user_data_core = 1;
    static const size_t m_user_data_size = 512 * 1024;

    h265_mastering_display_colour_volume_t m_mastering;
    h265_vui_param_t m_vui;
    h265_colour_remapping_info_t m_remapping;

    Ts103433Info m_ts103433_info;
    ColourVolumeReconstructionInfo m_cvri;

    vpu_buffer_t m_vpu_buffer;
    Uint8 *mp_work_buffer;

    _DecConfigParam *mp_vpu;

    int m_query_type;
};

