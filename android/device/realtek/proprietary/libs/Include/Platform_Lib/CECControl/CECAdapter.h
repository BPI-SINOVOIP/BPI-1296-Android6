#ifndef __CECAdapter_H__
#define __CECAdapter_H__

#include "CEC_Def.h"
#include "CEC_Types.h"

#define PHY_ENABLE          0x00
#define PHY_LOGICAL_ADDR    0x01
#define PHY_PHYSICAL_ADDR   0x02
#define PHY_POWER_STATUS    0x03            
#define PHY_STANDBY_MODE    0x04
#define PHY_RX_MASK         0x05
#define PHY_CEC_VERSION     0x07
#define PHY_DEVICE_VENDOR_ID  0x08
class CECAdapter 
{
public:

    CECAdapter();
    
    virtual ~CECAdapter();
    
public:        

    //-------------------------------------------------------------------------------//
    // Hardware Dependent Layer
    //-------------------------------------------------------------------------------//
    
    virtual int Init();            
    virtual int UnInit();                    
    virtual int SendMessage(const CEC_MSG* pMsg);
    virtual int ReadMessage(CEC_MSG* pMsg);    
    virtual int SetHW(unsigned char opcode, unsigned long val);    
    //-------------------------------------------------------------------------------//
    // CEC Command Helper
    //-------------------------------------------------------------------------------//
                             
public:
                                  
    //-------------------------------------------------------------------------------//
    // One Touch Play
    //-------------------------------------------------------------------------------//           
    virtual int SendImageViewOn             (unsigned char              InitAddr);    
    
    virtual int SendTextViewOn              (unsigned char              InitAddr);
    
    //-------------------------------------------------------------------------------//
    // Routing Info
    //-------------------------------------------------------------------------------//    
    virtual int SendActiveSource            (unsigned char              InitAddr,
                                             unsigned short             PhyAdd);
    
    virtual int SendInactiveSource          (unsigned char              InitAddr,
                                             unsigned short             Addr);
    
    virtual int SendRequestActiveSource     (unsigned char              InitAddr);
     
    virtual int SendSetStreamPath           (unsigned char              InitAddr,
                                             unsigned short             Addr);
    
    virtual int SendRoutingChange           (unsigned char              InitAddr,
                                             unsigned short             OldAddr, 
                                             unsigned short             NewAddr);
    
    virtual int SendRoutingInfo             (unsigned char              InitAddr,
                                             unsigned short             ActivatedAddr);
    
    //-------------------------------------------------------------------------------//
    // System Standby
    //-------------------------------------------------------------------------------//
    virtual int SendSystemStandby           (unsigned char              InitAddr,
                                             unsigned char              DestAddr);
           
    //-------------------------------------------------------------------------------//
    // Record Control
    //-------------------------------------------------------------------------------//            
    virtual int SendRecordOn                (unsigned char                  InitAddr,
                                             unsigned char                  DestAddr, 
                                             const CEC_RECORD_SOURCE_ID*    pSource);
    
    virtual int SendRecordOff               (unsigned char                  InitAddr,
                                             unsigned char                  DestAddr);
     
    virtual int SendRecordTvScreen          (unsigned char                  InitAddr,
                                             unsigned char                  DestAddr);
    
    virtual int SendRecordStatus            (unsigned char                  InitAddr,
                                             unsigned char                  DestAddr, 
                                             CEC_RECORD_STATUS_INFO         status);
    
    //-------------------------------------------------------------------------------//
    // Timer Control
    //-------------------------------------------------------------------------------//
    
    virtual int SendClearAnalogueTimer      (unsigned char                  InitAddr,
                                             unsigned char                  DestAddr,     
                                             const CEC_TIMER_INFO*          pTimer,    
                                             const CEC_ANALOG_SERVICE_ID*   pServiceID);
    
    virtual int SendSetAnalogueTimer        (unsigned char                  InitAddr,
                                             unsigned char                  DestAddr,     
                                             const CEC_TIMER_INFO*          pTimer,    
                                             const CEC_ANALOG_SERVICE_ID*   pServiceID);
     
    virtual int SendClearExternalTimer      (unsigned char                  InitAddr,
                                             unsigned char                  DestAddr, 
                                             const CEC_TIMER_INFO*          pTimer,
                                             const CEC_EXTERNAL_SOURCE_ID*  pSourceID);
    
    virtual int SendSetExternalTimer        (unsigned char                  InitAddr,
                                             unsigned char                  DestAddr,         
                                             const CEC_TIMER_INFO*          pTimer,
                                             const CEC_EXTERNAL_SOURCE_ID*  pSourceID);
    
    virtual int SendClearDigitalTimer       (unsigned char                  InitAddr,
                                             unsigned char                  DestAddr,     
                                             const CEC_TIMER_INFO*          pTimer,
                                             const CEC_DIGITAL_SERVICE_ID*  pServiceID);
    
    virtual int SendSetDigitalTimer         (unsigned char                  InitAddr,
                                             unsigned char                  DestAddr, 
                                             const CEC_TIMER_INFO*          pTimer,    
                                             const CEC_DIGITAL_SERVICE_ID*  pServiceID);
    
    virtual int SendSetTimerProgramTitle    (unsigned char                  InitAddr,
                                             unsigned char                  DestAddr, 
                                             const char*                    pStr,
                                             unsigned char                  Length);
    
    virtual int SendTimerClearedStatus      (unsigned char                  InitAddr,
                                             unsigned char                  DestAddr, 
                                             CEC_TIMER_CLEAR_STATUS_DATA    Status);
    
    virtual int SendTimerStatus             (unsigned char                  InitAddr,
                                             unsigned char                  DestAddr,         
                                             CEC_TIMER_OVERLAP_WARNING      Overlap,    
                                             CEC_TIMER_MEDIA_INFO           MediaInfo,    
                                             CEC_PROGRAMED_INDICATOR        Indicator,    
                                             unsigned char                  Info,
                                             const CEC_DURATION*            pDuration);
    
    
    //-------------------------------------------------------------------------------//
    // System Info
    //-------------------------------------------------------------------------------//    
    virtual int SendGivePhysicalAddress     (unsigned char              InitAddr,
                                             unsigned char              DestAddr);
    
    virtual int SendReportPhysicalAddress   (unsigned char              InitAddr,
                                             unsigned short             PhysicalAddr,
                                             CEC_DEVICE_TYPE            Type);
    
    virtual int SendCECVersion              (unsigned char              InitAddr,
                                             unsigned char              DestAddr,
                                             CEC_VERSION                Version);    
    
    virtual int SendGetCECVersion           (unsigned char              InitAddr,
                                             unsigned char              DestAddr);
    
    virtual int SendGetMenuLanguage         (unsigned char              InitAddr,
                                             unsigned char              DestAddr);
    
    virtual int SendSetMenuLanguage         (unsigned char              InitAddr,
                                             CEC_MENU_LANGUAGE          Language);
    
    virtual int SendPoll                    (unsigned char              InitAddr,
                                             unsigned char              DestAddr);                             
    
    //-------------------------------------------------------------------------------//
    // Deck Control Command 
    //-------------------------------------------------------------------------------//        
    virtual int SendDeckControl             (unsigned char              InitAddr,
                                             unsigned char              DestAddr, 
                                             CEC_DECK_CTRL_MODE         Mode);
    
    virtual int SendDeckPlay                (unsigned char              InitAddr,
                                             unsigned char              DestAddr, 
                                             CEC_PLAY_MODE              Mode);
    
    virtual int SendDeckStatus              (unsigned char              InitAddr,
                                             unsigned char              DestAddr, 
                                             CEC_DECK_INFO              Status);
    
    virtual int SendGiveDeckStatus          (unsigned char              InitAddr,
                                             unsigned char              DestAddr, 
                                             CEC_STATUS_REQUEST         Request);
    
    //-------------------------------------------------------------------------------//
    // Tuner Control Command 
    //-------------------------------------------------------------------------------//    
    virtual int SendGiveTunerDeviceStatus   (unsigned char                  InitAddr,
                                             unsigned char                  DestAddr, 
                                             CEC_STATUS_REQUEST             Request);
    
    virtual int SendTunerDeviceStatus       (unsigned char                  InitAddr,
                                             unsigned char                  DestAddr, 
                                             const CEC_TUNER_DEVICE_INFO*   pInfo);
    
    virtual int SendSelectAnalogueService   (unsigned char                  InitAddr,
                                             unsigned char                  DestAddr, 
                                             const CEC_ANALOG_SERVICE_ID*   pServiceID);
    
    virtual int SendSelectDigitalService    (unsigned char                  InitAddr,
                                             unsigned char                  DestAddr, 
                                             const CEC_DIGITAL_SERVICE_ID*  pServiceID);
       
    virtual int SendTunerStepDecrement      (unsigned char                  InitAddr,
                                             unsigned char                  DestAddr);
        
    virtual int SendTunerStepIncrement      (unsigned char                  InitAddr,
                                             unsigned char                  DestAddr);
    
    //-------------------------------------------------------------------------------//
    // Vendor Command Status
    //-------------------------------------------------------------------------------//                                             
    virtual int SendGiveDeviceVendorID      (unsigned char              InitAddr,
                                             unsigned char              DestAddr);
    
    virtual int SendVendorCommand           (unsigned char              InitAddr,
                                             unsigned char              DestAddr, 
                                             unsigned char*             pData, 
                                             unsigned char              DataLength);
    
    virtual int SendVendorCommandWithID     (unsigned char              InitAddr,
                                             unsigned char              DestAddr,
                                             unsigned long              VendorID, 
                                             unsigned char*             pData, 
                                             unsigned char              DataLength);
        
    virtual int SendVendorRemoteButtonDown  (unsigned char              InitAddr,
                                             unsigned char              DestAddr, 
                                             unsigned char              RCCode);
    
    virtual int SendVendorRemoteButtonUp    (unsigned char              InitAddr,
                                             unsigned char              DestAddr);
                                                                    
    virtual int SendDeviceVendorID          (unsigned char              InitAddr,
                                             unsigned long              VendorID);                                            
    
    //-------------------------------------------------------------------------------//
    // Device OSD Status
    //-------------------------------------------------------------------------------//
    virtual int SendSetOSDString            (unsigned char              InitAddr,
                                             unsigned char              DestAddr, 
                                             CEC_DISPLAY_CTRL           Ctrl,
                                             const void*                Str, 
                                             unsigned char              Length);
        
    //-------------------------------------------------------------------------------//
    // Device OSD Name Transfer
    //-------------------------------------------------------------------------------//                                     
    virtual int SendSetOSDName              (unsigned char              InitAddr,
                                             unsigned char              DestAddr,
                                             const char*                OSDName,
                                             unsigned char              Length);
      
    virtual int SendGiveOSDName             (unsigned char              InitAddr,
                                             unsigned char              DestAddr);
    
    //-------------------------------------------------------------------------------//
    // Device Menu Control
    //-------------------------------------------------------------------------------//                                         
    virtual int SendMenuRequest             (unsigned char              InitAddr,
                                             unsigned char              DestAddr, 
                                             CEC_MENU_REQUEST_TYPE      Request);
    
    virtual int SendMenuStatus              (unsigned char              InitAddr,
                                             unsigned char              DestAddr, 
                                             CEC_MENU_STATE             State);    
    //-------------------------------------------------------------------------------//
    // Remote Control
    //-------------------------------------------------------------------------------//
    virtual int SendUserCommandPressed      (unsigned char              InitAddr,
                                             unsigned char              DestAddr,     
                                             unsigned char              Code);
                                         
    virtual int SendUserCommandReleased     (unsigned char              InitAddr,
                                             unsigned char              DestAddr);
    
    //-------------------------------------------------------------------------------//
    // Power Status
    //-------------------------------------------------------------------------------//    
                
    virtual int SendGiveDevicePowerStatus   (unsigned char              InitAddr,
                                             unsigned char              DestAddr);
    
    virtual int SendReportPowerStatus       (unsigned char              InitAddr,
                                             unsigned char              DestAddr,
                                             CEC_POWER_STATUS           PowerStatus);
    
    //-------------------------------------------------------------------------------//
    // General Protocol
    //-------------------------------------------------------------------------------//
    virtual int SendFeatureAbort            (unsigned char              InitAddr,
                                             unsigned char              DestAddr, 
                                             CEC_MSG_OPCODE             Opcode, 
                                             CEC_ABORT_REASON           Reason);
    
    //-------------------------------------------------------------------------------//
    // Audio System Functions
    //-------------------------------------------------------------------------------//        
    virtual int SendGiveAudioStatus         (unsigned char              InitAddr,
                                             unsigned char              DestAddr);
    
    virtual int SendGiveSystemAudioStatus   (unsigned char              InitAddr,
                                             unsigned char              DestAddr);
    
    virtual int SendReportAudioStatus       (unsigned char              InitAddr,
                                             unsigned char              DestAddr,
                                             CEC_AUDIO_MUTE_STATUS      Mute,         
                                             unsigned char              Volume);
    
    virtual int SendSetSystemAudioMode      (unsigned char              InitAddr,
                                             unsigned char              DestAddr,     
                                             CEC_SYSTEM_AUDIO_STATUS    Status);
    
    virtual int SendSystemAudioModeRequest  (unsigned char              InitAddr,
                                             unsigned char              DestAddr,     
                                             unsigned short             Addr);
     
    virtual int SendSystemAudioModeStatus   (unsigned char              InitAddr,
                                             unsigned char              DestAddr,     
                                             CEC_SYSTEM_AUDIO_STATUS    Status);
     
    virtual int SendSetAudioRate            (unsigned char              InitAddr,
                                             unsigned char              DestAddr, 
                                             CEC_AUDIO_RATE             Rate);          
                                             
    //-------------------------------------------------------------------------------//
    // Audio Return Channel Control Functions
    //-------------------------------------------------------------------------------//        
    virtual int SendInitiateARC             (unsigned char              InitAddr,
                                             unsigned char              DestAddr);
    
    virtual int SendTerminateARC            (unsigned char              InitAddr,
                                             unsigned char              DestAddr);
    
    virtual int SendRequestARCInitiation    (unsigned char              InitAddr,
                                             unsigned char              DestAddr);
    
    virtual int SendRequestARCTermination   (unsigned char              InitAddr,
                                             unsigned char              DestAddr);
    
    virtual int SendReportARCInitiated      (unsigned char              InitAddr,
                                             unsigned char              DestAddr);
    
    virtual int SendReportARCTerminated     (unsigned char              InitAddr,
                                             unsigned char              DestAddr);                                                                                        
};


#endif  // __CECAdapter_H__


