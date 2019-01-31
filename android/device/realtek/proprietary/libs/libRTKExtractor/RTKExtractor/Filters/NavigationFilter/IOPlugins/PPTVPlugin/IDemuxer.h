// IDemuxer.h

#ifndef _PPBOX_PPBOX_I_DEMUXER_H_
#define _PPBOX_PPBOX_I_DEMUXER_H_

#include "IPpbox.h"

#if __cplusplus
extern "C" {
#endif // __cplusplus

    //��һ����Ƶ
    PPBOX_DECL PP_int32 PPBOX_Open(
        PP_char const * playlink);

    typedef void (*PPBOX_Open_Callback)(PP_int32);

    PPBOX_DECL void PPBOX_AsyncOpen(
        PP_char const * playlink, 
        PPBOX_Open_Callback callback);

    //��ͣ
    PPBOX_DECL PP_int32 PPBOX_Pause();

    //ǿ�ƽ���
    PPBOX_DECL void PPBOX_Close();

    //����ж�����
    PPBOX_DECL PP_uint32 PPBOX_GetStreamCount();

    enum PPBOX_StreamTypeEnum
    {
        ppbox_video = 1, 
        ppbox_audio = 2, 
    };

    enum PPBOX_StreamSubTypeEnum
    {
        ppbox_video_avc = 1, 
        ppbox_audio_aac = 2, 
        ppbox_audio_mp3 = 3, 
        ppbox_audio_wma = 4,
    };

    enum PPBOX_StreamFormatTypeEnum
    {
        ppbox_video_avc_packet = 1, 
        ppbox_video_avc_byte_stream = 2, 
        ppbox_audio_microsoft_wave = 3, 
        ppbox_audio_iso_mp4 = 4, 
    };

    typedef struct tag_PPBOX_StreamInfo
    {
        PP_int32 type;          // TypeEnum
        PP_int32 sub_type;      // SubTypeEnum
        PP_int32 format_type;   // ��ʽ˵��������
        PP_uint32 format_size;  // ��ʽ˵���Ĵ�С
        PP_uchar const * format_buffer;     // ��ͬ�Ľ��벻ͬ�Ľṹ�壬��ʽ˵���ĵ�����
    } PPBOX_StreamInfo;

    //���������ϸ��Ϣ
    PPBOX_DECL PP_int32 PPBOX_GetStreamInfo(
        PP_uint32 index, 
        PPBOX_StreamInfo * info);

    typedef struct tag_PPBOX_VideoInfo
    {
        PP_uint32 width;
        PP_uint32 height;
        PP_uint32 frame_rate;
    } PPBOX_VideoInfo;

    typedef struct tag_PPBOX_AudioInfo
    {
        PP_uint32 channel_count;
        PP_uint32 sample_size;
        PP_uint32 sample_rate;
    } PPBOX_AudioInfo;

    typedef struct tag_PPBOX_StreamInfoEx
    {
        PP_int32 type;          // TypeEnum
        PP_int32 sub_type;      // SubTypeEnum
        union 
        {
            PPBOX_VideoInfo video_format;
            PPBOX_AudioInfo audio_format;
        };
        PP_int32 format_type;   // ��ʽ˵��������
        PP_uint32 format_size;  // ��ʽ˵���Ĵ�С
        PP_uchar const * format_buffer;   // ��ͬ�Ľ��벻ͬ�Ľṹ�壬��ʽ˵���ĵ�����
    } PPBOX_StreamInfoEx;

    //���������ϸ��չ��Ϣ
    PPBOX_DECL PP_int32 PPBOX_GetStreamInfoEx(
        PP_uint32 index,
        PPBOX_StreamInfoEx *info);

    //�����ʱ��
    PPBOX_DECL PP_uint32 PPBOX_GetDuration();

    //��ò��ų�������
    PPBOX_DECL PP_int32 PPBOX_GetWidthHeight(
        PP_uint32 * pwidth, 
        PP_uint32 * pheight);

    //����ĳ��ʱ�̿�ʼ����
    PPBOX_DECL PP_int32 PPBOX_Seek(
        PP_uint32 start_time);

    //���AVC��Ƶ�����AVCDecoderConfigurationRecord����
    PPBOX_DECL PP_int32 PPBOX_GetAvcConfig(
        PP_uchar const * * buffer, 
        PP_uint32 * length);

#ifdef PPBOX_DEMUX_RETURN_SEGMENT_INFO

    PPBOX_DECL PP_int32 PPBOX_GetFirstSegHeader(
        PP_uchar const * * buffer, 
        PP_uint32 * length);

    PPBOX_DECL PP_int32 PPBOX_GetSegHeader(
        PP_uint32 index, 
        PP_uchar const * * buffer, 
        PP_uint32 * length);

    PPBOX_DECL PP_uint32 PPBOX_GetSegDataSize(
        PP_uint32 index);

    typedef struct tag_PPBOX_SegmentInfo
    {
        PP_uint32 index;
        PP_uint32 duration;
        PP_uint32 duration_offset;
        PP_uint32 head_length;
        PP_uint64 file_length;
        PP_uchar const * head_buffer;
    } PPBOX_SegmentInfo;

    PPBOX_DECL PP_uint32 PPBOX_GetSegmentCount();

    PPBOX_DECL PP_int32 PPBOX_GetSegmentInfo(
        PPBOX_SegmentInfo * segment_info);

#endif

    typedef struct tag_PPBOX_Sample
    {
        PP_uint32 stream_index;     // ���ı��
        PP_uint32 start_time;       // Sample��Ӧ��ʱ���, ��λ�Ǻ���
        PP_uint32 buffer_length;    // Sample�Ĵ�С
        PP_uchar const * buffer;    // Sample������
        //�����ñ������ĺ����������ڴ�Ȼ�����Sample
        //��������һ��ReadSample������ʱ�򣬻���Stop��ʱ���ڴ��ͷŵ�
    } PPBOX_Sample;

    //ͬ����ȡSample�ӿڣ�������
    PPBOX_DECL PP_int32 PPBOX_ReadSample(
        PPBOX_Sample * sample);

    typedef struct tag_PPBOX_SampleEx
    {
        PP_uint32 stream_index;     // ���ı��
        PP_uint32 start_time;       // Sample��Ӧ��ʱ���, ��λ�Ǻ���
        PP_uint64 offset_in_file;
        PP_uint32 buffer_length;    // Sample�Ĵ�С
        PP_uint32 duration;
        PP_uint32 desc_index;
        PP_uint64 decode_time;
        PP_uint32 composite_time_delta;
        PP_bool is_sync;
        PP_uchar const * buffer;    // Sample������
        //�����ñ������ĺ����������ڴ�Ȼ�����Sample
        //��������һ��ReadSample������ʱ�򣬻���Stop��ʱ���ڴ��ͷŵ�
    } PPBOX_SampleEx;

    PPBOX_DECL PP_int32 PPBOX_ReadSampleEx(
        PPBOX_SampleEx * sample);

    typedef struct tag_PPBOX_SampleEx2
    {
        PP_uint32 stream_index;     // ���ı��
        PP_uint64 start_time;       // Sample��Ӧ��ʱ���, ��λ��΢��
        PP_uint32 buffer_length;    // Sample�Ĵ�С
        PP_uint32 duration;
        PP_uint32 desc_index;
        PP_uint64 decode_time;
        PP_uint32 composite_time_delta;
        PP_bool is_sync;
        PP_bool is_discontinuity;   // ��λ��ʶ��֡����һ֮֡���Ƿ���������sample; �����sample��seek���sample����ǰ�涪ʧ��sample,���λ��1������Ϊ0��
        PP_uchar const * buffer;    // Sample������
        //�����ñ������ĺ����������ڴ�Ȼ�����Sample
        //��������һ��ReadSample������ʱ�򣬻���Stop��ʱ���ڴ��ͷŵ�
    } PPBOX_SampleEx2;

    PPBOX_DECL PP_int32 PPBOX_ReadSampleEx2(
        PPBOX_SampleEx2 * sample);

    //�������ػ�������С ��ֻ����Openǰ���ã�
    // ��Ҫ���ڿ����ڴ棬��������ٶȴ���ReadSample���ٶȣ���ô���ص����ݽ����
    // ���ڴ�֮�У����ڴ��е����ػ���������Ԥ��ֵ����ô��ֹͣ���ء�ֱ����������
    // ReadSample������һЩ�ڴ�ռ�ú��ټ������أ�
    // length: Ԥ��������ڴ滺��Ĵ�С
    PPBOX_DECL void PPBOX_SetDownloadBufferSize(
        PP_uint32 length);

    PPBOX_DECL void PPBOX_SetHttpProxy(
        PP_char const * addr);

    PPBOX_DECL void PPBOX_SetDownloadMaxSpeed(
        PP_uint32 speed);

    //���ò��Ż������Ļ���ʱ�� (��ʱ���Ե���)
    // ��Ҫ���ڼ��㲥��״̬��������������������Ĭ��3s
    // ��� ���ػ��������ݵ���ʱ�� < ���Ż���ʱ�� �� ���� buffering ״̬
    // ��� ���ػ��������ݵ���ʱ�� >=���Ż���ʱ�� �� ���� playing ״̬
    // ��� ��Ϊ������ Pause ʹ֮��ͣ�ģ����� Pausing ״̬
    // �������buffering״̬�����ػ��������ݵ���ʱ��/���Ż���ʱ��*100%���ǻ���ٷֱ�
    PPBOX_DECL void PPBOX_SetPlayBufferTime(
        PP_uint32 time);

    enum PPBOX_PlayStatusEnum
    {
        ppbox_closed = 0, 
        ppbox_playing, 
        ppbox_buffering, 
        ppbox_paused, 
    };

    typedef struct tag_PPBOX_PlayStatistic
    {
        PP_uint32 length;           //���ṹ��ĳ���
        PP_int32 play_status;       //����״̬ 0-δ���� 1-playing̬ 2-buffering̬ 3-Pausing̬
        PP_uint32 buffering_present;//���Ż���ٷֱ� 10 ��ʾ 10%
        PP_uint32 buffer_time;      //���ػ��������ݵ���ʱ��
    } PPBOX_PlayStatistic;

    //��ò�����Ϣ
    //����ֵ: ������
    //    ppbox_success      ��ʾ�ɹ�
    //    ������ֵ��ʾʧ��
    PPBOX_DECL PP_int32 PPBOX_GetPlayMsg(
        PPBOX_PlayStatistic * statistic_Msg);

    typedef struct tag_PPBOX_DownloadMsg
    {
        PP_uint32 length;                   // ���ṹ��ĳ���
        PP_uint32 start_time;               // ��ʼʱ��
        PP_uint32 total_download_bytes;     // �ܹ����ص��ֽ���
        PP_uint32 total_upload_bytes;       // �ܹ��ϴ�ʱ�ֽ���
        PP_uint32 http_download_bytes;      // Http���ص��ֽ���
        PP_uint32 http_downloader_count;    // Http���ظ���
        PP_uint32 p2p_download_bytes;       // P2P���ص��ֽ���
        PP_uint32 p2p_upload_bytes;         // P2P�ϴ����ֽ���
        PP_uint32 p2p_downloader_count;     // P2P���ظ���
        PP_uint32 p2p_downloader_count_ext; // ��P2P���ص���Դ����
        PP_uint32 total_upload_cache_request_count; // �ܹ����ϴ�Cache������
        PP_uint32 total_upload_cache_hit_count;     // �ܹ����ϴ�Cache������
        PP_uint32 download_duration_in_sec; // �����ܹ�����ʱ��(��)
        PP_uint32 local_peer_version;       // �Լ��ں˰汾��
    } PPBOX_DownloadMsg;

    //���������Ϣ
    // download_Msg: ��������ӿں����ڻ�ȡ��������Ϣ����
    //����ֵ: ������
    //    ppbox_success      ��ʾ�ɹ�
    //    ������ֵ��ʾʧ��
    PPBOX_DECL PP_int32 PPBOX_GetDownMsg(
        PPBOX_DownloadMsg* download_Msg);

    typedef struct tag_PPBOX_DownloadSpeedMsg
    {
        PP_uint32 length;                   // ���ṹ��ĳ���
        PP_uint32 now_download_speed;       // ��ǰ�����ٶ� <5sͳ��>
        PP_uint32 now_upload_speed;         // ��ǰ�ϴ��ٶ� <5sͳ��>
        PP_uint32 minute_download_speed;    // ���һ����ƽ�������ٶ� <60sͳ��>
        PP_uint32 minute_upload_speed;      // ���һ����ƽ���ϴ��ٶ� <60sͳ��>
        PP_uint32 avg_download_speed;       // ��ʷƽ�������ٶ�
        PP_uint32 avg_upload_speed;         // ��ʷƽ���ϴ��ٶ�

        PP_uint32 recent_download_speed;    // ��ǰ�����ٶ� <20sͳ��>
        PP_uint32 recent_upload_speed;      // ��ǰ�ϴ��ٶ� <20sͳ��>
        PP_uint32 second_download_speed;    // ��ǰ1s�������ٶ�
        PP_uint32 second_upload_speed;      // ��ǰ1s���ϴ��ٶ�
    } PPBOX_DownloadSpeedMsg;

    //��������ٶ���Ϣ
    // download_Msg: ��������ӿں����ڻ�ȡ��������Ϣ����
    //����ֵ: ������
    //    ppbox_success      ��ʾ�ɹ�
    //    ������ֵ��ʾʧ��
    PPBOX_DECL PP_int32 PPBOX_GetDownSedMsg(
        PPBOX_DownloadSpeedMsg* download_spped_Msg);

#if __cplusplus
}
#endif // __cplusplus

#endif // _PPBOX_PPBOX_I_DEMUXER_H_
