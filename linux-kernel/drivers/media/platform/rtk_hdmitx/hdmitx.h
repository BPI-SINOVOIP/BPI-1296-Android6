/*
 * hdmitx.h - RTK hdmitx driver header file
 *
 * Copyright (C) 2017 Realtek Semiconductor Corporation
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */

#ifndef _HDMITX_H_
#define _HDMITX_H_

#include <drm/drm_edid.h>
#include <drm/drm_crtc.h>
#include <linux/types.h>
#include <linux/workqueue.h>

#include "hdmitx_rpc.h"

#define __RTK_HDMI_GENERIC_DEBUG__  0
#define HDMI_RX_SENSE_SUPPORT 0

#if __RTK_HDMI_GENERIC_DEBUG__
#define HDMI_DEBUG(format, ...) printk("[HDMITx_DBG] " format "\n", ## __VA_ARGS__)
#else
#define HDMI_DEBUG(format, ...) 
#endif

#define HDMI_ERROR(format, ...) printk(KERN_ERR "[HDMITx_ERR] " format "\n", ## __VA_ARGS__)
#define HDMI_INFO(format, ...) printk(KERN_WARNING "[HDMITx] " format "\n", ## __VA_ARGS__)

#define wr_reg(x,y)                     writel(y,(volatile unsigned int*)(x))
#define rd_reg(x)                       readl((volatile unsigned int*)(x))

#define field_get(val, start, end) 		(((val) & field_mask(start, end)) >> (end))
#define field_mask(start, end)    		(((1 << (start - end + 1)) - 1) << (end))
#define setbits(base,offset, Mask) 		wr_reg( (base+offset), ( rd_reg(base+offset) | Mask))
#define readbits(value, Mask) 			((value >> (Mask)) & 0x1)
#define clearbits(base,offset, Mask)	wr_reg((base+offset),(rd_reg(base+offset) & ~(Mask)))


#define WR_REG_32(base, offset, val)    writel(val,(volatile unsigned int*)(base + offset))
#define RD_REG_32(base, offset)         readl((volatile unsigned int*)(base + offset))

#define FLD_GET(val, start, end) (((val) & FLD_MASK(start, end)) >> (end))
#define FLD_MASK(start, end)    (((1 << (start - end + 1)) - 1) << (end))
#define SETBITS(base,offset, Mask) WR_REG_32(base, offset, ( RD_REG_32(base,offset) | Mask))
#define READBITS(value, Mask) ((value >> (Mask)) & 0x1)
#define CLEARBITS(base,offset, Mask) WR_REG_32(base, offset,((RD_REG_32(base,offset) & ~(Mask))))
#define MASK_REG_32(base,offset, andMask, orMask) WR_REG_32(base,offset, ((RD_REG_32(base,offset) & (andMask)) | (orMask)))


#define MAX_ELD_BYTES	128

enum {
	Vid_1920x1080p_60Hz =16,
    Vid_1920x1080p_50Hz =31,
};

enum HDMI_MODE {
	HDMI_MODE_UNDEF= 0,
	HDMI_MODE_HDMI= 1,
	HDMI_MODE_DVI= 2,
    HDMI_MODE_MHL= 3, 
	HDMI_MODE_MAX
};

enum TMDS_MODE {
	TMDS_MODE_UNKNOW = 0,
	TMDS_HDMI_DISABLED = 1,
	TMDS_HDMI_ENABLED = 2,
	TMDS_MHL_ENABLED = 3,
	TMDS_MODE_MAX
};

enum HDMI_EXTENDED_MODE {
	EXTENDED_MODE_RESERVED = 0,
	EXTENDED_MODE_3840_2160_30HZ = 1,
	EXTENDED_MODE_3840_2160_25HZ = 2,
	EXTENDED_MODE_3840_2160_24HZ = 3,
	EXTENDED_MODE_4096_2160_24HZ = 4,
	EXTENDED_MODE_MAX
};

#define RTK_EDID_DIGITAL_DEPTH_6       (1 << 0)
#define RTK_EDID_DIGITAL_DEPTH_8       (1 << 1)
#define RTK_EDID_DIGITAL_DEPTH_10      (1 << 2)
#define RTK_EDID_DIGITAL_DEPTH_12      (1 << 3)
#define RTK_EDID_DIGITAL_DEPTH_14      (1 << 4)
#define RTK_EDID_DIGITAL_DEPTH_16      (1 << 5)

struct raw_edid {unsigned char edid_data[256];};

/**
 * struct Audio_desc - Audio descriptor for audio format
 * @coding_type: Audio format code
 * @channel_count: Max number of channels
 * @sample_freq_all: Bit6[192KHz]/[176.4KHz]/[96KHz]/[88.2KHz]/[48KHz]/[44.1KHz]/Bit0[32KHz]
 * @sample_size_all: For Audio Format Code 1(L-PCM)
 * @max_bit_rate_divided_by_8KHz: For Audio Format Code 2 to 8
 */
struct Audio_desc
{
	unsigned char	coding_type;
	unsigned char	channel_count;
	unsigned char	sample_freq_all;
	unsigned char	sample_size_all;
	unsigned char	max_bit_rate_divided_by_8KHz;
}__attribute__ ((packed));

struct Audio_Data
{
	/*  Audio Data Block */
	char ADB_length;
	struct Audio_desc ADB[10];
	/* Speaker Allocation Data Block */
	char SADB_length;
	unsigned char SADB[3];	
}__attribute__ ((packed));

struct Specific_3D
{
	unsigned char vic;
	unsigned char format;// 0:Frame packing, 6:Top-and-Bottom, 8:Side-by-Side(Half)
}__attribute__ ((packed));

struct Video_Display_Info {
	/* Physical size */
	unsigned int width_mm;
	unsigned int height_mm;
	unsigned int pixel_clock;
	unsigned int bpc;
	u32 color_formats;
	u8 cea_rev;
}__attribute__ ((packed));

struct sink_capabilities_t {
	
	unsigned int hdmi_mode;
	//basic
	unsigned int est_modes;
	
	//audio
	unsigned char eld[MAX_ELD_BYTES];
	struct Audio_Data audio_data;
	
	//Vendor-Specific Data Block(VSDB)
	unsigned char cec_phy_addr[2];
	bool support_AI;  // needs info from ACP or ISRC packets
	bool DC_Y444;	// 4:4:4 in deep color modes
	unsigned char color_space;  
	bool dvi_dual;	//DVI Dual Link Operation
	int max_tmds_clock;	/* in MHz */
	bool latency_present[2];
	unsigned char video_latency[2];	/* [0]: progressive, [1]: interlaced */
	unsigned char audio_latency[2];
	
	bool _3D_present;
	__u16 structure_all;
	unsigned char _3D_vic[16];
	struct Specific_3D spec_3d[18];

	//video
	struct Video_Display_Info display_info;
	__u64 vic;//VIC 1~64, BIT0=VIC1
	unsigned char extended_vic;
	__u64 vic2;//VIC 65~128, BIT0=VIC65
	__u64 vic2_420;//YCbCr420 format, VIC 65~128, BIT0=VIC65
	struct VIDEO_RPC_VOUT_EDID_DATA vout_edid_data;
}__attribute__ ((packed));


typedef struct{
	struct sink_capabilities_t sink_cap;
	bool sink_cap_available;
	unsigned char *edid_ptr;	
}asoc_hdmi_t;


struct hdmitx_switch_data{
	int	state;
#if HDMI_RX_SENSE_SUPPORT
	int hpd_state;
#endif
	unsigned int irq;
	int pin;
	struct work_struct work;	
};

struct ext_edid{
	int	extension;	
	int current_blk;
	unsigned char data[2*EDID_LENGTH];
};

#define HDMI_IOCTL_MAGIC 0xf1
#define HDMI_CHECK_LINK_STATUS			_IOR (HDMI_IOCTL_MAGIC,2,  int)
#define HDMI_CHECK_Rx_Sense				_IOR (HDMI_IOCTL_MAGIC,11, int)
#define HDMI_GET_EXT_BLK_COUNT			_IOR (HDMI_IOCTL_MAGIC,12, int)
#define HDMI_GET_EXTENDED_EDID			_IOWR(HDMI_IOCTL_MAGIC,13, struct ext_edid)
#define HDMI_QUERY_DISPLAY_STANDARD		_IOR (HDMI_IOCTL_MAGIC,14, int)
#define HDMI_SEND_VOUT_EDID_DATA		_IOWR(HDMI_IOCTL_MAGIC,15, struct VIDEO_RPC_VOUT_EDID_DATA)


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

enum HDMI_ERROR_CODE {
	HDMI_ERROR_RESERVED = 0,
	HDMI_ERROR_NO_MEMORY = 1,
	HDMI_ERROR_I2C_ERROR = 2,
	HDMI_ERROR_HPD = 3,
	HDMI_ERROR_INVALID_EDID = 4,
	//HDMI_ERROR_CBUS_DDC_ERROR = 5,
	HDMI_ERROR_MAX
};

enum HDMI_RX_SENSE_STATUS {
	HDMI_RX_SENSE_OFF = 0,
	HDMI_RX_SENSE_ON = 1,
	HDMI_RX_SENSE_MAX
};

#define USE_ION_AUDIO_HEAP


/* hdmitx_info */
ssize_t show_hdmitx_info(struct device *cd, struct device_attribute *attr, char *buf);
ssize_t show_edid_info(struct device *cd, struct device_attribute *attr, char *buf);
void register_hdmitx_sysfs(struct device *dev);

/* rtk_mute_gpio */
void set_mute_gpio_pulse(void);
void setup_mute_gpio(struct device_node *dev);


#endif /* _HDMITX_H_ */
