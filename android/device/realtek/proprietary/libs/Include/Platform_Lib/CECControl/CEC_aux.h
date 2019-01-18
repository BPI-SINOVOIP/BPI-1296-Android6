#ifndef __CEC_AUX_H__
#define __CEC_AUX_H__

#if __cplusplus 
extern "C" {
#endif
    
#include "CEC_Def.h"
#include "CEC_Types.h"

extern const CEC_DEVICE_TYPE DEVICE_TYPE_MAP[16];
extern const char*  GetDeviceTypeName(CEC_DEVICE_TYPE type);

extern const char*  GetLogicalAddressName(unsigned char Addr);
extern const char*  PowerStatusName(CEC_POWER_STATUS PowerStatus);
extern const char*  SystemAudioStatusName(CEC_SYSTEM_AUDIO_STATUS Status);
extern const char*  VersionName(CEC_VERSION ver);
extern const char*  GetLogicalAddressName(unsigned char Addr);

extern CEC_FEARURE_ID GetMessageFeatureID(const CEC_MSG* pMsg);

extern const char* CECErrStr(int err);

#define IsBroadcastMessage(pMsg)        (pMsg->Header.Dest==0xF)
#define GetMessageTypeStr(pMsg)         (IsBroadCastMessage(pMsg) ? "broadcast" : "direct address") 

extern int CheckMessageCorrectness(const CEC_MSG* pMsg);


extern int EncodeAnalogServiceID(const CEC_ANALOG_SERVICE_ID* pServiceID, unsigned char* pBuff, unsigned char Size);
extern int DecodeAnalogServiceID(unsigned char* pBuff, unsigned char Size, CEC_ANALOG_SERVICE_ID* pServiceID);

extern int EncodeExternalSourceID(const CEC_EXTERNAL_SOURCE_ID* pSourceID, unsigned char* pBuff, unsigned char Size);
extern int DecodeExternalSourceID(unsigned char* pBuff, unsigned char Size, CEC_EXTERNAL_SOURCE_ID* pSourceID);

extern int EncodeDigitalServiceID(const CEC_DIGITAL_SERVICE_ID* pServiceID, unsigned char* pBuff, unsigned char Size);
extern int DecodeDigitalServiceID(unsigned char* pBuff, unsigned char Size, CEC_DIGITAL_SERVICE_ID* pServiceID);

extern int EncodeTimerInfo(const CEC_TIMER_INFO* pTimer, unsigned char* pBuff, unsigned char Size);
extern int DecodeTimerInfo(unsigned char* pBuff, unsigned char Size, CEC_TIMER_INFO* pTimer);
    
#define TEST_BIT(val, bit)          ((val >> bit) & 1)
#define SET_BIT(val, bit)           (val | (1 << bit))
#define BYTE_TO_SHORT(HB,LB)        (((unsigned short) HB <<8) + (unsigned short) LB)
#define DecodeVendorID(B1,B2,B3)    (((unsigned long) B1 <<16) + ((unsigned long) B2 <<8) + (unsigned long) B3)

extern const char* GetMessageName(unsigned char Opcode);
extern const char* RCKeyName(unsigned char Key);

//-------------- Menu Language -------------------------//
#define DecodeMenuLanguage(B1,B2,B3)(((unsigned long) B1 <<16) + ((unsigned long) B2 <<8) + (unsigned long) B3)
extern void GetLanguageName(CEC_MENU_LANGUAGE Language, char* pLangStr);

#define CEC_MENU_LANG_UNKNOWN  DecodeMenuLanguage('\0','\0','\0')
#define CEC_MENU_LANG_ENG      DecodeMenuLanguage('e','n','g')
#define CEC_MENU_LANG_CHI      DecodeMenuLanguage('c','h','i')
#define CEC_MENU_LANG_JPN      DecodeMenuLanguage('j','p','n')
#define CEC_MENU_LANG_SPA      DecodeMenuLanguage('s','p','a')
#define CEC_MENU_LANG_FRA      DecodeMenuLanguage('f','r','a')
#define CEC_MENU_LANG_FRE      DecodeMenuLanguage('f','r','e')
#define CEC_MENU_LANG_DEU      DecodeMenuLanguage('d','e','u')
#define CEC_MENU_LANG_GER      DecodeMenuLanguage('g','e','r')
#define CEC_MENU_LANG_ITA      DecodeMenuLanguage('i','t','a')
#define CEC_MENU_LANG_KOR      DecodeMenuLanguage('k','o','r')
#define CEC_MENU_LANG_DUT      DecodeMenuLanguage('d','u','t')
#define CEC_MENU_LANG_NLD      DecodeMenuLanguage('n','l','d')
#define CEC_MENU_LANG_RUS      DecodeMenuLanguage('r','u','s')
#define CEC_MENU_LANG_ZHO      DecodeMenuLanguage('z','h','o')
#define CEC_MENU_LANG_POR      DecodeMenuLanguage('p','o','r')
#define CEC_MENU_LANG_SWE      DecodeMenuLanguage('s','w','e')
#define CEC_MENU_LANG_DAN      DecodeMenuLanguage('d','a','n')
#define CEC_MENU_LANG_NOR      DecodeMenuLanguage('n','o','r')
#define CEC_MENU_LANG_NOB      DecodeMenuLanguage('n','o','b')
#define CEC_MENU_LANG_NNO      DecodeMenuLanguage('n','n','o')
#define CEC_MENU_LANG_FIN      DecodeMenuLanguage('f','i','n')
#define CEC_MENU_LANG_POL      DecodeMenuLanguage('p','o','l')
#define CEC_MENU_LANG_TUR      DecodeMenuLanguage('t','u','r')
#define CEC_MENU_LANG_SLK      DecodeMenuLanguage('s','l','k')
#define CEC_MENU_LANG_SLO      DecodeMenuLanguage('s','l','o')
#define CEC_MENU_LANG_SLV      DecodeMenuLanguage('s','l','v')
#define CEC_MENU_LANG_SCR      DecodeMenuLanguage('s','c','r')
#define CEC_MENU_LANG_HRV      DecodeMenuLanguage('h','r','v')
#define CEC_MENU_LANG_CZE      DecodeMenuLanguage('c','z','e')
#define CEC_MENU_LANG_CES      DecodeMenuLanguage('c','e','s')
#define CEC_MENU_LANG_HUN      DecodeMenuLanguage('h','u','n')
#define CEC_MENU_LANG_RUM      DecodeMenuLanguage('r','u','m')
#define CEC_MENU_LANG_RON      DecodeMenuLanguage('r','o','n')
#define CEC_MENU_LANG_MAY      DecodeMenuLanguage('m','a','y')
#define CEC_MENU_LANG_MSA      DecodeMenuLanguage('m','s','a')
#define CEC_MENU_LANG_GRE      DecodeMenuLanguage('g','r','e')
#define CEC_MENU_LANG_ELL      DecodeMenuLanguage('e','l','l')
#define CEC_MENU_LANG_ENG      DecodeMenuLanguage('e','n','g')
#define CEC_MENU_LANG_POR      DecodeMenuLanguage('p','o','r')
#define CEC_MENU_LANG_SPA      DecodeMenuLanguage('s','p','a')
#define CEC_MENU_LANG_ESP      DecodeMenuLanguage('e','s','p')
#define CEC_MENU_LANG_GRE      DecodeMenuLanguage('g','r','e')
#define CEC_MENU_LANG_SUN      DecodeMenuLanguage('s','u','n')
#define CEC_MENU_LANG_SVE      DecodeMenuLanguage('s','v','e')  
#define CEC_MENU_LANG_HIN      DecodeMenuLanguage('h','i','n') 
#define CEC_MENU_LANG_THA      DecodeMenuLanguage('t','h','a')     
#define CEC_MENU_LANG_UKR      DecodeMenuLanguage('u','k','r')     


#define BYTE1(x)                    ((x & 0xFF))
#define BYTE2(x)                    ((x>> 8) & 0xFF)
#define BYTE3(x)                    ((x>>16) & 0xFF)
#define BYTE4(x)                    ((x>>24) & 0xFF)

#define FREQ_STEP                   62500
#define FREQ_DIV(x)                 ((((unsigned long)x) + (FREQ_STEP>>1))/FREQ_STEP)


#if __cplusplus 
}
#endif

#endif  //__CEC_AUX_H__
