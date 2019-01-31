#ifndef __CEC_DEF_H__
#define __CEC_DEF_H__

#define MAX_OSD_NAME_SIZE               (14)

typedef enum {    
    CEC_FEARURE_ONE_TOUCH_PLAY          = 0x00,
    CEC_FEARURE_ROUTING_CONTROL,
    CEC_FEARURE_STANDBY,
    CEC_FEARURE_ONE_TOUCH_RECORD,
    CEC_FEARURE_TIMER_PROGRAMING,
    CEC_FEARURE_SYSTEM_INFO,
    CEC_FEARURE_DECK_CONTROL,
    CEC_FEARURE_TUNER_CONTROL,
    CEC_FEARURE_VENDOR_SPECIFIC,
    CEC_FEARURE_OSD_DISPLAY,
    CEC_FEARURE_OSD_TRANSFER,    
    CEC_FEARURE_DEVICE_MENU_CONTROL,
    CEC_FEARURE_REMOTE_CONTROL_PASSTHROUGH,
    CEC_FEARURE_POWER_STATUS,
    CEC_FEARURE_GENERIC,            
    CEC_FEARURE_SYSTEM_AUDIO_CONTROL,    
    CEC_FEARURE_AUDIO_RATE_CONTROL,    
    CEC_FEARURE_AUDIO_RETURN_CHANNEL_CONTROL,    
    CEC_FEARURE_CAPABILITY_DISCOVERY_AND_CONTROL,    
    CEC_FEARURE_RESERVED,
}CEC_FEARURE_ID;

/************************************************     
 * CEC MSG ID 
 ************************************************/    
typedef enum 
{    
    //One Touch Play	    
    CEC_IMAGE_VIEW_ON                   = 0x04,	 
    CEC_TEXT_VIEW_ON                    = 0x0D,	
	                                    
    //Routing Control	                
    CEC_ACTIVE_SOURCE                   = 0x82, 
    CEC_INACTIVE_SOURCE                 = 0x9D, // <HDMI 1.3a>
    CEC_REQUEST_ACTIVE_SOURCE           = 0x85,	
    CEC_SET_STREAM_PATH                 = 0x86,	
    CEC_ROUTING_CHANGE                  = 0x80,	
    CEC_ROUTING_INFORMATION             = 0x81,
	                                    
    //System Standby	                
    CEC_STANDBY                         = 0x36, 
	                                    
    //One Touch Record	                
    CEC_RECORD_ON                       = 0x09,	
    CEC_RECORD_OFF                      = 0x0B,	
    CEC_RECORD_STATUS                   = 0x0A,	
    CEC_RECORD_TV_SCREEN                = 0x0F,	
	                                    
	//Timer Programming	                
	CEC_CLEAR_ANALOGUE_TIMER            = 0x33, // <HDMI 1.3a>
    CEC_CLEAR_DIGITAL_TIMER             = 0x99, // <HDMI 1.3a>
    CEC_CLEAR_EXTERNAL_TIMER            = 0xA1, // <HDMI 1.3a>   
    CEC_SET_ANALOGUE_TIMER              = 0x34, // <HDMI 1.3a>
    CEC_SET_DIGITAL_TIMER               = 0x97, // <HDMI 1.3a>
    CEC_SET_EXTERNAL_TIMER              = 0xA2, // <HDMI 1.3a>
    CEC_SET_TIMER_PROGRAM_TITLE         = 0x67, // <HDMI 1.3a>
    CEC_TIMER_CLEARED_STATUS            = 0x43, // <HDMI 1.3a>
    CEC_TIMER_STATUS                    = 0x35, // <HDMI 1.3a> 
	                                    
    //System Information	            
    CEC_CEC_VERSION                     = 0x9E, // <HDMI 1.3a>     
	CEC_GET_CEC_VERSION                 = 0x9F, // <HDMI 1.3a>
	CEC_GET_MENU_LANGUAGE               = 0x91, 
	CEC_SET_MENU_LANGUAGE               = 0x32,
	CEC_GIVE_PHYSICAL_ADDRESS           = 0x83,
	CEC_REPORT_PHYSICAL_ADDRESS         = 0x84,    
	                                    
    //Deck Control	                    
    CEC_GIVE_DECK_STATUS                = 0x1A,	
    CEC_DECK_STATUS                     = 0x1B,	
    CEC_DECK_CONTROL                    = 0x42,	
    CEC_PLAY                            = 0x41,	
	                                    
    //Tuner Control	                    
    CEC_GIVE_TUNER_DEVICE_STATUS        = 0x08,	
    CEC_TUNER_DEVICE_STATUS             = 0x07,	
    CEC_SELECT_ANALOGUE_SERVICE         = 0x92,	// <HDMI 1.3a>
    CEC_SELECT_DIGITAL_SERVICE          = 0x93,	
    CEC_TUNER_STEP_DECREMENT            = 0x06,	
    CEC_TUNER_STEP_INCREMENT             = 0x05,	
	                                    
    //Vendor Specific Command	        
	CEC_DEVICE_VENDOR_ID                = 0x87,
	CEC_GIVE_DEVICE_VENDOR_ID           = 0x8C,
	CEC_VENDOR_COMMAND                  = 0x89,
	CEC_VENDOR_COMMAND_WITH_ID          = 0xA0,	// <HDMI 1.3a>
	CEC_VENDOR_REMOTE_BUTTON_DOWN       = 0x8A,
	CEC_VENDOR_REMOTE_BUTTON_UP         = 0x8B,
	                                    
    //OSD Status Display	            
    CEC_SET_OSD_STRING                  = 0x64,
	                                    
    //OSD Status Display	            
    CEC_GIVE_OSD_NAME                   = 0x46,	
    CEC_SET_OSD_NAME                    = 0x47,
	                                    
    //Device Menu Control	            
    CEC_MENU_REQUEST                    = 0x8D,	
    CEC_MENU_STATUS                     = 0x8E,	
	                                    
    //Remote Control Pass Through	    
    CEC_USER_CONTROL_PRESSED            = 0x44,
    CEC_USER_CONTROL_RELEASED           = 0x45,
	                                    
    //Power Status Feature	            
    CEC_GIVE_DEVICE_POWER_STATUS        = 0x8F,
    CEC_REPORT_POWER_STATUS             = 0x90,
	                                    
    //General  Protocol 	            
    CEC_FEATURE_ABORT                   = 0x00,
    CEC_ABORT                           = 0xFF,
                                        
    //System Audio Control              
    CEC_GIVE_AUDIO_STATUS               = 0x71, // <HDMI 1.3a>
    CEC_GIVE_SYSTEM_AUDIO_MODE_STATUS   = 0x7D, // <HDMI 1.3a>   
    CEC_REPORT_AUDIO_STATUS             = 0x7A, // <HDMI 1.3a>
    CEC_SET_SYSTEM_AUDIO_MODE           = 0x72, // <HDMI 1.3a>
    CEC_SYSTEM_AUDIO_MODE_REQUEST       = 0x70, // <HDMI 1.3a>
    CEC_SYSTEM_AUDIO_MODE_STATUS        = 0x7E, // <HDMI 1.3a>
                                                             
    // Audio Rate Control                                    
    CEC_SET_AUDIO_RATE                  = 0x9A, // <HDMI 1.3a>
    
    // Audio Return channel
    CEC_INITIATE_ARC                    = 0xC0, // <HDMI 1.4>
    CEC_REPORT_ARC_INITIATED            = 0xC1, // <HDMI 1.4>
    CEC_REPORT_ARC_TERMINATED           = 0xC2, // <HDMI 1.4>
    CEC_REQUEST_ARC_INITIATION          = 0xC3, // <HDMI 1.4>
    CEC_REQUEST_ARC_TERMINATION         = 0xC4, // <HDMI 1.4>
    CEC_TERMINATE_ARC                   = 0xC5, // <HDMI 1.4>
    
    // Capability Discovery and Control 
    CEC_CDC_MESSAGE                     = 0xF8, // <HDMI 1.4>

}CEC_MSG_OPCODE;
                      

typedef unsigned char  CEC_LOGICAL_ADDRESS;                      
typedef unsigned short CEC_PHYSICAL_ADDRESS;
#define UNREGISTERED_LOGICAL_ADDRESS    0xF
#define UNKNOWN_PHYSICAL_ADDRESS        0xFFFF

/************************************************    
 * CEC MSG OPERAND : PAGE 55-62, TABLE 22 & 23
 ************************************************/   
typedef enum {    
    CEC_ABORT_UNKNOWN_OPCODE                = 0x00,
    CEC_ABORT_NOT_IN_CORECT_MODE            = 0x01,
    CEC_ABORT_CAN_NOT_PROVIDE_SOURCE        = 0x02,
    CEC_ABORT_INVALID_OPERAND               = 0x03,
    CEC_ABORT_REFUSED                       = 0x04,
}CEC_ABORT_REASON;
 

typedef enum {
    CEC_DECK_CTRL_SKIP_FORWARD              = 0x01,
    CEC_DECK_CTRL_SKIP_BACKWARD             = 0x02,
    CEC_DECK_CTRL_STOP                      = 0x03,
    CEC_DECK_CTRL_EJECT                     = 0x04,    
}CEC_DECK_CTRL_MODE;


typedef enum {
    CEC_VERSION_1_1                         = 0x00,
    CEC_VERSION_1_2                         = 0x01,
    CEC_VERSION_1_2A                        = 0x02,
    CEC_VERSION_1_3                         = 0x03,
    CEC_VERSION_1_3A                        = 0x04,
    CEC_VERSION_1_4                         = 0x05,
    CEC_VERSION_UNKNOWN                     = 0xFF,
}CEC_VERSION;

typedef enum {
    CEC_DECK_INFO_PLAY                      = 0x11,
    CEC_DECK_INFO_RECORD                    = 0x12,
    CEC_DECK_INFO_PLAY_REVERSE              = 0x13,
    CEC_DECK_INFO_STILL                     = 0x14,
    CEC_DECK_INFO_SLOW                      = 0x15,
    CEC_DECK_INFO_SLOW_REVERSE              = 0x16,
    CEC_DECK_INFO_SEARCH_FORWARD            = 0x17,
    CEC_DECK_INFO_SEARCH_REVERSE            = 0x18,
    CEC_DECK_INFO_NO_MEDIA                  = 0x19,
    CEC_DECK_INFO_STOP                      = 0x1A,
    CEC_DECK_INFO_WIND                      = 0x1B,
    CEC_DECK_INFO_REWIND                    = 0x1C,
    CEC_DECK_INFO_INDEX_SEARCH_FORWARD      = 0x1D,
    CEC_DECK_INFO_INDEX_SEARCH_REVERSE      = 0x1E,
    CEC_DECK_INFO_OTHER_STATUS              = 0x1F,
}CEC_DECK_INFO;
                  

typedef enum {    
    CEC_DEVICE_TV                           = 0x00,
    CEC_DEVICE_RECORDING_DEVICE             = 0x01,
    CEC_DEVICE_RESERVED                     = 0x02,
    CEC_DEVICE_TUNER                        = 0x03,
    CEC_DEVICE_PLAYBACK_DEVICE              = 0x04,
    CEC_DEVICE_AUDIO_SYSTEM                 = 0x05,        
    CEC_DEVICE_PURE_CEC_SWITCH              = 0x06,        
    CEC_DEVICE_VIDEO_PROCESSOR              = 0x07,
    CEC_DEVICE_TYPE_MAX                     = 0x08,      
}CEC_DEVICE_TYPE;

#define CEC_DEVICE_TYPE_BIT(x)              (1<< x)

typedef enum {
    CEC_ANALOGUE_BROADCAST_CABLE            = 0x00,
    CEC_ANALOGUE_BROADCAST_SATELITE         = 0x01,
    CEC_ANALOGUE_BROADCAST_TERRESTRIAL      = 0x02,
}CEC_ANALOGUE_BROADCAST_TYPE;


typedef enum {
    CEC_BROADCAST_SYSTEM_PAL_BG             = 0x00,
    CEC_BROADCAST_SYSTEM_SECAM_LS           = 0x01,
    CEC_BROADCAST_SYSTEM_PAL_M              = 0x02,
    CEC_BROADCAST_SYSTEM_NTSC_M             = 0x03,
    CEC_BROADCAST_SYSTEM_PAL_I              = 0x04,
    CEC_BROADCAST_SYSTEM_SECAM_DK           = 0x05,    
    CEC_BROADCAST_SYSTEM_SECAM_BG           = 0x06,
    CEC_BROADCAST_SYSTEM_SECAM_L            = 0x07,
    CEC_BROADCAST_SYSTEM_PAL_DK             = 0x08,    
    CEC_BROADCAST_SYSTEM_OTHERS             = 0x1F,        
}CEC_BROADCAST_SYSTEM;

/*
 * Name : CEC_DIGITAL_BROADCAST_SYSTEM
 * Desc : CEC Digital Broadcast System ID 
 */
typedef enum {
    CEC_DIGITAL_BROADCAST_SYSTEM_ARIB       = 0x00,
    CEC_DIGITAL_BROADCAST_SYSTEM_ATSC       = 0x01,
    CEC_DIGITAL_BROADCAST_SYSTEM_DVB        = 0x02,    
                             
    CEC_DIGITAL_BROADCAST_SYSTEM_ARIB_BS    = 0x08,
    CEC_DIGITAL_BROADCAST_SYSTEM_ARIB_CS    = 0x09,
    CEC_DIGITAL_BROADCAST_SYSTEM_ARIB_T     = 0x0A,
                             
    CEC_DIGITAL_BROADCAST_SYSTEM_ATSC_C     = 0x10,
    CEC_DIGITAL_BROADCAST_SYSTEM_ATSC_S     = 0x11,
    CEC_DIGITAL_BROADCAST_SYSTEM_ATSC_T     = 0x12,
                             
    CEC_DIGITAL_BROADCAST_SYSTEM_DVB_C      = 0x18,
    CEC_DIGITAL_BROADCAST_SYSTEM_DVB_S      = 0x19,
    CEC_DIGITAL_BROADCAST_SYSTEM_DVB_S2     = 0x1A,
    CEC_DIGITAL_BROADCAST_SYSTEM_DVB_T      = 0x1B,
    
}CEC_DIGITAL_BROADCAST_SYSTEM;

typedef enum {
    CEC_DISPLAY_CTRL_DEFAULT                = 0x00,
    CEC_DISPLAY_CTRL_UNTIL_CLEARED          = 0x40,
    CEC_DISPLAY_CTRL_CLEAR_PREVIOUS         = 0x80,    
}CEC_DISPLAY_CTRL;


typedef enum {
    CEC_MENU_REQUEST_ACTIVATE               = 0x00,
    CEC_MENU_REQUEST_DEACTIVATE             = 0x01,
    CEC_MENU_REQUEST_QUERY                  = 0x02,    
    CEC_MENU_REQUEST_NONE                   = 0xFF,    
}CEC_MENU_REQUEST_TYPE;


typedef enum {
    CEC_MENU_STATE_ACTIVATE                 = 0x00,
    CEC_MENU_STATE_DEACTIVATE               = 0x01,    
}CEC_MENU_STATE;


typedef enum {
    CEC_PLAY_MODE_PLAY_FORWARD              = 0x24,
    CEC_PLAY_MODE_PLAY_REVERSE              = 0x20,
    CEC_PLAY_MODE_PLAY_STILL                = 0x25,
                 
    CEC_PLAY_MODE_SCAN_FORWARD_MIN_SPD      = 0x05,
    CEC_PLAY_MODE_SCAN_FORWARD_MED_SPD      = 0x06,
    CEC_PLAY_MODE_SCAN_FORWARD_MAX_SPD      = 0x07,
                 
    CEC_PLAY_MODE_SCAN_REVERSE_MIN_SPD      = 0x09,
    CEC_PLAY_MODE_SCAN_REVERSE_MED_SPD      = 0x0A,
    CEC_PLAY_MODE_SCAN_REVERSE_MAX_SPD      = 0x0B,
                 
    CEC_PLAY_MODE_SLOW_FORWARD_MIN_SPD      = 0x15,
    CEC_PLAY_MODE_SLOW_FORWARD_MED_SPD      = 0x16,
    CEC_PLAY_MODE_SLOW_FORWARD_MAX_SPD      = 0x17,
                 
    CEC_PLAY_MODE_SLOW_REVERSE_MIN_SPD      = 0x19,
    CEC_PLAY_MODE_SLOW_REVERSE_MED_SPD      = 0x1A,
    CEC_PLAY_MODE_SLOW_REVERSE_MAX_SPD      = 0x1B,

}CEC_PLAY_MODE;


typedef enum {
    CEC_POWER_STATUS_ON                     = 0x00,
    CEC_POWER_STATUS_STANDBY                = 0x01,
    CEC_POWER_STATUS_STANDBY_TO_ON          = 0x02,
    CEC_POWER_STATUS_ON_TO_STANDBY          = 0x03,
    CEC_POWER_STATUS_UNKNOWN                = 0xFF,
}CEC_POWER_STATUS;


typedef enum {
    CEC_RECORD_SOURCE_NONE                      = 0x00,
    CEC_RECORD_SOURCE_OWN_SROURCE               = 0x01,
    CEC_RECORD_SOURCE_DIGITAL_SERVICE           = 0x02,    
    CEC_RECORD_SOURCE_ANALOGUE_SERVICE          = 0x03, // <HDMI 1.3a>        
    CEC_RECORD_SOURCE_EXTERNAL_PLUG             = 0x04, // <HDMI 1.3a>        
    CEC_RECORD_SOURCE_EXTERNAL_PHYSICAL_ADDRESS = 0x05, // <HDMI 1.3a>        
}CEC_RECORD_SOURCE_TYPE;


typedef enum {
    CEC_SERVICE_ID_BY_DIGITAL_ID                = 0x00, // <HDMI 1.3a> 
    CEC_SERVICE_ID_BY_CHANNEL                   = 0x80, // <HDMI 1.3a>         
}CEC_SERVICE_ID_METHOD;


typedef enum {
    CEC_RECORD_STATUS_RECORDING_OWN_SOURCE                            = 0x01,
    CEC_RECORD_STATUS_RECORDING_DIGITAL_SERVICE                       = 0x02,    
    CEC_RECORD_STATUS_RECORDING_ANALOG_SERVICE                        = 0x03, // <HDMI 1.3a>    
    CEC_RECORD_STATUS_RECORDING_EXTERNAL_SERVICE                      = 0x04, // <HDMI 1.3a>            
    CEC_RECORD_STATUS_NO_RECORDING_UNABLE_TO_REC_DIGITAL_SERVICE      = 0x05,
    CEC_RECORD_STATUS_NO_RECORDING_UNABLE_TO_REC_ANALOGUE_SERVICE     = 0x06, // <HDMI 1.3a>   
    CEC_RECORD_STATUS_NO_RECORDING_UNABLE_TO_SELECT_REQUIRED_SERVICE  = 0x07, // <HDMI 1.3a>   
    CEC_RECORD_STATUS_NO_RECORDING_INVALID_EXTERNAL_PLUG_NUMBER       = 0x09, // <HDMI 1.3a>   
    CEC_RECORD_STATUS_NO_RECORDING_INVALID_EXTERNAL_PHYSICAL_ADDRESS  = 0x0A, // <HDMI 1.3a>   
    CEC_RECORD_STATUS_NO_RECORDING_CA_SYSTEM_NOT_SUPPORTED            = 0x0B, // <HDMI 1.3a>   
    CEC_RECORD_STATUS_NO_RECORDING_NO_OR_INSUFFICIENT_CA_ENTITLEMENTS = 0x0C, // <HDMI 1.3a>   
    CEC_RECORD_STATUS_NO_RECORDING_NOT_ALLOWED_TO_COPY_SOURCE         = 0x0D, // <HDMI 1.3a>   
    CEC_RECORD_STATUS_NO_RECORDING_NO_FURTHER_COPIES_ALLOWED          = 0x0E, // <HDMI 1.3a>       
    CEC_RECORD_STATUS_NO_RECORDING_NO_MEDIA                           = 0x10,
    CEC_RECORD_STATUS_NO_RECORDING_PLAYING                            = 0x11,
    CEC_RECORD_STATUS_NO_RECORDING_ALREADY_RECORDING                  = 0x12,
    CEC_RECORD_STATUS_NO_RECORDING_MEDIA_PROTECTED                    = 0x13,
    CEC_RECORD_STATUS_NO_RECORDING_NO_SOURCE                          = 0x14,
    CEC_RECORD_STATUS_NO_RECORDING_MEDIA_PROBLEM                      = 0x15,    
    CEC_RECORD_STATUS_NO_RECORDING_NOT_ENGOUGH_SPACE_AVAILABLE        = 0x16, // <HDMI 1.3a>       
    CEC_RECORD_STATUS_NO_RECORDING_PARENTAL_LOCK_ON                   = 0x17, // <HDMI 1.3a>   
    CEC_RECORD_STATUS_NO_RECORDING_RECORDING_TERMINATED_NORMALLY      = 0x1A, // <HDMI 1.3a>   
    CEC_RECORD_STATUS_NO_RECORDING_RECORDING_HAS_ALREADY_TERMINATED   = 0x1B, // <HDMI 1.3a>       
    CEC_RECORD_STATUS_NO_RECORDING_OTHER_REASON                       = 0x1F,
}CEC_RECORD_STATUS_INFO;


typedef enum {
    CEC_STATUS_REQUEST_ON                   = 0x01,
    CEC_STATUS_REQUEST_OFF                  = 0x02,
    CEC_STATUS_REQUEST_ONCE                 = 0x03,
}CEC_STATUS_REQUEST;


typedef enum {
    CEC_RECORD_SEQUENCE_SUN                 = 0x01, // <HDMI 1.3a>       
    CEC_RECORD_SEQUENCE_MON                 = 0x02, // <HDMI 1.3a>           
    CEC_RECORD_SEQUENCE_TUE                 = 0x04, // <HDMI 1.3a>           
    CEC_RECORD_SEQUENCE_WED                 = 0x08, // <HDMI 1.3a>           
    CEC_RECORD_SEQUENCE_THU                 = 0x10, // <HDMI 1.3a>           
    CEC_RECORD_SEQUENCE_FRI                 = 0x20, // <HDMI 1.3a>           
    CEC_RECORD_SEQUENCE_SAT                 = 0x40, // <HDMI 1.3a>               
    CEC_RECORD_SEQUENCE_ONCE                = 0x00, // <HDMI 1.3a>               
}CEC_RECORD_SEQUENCE;

typedef enum {
    CEC_TUNER_NOT_USED_FOR_RECORDING        = 0x00, // <HDMI 1.3a>   
    CEC_TUNER_USED_FOR_RECORDING            = 0x01, // <HDMI 1.3a>    
}CEC_TUNER_RECORDING_FLAG;

typedef enum {
    CEC_TUNER_DISPLAYING_DIGITAL_TUNER      = 0x00, // <HDMI 1.3a>    
    CEC_TUNER_NOT_DISPLAYING                = 0x01,
    CEC_TUNER_DISPLAYING_ANALOGUE_TUNER     = 0x02,
}CEC_TUNER_DISPLAY_INFO;


typedef enum {
    CEC_AUDIO_RATE_CONTROL_OFF              = 0x00, // <HDMI 1.3a>       
    CEC_AUDIO_RATE_STANDARD_RATE_100        = 0x01, // <HDMI 1.3a>           
    CEC_AUDIO_RATE_FAST_RATE_101            = 0x02, // <HDMI 1.3a>           
    CEC_AUDIO_RATE_SLOW_RATE_99             = 0x03, // <HDMI 1.3a>           
    CEC_AUDIO_RATE_STANDARD_RATE_100_0      = 0x04, // <HDMI 1.3a>           
    CEC_AUDIO_RATE_FAST_RATE_100_1          = 0x05, // <HDMI 1.3a>           
    CEC_AUDIO_RATE_SLOW_RATE_99_9           = 0x06, // <HDMI 1.3a>               
}CEC_AUDIO_RATE;


typedef enum {
    
    CEC_USR_CTRL_SELECT                     = 0x00,
    CEC_USR_CTRL_UP                         = 0x01,
    CEC_USR_CTRL_DOWN                       = 0x02,
    CEC_USR_CTRL_LEFT                       = 0x03,
    CEC_USR_CTRL_RIGHT                      = 0x04,
    CEC_USR_CTRL_RIGHT_UP                   = 0x05,
    CEC_USR_CTRL_RIGHT_DOWN                 = 0x06,    
    CEC_USR_CTRL_LEFT_UP                    = 0x07,
    CEC_USR_CTRL_LEFT_DOWN                  = 0x08,
    CEC_USR_CTRL_ROOT_MENU                  = 0x09,
    CEC_USR_CTRL_SETUP_MENU                 = 0x0A,
    CEC_USR_CTRL_CONTENTS_MENU              = 0x0B,
    CEC_USR_CTRL_FAVORITE_MENU              = 0x0C,
    CEC_USR_CTRL_EXIT                       = 0x0D,
    CEC_USR_CTRL_NUMBER_0                   = 0x20,
    CEC_USR_CTRL_NUMBER_1                   = 0x21,
    CEC_USR_CTRL_NUMBER_2                   = 0x22,
    CEC_USR_CTRL_NUMBER_3                   = 0x23,
    CEC_USR_CTRL_NUMBER_4                   = 0x24,
    CEC_USR_CTRL_NUMBER_5                   = 0x25,
    CEC_USR_CTRL_NUMBER_6                   = 0x26,
    CEC_USR_CTRL_NUMBER_7                   = 0x27,
    CEC_USR_CTRL_NUMBER_8                   = 0x28,
    CEC_USR_CTRL_NUMBER_9                   = 0x29,
    CEC_USR_CTRL_DOT                        = 0x2A,
    CEC_USR_CTRL_ENTER                      = 0x2B,
    CEC_USR_CTRL_CLEAR                      = 0x2C,    
    CEC_USR_CTRL_NEXT_FAVORITE              = 0x2F, 
    CEC_USR_CTRL_CHANNEL_UP                 = 0x30,
    CEC_USR_CTRL_CHANNEL_DOWN               = 0x31,
    CEC_USR_CTRL_PREVIOUS_CHANNEL           = 0x32,
    CEC_USR_CTRL_SOUND_SELECT               = 0x33,
    CEC_USR_CTRL_INPUT_SELECT               = 0x34,
    CEC_USR_CTRL_DISPLAY_INFORMATION        = 0x35,
    CEC_USR_CTRL_HELP                       = 0x36,
    CEC_USR_CTRL_PAGE_UP                    = 0x37,
    CEC_USR_CTRL_PAGE_DOWN                  = 0x38,
    CEC_USR_CTRL_POWER                      = 0x40,
    CEC_USR_CTRL_VOLUME_UP                  = 0x41,
    CEC_USR_CTRL_VOLUME_DOWN                = 0x42,
    CEC_USR_CTRL_MUTE                       = 0x43,
    CEC_USR_CTRL_PLAY                       = 0x44,
    CEC_USR_CTRL_STOP                       = 0x45,
    CEC_USR_CTRL_PAUSE                      = 0x46,
    CEC_USR_CTRL_RECORD                     = 0x47,
    CEC_USR_CTRL_REWIND                     = 0x48,
    CEC_USR_CTRL_FAST_FORWARD               = 0x49,
    CEC_USR_CTRL_EJECT                      = 0x4A,
    CEC_USR_CTRL_FORWARD                    = 0x4B,
    CEC_USR_CTRL_BACKWARD                   = 0x4C,
    CEC_USR_CTRL_STOP_RECORD                = 0x4D,
    CEC_USR_CTRL_PAUSE_RECORD               = 0x4E,
    CEC_USR_CTRL_ANGLE                      = 0x50,
    CEC_USR_CTRL_SUB_PICTURE                = 0x51,
    CEC_USR_CTRL_VIDEO_ON_DEMAND            = 0x52,
    CEC_USR_CTRL_GUIDE                      = 0x53,
    CEC_USR_CTRL_TIMER                      = 0x54,
    CEC_USR_CTRL_INIT_CONFIGURATION         = 0x55,
    CEC_USR_CTRL_PLAY_FUNCTION              = 0x60, 
    CEC_USR_CTRL_PAUSE_PLAY_FUNCTION        = 0x61,
    CEC_USR_CTRL_RECORD_FUNCTION            = 0x62,
    CEC_USR_CTRL_PAUSE_RECORD_FUNCTION      = 0x63,
    CEC_USR_CTRL_STOP_FUNCTION              = 0x64,
    CEC_USR_CTRL_MUTE_FUNCTION              = 0x65,
    CEC_USR_CTRL_RESTORE_VOLUME_FUNCTION    = 0x66,
    CEC_USR_CTRL_TUNE_FUNCTION              = 0x67,
    CEC_USR_CTRL_SELECT_MEDIA_FUNCTION      = 0x68,
    CEC_USR_CTRL_SELECT_AV_INPUT_FUNCTION   = 0x69,
    CEC_USR_CTRL_SELECT_AUDIO_INPUT_FUNCTION= 0x6A,
    CEC_USR_CTRL_POWER_TOGGLE               = 0x6B,
    CEC_USR_CTRL_POWER_OFF                  = 0x6C,
    CEC_USR_CTRL_POWER_ON                   = 0x6D,
    CEC_USR_CTRL_F1                         = 0x71,  // BLUE
    CEC_USR_CTRL_F2                         = 0x72,  // RED
    CEC_USR_CTRL_F3                         = 0x73,  // GREEN
    CEC_USR_CTRL_F4                         = 0x74,  // YELLOW
    CEC_USR_CTRL_F5                         = 0x75,    
    CEC_USR_CTRL_DATA                       = 0x76,    
    CEC_USR_CTRL_UNKNOWN                    = 0xFF,    
    
}CEC_USER_CTRL_CODE;


typedef enum {
    CEC_ARC_CTRL_TERMINATE                  = 0x00,    
    CEC_ARC_CTRL_INITIATE,
    CEC_ARC_CTRL_PREPARE,
    CEC_ARC_CTRL_MAX,
}CEC_ARC_CTRL;

typedef enum {
    CEC_ARC_STATUS_TERMINATE                = 0x00,    
    CEC_ARC_STATUS_INITIATE,
    CEC_ARC_STATUS_MAX,
}CEC_ARC_STATUS;

typedef unsigned short              CEC_PHYSICAL_ADDR;
typedef unsigned long               CEC_MENU_LANGUAGE;

/* 
 * Name  : CEC_ARIB_SERVICE_ID
 * Desc  : This data structure is used to specify an unique ARIB service 
 * Param : TransportStremID      : Transport Stream ID
 *         ServiceID             : Service ID
 *         NetworkID             : Network ID
 */   
typedef struct 
{
    unsigned short              TransportStremID;
    unsigned short              ServiceID;
    unsigned short              NetworkID;   
}CEC_ARIB_SERVICE_ID;


/* 
 * Name  : CEC_DVB_SERVICE_ID
 * Desc  : This data structure is used to specify an unique DVB service 
 * Param : TransportStremID      : Transport Stream ID
 *         ServiceID             : Service ID
 *         NetworkID             : Network ID
 */      
typedef struct 
{
    unsigned short              TransportStremID;
    unsigned short              ServiceID;
    unsigned short              NetworkID;   
}CEC_DVB_SERVICE_ID;


/* 
 * Name  : CEC_ATSC_SERVICE_ID
 * Desc  : This data structure is used to specify an unique ATSC service 
 * Param : TransportStremID      : Transport Stream ID
 *         ServiceID             : Service ID
 */      
typedef struct 
{
    unsigned short              TransportStremID;
    unsigned short              ServiceID;    
}CEC_ATSC_SERVICE_ID;

    
/* 
 * Name  : CEC_CHANNEL_SERVICE_ID
 * Desc  : This data structure is used to specify an unique digital service via channel ID
 * Param : Format                : Format of Channel Number 
 *            0x01: 1-part channel number        
 *            0x02: 2-part channel number        
 *         MajorChannelNumber    : major channel ID (only valid in 2-part channel number format) 
 *         MinorChannelNumber    : minor channel ID 
 */      
typedef struct {
    unsigned char               Format;
    unsigned short              MajorChannelNumber;
    unsigned short              MinorChannelNumber;                            
}CEC_CHANNEL_SERVICE_ID;        
                
        
/* 
 * Name  : DIGITAL_SERVICE_ID
 * Desc  : This data structure is used to specify an unique digital service
 * Param : Method    : service identify methodology
 *         System    : digital broadcasting system
 *         ID        : Service ID, union of Arib/Dvb/Atsc/Channel Service ID
 */    
typedef struct 
{
    CEC_SERVICE_ID_METHOD               Method;
    CEC_DIGITAL_BROADCAST_SYSTEM        System;
    
    union 
    {
        CEC_ARIB_SERVICE_ID             Arib; 
        CEC_DVB_SERVICE_ID              Dvb;
        CEC_ATSC_SERVICE_ID             Atsc;
        CEC_CHANNEL_SERVICE_ID          Ch;        
    }ID;
    
}CEC_DIGITAL_SERVICE_ID;

/* 
 * Name  : ANALOG_SERVICE_ID
 * Desc  : Analog Service ID which used to specify an analog service
 * Param : Type      : Broadcasting Type
 *         Frequency : channel frequency
 *         System    : Broadcasting System 
 */
typedef struct 
{
    CEC_ANALOGUE_BROADCAST_TYPE     Type;
    unsigned long                   Frequency;
    CEC_BROADCAST_SYSTEM            System;            
}CEC_ANALOG_SERVICE_ID;


/* 
 * Name  : CEC_TUNER_DEVICE_INFO
 * Desc  : This data structure is used to identify current Tuner Status
 * Param : Recording : Tuner Userd for Recording ?
 *         Display   : Tuner Display Status
 *         Service   : current Service 
 */
typedef struct 
{    
    CEC_TUNER_RECORDING_FLAG        Recording;
    CEC_TUNER_DISPLAY_INFO          Display;
    union {
        CEC_DIGITAL_SERVICE_ID      Digital;
        CEC_ANALOG_SERVICE_ID       Analog;
    }Service;
}CEC_TUNER_DEVICE_INFO;

///////////////////////////////////////////////////////////////
//  Timer Control
///////////////////////////////////////////////////////////////

typedef struct {
    unsigned char Hour;
    unsigned char Minute;
}CEC_TIME;

typedef struct {
    unsigned char Hour;
    unsigned char Minute;
}CEC_DURATION;

typedef struct {
    unsigned char Month;
    unsigned char Day;
}CEC_DATE;

/* 
 * Name  : CEC_TIMER_INFO
 * Desc  : This data structure is used to specify parameters of timer 
 * Param : Date     : Record Date 
 *         Time     : Record Time       (BCD)
 *         Duration : Record Duration   (BCD)
 *         Sequence : Record Sequence
 */
typedef struct 
{
    CEC_DATE                        Date;
    CEC_TIME                        Time;
    CEC_DURATION                    Duration;
    unsigned char                   Sequence;
}CEC_TIMER_INFO;


typedef enum {
    CEC_EXTERNAL_SOURCE_PLUG               = 0x04, // <HDMI 1.3a>       
    CEC_EXTERNAL_SOURCE_PHYSICAL_ADDRESS   = 0x05, // <HDMI 1.3a>               
}CEC_EXTERNAL_SOURCE_TYPE;


typedef struct 
{
    CEC_EXTERNAL_SOURCE_TYPE        Type;
    unsigned short                  ID;
}CEC_EXTERNAL_SOURCE_ID;



/* 
 * Name  : CEC_RECORD_SOURCE_ID
 * Desc  : This data structure is used to specify an service which want to be recorded.
 * Param : SourceType : Source Type (Digital / Analog / External Plug / External Physical Addr)
 *         ServiceID  : Service Identification 
 */
typedef struct 
{
    CEC_RECORD_SOURCE_TYPE              Type;
    union {
        CEC_DIGITAL_SERVICE_ID          Digital;
        CEC_ANALOG_SERVICE_ID           Analog;        
        unsigned char                   ExternalPlugID;
        unsigned short                  ExternalPhysicalAddr;    
    }ID;
    
}CEC_RECORD_SOURCE_ID;


typedef enum {
    CEC_TIMER_NOT_CLEARED_RECORDING        = 0x00, // <HDMI 1.3a>       
    CEC_TIMER_NOT_CLEARED_NO_MATCHING      = 0x01, // <HDMI 1.3a>       
    CEC_TIMER_NOT_CLEARED_NO_INFO_AVAILABLE= 0x02, // <HDMI 1.3a>       
    CEC_TIMER_CLEARED                      = 0x80, // <HDMI 1.3a>       
}CEC_TIMER_CLEAR_STATUS_DATA;

typedef enum {
    CEC_TIMER_NO_OVERLAP                     = 0x00, // <HDMI 1.3a>       
    CEC_TIMER_OVERLAP                        = 0x01, // <HDMI 1.3a>           
}CEC_TIMER_OVERLAP_WARNING;

typedef enum {
    CEC_TIMER_MEDIA_PRESENT_AND_NO_PROTECTED = 0x00, // <HDMI 1.3a>       
    CEC_TIMER_MEDIA_PRESENT_BUT_PROTECTED    = 0x01, // <HDMI 1.3a>       
    CEC_TIMER_MEDIA_NOT_PRESENT              = 0x02, // <HDMI 1.3a>           
}CEC_TIMER_MEDIA_INFO;

typedef enum {
    CEC_TIMER_NOT_PROGRAMED                  = 0x00, // <HDMI 1.3a>       
    CEC_TIMER_PROGRAMED                      = 0x01, // <HDMI 1.3a>           
}CEC_PROGRAMED_INDICATOR;

typedef enum {
    CEC_ENOUGH_SPACE_AVAILABLE               = 0x00,     // <HDMI 1.3a>       
    CEC_NOT_ENOUGH_SPACE_AVAILABLE           = 0x09,     // <HDMI 1.3a>           
    CEC_MAY_NOT_BE_ENOUGH_SPACE_AVAILABLE    = 0x0B,     // <HDMI 1.3a>           
    CEC_NO_MEDIA_INFO_AVAILABLE              = 0x0A,     // <HDMI 1.3a>           
}CEC_PROGRAM_INFO;

typedef enum {
    CEC_NO_FREE_TIMER_AVAILABLE              = 0x01, // <HDMI 1.3a>       
    CEC_DATE_OUT_OF_RANGE                    = 0x02, // <HDMI 1.3a>           
    CEC_RECORDING_SEQUENCE_ERROR             = 0x03, // <HDMI 1.3a>           
    CEC_INVALID_EXTERNAL_PLUG_NUMBER         = 0x04, // <HDMI 1.3a>           
    CEC_INVALID_EXTERNAL_PHYSICAL_ADDRESS    = 0x05, // <HDMI 1.3a>           
    CEC_CA_SYSTEM_NOT_SUPPORTED              = 0x06, // <HDMI 1.3a>           
    CEC_NOT_OR_INSUFFICIENT_CA_ENTITLEMENT   = 0x07, // <HDMI 1.3a>           
    CEC_DOES_NOT_SUPPORT_RESOLUTION          = 0x08, // <HDMI 1.3a>           
    CEC_PARENTAL_LOCK_ON                     = 0x09, // <HDMI 1.3a>           
    CEC_CLOCK_FAILURE                        = 0x0A, // <HDMI 1.3a>           
    CEC_DUPLICATE_ALREADY_PROGRAMED          = 0x0E, // <HDMI 1.3a>           
}CEC_NOT_PROGRAM_ERROR_INFO;



///////////////////////////////////////////////////////////////
//  AUDIO SYSTEM FUNCTION
///////////////////////////////////////////////////////////////
typedef enum {
    CEC_AUDIO_MUTE_OFF                      = 0x00, // <HDMI 1.3a>    
    CEC_AUDIO_MUTE_ON                       = 0x01, // <HDMI 1.3a>    
}CEC_AUDIO_MUTE_STATUS;

typedef enum {
    CEC_SYSTEM_AUDIO_STATUS_OFF             = 0x00, // <HDMI 1.3a>       
    CEC_SYSTEM_AUDIO_STATUS_ON              = 0x01, // <HDMI 1.3a>               
}CEC_SYSTEM_AUDIO_STATUS;

typedef struct {    
    unsigned char Mute   : 1;
    unsigned char Volume : 7;
}CEC_AUDIO_STATUS;


typedef struct {
    struct {
        unsigned char Init:4;
        unsigned char Dest:4;
    }Header;    
    
    unsigned char Poll;                
    // the following fields are available when poll equal to 0
    unsigned char Opcode;               
    unsigned char ParamLength;
    unsigned char Param[21];
}CEC_MSG;




#endif  // __CEC_DEF_H__


