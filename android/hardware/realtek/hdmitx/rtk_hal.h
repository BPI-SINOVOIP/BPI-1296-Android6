#ifndef __RTK_HAL_H__
#define __RTK_HAL_H__



#define HDMI_IOCTL_MAGIC 0xf1
#define HDMI_CHECK_LINK_STATUS			_IOR(HDMI_IOCTL_MAGIC,2, int)
#define HDMI_CHECK_Rx_Sense				_IOR(HDMI_IOCTL_MAGIC,11, int)
#ifdef KYLIN_TARGET_BOARD
#define HDMI_SEND_VOUT_EDID_DATA			_IOWR(HDMI_IOCTL_MAGIC,15, struct VIDEO_RPC_VOUT_EDID_DATA)
#endif

/* HDMI ioctl */
enum {
    HDMI_GET_SINK_CAPABILITY,
    HDMI_GET_RAW_EDID,
    HDMI_GET_LINK_STATUS,   
    HDMI_GET_VIDEO_CONFIG,       
    HDMI_SEND_AVMUTE,
    HDMI_CONFIG_TV_SYSTEM,
    HDMI_CONFIG_AVI_INFO,
    HDMI_SET_FREQUNCY,
    HDMI_SEND_AUDIO_MUTE,
    HDMI_SEND_AUDIO_VSDB_DATA,
    HDMI_SEND_AUDIO_EDID2,
	HDMI_CHECK_TMDS_SRC,
};


enum HDMI_AVMUTE{
	HDMI_CLRAVM =0,
	HDMI_SETAVM 
};

//synchronize with linux hdmitx driver.
//Reference of sink_capabilities_t.hdmi_mode
enum HDMI_MODE {
	HDMI_MODE_UNDEF= 0,
	HDMI_MODE_HDMI= 1,
	HDMI_MODE_DVI= 2,
    HDMI_MODE_MHL= 3, 
	HDMI_MODE_MAX
};



#endif
