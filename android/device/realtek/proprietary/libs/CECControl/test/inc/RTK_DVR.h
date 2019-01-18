#ifndef __RTK_DVR_H__
#define __RTK_DVR_H__

#include "CEC_Def.h"    
#include "CEC_aux.h"     
#include "CECController.h"               
#include "CECController_Multi.h"          
#include "CECAPBridge.h"                                           
        
class RTKDVRAP
{
public:
    RTKDVRAP();
    ~RTKDVRAP();

private:
    
    CECController*      m_pCEC;
    CECAPBridge*        m_pAPBridge;
    CECAdapter*         m_pAdapter;
    
public:    
    CEC_DEVICE_TYPE     m_DeviceType;
    CEC_MENU_LANGUAGE   m_MenuLanguage;
    char                m_OSDName[MAX_OSD_NAME_SIZE];
    unsigned long       m_VendorID;
    CEC_MENU_STATE      m_MenuStatus;
    CEC_SYSTEM_AUDIO_STATUS  m_SystemAudioStatus;     
    unsigned short      m_PhysicalAddress;
    CEC_POWER_STATUS    m_PowerStatus;
   
    
public:
    
    CECAdapter* GetAdapter() { return m_pCEC->GetCECAdapter(); }        
    void PowerOn(unsigned char OnOff);        
    void UpdatePhyscialAddress(unsigned short PhyAddr);    
    
public:    


//-----------------------------------------------------------------------------------    
// One Touch Play
//-----------------------------------------------------------------------------------
    int SetImageMode(unsigned char mode);
    int OneTouchPlay(unsigned char TextMode, unsigned long wait_time);
            
//-------------------------------------------------------------------------------//
// Routing Control
//-------------------------------------------------------------------------------//            
    
    int ActiveSource();    

    int InactiveSource();    
    
    int GetCurrentActiveSource(unsigned short* pPhyAddr);               
    
    int GotActiveSource(unsigned short PhyAddr);
    
    int GotInactiveSource(unsigned short PhyAddr);
    
    int GotStreamPath(unsigned short PhyAddr);
          
//-------------------------------------------------------------------------------//
// System Stansby
//-------------------------------------------------------------------------------//      

    int StandbyDevice(unsigned char LogicalAddress);
    
//-----------------------------------------------------------------------------------    
// One Touch Record
//-----------------------------------------------------------------------------------
    
//-------------------------------------------------------------------------------//
// Timer Control
//-------------------------------------------------------------------------------//      
              
//-------------------------------------------------------------------------------//
// System Info
//-------------------------------------------------------------------------------//    

    int GetDeviceCECVersion(unsigned char LogicalAddr, CEC_VERSION* pVersion);
    
    int GetDevicePhysicalAddress(unsigned char LogicalAddr, unsigned short* pAddr);
    
    int GetDeviceMenuLanguage(unsigned char LogicalAddr);

    int SetMenuLanguage(CEC_MENU_LANGUAGE Language);    

//-------------------------------------------------------------------------------//
// Deck Control
//-------------------------------------------------------------------------------//
    
//-------------------------------------------------------------------------------//
// Tuner Control
//-------------------------------------------------------------------------------//
    
//-------------------------------------------------------------------------------//
// Vendor Command Status
//-------------------------------------------------------------------------------//
    int GetDeviceVendorID(unsigned char LogicalAddr, unsigned long* pVendorID);    
    
    int DoVendorCommand(unsigned char Initiator, unsigned char* pCmd, unsigned char CmdLen);    
    int DoVendorCommandWithID(unsigned char Initiator, unsigned long VendorID, unsigned char* pCmd, unsigned char CmdLen);        
    int DoVendorRemoteButtonDown(unsigned char key);    
    
//-------------------------------------------------------------------------------//
// OSD Display
//-------------------------------------------------------------------------------//
    int DisplayOSDString(CEC_DISPLAY_CTRL Ctrl, const char* pStr, unsigned char Len);    
    
//-------------------------------------------------------------------------------//
// Device OSD Name Transfer
//-------------------------------------------------------------------------------//
    int GetDeviceOSDName(unsigned char LogicalAddr, unsigned char* pBuff, unsigned char BuffLen, unsigned char* pNameLen);


//-----------------------------------------------------------------------------------    
// Device Menu Control
//-----------------------------------------------------------------------------------

    int GetMenuStatus(CEC_MENU_STATE* pState);
    
    int MenuControl(unsigned char Control);
    
//-----------------------------------------------------------------------------------    
// Remote Control
//-----------------------------------------------------------------------------------
    int UserControlPressed(unsigned char key);
   
//-------------------------------------------------------------------------------//
// Power Status
//-------------------------------------------------------------------------------//
    int GetDevicePowerStatus(unsigned char LogicalAddr, CEC_POWER_STATUS* pStatus);

    void UpdatePowerStatus(CEC_POWER_STATUS Status);    
    
//-----------------------------------------------------------------------------------    
// System Audio Control
//-----------------------------------------------------------------------------------

    int GetSystemAudioStatus(CEC_SYSTEM_AUDIO_STATUS* pStatus)
    {
        *pStatus = m_SystemAudioStatus;
        return CEC_CTRL_OK;          
    }
    
    int SetSystemAudioMode(unsigned char On)
    {
        return m_pCEC->SetSystemAudioMode(On);   
    }
    
    int RequestSystemAudioMode(unsigned char On, unsigned short PhyAddr)
    {
        if (On)
        {
            printf("Zap channel to %04x\n", PhyAddr);
            m_SystemAudioStatus = CEC_SYSTEM_AUDIO_STATUS_ON;            
            printf("System Audio Mode : ON\n");
        }
        else
        {
            m_SystemAudioStatus = CEC_SYSTEM_AUDIO_STATUS_OFF;
            printf("System Audio Mode : OFF\n");
        }

        return CEC_CTRL_OK;   
    }
          
     
//-----------------------------------------------------------------------------------    
// Audio Rate Control
//-----------------------------------------------------------------------------------    
    
    
};



class RTKDVRBridge : public CECAPBridge 
{
public:

    RTKDVRBridge(RTKDVRAP* pAP)
    {
        m_pAP = pAP;
    }
    
    virtual ~RTKDVRBridge(){}
    
private:

    RTKDVRAP* m_pAP;
    CEC_AUDIO_STATUS    m_AudioStatus;
     
public:
    
    int Standby()
    {
        m_pAP->PowerOn(0);
        return CEC_CTRL_OK;
    }
    
    /* AP that supports - One Touch Play */
    int ImageViewOn(unsigned char mode)
    {
        return m_pAP->SetImageMode(mode);
    }
    
    /* AP that supports - Routing Control */
    int GotActiveSource(unsigned short PhyAddr)
    {         
        return m_pAP->GotActiveSource(PhyAddr); 
    }
    
    int GotInactiveSource(unsigned short PhyAddr)
    { 
        return m_pAP->GotInactiveSource(PhyAddr); 
    }
        
    int GotStreamPath(unsigned short PhyAddr)
    { 
        return m_pAP->GotStreamPath(PhyAddr); 
    }
    
    /* AP that supports - One Touch Record */
    int GetRecordStatus(CEC_RECORD_STATUS_INFO *pStatus)
    {
        return CEC_CTRL_UNSUPPORTED_FEATURE;
    }

    int GetScreenSource(CEC_RECORD_SOURCE_ID *pSource)
    {
        return CEC_CTRL_UNSUPPORTED_FEATURE;
    }

    int StartRecord(CEC_RECORD_SOURCE_ID *pSource)
    {
        return CEC_CTRL_UNSUPPORTED_FEATURE;
    }

    int StopRecord()
    {
        return CEC_CTRL_UNSUPPORTED_FEATURE;
    }
    
    /* AP that supports - System Info */     
    unsigned short GetPhysicalAddress()
    {
        return m_pAP->m_PhysicalAddress;
    }

    CEC_DEVICE_TYPE GetDeviceType()
    {        
        return m_pAP->m_DeviceType;    
    }
    
    unsigned short GetDeviceTypes()
    {        
        return CEC_DEVICE_TYPE_BIT(CEC_DEVICE_PLAYBACK_DEVICE)  | 
               CEC_DEVICE_TYPE_BIT(CEC_DEVICE_AUDIO_SYSTEM) ;    
    }    

    int GetMenuLanguage(CEC_MENU_LANGUAGE* pLanguage)
    {
        *pLanguage = m_pAP->m_MenuLanguage;
        return CEC_CTRL_OK;
    }

    int SetMenuLanguage(CEC_MENU_LANGUAGE Language)
    {
        return m_pAP->SetMenuLanguage(Language);
    }

    /* AP that supports - Deck Control */ 
    int GetDeckStatus(CEC_DECK_INFO *DeckInfo)
    {    
        return CEC_CTRL_UNSUPPORTED_FEATURE;
    }
    
    int DeckControl(unsigned char Ctrl)
    {     
        return CEC_CTRL_UNSUPPORTED_FEATURE;
    }
    
    /* AP that supports - Tuner Control */
        
    /* AP that supports - Vendor Specific Feature */
    int GetVendorID(unsigned long* pVendorID)
    {
        *pVendorID = m_pAP->m_VendorID;
        return CEC_CTRL_OK;
    }

    int DoVendorCommand(unsigned char Initiator, unsigned char* pCmd, unsigned char CmdLen)
    {        
        return m_pAP->DoVendorCommand(Initiator, pCmd, CmdLen);        
    }

    int DoVendorCommandWithID(unsigned char Initiator, unsigned long VendorID, unsigned char* pCmd, unsigned char CmdLen)
    {
        return m_pAP->DoVendorCommandWithID(Initiator, VendorID, pCmd, CmdLen);        
    }
        
    int DoVendorRemoteButtonDown(unsigned char key)
    {
        return m_pAP->DoVendorRemoteButtonDown(key);
    }
    
    /* AP that supports - OSD Display */
    int DisplayOSDString(CEC_DISPLAY_CTRL Ctrl, const char* pStr, unsigned char Len)
    {
        return m_pAP->DisplayOSDString(Ctrl,pStr, Len);
    }
    
    /* AP that supports - OSD Transfer */
    int GetOSDName(char* pOSDName, unsigned char* pLen)
    {
         *pLen = strlen(m_pAP->m_OSDName);
         memcpy(pOSDName, m_pAP->m_OSDName, *pLen);                   
        return CEC_CTRL_OK;
    }
    
    /* AP that supports - Device Menu Control */
    int GetMenuStatus(CEC_MENU_STATE* pStatus)
    {
        *pStatus = m_pAP->m_MenuStatus;
        return CEC_CTRL_OK;
    }

    int MenuControl(unsigned char Ctrl)
    {
        return m_pAP->MenuControl(Ctrl);
    }
    
    /* AP that supports - Remote Control */
    int UserControlPressed(unsigned char key)
    {
        return m_pAP->UserControlPressed(key);
    }
    
    /* AP that supports - Power Status */
    int GetPowerStatus(CEC_POWER_STATUS* pStatus)
    {
        *pStatus = m_pAP->m_PowerStatus;
        return CEC_CTRL_OK;
    }
    
    int GetAudioStatus(CEC_AUDIO_STATUS* pStatus)
    {
        pStatus->Mute = 0;
        pStatus->Volume = 20;
        return CEC_CTRL_OK;
    }
    
    int GetSystemAudioStatus(CEC_SYSTEM_AUDIO_STATUS* pStatus)
    {
        return m_pAP->GetSystemAudioStatus(pStatus);   
    }
    
    int SetSystemAudioMode(CEC_SYSTEM_AUDIO_STATUS Status)
    {      
        return m_pAP->SetSystemAudioMode(Status);   
    }
       
    int RequestSystemAudioMode(unsigned char On, unsigned short PhyAddr = 0xFFFF)
    {        
        return m_pAP->RequestSystemAudioMode(On, PhyAddr);
    }
};

#endif  // __RTK_DVR_H__

