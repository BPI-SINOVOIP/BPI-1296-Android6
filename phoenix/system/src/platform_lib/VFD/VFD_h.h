/*
        VFD High Level API

		These Functions are provided in platform independent form
		to provide AP developers to control the front panel without
		taking care of hardware implementation

*/
#include <StreamClass/EType.h>                                                                               
#include <Filters/NavigationFilter/NavDef.h>
#include <Util.h>
#include <Platform_Lib/FrontControl/VFD_l.h>
#include <time.h>
#ifndef __VFD_H_H__
#define __VFD_H_H__
#define MAX_STRING 40

#ifdef ELN_1262_VFD_DEBUG	/* 08-05-10 add enum to display browsing type and device and video system */
//#include "Application/AppClass/setupdef.h"
typedef enum {
    MUSIC,
    PHOTO,
    MOVIE,
#if AP_CONFIG_WITH(APCFG_BROWSER_LIST_ALL)
    ALL,
    BT_NONE
#endif	
} BrowserTypes;

typedef enum 
{ 
    INVALID,
    USB, 
    HDD,
    DVD,
    NET,
    CARD,
    RECYCLE,
    DEVICE_NUMBER,
} PLUGIN_DEVICES;

typedef enum video_sys {
	VIDEO_SYS_NTSC,
	VIDEO_SYS_PAL,
	VIDEO_SYS_HD720_50HZ,  // 720 only have progressive
	VIDEO_SYS_HD720_60HZ,
	VIDEO_SYS_HD1080_50HZ, // 1080 can have interleaved or progressive
	VIDEO_SYS_HD1080_60HZ,
	VIDEO_SYS_SVGA800x600,
	VIDEO_SYS_AUTO,        // for REC_TV_SYSTEM only
	VIDEO_SYS_NUM,
} ENUM_VIDEO_SYS;

typedef enum video_std {
	VIDEO_STD_INTERLACED,
	VIDEO_STD_PROGRESSIVE,
} ENUM_VIDEO_STD;
#endif
typedef enum _tags_playing_source {
	SOURCE_HDD,
	SOURCE_DVD,
	SOURCE_NETWORK,
	SOURCE_USB,
	SOURCE_1394,
} PLAYING_SOURCE;

// C++ Interface Here
class AbstractAP;

class VenusVFD {
private:
	VenusVFD();
	~VenusVFD();

	static VenusVFD *ptr;

public:
	static bool LED(unsigned char bits);
#ifdef ELN_1262_VFD_DEBUG	/* 08-05-10 add function to display browsing type and device and video system */
	static bool BROWSER_TYPE(BrowserTypes type);
	static bool BROWSER_DEVICE(PLUGIN_DEVICES device);
	static bool VIDEO_RESOLUTION(ENUM_VIDEO_SYS video, ENUM_VIDEO_STD std);
	static bool DISPLAY_HDMI(char fgHDMIOn);
#endif
	static bool AUDIO_FORMAT(NAVAUDIOATTR attr);
	static bool MEDIA_TYPE(DATA_MEDIA_TYPE type, PLAYING_SOURCE source = SOURCE_DVD);
	static bool show_msg(char *msg);
#ifdef ELN_1262_VFD_DEBUG	/* 08-05-10 add functions of showing all GRID &  displaying time */
	static bool show_all(void);
	static bool show_num(char *msg);
#endif
	static bool PlaybackStatus(NAVPLAYBACKSTATUS);
	static bool RecordStatus(int hour, int minute, int second, int frame, FILTER_STATE state);
	static bool showTime(time_t timestamp);
	static bool RepeatMode(NAV_REPEAT_MODE mode);
	static void clearVFD(void);
	static void sClearVFD(void);
#ifdef ELN_1262_VFD_DEBUG	/* 08-05-10 add functions that clear only strings(1G~7G & except P16) */
	static void sClearVFDStrings(void);
	static void sClearVFDTime(void);	// add function what clear time digit only
#endif
	static bool DVPlayStatus(int hour, int minute, int second, int frame);
	static bool setDimmingQuality(unsigned char pulseWidth) {
		if(ptr == NULL) {
			ptr = new VenusVFD;
			controller->InitDisplayRAM();
		}
		controller->setDimmingQuality(pulseWidth);
		
		return true;
	}
    static bool playSign(bool playing);
    static bool PlaybackTime(int chapter, int second);
	static bool IsUpdateComplete(void) {
		if(ptr == NULL) {
			ptr = new VenusVFD;
			controller->InitDisplayRAM();
		}
		return controller->IsUpdateComplete();
	}

	friend class AbstractAP;

public:
	static VFD_Controller *controller;

	static char *STATUS_message;
	static char *FORMAT_message;
	static char *MEDIA_message;
	static char *TV_message;
	static unsigned char LED_BITS;
	static void deleteInstance(void);

};

#endif
