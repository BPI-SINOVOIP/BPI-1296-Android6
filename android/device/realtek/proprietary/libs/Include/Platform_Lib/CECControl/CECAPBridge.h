#ifndef __CEC_AP_BRIDGE_H__
#define __CEC_AP_BRIDGE_H__

#include "CEC_Def.h"
#include "CEC_Types.h"
#include "CECAdapter.h"
        
class CECAPBridge 
{
public:

    CECAPBridge();
    
    virtual ~CECAPBridge();

public:             
    
    /* AP that supports - One Touch Play */
    virtual int ImageViewOn(unsigned char mode);        
    
    /* AP that supports - Routing Control */
    virtual int GotActiveSource(unsigned short PhyAddr);
    virtual int GotInactiveSource(unsigned short PhyAddr);
    virtual int GotStreamPath(unsigned short PhyAddr);
    
    /* AP that supports - Standby */    
    virtual int Standby();
    
    /* AP that supports - One Touch Record */
    virtual int GetRecordStatus(CEC_RECORD_STATUS_INFO *pStatus);
    virtual int GetScreenSource(CEC_RECORD_SOURCE_ID *pSource);
    virtual int StartRecord(CEC_RECORD_SOURCE_ID *pSource);
    virtual int StopRecord();
    
    /* AP that supports - System Info */ 
    virtual unsigned short GetPhysicalAddress();
    virtual CEC_DEVICE_TYPE GetDeviceType();
    virtual unsigned short  GetDeviceTypes();
    virtual int GetMenuLanguage(CEC_MENU_LANGUAGE* pLanguage);
    virtual int SetMenuLanguage(CEC_MENU_LANGUAGE Language);
    virtual void ReportPhysicalAddress(unsigned short PA, unsigned char LA);

    /* AP that supports - Deck Control */ 
    virtual int GetDeckStatus(CEC_DECK_INFO *DeckInfo);
    virtual int DeckControl(unsigned char Ctrl);
    
    /* AP that supports - Tuner Control */
        
    /* AP that supports - Vendor Specific Feature */
    virtual int GetVendorID(unsigned long* pVendorID);
    virtual int DoVendorCommand(unsigned char Initiator, unsigned char* pCmd, unsigned char CmdLen);
    virtual int DoVendorCommandWithID(unsigned char Initiator, unsigned long VendorID, unsigned char* pCmd, unsigned char CmdLen);
    virtual int DoVendorRemoteButtonDown(unsigned char key);
    
    /* AP that supports - OSD Display */
    virtual int DisplayOSDString(CEC_DISPLAY_CTRL Ctrl, const char* pStr, unsigned char Len);       
    
    /* AP that supports - OSD Transfer */
    virtual int GetOSDName(char* pOSDName, unsigned char* Len);
    
    /* AP that supports - Device Menu Control */
    virtual int GetMenuStatus(CEC_MENU_STATE* pStatus);
    virtual int MenuControl(unsigned char Ctrl);
    
    /* AP that supports - System Audio Control */    
    virtual int GetAudioStatus(CEC_AUDIO_STATUS* pStatus);
    virtual int GetSystemAudioStatus(CEC_SYSTEM_AUDIO_STATUS* pStatus);
    virtual int SetSystemAudioMode(CEC_SYSTEM_AUDIO_STATUS Status);    
    virtual int RequestSystemAudioMode(unsigned char Mode, unsigned short PhyAddr = 0xFFFF);      
    #define SYSTEM_AUDIO_MODE_OFF       0       // Disable System Audio Mode
    #define SYSTEM_AUDIO_MODE_ON        1       // Enable System Audio Mode
    #define SYSTEM_AUDIO_MODE_PREPARE   2       // Going to Enable System Audio Mode but not enabled yet
    
    /* AP that supports - Remote Control */    
    virtual int UserControlPressed(unsigned char key, unsigned char* param, unsigned char param_len);
    
    /* AP that supports - Power Status */
    virtual int GetPowerStatus(CEC_POWER_STATUS* pStatus);
    
    /* AP that supports - Audio Return Channel */ 
    virtual int ARCTxControl(CEC_ARC_CTRL Ctrl);
    virtual int ARCRxControl(CEC_ARC_CTRL Ctrl);
    virtual int GetARCStatus(CEC_ARC_STATUS* pStatus);
        
};

#endif  // __CEC_AP_BRIDGE_H__

