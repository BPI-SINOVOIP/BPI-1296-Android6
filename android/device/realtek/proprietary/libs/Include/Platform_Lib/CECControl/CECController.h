#ifndef __CEC_CONTROL_H__
#define __CEC_CONTROL_H__

#include "CEC_Def.h"
#include "CEC_Types.h"
#include "CECAdapter.h"
#include "CECAPBridge.h"
#include "OSAL.h"

    
class CECController 
{
public:

    CECController();    

    CECController(CECAPBridge* pBridge, unsigned char PrefferedLogicalAddress=0xF);
    
    virtual ~CECController();
        
protected:

    CEC_DEVICE_TYPE       m_DeviceType;
    CECAdapter*           m_pAdapter;       
    CECAPBridge*          m_pBridge;    
    CEC_LOGICAL_ADDRESS   m_LogicalAddress;
    CEC_PHYSICAL_ADDRESS  m_PhysicalAddress;
    osal_mutex_t          m_Lock; 
    CEC_POWER_STATUS      m_TvPowerStatus;
    unsigned char         m_TvSupportSystemAudio;
    
public:        
    
    virtual int  Init();
    
    virtual int  UnInit();
    
    virtual int  Enable(unsigned char OnOff);
    
    virtual int  UpdateStatus(CEC_CTRL_STATUS_ID Id);
    
    virtual int  PollDevice(unsigned char LogicalAddr);    
    
    virtual int  SetStandbyMode(unsigned long Mode);
    
    CECAdapter*  GetCECAdapter() {  return m_pAdapter;   }
    
//------------------------------------------------------------------
// Message Handler
//------------------------------------------------------------------    
    
protected:
    virtual void MessageHandler(CEC_MSG* pMsg);            

    //------------------------------------------------------------------    
    // CEC Operations
    //------------------------------------------------------------------
protected:             
    CEC_LOGICAL_ADDRESS m_PreLogicalAddress;
                
    virtual CEC_LOGICAL_ADDRESS LogicalAddressAllocation(CEC_DEVICE_TYPE Type, CEC_LOGICAL_ADDRESS PrevLogicalAddress);    

public:
    // This funnction is used to tell AP what is the last valid logical address used by
    // CEC controller. AP can store this value in an involatile memory, and use it at next
    // start up. 
    virtual unsigned char GetLastLogicalAddress() { return m_PreLogicalAddress; }
    
    virtual unsigned short GetLastLogicalAddresses() { return (1 << m_PreLogicalAddress); }
    
//-----------------------------------------------------------------------------------    
// One Touch Play
//-----------------------------------------------------------------------------------
protected:        
    
    virtual int OneTouchPlayHandler(CEC_MSG* pMsg);    

public:

    // mode
    #define OTP_IMAGE_MODE        0x00
    #define OTP_TEXT_MODE         0x80
    
    // mode
    #define AUTO_WAKEUP_TV        0x00      // wake up TV if TV is not Power On
    #define FORCE_WAKEUP_TV       0x01      // always wake up TV
    #define DONT_WAKEUP_TV        0x02      // never wake up TV   
    #define WAKEUP_TV_MODE(x)     (x & 0x3)
    
    #define AUTO_IMAGE_VIEW_ON    (OTP_IMAGE_MODE | AUTO_WAKEUP_TV) 
    #define AUTO_TEXT_VIEW_ON     (OTP_TEXT_MODE  | AUTO_WAKEUP_TV) 
    #define FORCE_IMAGE_VIEW_ON   (OTP_IMAGE_MODE | FORCE_WAKEUP_TV) 
    #define FORCE_TEXT_VIEW_ON    (OTP_TEXT_MODE  | FORCE_WAKEUP_TV) 
    #define ACTIVE_SOURCE_ONLY    (OTP_IMAGE_MODE | DONT_WAKEUP_TV)     
    
    virtual int OneTouchPlay(unsigned char Mode = AUTO_IMAGE_VIEW_ON, unsigned long WaitTVTimeout = 0);
            
//-------------------------------------------------------------------------------//
// Routing Control
//-------------------------------------------------------------------------------//            
protected:
        
    struct {
        unsigned short  CurActSrc;
        unsigned char   State;                
    }m_Route;
        
    virtual int RoutingControlHandler(CEC_MSG* pMsg);    

public:
    
    virtual int ActiveSource();    

    virtual int InactiveSource(unsigned char NotifyTV=1);    
    
    virtual int GetCurrentActiveSource(unsigned short* pPhyAddr, unsigned char QueryIfUnkonwn=1);               
          
//-------------------------------------------------------------------------------//
// System Stansby
//-------------------------------------------------------------------------------//      
protected:
    
    virtual int SystemStandbyHandler(CEC_MSG* pMsg);    
    
public:    

    virtual int StandbyDevice(unsigned char LogicalAddress);
    
//-----------------------------------------------------------------------------------    
// One Touch Record
//-----------------------------------------------------------------------------------
protected:

    virtual int OneTouchRecordHandler(CEC_MSG* pMsg);

    
//-------------------------------------------------------------------------------//
// Timer Control
//-------------------------------------------------------------------------------//      
protected:
    virtual int TimerProgramingHandler(CEC_MSG* pMsg);                
    
              
//-------------------------------------------------------------------------------//
// System Info
//-------------------------------------------------------------------------------//    
protected:
    struct {
        
        struct {
            unsigned char       State;
            unsigned char       Target;
            CEC_VERSION         Version;
        }Ver;
        
        struct {
            unsigned char       State;
            unsigned char       Target;
            unsigned short      PhyAddr;            
        }Addr;
        
    }m_SI;
    
    virtual int SystemInfoHandler(CEC_MSG* pMsg);

public:    

    virtual int GetDeviceCECVersion(unsigned char LogicalAddr, CEC_VERSION* pVersion);
    
    virtual int GetDevicePhysicalAddress(unsigned char LogicalAddr, unsigned short* pAddr);
    
    virtual int GetDeviceMenuLanguage(unsigned char LogicalAddr);


//-------------------------------------------------------------------------------//
// Deck Control
//-------------------------------------------------------------------------------//
protected:    

    struct {
        unsigned char       StatusRequestor;        
    }m_DC;

    virtual int DeckCtrlHandler(CEC_MSG* pMsg);    
    
//-------------------------------------------------------------------------------//
// Tuner Control
//-------------------------------------------------------------------------------//
protected:    

    virtual int TunerCtrlHandler(CEC_MSG* pMsg);    
    
//-------------------------------------------------------------------------------//
// Vendor Command Status
//-------------------------------------------------------------------------------//
protected:
    
    struct {
        unsigned char       State;
        unsigned char       Target;
        unsigned long       VendorID;        
    }m_VD;

    virtual int VendorSpecificHandler(CEC_MSG* pMsg);
    
public:       
    virtual int GetDeviceVendorID(unsigned char LogicalAddr, unsigned long* pVendorID);
    
    virtual int SendVendorCommand(unsigned char LogicalAddr, unsigned char* pCmd, unsigned long CmdLen);
    
    virtual int SendVendorCommandWithID(unsigned char LogicalAddr, unsigned char* pCmd, unsigned long CmdLen);

//-------------------------------------------------------------------------------//
// Remote Control Pass Through --Send out
//-------------------------------------------------------------------------------//
public:
    virtual int SendUserCommandPressed(unsigned char DestAddr, unsigned char Code);
    virtual int SendUserCommandReleased(unsigned char DestAddr); 
//-------------------------------------------------------------------------------//
// OSD Display
//-------------------------------------------------------------------------------//

protected:

    virtual int OSDDisplayHandler(CEC_MSG* pMsg);    
    
public:
            
    virtual int SendOSDString(CEC_DISPLAY_CTRL Ctrl, const char* String);    
                
    virtual int SendOSDString(CEC_DISPLAY_CTRL Ctrl, const char* String, CEC_DEVICE_TYPE Interface);    

//-------------------------------------------------------------------------------//
// Device OSD Name Transfer
//-------------------------------------------------------------------------------//
protected:
    
    struct {
        unsigned char       State;
        unsigned char       Target;
        unsigned char       OSDName[MAX_OSD_NAME_SIZE];
        unsigned char       OSDNameSize;
    }m_OT;

    virtual int OSDTransferHandler(CEC_MSG* pMsg);
    
public:    
    virtual int GetDeviceOSDName(unsigned char LogicalAddr, unsigned char* pBuff, unsigned char BuffLen, unsigned char* pNameLen);


//-----------------------------------------------------------------------------------    
// Device Menu Control
//-----------------------------------------------------------------------------------
protected:

    virtual int DeviceMenuCtrlHandler(CEC_MSG* pMsg);


//-----------------------------------------------------------------------------------    
// Remote Control
//-----------------------------------------------------------------------------------
protected:
    
    virtual int RemoteControlHandler(CEC_MSG* pMsg);    
    
//-------------------------------------------------------------------------------//
// Power Status
//-------------------------------------------------------------------------------//
protected:
    
    struct {
        unsigned char       State;                
        unsigned char       Target;
        CEC_POWER_STATUS    Status;
    }m_PS;

    virtual int PowerStatusHandler(CEC_MSG* pMsg);
    
public:

    virtual int GetDevicePowerStatus(unsigned char LogicalAddr, CEC_POWER_STATUS* pStatus);

//-----------------------------------------------------------------------------------    
// System Audio Control
//-----------------------------------------------------------------------------------
protected:
     
    struct {
        unsigned char       State;                
        unsigned char       Target;
    }m_SA;           
    
    virtual int SystemAudioControlHandler(CEC_MSG* pMsg);
    
public:
            
    virtual int SetSystemAudioMode(unsigned char mode);
    
    virtual int CheckSystemAudioCapabililty(unsigned char addr, unsigned long timeout);
        
//-----------------------------------------------------------------------------------    
// Audio Rate Control
//-----------------------------------------------------------------------------------
protected:    
    
    virtual int AudioRateControlHandler(CEC_MSG* pMsg);

//-----------------------------------------------------------------------------------    
// Audio Return Channel
//-----------------------------------------------------------------------------------
protected:        
         
    struct {
        unsigned char       State;                
        unsigned char       Target;        
    }m_ARC;           
    
    virtual int AudioReturnChannelControlHandler(CEC_MSG* pMsg);

public:    
       
    virtual int ARCTxControl(CEC_ARC_CTRL Ctrl);
    
    virtual int ARCRxControl(CEC_ARC_CTRL Ctrl);

//------------------------------------------------------------------    
// Misc
//------------------------------------------------------------------
public:
    
    virtual int GetInterfaceAddress(CEC_DEVICE_TYPE type, CEC_LOGICAL_ADDRESS* pLogicalAddress);  
    
//------------------------------------------------------------------    
// Rx Thread Manipulation
//------------------------------------------------------------------
private:
            
    osal_thread_t           m_RxThreadId;
        
    unsigned char           m_RxThreadStatus;        
    
    unsigned long           m_RxPollingInterval;
    
    #define THREAD_ENABLED      0x01    
    #define THREAD_EXIT         0x80    

public:    

    virtual void RxThread();         
    
    void Lock();
    
    void Unlock();              
    
};


#endif  // __CEC_CONTROL_H__


