#ifndef _CECAPBRIDGEIMPL_H
#define _CECAPBRIDGEIMPL_H

#include <Platform_Lib/CECControl/CECController_Multi.h>
#include <Platform_Lib/CECControl/CECController.h>
#include <Platform_Lib/CECControl/CECAPBridge.h>
#include <Platform_Lib/HDMIControl/hdmi.h>
#include <utils/KeyedVector.h>
//#include <Platform_Lib/Graphics/user_cmd.h>


typedef enum {    
    CEC_CFG,
    CEC_CFG_SYSTEM_AUDIO_CONTROL,
//    CEC_CFG_RESCAN_PHY_ADDR,    //eastech 
}CEC_CFG_ITEM;

typedef enum {
	STATE_SOURCE_MPEG,
	STATE_SOURCE_ARC,
}SOURCE_STATE;		    

class CECAPBridgeImpl : public CECAPBridge
{
public:
/*
    virtual Call back function
*/
    CECAPBridgeImpl();
    virtual ~CECAPBridgeImpl(void);

private:
    static unsigned char    m_BootUpOneTouchPlay;        
    static unsigned short   m_PhyAddr;        
    CEC_POWER_STATUS        m_PowerState;
    static CEC_SYSTEM_AUDIO_STATUS     m_SystemAudioStatus;    
    static CEC_ARC_STATUS   m_ARCStatus;
	static CECController*   m_pCECController;
	char                    SLanguage[255];
    int                     m_fdRemote;
    android::KeyedVector<unsigned short, unsigned char> m_LAList;
	
	struct {
	    unsigned char       cec_enabled : 1;        
        unsigned char       wait_tv_language_retry;  
        unsigned char       wait_tv_power_on_retry;        
	}m_Status;

public:
    // evneu
    static int CallBack_hdmi_event(bool PlugIn);
    void CECConfigUpdate(CEC_CFG_ITEM item);
	time_t m_PreviousStandbyT;

/*--------------------------------------------------    
 * CEC AP Thread
 --------------------------------------------------*/
private:
    osal_thread_t           m_APThread;
    unsigned char           m_APThreadExit;
   	CEC_AUDIO_STATUS        m_audio_status;
	int 					m_System_Source; 
public:        
    void Run();
    
/*--------------------------------------------------    
 * Control
 --------------------------------------------------*/
    
    int Enable(unsigned char On);


/*--------------------------------------------------    
 * CEC AP Bridge Callback
 --------------------------------------------------*/        
public:     
        
    /* AP that supports - Routing Control */
	virtual int GotActiveSource(unsigned short PhyAddr);
	virtual int GotStreamPath(unsigned short PhyAddr);
	        
    /* AP that supports - Standby */  
    virtual int Standby();
    
    /* AP that supports - System Info */     
    virtual unsigned short GetPhysicalAddress();
    virtual CEC_DEVICE_TYPE GetDeviceType();
    virtual unsigned short GetDeviceTypes();
    
    virtual int GetMenuLanguage(CEC_MENU_LANGUAGE* pLanguage);
    virtual int SetMenuLanguage(CEC_MENU_LANGUAGE Language);
    virtual void ReportPhysicalAddress(unsigned short PA, unsigned char LA);
    
    /* AP that supports - Vendor Specific Feature */    
    virtual int GetVendorID(unsigned long* pVendorID);
    
    /* AP that supports - OSD Transfer */    
    virtual int GetOSDName(char* pOSDName, unsigned char* Len);

    /* AP that supports - Device Menu Control */
    virtual int GetMenuStatus(CEC_MENU_STATE* pStatus);
    virtual int MenuControl(unsigned char Ctrl);
    
    /* AP that supports - System Audio Control */    
    virtual int GetAudioStatus(CEC_AUDIO_STATUS * pStatus);
    virtual int GetSystemAudioStatus(CEC_SYSTEM_AUDIO_STATUS * pStatus);
    virtual int RequestSystemAudioMode(unsigned char On, unsigned short PhyAddr);    
    
    /* AP that supports - Remote Control */   
    virtual int UserControlPressed(unsigned char key, unsigned char* param, unsigned char param_len);
    
    /* AP that supports - Power Status */    
    virtual int GetPowerStatus(CEC_POWER_STATUS* pStatus);

    /* AP that supports - Audio Return Channel */ 
    virtual int ARCRxControl(CEC_ARC_CTRL Ctrl);
    virtual int GetARCStatus(CEC_ARC_STATUS* pStatus);

/*
    for AP launch Action
*/
    int doOneTouchPlay(int Intensity);
    int SendRemotePressed(unsigned char DevAddr, unsigned char Key, bool SendReleased);
    unsigned char GetLAOf(unsigned char DevAddr);
    int doUpdatePhysicalAddress(unsigned short PhysicalAddress);  
    int doUpdatePowerState(CEC_POWER_STATUS pStatus);
    int SetStandbyMode();
    int doAllSystemStandby(void);
	char *GetLanguage();
    void resetCecLanguage();
    void InitCecLanguage();
    void AskMenuLanguage();
    
    void ReportAudioStatus();
	void SendOSDString(const char* String);
	void RequestSwitchSource(SOURCE_STATE src);
	void DisablePlayerActiveSource();
    void write_key_event(int fdRemote, int keycode, int downOrUp);
//from HDMI Controller    
private:

	void RequestAudioMode(bool onoff);
    int SystemAudioControl(unsigned char On, unsigned short PhyAddr);

//Interface to info up level 
public :
    void RegCallback_SetMenuLanguage(int (*UpHandler)(CEC_MENU_LANGUAGE));
    void RegCallback_GetSystemStandby(void (*UpHandler)());
private:
    static int (*InfoSetMenuLanguage)(CEC_MENU_LANGUAGE);
    static void (*InfoGetSystemStandby)();

       
/*--------------------------------------------------    
 * Misc Function
 --------------------------------------------------*/   
private:

    int BinarySearch(int *array,int array_num,int findKey);
    int GetAndroidCmdFromCecCode(CEC_USER_CTRL_CODE cec_code);
	//unsigned short getHDMISourceAddress(); 
};

#endif



