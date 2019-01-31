#ifndef __CEC_TYPE_H__
#define __CEC_TYPE_H__

#include "CEC_Def.h"
#include "CEC_Types.h"


typedef enum 
{
    CEC_CTRL_OK                                  = 0x00,    
    CEC_CTRL_FAIL                                = 0xFF,    
    CEC_CTRL_UNSUPPORTED_FEATURE                 = 0x01,
    CEC_CTRL_ABORT_UNKNOWN_OPCODE                = 0x01,
    CEC_CTRL_ABORT_NOT_IN_CORECT_MODE            = 0x02,
    CEC_CTRL_ABORT_CAN_NOT_PROVIDE_SOURCE        = 0x03,
    CEC_CTRL_ABORT_INVALID_OPERAND               = 0x04,
    CEC_CTRL_ABORT_REFUSED                       = 0x05,    
    CEC_CTRL_INVALID_MSG_TOO_FEW_PARAMETER       = 0x06,
    CEC_CTRL_INVALID_MSG_TOO_MUCH_PARAMETER      = 0x07,
    CEC_CTRL_INVALID_MSG_SHOULD_NOT_BROADCAST    = 0x08,
    CEC_CTRL_INVALID_MSG_SHOULD_NOT_DIRECT_ADDRESS = 0x09,
    CEC_CTRL_INVALID_MSG_SHOULD_NOT_BE_SENT_BY_UNREGISITERED_DEVICE = 0x0A,
    CEC_CTRL_WAIT_RESPONSE_TIMEOUT               = 0x0B,      
}CEC_RET;


typedef struct {    
    CEC_FEARURE_ID      FeatureId;
    unsigned char       Opcode;
    unsigned char       MinArgs;
    unsigned char       MaxArgs;
    unsigned char       Flags;

    #define NO_UNREGISTERED 0x04      // ignore unregistered
    #define BROADCAST       0x02
    #define DIRECT_ADDRESS  0x01
    
}CEC_MSG_INFO;

typedef enum 
{
    BECOME_ACTIVE_SOURCE                          = 0x00,
    LOST_ACTIVE_SOURCE                            = 0x01,
    ACTIVE_SOURCE_CHANGE                          = 0x02,
}ACTIVE_SOURCE_STATE;

// Standby Flags
#define BIT(x)                      (1UL << x)
#define CEC_STANBY_RESPONSE_GIVE_POWER_STATUS       BIT(0)
#define CEC_STANBY_RESPONSE_POOLING                 BIT(1)
#define CEC_STANBY_RESPONSE_GIVE_PHYSICAL_ADDR      BIT(2)
#define CEC_STANBY_RESPONSE_GET_CEC_VERISON         BIT(3)
#define CEC_STANBY_RESPONSE_GIVE_DEVICE_VENDOR_ID   BIT(4)
#define CEC_STANBY_WAKEUP_BY_ROUTING_INFO           BIT(26)
#define CEC_STANBY_WAKEUP_BY_ROUTING_CHANGE         BIT(27)
#define CEC_STANBY_WAKEUP_BY_REQUEST_AUDIO_SYSTEM   BIT(28)
#define CEC_STANBY_WAKEUP_BY_USER_CONTROL           BIT(29)
#define CEC_STANBY_WAKEUP_BY_IMAGE_VIEW_ON          BIT(30)
#define CEC_STANBY_WAKEUP_BY_SET_STREAM_PATH        BIT(31)

typedef enum {
    EVENT_GOT_CEC_MESSAGE,
    EVENT_GOT_LOGICAL_ADDR,
    EVENT_CEC_COMMAND_COMPLETE,
}CEC_EVENT;

            
typedef struct 
{
    CEC_EVENT       type;
            
    union 
    {        
        unsigned char   data[14];  
        
        //-------------------------------------------
        // Event type is EVENT_GOT_CEC_MESSAGE
        //-------------------------------------------
        union 
        {
            unsigned char           Id;         // for all cec msg
                                                
            // cec msg with extra parameters
                                                    
            struct {
                unsigned char       Id;         // CEC_ACTIVE_SOURCE / CEC_INACTIVE_SOURCE
                unsigned short      PhyAddr;        
            }ActiveSource;                
            
            struct {
                unsigned char       Id;         // CEC_CEC_VERSION
                unsigned char       Version;
            }CecVersion;
            
            struct {
                unsigned char       Id;         // CEC_CEC_VERSION
                unsigned short      PhyAddr;
                CEC_DEVICE_TYPE     DeviceType;
            }ReportPhysicalAddress;
                        
            struct {
                unsigned char       Id;         // CEC_SET_MENU_LANGUAGE
                unsigned char       Language[3];
            }SetMenuLanguage;
            
            struct {
                unsigned char       Id;         // CEC_USER_CONTROL_PRESSED                        
                unsigned char       RCCode;     // RC code  
                unsigned char       LongPressed;// 0: short press, 1: long press (for CEC_USER_CONTROL_PRESSED only)
            }UserControl;
            
            // Ver: 0.9                                    
            struct {
                unsigned char       Id;         // CEC_REPORT_DEVICE_POWER_STATUS                        
                CEC_POWER_STATUS    Status;     // Power Status
            }ReportPowerStatus;                    

            struct {
                unsigned char       Id;             // CEC_SET_SYSTEM_AUDIO_MODE                        
                unsigned short      PhyAddr;
            }SystemuAdioModeRequest;
            
            struct {
                unsigned char       Id;             // CEC_SET_SYSTEM_AUDIO_MODE                        
                CEC_SYSTEM_AUDIO_STATUS    Mode;    // Audio Status
            }SetSystemuAdioMode;
            
            struct {
                unsigned char       Id;             // CEC_SET_SYSTEM_AUDIO_MODE                        
                CEC_SYSTEM_AUDIO_STATUS    Mode;    // Audio Status
            }SystemuAdioMode;            
            
            struct {
                unsigned char       Id;             // CEC_SET_STREAM_PATH                        
                unsigned short      PhyAddr;        // Physical Address
            }SetStreamPath;     
            
            struct {
                unsigned char       Id;             // CEC_ROUTING_INFORMATION                        
                unsigned short      PhyAddr;        // Physical Address
            }RoutingInformation;
            
            struct {
                unsigned char       Id;             // CEC_ROUTING_CHANGE                        
                unsigned short      OrgPhyAddr;     // Origional Physical Address
                unsigned short      NewPhyAddr;     // New Physical Address
            }RoutingChange;                    
            
            struct {
                unsigned char       Id;             // CEC_FEATURE_ABORT                        
                unsigned char       Opcode;         // OP Code
                CEC_ABORT_REASON    Reason;         // Abort Reasin
            }FeatureAbort;
                    
        }msg;  
        
        //-------------------------------------------
        // Event type is EVENT_CEC_COMMAND_COMPLETE
        //-------------------------------------------  
        unsigned char LogAddr;             
    }u;
    
}cec_event;


/*------------------------------------------------------------------ 
 * Name : CEC_ONE_TOUCH_PLAY_OPS
 *
 * Desc : Communication path between CEC Module & AP
 *        When CEC Module receieved Roucting Control Messages comes from external CEC device.
 *        These functions will be invoked to notify AP. 
 *------------------------------------------------------------------*/
typedef struct 
{
    int (*ImageViewOn)(unsigned char mode);
    
}CEC_ONE_TOUCH_PLAY_OPS;
    
    
    
/*------------------------------------------------------------------ 
 * Name : CEC_ROUTING_CTRL_OPS
 *
 * Desc : Communication path between CEC Module & AP
 *        When CEC Module receieved Roucting Control Messages comes from external CEC device.
 *        These functions will be invoked to notify AP. 
 *------------------------------------------------------------------*/
typedef struct 
{        
    int (*LostActiveSource)();                      // current device has lost active source
    int (*GotInactiveSource)();                     // current active source has no data to play
    int (*GotStreamPath)();                         // current device has been selected as new active source
}CEC_ROUTING_CTRL_OPS;



/*------------------------------------------------------------------ 
 * Name : ONE_TOUCH_RECORD_OPS
 *
 * Desc : Communication path between CEC Module & AP
 *        When CEC Module receieved One Touch Record Messages comes from external CEC device.
 *        These functions will be invoked to notify AP. 
 *------------------------------------------------------------------*/
typedef struct 
{
    int (*GetTVScreenSource)(CEC_RECORD_SOURCE_ID *pSource);
    
    int (*StartRecord)(CEC_RECORD_SOURCE_ID Source);
    
    int (*StopRecord)();
    
}ONE_TOUCH_RECORD_OPS;



/*------------------------------------------------------------------ 
 * Name : CEC_SYS_INFO_OPS
 *
 * Desc : Communication path between CEC Module & AP
 *        When CEC Module receieved System Info Messages comes from external CEC device.
 *        These functions will be invoked to notify AP. 
 *------------------------------------------------------------------*/
typedef struct 
{        
    int (*SetMenuLanguage)(char Lang[3]);
    
}CEC_SYS_INFO_OPS;


/*------------------------------------------------------------------ 
 * Name : CEC_DECK_CTRL_OPS
 *
 * Desc : Communication path between CEC Module & AP
 *        When CEC Module receieved Deck Control Messages comes from external CEC device.
 *        These functions will be invoked to notify AP. 
 *------------------------------------------------------------------*/
typedef struct 
{
    int (*DeckControl)(unsigned char Ctrl);                     // Deck Control Helper
    
}CEC_DECK_CTRL_OPS;


/*------------------------------------------------------------------ 
 * Name : CEC_VENDOR_CTRL_OPS
 *
 * Desc : Communication path between CEC Module & AP
 *        When CEC Module receieved Vendor Command Messages comes from external CEC device.
 *        These functions will be invoked to notify AP. 
 *------------------------------------------------------------------*/
typedef struct 
{    
    int (*DoVendorCommand)(unsigned char Initiator, unsigned char* pCmd, unsigned char CmdLen);
    int (*DoVendorCommandWithID)(unsigned char Initiator, unsigned long VendorID, unsigned char* pCmd, unsigned char CmdLen);
    int (*VendorRemoteButtonDown)(unsigned char key);    
}CEC_VENDOR_COMMAND_OPS;


/*------------------------------------------------------------------ 
 * Name : CEC_OSD_DISPLAY_CTRL_OPS
 *
 * Desc : Communication path between CEC Module & AP
 *        When CEC Module receieved OSD Display Control Messages comes from external CEC device.
 *        These functions will be invoked to notify AP. 
 *------------------------------------------------------------------*/
typedef struct 
{    
    int (*DisplayOSDString)(CEC_DISPLAY_CTRL Ctrl, const char* pStr, unsigned char Len);       
}CEC_OSD_DISPLAY_CTRL_OPS;



/*------------------------------------------------------------------ 
 * Name : CEC_MENU_CTRL_OPS
 *
 * Desc : Communication path between CEC Module & AP
 *        When CEC Module receieved MenuControl Messages comes from external CEC device.
 *        These functions will be invoked to notify AP. 
 *------------------------------------------------------------------*/
typedef struct 
{
    int (*MenuControl)(unsigned char Ctrl);                     // Deck Control Helper
    
}CEC_MENU_CTRL_OPS;


/*------------------------------------------------------------------ 
 * Name : CEC_REMOTE_CTRL_OPS
 *
 * Desc : Communication path between CEC Module & AP
 *        When CEC Module receieved Remote Control Messages comes from external CEC device.
 *        These functions will be invoked to notify AP. 
 *------------------------------------------------------------------*/
typedef struct 
{    
    int (*UserControlPressed)(unsigned char key);
}CEC_REMOTE_CTRL_OPS;


    
/*------------------------------------------------------------------ 
 * Name : CEC_OPS
 *
 * Desc : Communication path between CEC Module & AP
 *        When CEC Module receieved requests comes from external CEC device.
 *        These functions will be invoked to notify AP.
 *    
 *------------------------------------------------------------------*/
typedef struct 
{    
    CEC_ONE_TOUCH_PLAY_OPS      OneTouchPlayOps;
    CEC_ROUTING_CTRL_OPS        RoutingCtrlOps;

    //---------------------------------
    // Standby
    //--------------------------------- 
    int (*Standby)();                               // running standby
    ONE_TOUCH_RECORD_OPS        OneTouchRecordOps;        
    CEC_SYS_INFO_OPS            SysInfoOps;        
    CEC_DECK_CTRL_OPS           DeckCtrlOps;        
    CEC_VENDOR_COMMAND_OPS      VendorCommandOps;            
    CEC_OSD_DISPLAY_CTRL_OPS    OSDDisplayOps;            
    CEC_MENU_CTRL_OPS           MenuCtrlOps;    
    CEC_REMOTE_CTRL_OPS         RemoteCtrlOps;            
    
}CEC_OPS;



typedef enum {
    CEC_CTRL_LOGICAL_ADDRESS,    
    CEC_CTRL_PHYSICAL_ADDRESS,    
    CEC_CTRL_POWER_STATUS,
    
    //--Optional--
    CEC_CTRL_MENU_LANGUAGE,
    CEC_CTRL_RECORD_STATUS,
    CEC_CTRL_DECK_STATUS,
    CEC_CTRL_MENU_STATUS,    
    CEC_CTRL_SYSTEM_AUDIO_STATUS,      
    CEC_CTRL_AUDIO_STATUS,         
}CEC_CTRL_STATUS_ID;


typedef struct
{    
    CEC_CTRL_STATUS_ID   Id;
    
    union {
        CEC_POWER_STATUS        PowerStatus;
        CEC_LOGICAL_ADDRESS     LogicalAddress;    
        CEC_PHYSICAL_ADDRESS    PhysicalAddress;                        
        CEC_RECORD_STATUS_INFO  RecStatus;
        CEC_DECK_INFO           DeckStatus;
        CEC_MENU_STATE          MenuState;
        char                    MenuLanguage[3];
    }u;        
}CEC_CTRL_STATUS;


#endif //__CEC_CONTROL_H__
