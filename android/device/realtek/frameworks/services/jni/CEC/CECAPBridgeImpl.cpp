#include <android/log.h>
#include <utils/Log.h>
#include <linux/input.h>
#include "Platform_Lib/HDMIControl/hdmi.h"
#include "CEC_Def.h"    
#include "CEC_aux.h" 
#include "CEC_Debug.h"     
#include "CECController_Multi.h"
#include "CECAPBridgeImpl.h"
#include "CECAPBridgeImplPriv.h"
//#include <Application/AppClass/SetupClass.h>
#include <Application/AppClass/SetupClassAndroid.h>

#define TAG "CECApBridgeImpl: "
/////////////////////////////////////////////////////////////
#define MAX_WAIT_TV_POWER_ON_RETRY          10
#define TV_POWER_STATUS_CHECK_INTERVAL      50      // unit : 100ms
#define MAX_WAIT_TV_MENU_LANGUAGE_RETRY     5
#define TV_MENU_LANGUAGE_CHECK_INTERVAL     30      // unit : 100ms
/////////////////////////////////////////////////////////////
static unsigned char        HDMIThreadEvent = 0;
//#define PBC_STATUS_FG	MAKE_RGB(0, 0xff, 0)//MAKE_RGB(0x9b, 0x6a, 0x45)
//#define PBC_STATUS_BG	RESERVED_COLOR_KEY//MAKE_RGB(0xff, 0xff, 0xff)

int (*(CECAPBridgeImpl::InfoSetMenuLanguage))(CEC_MENU_LANGUAGE);
void (*(CECAPBridgeImpl::InfoGetSystemStandby))();

unsigned short CECAPBridgeImpl::m_PhyAddr=PHYSICAL_ADDR_NULL;
CEC_ARC_STATUS CECAPBridgeImpl::m_ARCStatus= CEC_ARC_STATUS_TERMINATE;
unsigned char  CECAPBridgeImpl::m_BootUpOneTouchPlay=0;
CEC_SYSTEM_AUDIO_STATUS CECAPBridgeImpl::m_SystemAudioStatus = CEC_SYSTEM_AUDIO_STATUS_OFF;        

CECController *CECAPBridgeImpl::m_pCECController=NULL;

static void HDMIDetectThread(void *Param)
{        
    ((CECAPBridgeImpl*)Param)->Run();                                 
}



/*------------------------------------------------------------------- 
 * Func : CECAPBridgeImpl 
 *
 * Desc : Constructor of CEC AP Bridge
 *
 * Parm : N/A
 *
 * Retn : N/A
 *-------------------------------------------------------------------*/ 
CECAPBridgeImpl::CECAPBridgeImpl()
{
	SetupClass::SetInstance(new SetupClassAndroid());
    int logicAddr = SetupClass::GetInstance()->GetCECPreferredLogicalAddress();
    logicAddr = logicAddr!=15?logicAddr:4;//If logicAddr is 15, give a default address :4 (Playback Device 1);
    
    m_fdRemote = open(DEV_REMOTE, O_WRONLY);
    if (m_fdRemote == -1)
    {
	    CEC_AP_INFO("Could not open REMOTE DEV\n");
    }
    m_BootUpOneTouchPlay = 0;
    m_PowerState         = CEC_POWER_STATUS_ON; 
    m_Status.cec_enabled = 0;    
    m_Status.wait_tv_language_retry = 0;     
    m_Status.wait_tv_power_on_retry = 0;
    m_pCECController     = new MultiDeviceCECController(this, 1<<logicAddr);
	m_System_Source = STATE_SOURCE_MPEG;
    m_PreviousStandbyT = 0; 
    
    //CEC_AP_INFO("Start HDMI Detect Thread\n");
    ALOGD(TAG "Start HDMI Detect Thread\n");
    HDMIThreadEvent = 0;
    m_APThreadExit  = 0;
    m_LAList.clear();
    osal_ThreadBegin(HDMIDetectThread, this, 0, 0, &m_APThread);    
    
    //Register_HdmiEvent_Func(CallBack_hdmi_event);
    resetCecLanguage();
    InitCecLanguage();//Remember CEC Language For TV doesn't support CEC menu language.
	        
    //SystemAudioControl(0, 0xf);     //Default :Amplifier will output audio
    
    CECConfigUpdate(CEC_CFG);       // update cec status, this will triger cec to start 
}



/*------------------------------------------------------------------- 
 * Func : ~CECAPBridgeImpl 
 *
 * Desc : Destructor of CEC AP Bridge
 *
 * Parm : N/A
 *
 * Retn : N/A
 *-------------------------------------------------------------------*/ 
CECAPBridgeImpl::~CECAPBridgeImpl()
{
    m_APThreadExit = 1;
    close(m_fdRemote);
    osal_ThreadEnd(&m_APThread, -1);             

    if(m_pCECController)
    {
        unsigned short LastLA = m_pCECController->GetLastLogicalAddresses();

        SetupClass::GetInstance()->SetCECPreferredLogicalAddress(LastLA);
         
	    m_pCECController->Enable(0);    
	    
        delete m_pCECController;

    }
	m_PhyAddr=PHYSICAL_ADDR_NULL;
    m_LAList.clear();
	SetupClass::DeleteInstance();
}



/*------------------------------------------------------------------- 
 * Func : CECConfigUpdate 
 *
 * Desc : Update CEC Configuration. Setup will issue this once the 
 *        setup is changed.
 *
 * Parm : item : which item has been changed
 *        CEC_CFG : CEC has beed disabled / enabled
 *        CEC_CFG_SYSTEM_AUDIO_CONTROL : System Audio Mode has been enable / disabled
 *
 * Retn : N/A
 *-------------------------------------------------------------------*/ 
void CECAPBridgeImpl::CECConfigUpdate(CEC_CFG_ITEM item)
{
    switch(item)
    {
	case CEC_CFG:
		//Setup will issue this once the setup is changed
		Enable(SetupClass::GetInstance()->GetEnableHdmiCec() ? 1: 0);		
		break;
		
    case CEC_CFG_SYSTEM_AUDIO_CONTROL:
        
        CEC_AP_INFO("CECConfigUpdate(CEC_CFG_SYSTEM_AUDIO_CONTROL | %d), CurStatus=%d\n", 
                SetupClass::GetInstance()->GetSystemAudioControl(),
                m_SystemAudioStatus);
                
        if (!SetupClass::GetInstance()->GetSystemAudioControl() && m_SystemAudioStatus==CEC_SYSTEM_AUDIO_STATUS_ON)        
        {            
            SystemAudioControl(0,0xf);
            m_pCECController->SetSystemAudioMode(0);
        }
        
        unsigned short ActiveSource;
        m_pCECController->GetCurrentActiveSource(&ActiveSource); 
        doUpdatePhysicalAddress(getHDMISourceAddress());               
        if (ActiveSource==m_PhyAddr)            
            m_pCECController->OneTouchPlay(ACTIVE_SOURCE_ONLY);
        break;  
    }
}




/*------------------------------------------------------------------- 
 * Func : Run 
 *
 * Desc : Main Thread for CEC AP Bridge
 *
 * Parm : N/A
 *
 * Retn : N/A
 *-------------------------------------------------------------------*/ 
void CECAPBridgeImpl::Run()
{
    unsigned long PollTvLanguageInterval = 0;
    unsigned long PollTvPowerOnInterval  = 0;
    
    while(!m_APThreadExit)           
    {        
        if (!HDMIThreadEvent) 
        {
            //CEC_AP_DBG("HDMIDetectThread alive\n");  
            usleep(100000);
            
            if (m_PhyAddr==PHYSICAL_ADDR_NULL)
                continue;
                        
            if (m_Status.wait_tv_power_on_retry)
            {                
                if (PollTvPowerOnInterval++ >= TV_POWER_STATUS_CHECK_INTERVAL)                    
                {
                    CEC_POWER_STATUS PowerStatus;

                    m_Status.wait_tv_power_on_retry--;
                    PollTvPowerOnInterval = 0;
                    
                    if (m_pCECController->GetDevicePowerStatus(0, &PowerStatus)==CEC_CTRL_OK)
                    {
                        CEC_AP_INFO("TV Power Status = %s\n", PowerStatusName(PowerStatus));
                    
                        if (PowerStatus==CEC_POWER_STATUS_ON)
                        {
                            m_pCECController->OneTouchPlay(ACTIVE_SOURCE_ONLY);
                            m_Status.wait_tv_power_on_retry = 0;
                        }
                    }                                                    
                }                                            
            }                     
            
            if (m_Status.wait_tv_language_retry)
            {            
                if (PollTvLanguageInterval++ >= TV_MENU_LANGUAGE_CHECK_INTERVAL)
                {
                    PollTvLanguageInterval = 0;
                    m_Status.wait_tv_language_retry--;

                    if (m_pCECController->GetDeviceMenuLanguage(0)!=CEC_CTRL_OK)
                        m_Status.wait_tv_language_retry = 0;      // TV does not support manulanguage                             
                }
            }
            
            continue;
        }
        
        if (m_PowerState!=CEC_POWER_STATUS_ON)
        {
            usleep(100000);
            continue;
        }
   
        HDMIThreadEvent = 0;        
        CEC_AP_INFO("*******************************************\n ");
        CEC_AP_INFO("Got HDMIThreadEvent, getHDMISourceAddress\n ");
        doUpdatePhysicalAddress(getHDMISourceAddress());     // Update Physical Address        
        CEC_AP_INFO("*******************************************\n ");
    }        
}    



 
/*------------------------------------------------------------------- 
 * Func : Enable 
 *
 * Desc : Enable / Disable CEC 
 *
 * Parm : On
 *
 * Retn : N/A
 *-------------------------------------------------------------------*/ 
int CECAPBridgeImpl::Enable(unsigned char On)
{
    if(On)
	{
	    if (!m_Status.cec_enabled)
	    {	        
		    m_PowerState = CEC_POWER_STATUS_ON; 
		    m_pCECController->UpdateStatus(CEC_CTRL_POWER_STATUS);
		    m_pCECController->Enable(1);
		    SetStandbyMode();
            HDMIThreadEvent = 1;    
		    m_Status.cec_enabled = 1;
	    }
	}
	else
	{
	    if (m_Status.cec_enabled)
	    {    		    		
    		m_pCECController->SetStandbyMode(0);
    		m_pCECController->Enable(0); 	        // turn off cec directly...
		    m_Status.cec_enabled = 0;  
		    m_PhyAddr = 0xFFFF;	    			
	    }
	}   
	
	return 0;
}



/*--------------------------------------------------------------------
 *                AP that supports - Standby                         *
 *-------------------------------------------------------------------*/
 

/*------------------------------------------------------------------- 
 * Func : Standby 
 *
 * Desc : this function will be invoked when CEC controller received <Standby> message
 *
 * Parm : N/A
 *
 * Retn : CEC_CTRL_OK
 *-------------------------------------------------------------------*/ 
int CECAPBridgeImpl::Standby()
{
	time_t IssueTime;
	bool IssueStandby = true;
    CEC_AP_INFO("\n\n\n[%s,%s,%d] Get Sytem Standby \n\n\n\n",__FILE__,__func__,__LINE__);
    if(SetupClass::GetInstance()->GetEnableHdmiCec() &&
        SetupClass::GetInstance()->GetEnableHdmiCecSystemStandby())
    {
		//That is because some TV will send CEC standby twice in short time. Panasonic TV will send out twice CEC standby in 2 second.
		if(m_PreviousStandbyT>0)
		{
			time(&IssueTime);
			if (IssueTime - m_PreviousStandbyT <5)
			{	
				IssueStandby = false;
			}
		}
		if (IssueStandby == true)
		{
			//Wen:WEN_TODO : Ask system go to standby.One callback API is required.
            if (InfoGetSystemStandby !=NULL)
            {
                InfoGetSystemStandby();
            }
            else
            {
                CEC_AP_WARNING("Can not feedback SetMenuLanguage to up level, because InfoGetSystemStandby is null.\n");
            }
			time(&m_PreviousStandbyT);
		}
        
    }
    return 0;
}


/*--------------------------------------------------------------------
 *                AP that supports - System Info                     *
 *-------------------------------------------------------------------*/



void CECAPBridgeImpl::resetCecLanguage()
{
	if ( strncmp(STROKE, "CONFIG_93", strlen("CONFIG_93")) == 0) {
		sprintf(SLanguage,"%s","Auto(ZHO)");
	}
	else {		
		sprintf(SLanguage,"%s","Auto(ENG)");
	}           
}

void CECAPBridgeImpl::InitCecLanguage()
{
	int number;
    if(SetupClass::GetInstance()->GetEnableHdmiCecAutoOsdLanguage())
    {

		MAP_AUTO_LANG *local_auto_map_ptr=&local_auto_map_7[0];  // default for STROKE 0
		number = sizeof(local_auto_map_7)/sizeof(MAP_AUTO_LANG);
		if(strncmp(STROKE, "CONFIG_12", strlen("CONFIG_12")) == 0) {      
			local_auto_map_ptr = &local_auto_map_0[0];
			number = sizeof(local_auto_map_0)/sizeof(MAP_AUTO_LANG);
		}
		else if(strncmp(STROKE, "CONFIG_05", strlen("CONFIG_05")) == 0) {      
			local_auto_map_ptr = &local_auto_map_0[0];
			number = sizeof(local_auto_map_0)/sizeof(MAP_AUTO_LANG);
		}
		else if (strncmp(STROKE, "CONFIG_37", strlen("CONFIG_37")) == 0) {
			
			local_auto_map_ptr = &local_auto_map_1[0];
			number = sizeof(local_auto_map_1)/sizeof(MAP_AUTO_LANG);

		}else if (strncmp(STROKE, "CONFIG_55", strlen("CONFIG_55")) == 0) {

			local_auto_map_ptr = &local_auto_map_2[0];
			number = sizeof(local_auto_map_2)/sizeof(MAP_AUTO_LANG);

		}else if (strncmp(STROKE, "CONFIG_75", strlen("CONFIG_75")) == 0) {

			local_auto_map_ptr = &local_auto_map_3[0];
			number = sizeof(local_auto_map_3)/sizeof(MAP_AUTO_LANG);

		}else if (strncmp(STROKE, "CONFIG_93", strlen("CONFIG_93")) == 0) {

			local_auto_map_ptr = &local_auto_map_4[0];
			number = sizeof(local_auto_map_4)/sizeof(MAP_AUTO_LANG);

		}else if (strncmp(STROKE, "CONFIG_96", strlen("CONFIG_96")) == 0) {

			local_auto_map_ptr = &local_auto_map_5[0];
			number = sizeof(local_auto_map_5)/sizeof(MAP_AUTO_LANG);

		}else if (strncmp(STROKE, "CONFIG_98", strlen("CONFIG_98")) == 0) {

			local_auto_map_ptr = &local_auto_map_6[0];
			number = sizeof(local_auto_map_6)/sizeof(MAP_AUTO_LANG);
		}

		
        ENUM_OSD_LANGUAGE osd_language=SetupClass::GetInstance()->GetOsdLanguage();
		
		CEC_AP_INFO("\n[%s,%d,%s]==========Area[%s]number[%d]TV Language[%d]\n",__FILE__,__LINE__,__func__,STROKE, number,osd_language);
        for(int i=0;i<number;i++)
        {
            if(local_auto_map_ptr[i].rtk_osd_language==osd_language)
            {
				memset(&SLanguage, 0, sizeof(SLanguage));
				sprintf(SLanguage,"%s",local_auto_map_ptr[i].Auto);				
                break;
            }
        }
	}
}
//AskMenuLanguage API is called from out side to ask TV's OSD language.
void CECAPBridgeImpl::AskMenuLanguage()
{
	m_pCECController->GetDeviceMenuLanguage(0);
}


int CECAPBridgeImpl::GetMenuLanguage(CEC_MENU_LANGUAGE* pLanguage)
{
    int ret=-1;
    if(SetupClass::GetInstance()->GetEnableHdmiCec() &&
        SetupClass::GetInstance()->GetEnableHdmiCecAutoOsdLanguage())
    {
        int number;
        ENUM_OSD_LANGUAGE osd_language=SetupClass::GetInstance()->GetOsdLanguage();
        number=sizeof(local_menu_map)/sizeof(MAP_MENU_LANG);
        for(int i=0;i<number;i++)
        {
            if(local_menu_map[i].rtk_osd_language==osd_language)
            {
                *pLanguage=local_menu_map[i].ISO639;
                ret=0;
                break;
            }
        }
    }
    return ret;
}
int CECAPBridgeImpl::SetMenuLanguage(CEC_MENU_LANGUAGE Language)
{
    CEC_AP_INFO("[%s]CEC Language[%x]\n",__func__,Language);
    /*
    CEC_AP_INFO(" CEC_MENU_LANG_ENG[%x]\n",DecodeMenuLanguage('e','n','g'));
    CEC_AP_INFO(" CEC_MENU_LANG_CHI[%x]\n",DecodeMenuLanguage('c','h','i'));
    CEC_AP_INFO(" CEC_MENU_LANG_JPN[%x]\n",DecodeMenuLanguage('j','p','n'));
    CEC_AP_INFO(" CEC_MENU_LANG_SPA[%x]\n",DecodeMenuLanguage('s','p','a'));
    CEC_AP_INFO(" CEC_MENU_LANG_FRA[%x]\n",DecodeMenuLanguage('f','r','a'));
    CEC_AP_INFO(" CEC_MENU_LANG_FRE[%x]\n",DecodeMenuLanguage('f','r','e'));
    CEC_AP_INFO(" CEC_MENU_LANG_DEU[%x]\n",DecodeMenuLanguage('d','e','u'));
    CEC_AP_INFO(" CEC_MENU_LANG_GER[%x]\n",DecodeMenuLanguage('g','e','r'));
    CEC_AP_INFO(" CEC_MENU_LANG_ITA[%x]\n",DecodeMenuLanguage('i','t','a'));
    CEC_AP_INFO(" CEC_MENU_LANG_KOR[%x]\n",DecodeMenuLanguage('k','o','r'));
    CEC_AP_INFO(" CEC_MENU_LANG_DUT[%x]\n",DecodeMenuLanguage('d','u','t'));
    CEC_AP_INFO(" CEC_MENU_LANG_NLD[%x]\n",DecodeMenuLanguage('n','l','d'));
    CEC_AP_INFO(" CEC_MENU_LANG_RUS[%x]\n",DecodeMenuLanguage('r','u','s'));
    CEC_AP_INFO(" CEC_MENU_LANG_ZHO[%x]\n",DecodeMenuLanguage('z','h','o'));
    CEC_AP_INFO(" CEC_MENU_LANG_POR[%x]\n",DecodeMenuLanguage('p','o','r'));
    CEC_AP_INFO(" CEC_MENU_LANG_SWE[%x]\n",DecodeMenuLanguage('s','w','e'));
    CEC_AP_INFO(" CEC_MENU_LANG_DAN[%x]\n",DecodeMenuLanguage('d','a','n'));
    CEC_AP_INFO(" CEC_MENU_LANG_NOR[%x]\n",DecodeMenuLanguage('n','o','r'));
    CEC_AP_INFO(" CEC_MENU_LANG_NOB[%x]\n",DecodeMenuLanguage('n','o','b'));
    CEC_AP_INFO(" CEC_MENU_LANG_NNO[%x]\n",DecodeMenuLanguage('n','n','o'));
    CEC_AP_INFO(" CEC_MENU_LANG_FIN[%x]\n",DecodeMenuLanguage('f','i','n'));
    CEC_AP_INFO(" CEC_MENU_LANG_POL[%x]\n",DecodeMenuLanguage('p','o','l'));
    CEC_AP_INFO(" CEC_MENU_LANG_TUR[%x]\n",DecodeMenuLanguage('t','u','r'));
    CEC_AP_INFO(" CEC_MENU_LANG_SLK[%x]\n",DecodeMenuLanguage('s','l','k'));
    CEC_AP_INFO(" CEC_MENU_LANG_SLO[%x]\n",DecodeMenuLanguage('s','l','o'));
    CEC_AP_INFO(" CEC_MENU_LANG_SLV[%x]\n",DecodeMenuLanguage('s','l','v'));
    CEC_AP_INFO(" CEC_MENU_LANG_SCR[%x]\n",DecodeMenuLanguage('s','c','r'));
    CEC_AP_INFO(" CEC_MENU_LANG_HRV[%x]\n",DecodeMenuLanguage('h','r','v'));
    CEC_AP_INFO(" CEC_MENU_LANG_CZE[%x]\n",DecodeMenuLanguage('c','z','e'));
    CEC_AP_INFO(" CEC_MENU_LANG_CES[%x]\n",DecodeMenuLanguage('c','e','s'));
    CEC_AP_INFO(" CEC_MENU_LANG_HUN[%x]\n",DecodeMenuLanguage('h','u','n'));
    CEC_AP_INFO(" CEC_MENU_LANG_RUM[%x]\n",DecodeMenuLanguage('r','u','m'));
    CEC_AP_INFO(" CEC_MENU_LANG_RON[%x]\n",DecodeMenuLanguage('r','o','n'));
    CEC_AP_INFO(" CEC_MENU_LANG_MAY[%x]\n",DecodeMenuLanguage('m','a','y'));
    CEC_AP_INFO(" CEC_MENU_LANG_MSA[%x]\n",DecodeMenuLanguage('m','s','a'));
    CEC_AP_INFO(" CEC_MENU_LANG_GRE[%x]\n",DecodeMenuLanguage('g','r','e'));
    CEC_AP_INFO(" CEC_MENU_LANG_ELL[%x]\n",DecodeMenuLanguage('e','l','l'));
    CEC_AP_INFO(" CEC_MENU_LANG_ENG[%x]\n",DecodeMenuLanguage('e','n','g'));
    CEC_AP_INFO(" CEC_MENU_LANG_POR[%x]\n",DecodeMenuLanguage('p','o','r'));
    CEC_AP_INFO(" CEC_MENU_LANG_SPA[%x]\n",DecodeMenuLanguage('s','p','a'));
    CEC_AP_INFO(" CEC_MENU_LANG_ESP[%x]\n",DecodeMenuLanguage('e','s','p'));
    CEC_AP_INFO(" CEC_MENU_LANG_GRE[%x]\n",DecodeMenuLanguage('g','r','e'));
    CEC_AP_INFO(" CEC_MENU_LANG_SUN[%x]\n",DecodeMenuLanguage('s','u','n'));
    CEC_AP_INFO(" CEC_MENU_LANG_SVE[%x]\n",DecodeMenuLanguage('s','v','e'));
    CEC_AP_INFO(" CEC_MENU_LANG_HIN[%x]\n",DecodeMenuLanguage('h','i','n'));
    CEC_AP_INFO(" CEC_MENU_LANG_THA[%x]\n",DecodeMenuLanguage('t','h','a'));
    CEC_AP_INFO(" CEC_MENU_LANG_UKR[%x]\n",DecodeMenuLanguage('u','k','r'));
    */
    if (InfoSetMenuLanguage !=NULL)
    {
        InfoSetMenuLanguage(Language);
    }
    else
    {
        CEC_AP_WARNING("Can not feedback SetMenuLanguage to up level, because InfoSetMenuLanguage is null.\n");
    }
 
    return 0;
}

void CECAPBridgeImpl::RegCallback_SetMenuLanguage(int (*UpHandler)(CEC_MENU_LANGUAGE))
{
    InfoSetMenuLanguage = UpHandler;
}

void CECAPBridgeImpl::RegCallback_GetSystemStandby(void (*UpHandler)())
{
    InfoGetSystemStandby = UpHandler;
}


int CECAPBridgeImpl::GetOSDName(char* pOSDName, unsigned char* Len)
{
    int osd_name_len=strlen(DEVICE_OSD_NAME);
    memset(pOSDName,0,*Len);
    if(osd_name_len<*Len)
        *Len=osd_name_len;
    strncpy(pOSDName,DEVICE_OSD_NAME,*Len);
    return CEC_CTRL_OK;
}



/*------------------------------------------------------------------- 
 * Func : GetMenuStatus 
 *
 * Desc : Get APs Menu Status
 *
 * Parm : pStatus  : Menu Status
 *
 * Retn : CEC_CTRL_OK /CEC_CTRL_UNSUPPORTED_FEATURE 
 *-------------------------------------------------------------------*/
int CECAPBridgeImpl::GetMenuStatus(
    CEC_MENU_STATE*         pStatus
    )
{
    *pStatus=CEC_MENU_STATE_ACTIVATE;  
    return 0;
}

/*------------------------------------------------------------------- 
 * Func : MenuControl 
 *
 * Desc : Do Menu Control
 *
 * Parm : Ctrl  : Menu Control Command
 *
 * Retn : CEC_CTRL_OK /CEC_CTRL_UNSUPPORTED_FEATURE 
 *-------------------------------------------------------------------*/
int CECAPBridgeImpl::MenuControl(unsigned char Ctrl)
{
    int ret=CEC_CTRL_UNSUPPORTED_FEATURE;
    if(SetupClass::GetInstance()->GetEnableHdmiCec())
    {
        ret=CEC_CTRL_OK;
        switch(Ctrl)
        {
        case CEC_MENU_REQUEST_ACTIVATE:
            break;
        case CEC_MENU_REQUEST_DEACTIVATE:
            break;
        default:
            ret=CEC_CTRL_UNSUPPORTED_FEATURE;
            break;
        }
    }
    return ret;
}

void CECAPBridgeImpl::write_key_event(int fdRemote, int keycode, int downOrUp)
/* downOrUp: 1 -> down, 0 -> up */
{
    struct input_event event_init, event, event_end;
    memset(&event_init, 0, sizeof(event_init));
    memset(&event, 0, sizeof(event));
    memset(&event_end, 0, sizeof(event_end));
    gettimeofday(&event_init.time, NULL);
    event_init.type = EV_MSC;
    event_init.code = MSC_SCAN;
    event_init.value = 500000;
    gettimeofday(&event.time, NULL);
    event.type = EV_KEY;
    event.code = keycode;
    event.value = downOrUp;
    gettimeofday(&event_end.time, NULL);
    event_end.type = EV_SYN;
    event_end.code = SYN_REPORT;
    event_end.value = 0;
    write(fdRemote, &event_init, sizeof(event_init)); // init
    write(fdRemote, &event, sizeof(event));// key event
    write(fdRemote, &event_end, sizeof(event_end));// Show
    //sleep(1);// wait
}


/*------------------------------------------------------------------- 
 * Func : UserControlPressed 
 *
 * Desc : Inform AP that remote device has been pressed a key of remote control
 *
 * Parm : key      : which key does it pressed
 *        param    : additional parameter
 *        param_len: size of additional parameters
 *
 * Retn : CEC_CTRL_OK /CEC_CTRL_UNSUPPORTED_FEATURE 
 *-------------------------------------------------------------------*/   
int CECAPBridgeImpl::UserControlPressed(
    unsigned char           key,
    unsigned char*          param,
    unsigned char           param_len    
    )
{
    CEC_AP_INFO("UserControlPressed\n");
	CEC_USER_CTRL_CODE CECKey;
    int Keycode;
	CECKey=(CEC_USER_CTRL_CODE)key;
    if(SetupClass::GetInstance()->GetEnableHdmiCec())
	{
        if ((Keycode = GetAndroidCmdFromCecCode(CECKey))!=KEYCODE_NONE)
        {
            if (m_fdRemote==-1)
            {
               m_fdRemote = open(DEV_REMOTE, O_WRONLY); 
            }
            if (m_fdRemote !=-1)
            {
                write_key_event(m_fdRemote, Keycode, 1);  // pressed
                write_key_event(m_fdRemote, Keycode, 0);  // released
            }
            //release remote device
            if (m_fdRemote)
            {
                close(m_fdRemote);
                m_fdRemote = -1;
            }
        }
	}
    return CEC_CTRL_OK;
}
 

/*--------------------------------------------------------------------
 *                AP that supports - Vendor Specific Control         *
 *-------------------------------------------------------------------*/
 
 
 
/*------------------------------------------------------------------- 
 * Func : GetVendorID 
 *
 * Desc : Get Vendor ID of AP
 *
 * Parm : pVendorID : who issue this command 
 *
 * Retn : CEC_CTRL_OK / CEC_CTRL_UNSUPPORTED_FEATURE
 *-------------------------------------------------------------------*/
int CECAPBridgeImpl::GetVendorID(
    unsigned long*          pVendorID
    )
{
    *pVendorID = DEVICE_VENDOR_ID;
    return CEC_CTRL_OK;    
}



/*--------------------------------------------------------------------
 *                   AP that supports - Power Status                 *
 *-------------------------------------------------------------------*/        



/*------------------------------------------------------------------- 
 * Func : GetPowerStatus 
 *
 * Desc : Get AP's Power Status
 *
 * Parm : pStatus      : AP's Power Status
 *
 * Retn : CEC_CTRL_OK /CEC_CTRL_UNSUPPORTED_FEATURE 
 *-------------------------------------------------------------------*/   
int CECAPBridgeImpl::GetPowerStatus(
    CEC_POWER_STATUS*       pStatus
    )
{
    *pStatus=m_PowerState;
    return CEC_CTRL_OK;
}
//doOneTouchPlay API is called from out side to ask TV display our source.
int CECAPBridgeImpl::doOneTouchPlay(int Intensity)
{
    int ret=-1;
	unsigned char Mode;
    unsigned short ActiveSource;
    if(SetupClass::GetInstance()->GetEnableHdmiCec() &&
        SetupClass::GetInstance()->GetEnableHdmiCecOneTouchPlay() &&
        m_pCECController)
    {
		ALOGD(TAG "Enter CECAPBridgeImpl::doOneTouchPlay %d",__LINE__);
		if (m_pCECController->PollDevice(0)==CEC_CTRL_OK)   // CEC TV exists
		{
		    unsigned short addr = getHDMISourceAddress();   // get current address for HDMI
		    
		    /* we need to check is the address changed or not.
		       Some TV (like sharp) won't generate HDMI HPD if we 
		       plug HDMI cable at non current HDMI channel.
		    */
		    if (m_PhyAddr!=addr)
		    {
		        m_PhyAddr = addr;
		        m_pCECController->UpdateStatus(CEC_CTRL_PHYSICAL_ADDRESS);
		            
		        if (m_PhyAddr != PHYSICAL_ADDR_NULL)
		        {
		            if(SetupClass::GetInstance()->GetEnableHdmiCecAutoOsdLanguage())
		                m_pCECController->GetDeviceMenuLanguage(0);     
		        }           
		    }
			if (Intensity>2)//3
				Mode = FORCE_IMAGE_VIEW_ON;
			else if(Intensity>1)//2
				Mode = AUTO_IMAGE_VIEW_ON;
			else//1
				Mode = ACTIVE_SOURCE_ONLY;
			if(CEC_CTRL_OK==m_pCECController->OneTouchPlay(Mode)) 
			{
				ret=0;
				ALOGD(TAG "doOneTouchPlay OK");
			}
		}
		else
		{
		    ALOGD("do one touch play failed - TV0 doesn't exist\n");
		}
    }
    return ret;
}
//SendRemotePressed API is called from out side to ask TV display our source.
int CECAPBridgeImpl::SendRemotePressed(unsigned char DevAddr, unsigned char Key, bool SendReleased )
{
    int rlt;//0: Send OK, -1: Send Fail, -2:Can not get LA of DevAddr
    unsigned char LA = GetLAOf(DevAddr);
    ALOGD("HDMI DevAddr[%d] LA[%d]\n",DevAddr,LA);
    if (LA>0){
        if (CEC_CTRL_OK == m_pCECController->SendUserCommandPressed(LA, Key)){
            if (SendReleased)
                m_pCECController->SendUserCommandReleased(LA);
            rlt =0;
        }
        else {
            rlt =-1;
        }
    }
    else{//Can not find LA of DevAddr
        rlt = -2;
    }
    return rlt;
}
unsigned char CECAPBridgeImpl::GetLAOf(unsigned char DevAddr)//HDMI Rx(1~15)
{
    unsigned short PA=0;
    if ((m_PhyAddr & 0x0FFF)==0){//Level 1
        PA = m_PhyAddr |(DevAddr<<8);
    }
    else if ((m_PhyAddr & 0x00FF)==0){//Level 2
        PA = m_PhyAddr |(DevAddr<<4);
    }
    else if ((m_PhyAddr & 0x000F)==0){//Level 3
        PA = m_PhyAddr |DevAddr;
    }
    else{//Level 4
        ALOGE("ERROR!!! That should not have any HDMI device behind of ME.");
    }
    if (PA>0 && m_LAList.indexOfKey(PA)>=0){
        return m_LAList.valueFor(PA);
    }
    else{
        return 0;
    }
}
void CECAPBridgeImpl::ReportPhysicalAddress(unsigned short PA, unsigned char LA)
{
    if (m_LAList.indexOfKey(PA)<0){
        m_LAList.add(PA,LA);
    }
    else{
        m_LAList.replaceValueFor(PA,LA);
    }
    ALOGD("Update PA[0x%04x] LA[%d] List",PA, LA);
}
int CECAPBridgeImpl::doUpdatePhysicalAddress(unsigned short PhyAddr)
{
	CEC_AP_INFO("CECAPBridgeImpl::doUpdatePhysicalAddress(%04x, %04x)\n", PhyAddr, m_PhyAddr);				    

    if (m_PhyAddr==PhyAddr && PhyAddr!=PHYSICAL_ADDR_NULL)
    {   
        CEC_AP_INFO("same physical address, do nothing....\n");
        return 0; 
    }
                  
    if (SetupClass::GetInstance()->GetEnableHdmiCec() && m_pCECController)
    {
#ifdef DOUBLE_CHECK_HDMI_CONNECTION 
        /* Some TV will set HPD to low when HDMI port is not activated
         * that will make CEC controller thought the HDMI cable has been 
         * removed from TV, here we tries to reread the HDMI EDID to make sure
         * the HDMI cable is really removed or not
         */
        if (PhyAddr==PHYSICAL_ADDR_NULL)
        {            
            if (m_pCECController->PollDevice(0)==CEC_CTRL_OK)   // CEC TV exists
            {   
                CEC_AP_INFO("HDMI HDP is down but CEC TV is still exists, keep origional physical addr %04x\n",m_PhyAddr);                                        
                return 0; 
            }              
        }
#endif			

        m_PhyAddr = PhyAddr;
        m_Status.wait_tv_language_retry = 0;
        m_Status.wait_tv_power_on_retry = 0;        

        m_pCECController->UpdateStatus(CEC_CTRL_PHYSICAL_ADDRESS);
        
        if(m_PhyAddr != PHYSICAL_ADDR_NULL)
        {                
       		if (SetupClass::GetInstance()->GetEnableHdmiCecOneTouchPlay())
       		{
       		    CEC_AP_INFO("Check One Toch Play Condition: m_BootUpOneTouchPlay=%d\n", m_BootUpOneTouchPlay);
       		    
       		    if (m_BootUpOneTouchPlay==0)
       		    {
       		        CEC_POWER_STATUS PowerStatus;
       		        
       		        usleep(100000); 
       		        m_pCECController->OneTouchPlay(AUTO_IMAGE_VIEW_ON, 3000);
       		        m_BootUpOneTouchPlay = 1;
                
                    // check if Tv Power On already?
                    if (m_pCECController->GetDevicePowerStatus(0, &PowerStatus)!=CEC_CTRL_OK || PowerStatus!=CEC_POWER_STATUS_ON)                
                    {
                        CEC_AP_WARNING("do One Tuch Play with wait Tv Power On timeout, set WaitTvPowerOn=1\n");
                        m_Status.wait_tv_power_on_retry = MAX_WAIT_TV_POWER_ON_RETRY;
                    }
       		    }
       		}
       		
       		if(SetupClass::GetInstance()->GetEnableHdmiCecAutoOsdLanguage())
       		{                   
       		    if (m_pCECController->GetDeviceMenuLanguage(0)==CEC_CTRL_OK)       		    
       		        m_Status.wait_tv_language_retry = MAX_WAIT_TV_MENU_LANGUAGE_RETRY;       		    
       		}
       		    
       		if (SetupClass::GetInstance()->GetSystemAudioControl())
       		{
	   			SystemAudioControl(1, 0xf);					  // After Hot plug, default will output Audio
       		    m_pCECController->SetSystemAudioMode(1);	  // try to start system audio mode
       		    
       		    if ((m_SystemAudioStatus==CEC_SYSTEM_AUDIO_STATUS_ON) && (m_PhyAddr==0x1000))
       		        m_pCECController->ARCTxControl(CEC_ARC_CTRL_INITIATE);
       		}
        }
    }    
    return 0;        
}


  
int CECAPBridgeImpl::doUpdatePowerState(CEC_POWER_STATUS pStatus)
{
    m_PowerState=pStatus;
    if (SetupClass::GetInstance()->GetEnableHdmiCec() &&
        m_pCECController)
    {
        m_pCECController->UpdateStatus(CEC_CTRL_POWER_STATUS);
    }
    return 0;
}



int CECAPBridgeImpl::SetStandbyMode()
{
	unsigned long Mode = CEC_STANBY_RESPONSE_GIVE_POWER_STATUS|CEC_STANBY_RESPONSE_POOLING|
                             CEC_STANBY_RESPONSE_GET_CEC_VERISON|CEC_STANBY_RESPONSE_GIVE_DEVICE_VENDOR_ID|
                             CEC_STANBY_RESPONSE_GIVE_PHYSICAL_ADDR;
// +++ fix mantis 36735 (2013/3/15)                            
	if (SetupClass::GetInstance()->GetEnableHdmiCecAutoPowerOnFromTv() && SetupClass::GetInstance()->GetEnableHdmiCec()&& m_pCECController)
		Mode = Mode |CEC_STANBY_WAKEUP_BY_SET_STREAM_PATH|CEC_STANBY_WAKEUP_BY_USER_CONTROL;
	m_pCECController->SetStandbyMode(Mode);
	return 0;
}
//doAllSystemStandby API is called from out side for asking all HDMI device go to standby.
int CECAPBridgeImpl::doAllSystemStandby(void)
{
    int ret=-1;
    CEC_AP_INFO("[%s,%s,%d] do system starndby \n\n\n\n",__FILE__,__func__,__LINE__);
    if (SetupClass::GetInstance()->GetEnableHdmiCec() &&
        m_pCECController)
    {
        m_pCECController->StandbyDevice(0xf); 
        ret=0;
    }
    return ret;
}

//CallBack_hdmi_event API should be called from out side once HDMI is plug IN/OUT
int CECAPBridgeImpl::CallBack_hdmi_event(bool PlugIn)
{
    CEC_AP_INFO("[%s,%s,%d] CallBack_hdmi_event.\n\n\n\n",__FILE__,__func__,__LINE__);
    unsigned short PA =getHDMISourceAddress() ;

    CEC_AP_INFO("Got HDMI Address : %s PA=%04x\n", PlugIn==true?"PLUG IN":"PLUG OUT", PA);  
	HDMIThreadEvent = (PA!=0xFFFF) ? 1 : 0;
    return 0;
}


char *CECAPBridgeImpl::GetLanguage()
{	
	return SLanguage;
}


/*--------------------------------------------------------------------
 *                AP that supports - System Info                     *
 *-------------------------------------------------------------------*/



/*------------------------------------------------------------------- 
 * Func : GetPhysicalAddress 
 *
 * Desc : Get Physical Address
 *
 * Parm : N/A
 *
 * Retn : Physical Address
 *-------------------------------------------------------------------*/  
unsigned short CECAPBridgeImpl::GetPhysicalAddress()
{
    return m_PhyAddr;
}



/*------------------------------------------------------------------- 
 * Func : GetDeviceType
 *
 * Desc : Get Device Type
 *
 * Parm : N/A
 *
 * Retn : Device Type
 *-------------------------------------------------------------------*/ 
CEC_DEVICE_TYPE CECAPBridgeImpl::GetDeviceType()
{
    return CEC_DEVICE_PLAYBACK_DEVICE;
}


/*------------------------------------------------------------------- 
 * Func : GetDeviceTypes
 *
 * Desc : Get Device Types (for multiple device type support) 
 *
 * Parm : N/A
 *
 * Retn : Device Types 
 *-------------------------------------------------------------------*/  
unsigned short CECAPBridgeImpl::GetDeviceTypes()
{
    unsigned short types = CEC_DEVICE_TYPE_BIT(CEC_DEVICE_PLAYBACK_DEVICE);
    if (SetupClass::GetInstance()->GetSystemAudioControl())
        types |= CEC_DEVICE_TYPE_BIT(CEC_DEVICE_AUDIO_SYSTEM);
    return types;
} 
 

/*--------------------------------------------------------------------
 *               AP that supports - System Audio Control             *
 *-------------------------------------------------------------------*/
 
 
 
/*------------------------------------------------------------------- 
 * Func : GetAudioStatus 
 *
 * Desc : report Audio status (Mute/Volume)
 *
 * Parm : pStatus : Audio Status output
 *
 * Retn : CEC_CTRL_OK /CEC_CTRL_UNSUPPORTED_FEATURE 
 *-------------------------------------------------------------------*/ 
int CECAPBridgeImpl::GetAudioStatus(
    CEC_AUDIO_STATUS*       pStatus
    )
{
    if (SetupClass::GetInstance()->GetSystemAudioControl())
    {
		//m_audio_status should be updated by out side amplifier
	    *pStatus=m_audio_status;
	    return CEC_CTRL_OK; 
	}
	else
	{
	    CEC_AP_WARNING("RequestSystemAudioMode failed: System Audio Control Feature does not enabled\n");
	    return CEC_CTRL_UNSUPPORTED_FEATURE; 	    
	}	   
}



/*------------------------------------------------------------------- 
 * Func : GetSystemAudioStatus 
 *
 * Desc : Get System Audio Status
 *
 * Parm : pStatus : System Audio Status output
 *         CEC_SYSTEM_AUDIO_STATUS_ON  : System Audio Mode is Enabled
 *         CEC_SYSTEM_AUDIO_STATUS_OFF : System Audio Mode is Disabled
 *
 * Retn : CEC_CTRL_OK /CEC_CTRL_UNSUPPORTED_FEATURE 
 *-------------------------------------------------------------------*/  
int CECAPBridgeImpl::GetSystemAudioStatus(
    CEC_SYSTEM_AUDIO_STATUS*    pStatus
    )
{
	bool state = SetupClass::GetInstance()->GetSystemAudioControl();//User control
	    
	if(state==true)
	{
		*pStatus = m_SystemAudioStatus;//Request come from HDMI device.
        CEC_AP_INFO("GetSystemAudioStatus : Status=%x  \n",*pStatus);		
		return CEC_CTRL_OK; 
	}
	else		
	{
        CEC_AP_WARNING("GetSystemAudioStatus failed: System Audio Control Feature does not enabled, Status=%x\n", m_SystemAudioStatus);
        return CEC_CTRL_UNSUPPORTED_FEATURE;
    }
}



/*------------------------------------------------------------------- 
 * Func : RequestSystemAudioMode 
 *
 *
 * Desc : Request System Audio Device to Start/Stop system audio mode.
 *        The system aduio device should switch its source to specified
 *        channel.
 * CECControler will use this call back function to ask AP change System Audio Mode
 *
 * Parm : On : 0  : disable System Audio Mode, others : Enable
 *        PhyAddr : Specified audio channel
 *
 * Retn : CEC_CTRL_OK /CEC_CTRL_UNSUPPORTED_FEATURE 
 *-------------------------------------------------------------------*/ 
int CECAPBridgeImpl::RequestSystemAudioMode(
    unsigned char           On, 
    unsigned short          PhyAddr
    )
{
    bool state = SetupClass::GetInstance()->GetSystemAudioControl();
    if (On == SYSTEM_AUDIO_MODE_PREPARE)
	{
		if (state==true)
			return CEC_CTRL_OK;
		else
			return CEC_CTRL_UNSUPPORTED_FEATURE;
	}                  
    if (state==true)
    {
        CEC_AP_INFO("RequestSystemAudioMode : On: %d, PA=%04x\n", On, PhyAddr);
       	SystemAudioControl(On, PhyAddr);
        return CEC_CTRL_OK;
    }
    else
    {
        CEC_AP_WARNING("RequestSystemAudioMode failed: System Audio Control Feature does not enabled\n");
        return CEC_CTRL_UNSUPPORTED_FEATURE;
    }         
}



void CECAPBridgeImpl::RequestAudioMode(bool onoff)
{
	//You may need to info out side Amplifier system audio status
}



/*------------------------------------------------------------------- 
 * Func : SystemAudioControl 
 *
 * Desc : Enable/Disable System Audio Mode
 *
 * Parm : On : 0  : disable System Audio Mode, others : Enable
 *
 * Retn : CEC_CTRL_OK /CEC_CTRL_UNSUPPORTED_FEATURE 
 *-------------------------------------------------------------------*/
int CECAPBridgeImpl::SystemAudioControl(unsigned char On, unsigned short PhyAddr)
{
    if (On)
    {
        CEC_AP_INFO("RequestSystemAudioMode: Enter System Audio Mode\n");
        m_SystemAudioStatus = CEC_SYSTEM_AUDIO_STATUS_ON;
		RequestAudioMode(true);
    }
    else
    {
        CEC_AP_INFO("RequestSystemAudioMode: Leave System Audio Mode\n");                        
        m_SystemAudioStatus = CEC_SYSTEM_AUDIO_STATUS_OFF;                      
		RequestAudioMode(false);
    }  
	return 0;          
}



/*------------------------------------------------------------------- 
 * Func : ReportAudioStatus
 *
 * Desc : Report Audio Status
 *
 *-------------------------------------------------------------------*/
void CECAPBridgeImpl::ReportAudioStatus()
{
	CEC_AP_INFO("[WT][ReportAudioStatus]mute[%d] volume[%d]\n",m_audio_status.Mute,m_audio_status.Volume);
 	m_pCECController->UpdateStatus(CEC_CTRL_AUDIO_STATUS);
}

/*------------------------------------------------------------------- 
 * Func : SendOSDString
 *
 * Desc : Send OSD String to TV
 *
 *-------------------------------------------------------------------*/
void CECAPBridgeImpl::SendOSDString(const char* String)
{
	CEC_DISPLAY_CTRL	Ctrl= CEC_DISPLAY_CTRL_DEFAULT;
	CEC_AP_INFO("[WT][SendOSDString]String[%s]\n",String);
 	m_pCECController->SendOSDString(Ctrl, String, CEC_DEVICE_AUDIO_SYSTEM);
}
void CECAPBridgeImpl::DisablePlayerActiveSource()
{
	m_pCECController->InactiveSource(0);    
}


/*------------------------------------------------------------------- 
 * Func : RequestSwitchSource 
 *
 * Desc : Enable/Disable System Audio Mode
 *
 * Parm : On : 0  : disable System Audio Mode, others : Enable
 *
 * Retn : CEC_CTRL_OK /CEC_CTRL_UNSUPPORTED_FEATURE 
 *-------------------------------------------------------------------*/
void CECAPBridgeImpl::RequestSwitchSource(SOURCE_STATE src)
{
	CEC_AP_INFO("switch src to %x \n ",src);
	//You may need to info out side amplifier switchSource to src
}



/*--------------------------------------------------------------------
 *              AP that supports - Routing Control                 *
 *-------------------------------------------------------------------*/
 

/*------------------------------------------------------------------- 
 * Func : GotStreamPath 
 *
 * Desc : Infom AP that it has been selected by set stream path
          Call from Controller.
          If Phy address is same as our Phy address we will switch SRC   
 *
 * Parm : PhyAddr : Physical Address of new stream path
 *
 * Retn : CEC_CTRL_OK 
 *-------------------------------------------------------------------*/  
int CECAPBridgeImpl::GotStreamPath(unsigned short PhyAddr)
{
    if (m_PhyAddr == PhyAddr)
	{
		RequestSwitchSource(STATE_SOURCE_MPEG);
    } 
    return CEC_CTRL_OK;
}   



/*-------------------------------------------------------------------
 * Func : GotActiveSource
 *
 * Desc : Infom AP Got active source
 *
 * Parm : PhyAddr : Address of New Active Source
 *
 * Retn : CEC_CTRL_OK
 *-------------------------------------------------------------------*/
int CECAPBridgeImpl::GotActiveSource(
    unsigned short          PhyAddr
    )
{
	//If System Audio Mode & ARC is work, will request Audio system module switch to ARC. 
	CEC_AP_INFO("[WEN] Got ActiveSource [%X]SAM[%d]ARC[%d]\n",PhyAddr,m_SystemAudioStatus,m_ARCStatus);
	if ((m_SystemAudioStatus == CEC_SYSTEM_AUDIO_STATUS_ON) && (m_ARCStatus == CEC_ARC_STATUS_INITIATE))
	{
		CEC_AP_INFO("[WEN] Enable ARC mode\n");
    	RequestSwitchSource(STATE_SOURCE_ARC);
	}
	
	return CEC_CTRL_OK;
}
   

/*--------------------------------------------------------------------
 *              AP that supports - Audio Return Channel             *
 *-------------------------------------------------------------------*/
 
 
 
/*------------------------------------------------------------------- 
 * Func : ARCRxControl 
 *
 * Desc : Initiate/Terminate ARC receiving
 *
 * Parm : Ctrl  : Control Command
 *          CEC_ARC_CTRL_INITIATE  : Initiate ARC Rx
 *          CEC_ARC_CTRL_TERMINATE : Terminate ARC Tx
 *
 * Retn : CEC_CTRL_OK : ARC Rx Initiated/Terminated
 *        CEC_CTRL_UNSUPPORTED_FEATURE : AP does not support ARC Rx
 *        CEC_CTRL_ABORT_NOT_IN_CORECT_MODE : ARC Rx is not ready to initiate ARC
 *-------------------------------------------------------------------*/           
int CECAPBridgeImpl::ARCRxControl(CEC_ARC_CTRL Ctrl)
{
	switch(Ctrl)
	{
		case CEC_ARC_CTRL_PREPARE:
		{
			CEC_AP_INFO("CEC_ARC_CTRL_PREPARE ");
			if (m_PhyAddr == 0x1000)
			{
				//Do not thing, just check my Phy address
			}
			else
			{
                CEC_AP_WARNING("[Failed][Phy address is not accepted]\n");
				return CEC_CTRL_ABORT_REFUSED;	
			}
		}
		break;
		case CEC_ARC_CTRL_INITIATE:
		{
            CEC_AP_INFO("[WEN]Phy address [%X] ARC INITIATE REQUEST \n",m_PhyAddr);
			if (m_PhyAddr == 0x1000)
			{
				//You may need to ask out side amplifier open ARC RX
				m_ARCStatus = CEC_ARC_STATUS_INITIATE;
                CEC_AP_INFO("[OK]\n");
			}
			else
			{
                CEC_AP_WARNING("[Failed][Phy address is not accepted]\n");
				return CEC_CTRL_ABORT_REFUSED;	
			}
		}
		CEC_AP_INFO("CEC_ARC_CTRL_INITIATE ");
		// TO DO: start MCU ARC channel over here
		break;
		case CEC_ARC_CTRL_TERMINATE:
		{
			//You may need to ask out side amplifier terminate ARC RX
			m_ARCStatus = CEC_ARC_STATUS_TERMINATE;
		}
		CEC_AP_INFO("CEC_ARC_CTRL_TERMINATE ");
		// TO DO: Stop MCU ARC channel over here
		break;
		default:
		CEC_AP_INFO("ARCRxControl  Ctr=%x ",Ctrl);
		m_ARCStatus = CEC_ARC_STATUS_TERMINATE;
		return  CEC_CTRL_UNSUPPORTED_FEATURE;
	}
	return CEC_CTRL_OK;        
}



/*------------------------------------------------------------------- 
 * Func : GetARCStatus 
 *
 * Desc : Get ARC status
 *
 * Parm : pStatus 
 *          CEC_ARC_STATUS_TERMINATE  : ARC terminated
 *          CEC_ARC_STATUS_INITIATE   : ARC initiate
 *
 * Retn : CEC_CTRL_OK : ARC Rx Initiated/Terminated
 *        CEC_CTRL_UNSUPPORTED_FEATURE : AP does not support ARC Rx
 *        CEC_CTRL_ABORT_NOT_IN_CORECT_MODE : ARC Rx is not ready to initiate ARC
 *-------------------------------------------------------------------*/           
int CECAPBridgeImpl::GetARCStatus(
    CEC_ARC_STATUS*         pStatus
    )
{
    if (SetupClass::GetInstance()->GetSystemAudioControl())
    {
   		*pStatus =  m_ARCStatus;
		return CEC_CTRL_OK;
    }
	else
	{
	    CEC_AP_WARNING("RequestSystemAudioMode failed: System Audio Control Feature does not enabled\n");
	    return CEC_CTRL_UNSUPPORTED_FEATURE; 	    
	}
}


/*--------------------------------------------------------------------
 *                MISC Functions                                     *
 *-------------------------------------------------------------------*/
 


/*------------------------------------------------------------------- 
 * Func : BinarySearch 
 *
 * Desc : BinarySearch
 *
 * Parm : 
 *
 * Retn : 0 : find value/ -1 : not find
 *-------------------------------------------------------------------*/           
int CECAPBridgeImpl::BinarySearch(
    int*                    array,
    int                     array_num,
    int                     findKey
    )
{
    int low = 0;
    int high = array_num - 1;
    while (low <= high) {
        int mid = (low + high) / 2;
        if (array[mid] > findKey)
            high = mid - 1;
        else if (array[mid] < findKey)
            low = mid + 1;
        else
            return 0; // found
    }
    return -1; // not found
}
//Wen: WEN_TODO:It should be take off once HDMI provides this API.
/*
unsigned short CECAPBridgeImpl::getHDMISourceAddress()
{
	unsigned short addr=0x1000;
	return addr;
}
*/
/*------------------------------------------------------------------- 
 * Func : GetAndroidCmdFromCecCode 
 *
 * Desc : GetAndroidCmdFromCecCode
 *
 * Parm : 
 *
 * Retn : -1 : not find Others: Android Key
 *-------------------------------------------------------------------*/ 
int CECAPBridgeImpl::GetAndroidCmdFromCecCode(
    CEC_USER_CTRL_CODE      cec_code
    )
{
    int number;
    RTK_Android_Key Key=KEYCODE_NONE;
    number=sizeof(local_cec2Android_cmdmap)/sizeof(MAP_RTKCMD_CECCODE);
	CEC_AP_INFO("[WT][CEC Code=%x]\n",cec_code);
    for(int i=0;i<number;i++)
    {
        if(local_cec2Android_cmdmap[i].CecUserCode == cec_code)
        {
            Key = local_cec2Android_cmdmap[i].Key;
            break;
        }
    }
    return Key;
}
