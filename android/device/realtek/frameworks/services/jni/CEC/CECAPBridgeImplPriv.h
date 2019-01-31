#ifndef  __CEC_AP_BRIDGE_IMPL_PRIV_H__
#define __CEC_AP_BRIDGE_IMPL_PRIV_H__


#define PHYSICAL_ADDR_NULL 0xFFFF
#define DEV_REMOTE  "/dev/input/event0"
//-----------------------------------------------
// CEC Compile option
//-----------------------------------------------
#define DOUBLE_CHECK_HDMI_CONNECTION
#define POWER_ON_CHECK_HDMI_CONNECTION
#define RC_PASSTHROUGH_ENABLE
#define FORCE_MENU_ACTIVATE
#define DO_ONE_TOUCH_PLAY_WITHOUT_WAKEUP_TV

//-----------------------------------------------
// Device Information
//-----------------------------------------------
#define DEVICE_OSD_NAME "DMP"
#define DEVICE_VENDOR_ID (0x007F80)
#define STROKE "CONFIG_0"
//-----------------------------------------------
// Menu Language Mapping
//-----------------------------------------------

typedef struct map_menu_lang
{
    unsigned long ISO639;
    ENUM_OSD_LANGUAGE rtk_osd_language;
} MAP_MENU_LANG;

typedef struct map_auto_lang
{
    unsigned long ISO639;
    ENUM_OSD_LANGUAGE rtk_osd_language;
	char *Auto;
} MAP_AUTO_LANG;

MAP_MENU_LANG local_menu_map[]={
    {CEC_MENU_LANG_ENG,OSD_LANG_ENGLISH},
	{CEC_MENU_LANG_CHI,OSD_LANG_CHINESE},
    {CEC_MENU_LANG_JPN,OSD_LANG_JAPANESE},
    {CEC_MENU_LANG_SPA,OSD_LANG_SPANISH},
    {CEC_MENU_LANG_ESP,OSD_LANG_SPANISH},
    {CEC_MENU_LANG_FRA,OSD_LANG_FRENCH},
    {CEC_MENU_LANG_FRE,OSD_LANG_FRENCH},
    {CEC_MENU_LANG_DEU,OSD_LANG_GERMAN},
    {CEC_MENU_LANG_GER,OSD_LANG_GERMAN},
    {CEC_MENU_LANG_ITA,OSD_LANG_ITALIAN},
    {CEC_MENU_LANG_KOR,OSD_LANG_KOREAN},
    {CEC_MENU_LANG_DUT,OSD_LANG_DUTCH},
    {CEC_MENU_LANG_NLD,OSD_LANG_DUTCH},
    {CEC_MENU_LANG_RUS,OSD_LANG_RUSSIAN},
	{CEC_MENU_LANG_ZHO,OSD_LANG_SCHINESE},
    {CEC_MENU_LANG_POR,OSD_LANG_PORTUGUESE},
    {CEC_MENU_LANG_SUN,OSD_LANG_SUDANESE},//maybe wrong
    {CEC_MENU_LANG_SVE,OSD_LANG_SWEDISH},//maybe wrong
   	{CEC_MENU_LANG_SWE,OSD_LANG_SWEDISH},	
    {CEC_MENU_LANG_FIN,OSD_LANG_FINNISH},
    {CEC_MENU_LANG_POL,OSD_LANG_POLISH},
    {CEC_MENU_LANG_TUR,OSD_LANG_TURKMEN},
    {CEC_MENU_LANG_DAN,OSD_LANG_DANISH},
    {CEC_MENU_LANG_GRE,OSD_LANG_GRECCE},
    {CEC_MENU_LANG_CZE,OSD_LANG_CZECH},
    {CEC_MENU_LANG_SLO,OSD_LANG_SLOVAK},
    {CEC_MENU_LANG_HUN,OSD_LANG_HUNGARIAN},
    {CEC_MENU_LANG_RON,OSD_LANG_ROMANIA},
    {CEC_MENU_LANG_FRA,OSD_LANG_CANADIAN_FRENCH},
    {CEC_MENU_LANG_POR,OSD_LANG_BRA_POR},
  	{CEC_MENU_LANG_HIN,OSD_LANG_BEHARASAMELAYU},
    {CEC_MENU_LANG_THA,OSD_LANG_THAI},
    {CEC_MENU_LANG_NOR,OSD_LANG_NORWEGIAN},
    {CEC_MENU_LANG_HIN,OSD_LANG_HINDI},
	{CEC_MENU_LANG_MAY,OSD_LANG_BEHARASAMELAYU}, 
	{CEC_MENU_LANG_MSA,OSD_LANG_BEHARASAMELAYU},
    {CEC_MENU_LANG_UKR,OSD_LANG_UKRAINE}, 
};

MAP_AUTO_LANG local_auto_map_0[]={  // for Area 0,1,2
	{CEC_MENU_LANG_ENG,OSD_LANG_ENGLISH,(char *)"Auto(ENG)"},
	{CEC_MENU_LANG_FRE,OSD_LANG_FRENCH,(char *)"Auto(FRE)"},
	{CEC_MENU_LANG_FRA,OSD_LANG_FRENCH,(char *)"Auto(FRE)"},
	{CEC_MENU_LANG_GER,OSD_LANG_GERMAN,(char *)"Auto(GER)"},
	{CEC_MENU_LANG_DEU,OSD_LANG_GERMAN,(char *)"Auto(GER)"},
	{CEC_MENU_LANG_ITA,OSD_LANG_ITALIAN,(char *)"Auto(ITA)"},
	{CEC_MENU_LANG_SPA,OSD_LANG_SPANISH,(char *)"Auto(SPA)"},
	{CEC_MENU_LANG_ESP,OSD_LANG_SPANISH,(char *)"Auto(ESP)"},
	{CEC_MENU_LANG_POL,OSD_LANG_POLISH,(char *)"Auto(POL)"},
	{CEC_MENU_LANG_DUT,OSD_LANG_DUTCH,(char *)"Auto(DUT)"},//que dum
	{CEC_MENU_LANG_NLD,OSD_LANG_DUTCH,(char *)"Auto(DUT)"},
	{CEC_MENU_LANG_DAN,OSD_LANG_DANISH,(char *)"Auto(DAN)"},
	{CEC_MENU_LANG_NOR,OSD_LANG_NORWEGIAN,(char *)"Auto(NOR)"},
	{CEC_MENU_LANG_SVE,OSD_LANG_SWEDISH,(char *)"Auto(SVE)"},		
	{CEC_MENU_LANG_SWE,OSD_LANG_SWEDISH,(char *)"Auto(SVE)"},		
	{CEC_MENU_LANG_FIN,OSD_LANG_FINNISH,(char *)"Auto(FIN)"},
	{CEC_MENU_LANG_TUR,OSD_LANG_TURKMEN,(char *)"Auto(TUR)"},
	{CEC_MENU_LANG_GRE,OSD_LANG_GRECCE,(char *)"Auto(GRE)"},
	{CEC_MENU_LANG_CZE,OSD_LANG_CZECH,(char *)"Auto(CZE)"},
	{CEC_MENU_LANG_SLO,OSD_LANG_SLOVAK,(char *)"Auto(SLO)"},
	{CEC_MENU_LANG_HUN,OSD_LANG_HUNGARIAN,(char *)"Auto(HUN)"},
	{CEC_MENU_LANG_RON,OSD_LANG_ROMANIA,(char *)"Auto(RON)"},
	{CEC_MENU_LANG_POR,OSD_LANG_PORTUGUESE,(char *)"Auto(POR)"},								
	{CEC_MENU_LANG_RUS,OSD_LANG_RUSSIAN,(char *)"Auto(RUS)"},
    {CEC_MENU_LANG_UKR,OSD_LANG_UKRAINE,(char *)"Auto(UKR)"},
};

MAP_AUTO_LANG local_auto_map_1[]={		 // for Area 3		
	{CEC_MENU_LANG_ENG,OSD_LANG_ENGLISH,(char *)"Auto(ENG)"},
	{CEC_MENU_LANG_FRE,OSD_LANG_CANADIAN_FRENCH,(char *)"Auto(FRE)"},
	{CEC_MENU_LANG_SPA,OSD_LANG_MEX_SPANISH,(char *)"Auto(SPA)"},
	{CEC_MENU_LANG_ESP,OSD_LANG_MEX_SPANISH,(char *)"Auto(ESP)"},
};

MAP_AUTO_LANG local_auto_map_2[]={   // for Area 4	
	{CEC_MENU_LANG_ENG,OSD_LANG_ENGLISH,(char *)"Auto(ENG)"},
	{CEC_MENU_LANG_POR,OSD_LANG_BRA_POR,(char *)"Auto(POR)"},
	{CEC_MENU_LANG_SPA,OSD_LANG_MEX_SPANISH,(char *)"Auto(SPA)"},
	{CEC_MENU_LANG_ESP,OSD_LANG_MEX_SPANISH,(char *)"Auto(ESP)"},
};
			
MAP_AUTO_LANG local_auto_map_3[]={   // for Area 5	
	{CEC_MENU_LANG_ENG,OSD_LANG_ENGLISH,(char *)"Auto(ENG)"},
};

MAP_AUTO_LANG local_auto_map_4[]={  // for Area 6,9
	{CEC_MENU_LANG_ENG,OSD_LANG_ENGLISH,(char *)"Auto(ENG)"},
	{CEC_MENU_LANG_CHI,OSD_LANG_SCHINESE,(char *)"Auto(CHI)"},
	{CEC_MENU_LANG_ZHO,OSD_LANG_SCHINESE,(char *)"Auto(ZHO)"},		
};

MAP_AUTO_LANG local_auto_map_5[]={		 // for Area 7
	{CEC_MENU_LANG_ENG,OSD_LANG_ENGLISH,(char *)"Auto(ENG)"},
	{CEC_MENU_LANG_CHI,OSD_LANG_CHINESE,(char *)"Auto(CHI)"},
	{CEC_MENU_LANG_ZHO,OSD_LANG_CHINESE,(char *)"Auto(ZHO)"},
};

MAP_AUTO_LANG local_auto_map_6[]={	// for Area 8	
	{CEC_MENU_LANG_ENG,OSD_LANG_ENGLISH,(char *)"Auto(ENG)"},
	{CEC_MENU_LANG_CHI,OSD_LANG_CHINESE,(char *)"Auto(CHI)"},
	{CEC_MENU_LANG_THA,OSD_LANG_THAI,(char *)"Auto(THA)"}, 
	{CEC_MENU_LANG_MAY,OSD_LANG_BEHARASAMELAYU, (char *)"Auto(MAY)"}, 
	{CEC_MENU_LANG_MSA,OSD_LANG_BEHARASAMELAYU, (char *)"Auto(MSA)"}, 
	{CEC_MENU_LANG_KOR,OSD_LANG_KOREAN,(char *)"Auto(KOR)"},
	{CEC_MENU_LANG_HIN,OSD_LANG_HINDI,(char *)"Auto(HIN)"}, 
	{CEC_MENU_LANG_ZHO,OSD_LANG_SCHINESE,(char *)"Auto(ZHO)"},
};

MAP_AUTO_LANG local_auto_map_7[]={  // for Area 9 (All)
	{CEC_MENU_LANG_ENG,OSD_LANG_ENGLISH,(char *)"Auto(ENG)"},
	{CEC_MENU_LANG_FRE,OSD_LANG_FRENCH,(char *)"Auto(FRE)"},
	{CEC_MENU_LANG_FRA,OSD_LANG_FRENCH,(char *)"Auto(FRE)"},
	{CEC_MENU_LANG_GER,OSD_LANG_GERMAN,(char *)"Auto(GER)"},
	{CEC_MENU_LANG_DEU,OSD_LANG_GERMAN,(char *)"Auto(GER)"},
	{CEC_MENU_LANG_ITA,OSD_LANG_ITALIAN,(char *)"Auto(ITA)"},
	{CEC_MENU_LANG_SPA,OSD_LANG_SPANISH,(char *)"Auto(SPA)"},
	{CEC_MENU_LANG_ESP,OSD_LANG_SPANISH,(char *)"Auto(ESP)"},
	{CEC_MENU_LANG_POL,OSD_LANG_POLISH,(char *)"Auto(POL)"},
	{CEC_MENU_LANG_DUT,OSD_LANG_DUTCH,(char *)"Auto(DUT)"},//que dum
	{CEC_MENU_LANG_NLD,OSD_LANG_DUTCH,(char *)"Auto(DUT)"},
	{CEC_MENU_LANG_DAN,OSD_LANG_DANISH,(char *)"Auto(DAN)"},
	{CEC_MENU_LANG_NOR,OSD_LANG_NORWEGIAN,(char *)"Auto(NOR)"},
	{CEC_MENU_LANG_SVE,OSD_LANG_SWEDISH,(char *)"Auto(SVE)"},		
	{CEC_MENU_LANG_SWE,OSD_LANG_SWEDISH,(char *)"Auto(SVE)"},		
	{CEC_MENU_LANG_FIN,OSD_LANG_FINNISH,(char *)"Auto(FIN)"},
	{CEC_MENU_LANG_TUR,OSD_LANG_TURKMEN,(char *)"Auto(TUR)"},
	{CEC_MENU_LANG_GRE,OSD_LANG_GRECCE,(char *)"Auto(GRE)"},
	{CEC_MENU_LANG_CZE,OSD_LANG_CZECH,(char *)"Auto(CZE)"},
	{CEC_MENU_LANG_ZHO,OSD_LANG_SCHINESE,(char *)"Auto(ZHO)"},
	{CEC_MENU_LANG_MAY,OSD_LANG_BEHARASAMELAYU, (char *)"Auto(MAY)"}, 
	{CEC_MENU_LANG_MSA,OSD_LANG_BEHARASAMELAYU, (char *)"Auto(MSA)"}, 
	{CEC_MENU_LANG_THA,OSD_LANG_THAI,(char *)"Auto(THA)"}, 	
	{CEC_MENU_LANG_KOR,OSD_LANG_KOREAN,(char *)"Auto(KOR)"},
	{CEC_MENU_LANG_CHI,OSD_LANG_CHINESE,(char *)"Auto(CHI)"},
	{CEC_MENU_LANG_HIN,OSD_LANG_HINDI,(char *)"Auto(HIN)"}, 
	{CEC_MENU_LANG_POR,OSD_LANG_PORTUGUESE,(char *)"Auto(POR)"},	
	{CEC_MENU_LANG_FRE,OSD_LANG_CANADIAN_FRENCH,(char *)"Auto(FRE)"},						
	{CEC_MENU_LANG_RUS,OSD_LANG_RUSSIAN,(char *)"Auto(RUS)"},
    {CEC_MENU_LANG_UKR,OSD_LANG_UKRAINE,(char *)"Auto(UKR)"},
};


//-----------------------------------------------
// User Control Mapping
//-----------------------------------------------
typedef enum {
    // Navigation control
    KEYCODE_DPAD_UP     = 103,
    KEYCODE_DPAD_DOWN   = 108,
    KEYCODE_DPAD_LEFT   = 105,
    KEYCODE_DPAD_RIGHT  = 106,
    KEYCODE_ENTER       = 352,
    KEYCODE_HOME        = 102,
    KEYCODE_MENU        = 357,
    KEYCODE_BACK        = 158,

    // Volume control
    KEYCODE_VOLUME_UP   = 115,
    KEYCODE_VOLUME_DOWN = 114,
    KEYCODE_VOLUME_MUTE = 113,

    // Playback control
    KEYCODE_PLAY        = 207, 
    KEYCODE_PAUSE       = 207,
    KEYCODE_STOP        = 128,
    KEYCODE_FFWD        = 208,
    KEYCODE_FRWD        = 168,
    KEYCODE_NEXT        = 407,
    KEYCODE_PREV        = 412,

    // Number key
    KEYCODE_ESC         = 1,
    KEYCODE_1           = 2,
    KEYCODE_2           = 3,
    KEYCODE_3           = 4,
    KEYCODE_4           = 5,
    KEYCODE_5           = 6,
    KEYCODE_6           = 7,
    KEYCODE_7           = 8,
    KEYCODE_8           = 9,
    KEYCODE_9           = 10,
    KEYCODE_0           = 11,

    // Color key: not implemented so far
    KEYCODE_OPTION_BLUE    = -1,
    KEYCODE_OPTION_RED     = -1,
    KEYCODE_OPTION_GREEN   = -1,
    KEYCODE_OPTION_YELLOW  = -1,
    KEYCODE_NONE        = -1,

}RTK_Android_Key;
typedef struct map_rtkcmd_ceccode
{
    RTK_Android_Key Key;
    CEC_USER_CTRL_CODE CecUserCode;
} MAP_RTKCMD_CECCODE;

MAP_RTKCMD_CECCODE local_cec2Android_cmdmap[]={
    {KEYCODE_ENTER       ,CEC_USR_CTRL_SELECT},
	{KEYCODE_DPAD_UP          ,CEC_USR_CTRL_UP},
	{KEYCODE_DPAD_DOWN          ,CEC_USR_CTRL_DOWN},
	{KEYCODE_DPAD_LEFT          ,CEC_USR_CTRL_LEFT},
	{KEYCODE_DPAD_RIGHT          ,CEC_USR_CTRL_RIGHT},
	{KEYCODE_DPAD_RIGHT          ,CEC_USR_CTRL_RIGHT_UP},
	{KEYCODE_DPAD_RIGHT          ,CEC_USR_CTRL_RIGHT_DOWN},
	{KEYCODE_DPAD_LEFT          ,CEC_USR_CTRL_LEFT_UP},
	{KEYCODE_DPAD_LEFT          ,CEC_USR_CTRL_LEFT_DOWN},
	{KEYCODE_HOME       ,CEC_USR_CTRL_ROOT_MENU},
	{KEYCODE_MENU      ,CEC_USR_CTRL_SETUP_MENU},
	{KEYCODE_MENU         ,CEC_USR_CTRL_CONTENTS_MENU},
	{KEYCODE_NONE          ,CEC_USR_CTRL_FAVORITE_MENU},//We Don't have now.
	{KEYCODE_BACK          ,CEC_USR_CTRL_EXIT},
    {KEYCODE_0          ,CEC_USR_CTRL_NUMBER_0},
    {KEYCODE_1          ,CEC_USR_CTRL_NUMBER_1},
    {KEYCODE_2          ,CEC_USR_CTRL_NUMBER_2},
    {KEYCODE_3          ,CEC_USR_CTRL_NUMBER_3},
    {KEYCODE_4          ,CEC_USR_CTRL_NUMBER_4},
    {KEYCODE_5          ,CEC_USR_CTRL_NUMBER_5},
    {KEYCODE_6          ,CEC_USR_CTRL_NUMBER_6},
    {KEYCODE_7          ,CEC_USR_CTRL_NUMBER_7},
    {KEYCODE_8          ,CEC_USR_CTRL_NUMBER_8},
    {KEYCODE_9          ,CEC_USR_CTRL_NUMBER_9},
	{KEYCODE_ESC            ,CEC_USR_CTRL_CLEAR},//We Don't have noew.
    {KEYCODE_NONE           ,CEC_USR_CTRL_SOUND_SELECT},
	{KEYCODE_NONE       ,CEC_USR_CTRL_DISPLAY_INFORMATION},
	{KEYCODE_NONE       ,CEC_USR_CTRL_POWER},
	{KEYCODE_PLAY           ,CEC_USR_CTRL_PLAY},
	{KEYCODE_STOP           ,CEC_USR_CTRL_STOP},
	{KEYCODE_PAUSE          ,CEC_USR_CTRL_PAUSE},
	{KEYCODE_FRWD           ,CEC_USR_CTRL_REWIND},
	{KEYCODE_FFWD           ,CEC_USR_CTRL_FAST_FORWARD},
	{KEYCODE_NONE          ,CEC_USR_CTRL_EJECT},//other than <NetBox>
	{KEYCODE_NEXT           ,CEC_USR_CTRL_FORWARD},
	{KEYCODE_PREV           ,CEC_USR_CTRL_BACKWARD},
	{KEYCODE_NONE        ,CEC_USR_CTRL_ANGLE},
	{KEYCODE_MENU         ,CEC_USR_CTRL_SUB_PICTURE},
	{KEYCODE_MENU           ,CEC_USR_CTRL_INIT_CONFIGURATION},//Initial configuration
	{KEYCODE_PAUSE          ,CEC_USR_CTRL_PAUSE_PLAY_FUNCTION},
	{KEYCODE_NONE          ,CEC_USR_CTRL_POWER_TOGGLE},
	{KEYCODE_NONE          ,CEC_USR_CTRL_POWER_OFF},
	{KEYCODE_NONE            ,CEC_USR_CTRL_POWER_ON},//We should not do anything for CEC_USR_CTRL_POWER_ON in AP running.
    {KEYCODE_OPTION_BLUE    ,CEC_USR_CTRL_F1},
    {KEYCODE_OPTION_RED     ,CEC_USR_CTRL_F2},
    {KEYCODE_OPTION_GREEN   ,CEC_USR_CTRL_F3},
    {KEYCODE_OPTION_YELLOW  ,CEC_USR_CTRL_F4},
    {KEYCODE_NONE        ,CEC_USR_CTRL_HELP},
    {KEYCODE_NONE            ,CEC_USR_CTRL_SELECT_AUDIO_INPUT_FUNCTION},
    {KEYCODE_VOLUME_UP            ,CEC_USR_CTRL_VOLUME_UP},
    {KEYCODE_VOLUME_DOWN            ,CEC_USR_CTRL_VOLUME_DOWN},
    {KEYCODE_VOLUME_MUTE            ,CEC_USR_CTRL_MUTE_FUNCTION},
    {KEYCODE_VOLUME_MUTE            ,CEC_USR_CTRL_MUTE},
    {KEYCODE_NONE            ,CEC_USR_CTRL_RESTORE_VOLUME_FUNCTION}
    };

//-----------------------------------------------
// MCU Related
//-----------------------------------------------
#define KIND_CEC_INFO                   0x04
#define CEC_REPORT_PHSY_ADDR            0x05
#define CEC_REPORT_PLAYER_LOG_ADDR      0x06
#define CEC_REPORT_AUDIO_LOG_ADDR       0x07
#define CEC_REPORT_MPEG_CEC_FUNC        0x08
#define EASY_LINK_TV_ARC_FUNCTION       0x0a

#endif   
