#ifndef __CEC_DEBUG_H__
#define __CEC_DEBUG_H__
#include <android/log.h>
#define AP_DEBUG_LEVEL 3
//-------------------------- 
#if (AP_DEBUG_LEVEL>0)//Error
	#define CEC_DBG_BASIC_EN
	#if (AP_DEBUG_LEVEL>1)//Debug
		#define CEC_DBG_EN
		#if (AP_DEBUG_LEVEL>2)//Trace
			#define CEC_TX_DBG_EN
			#define CEC_RX_DBG_EN
		#endif
	#endif
#endif
//-------------------------- 
#ifdef CEC_DBG_EN
    #define CEC_DBG(fmt, ...)            __android_log_print(ANDROID_LOG_DEBUG,"[CEC] ", fmt, ##__VA_ARGS__)
#else    
    #define CEC_DBG(fmt, ...)            
#endif

#ifdef CEC_DBG_BASIC_EN
	#define CEC_PRINT(fmt, ...)             __android_log_print(ANDROID_LOG_INFO,"[CEC] ", fmt, ##__VA_ARGS__)
	#define CEC_INFO(fmt, ...)              __android_log_print(ANDROID_LOG_INFO,"[CEC] INFO,", fmt, ##__VA_ARGS__)
	#define CEC_WARNING(fmt, ...)           __android_log_print(ANDROID_LOG_WARN,"[CEC] WARNING, ", fmt, ##__VA_ARGS__)
	#define CEC_AP_INFO(fmt, ...)            __android_log_print(ANDROID_LOG_INFO,"[CEC] AP INFO,", fmt, ##__VA_ARGS__)
	#define CEC_AP_WARNING(fmt, ...)         __android_log_print(ANDROID_LOG_WARN,"[CEC] AP WARNING, ", fmt, ##__VA_ARGS__)
#else
	#define CEC_PRINT(fmt, ...)             
	#define CEC_INFO(fmt, ...)              
	#define CEC_WARNING(fmt, ...)           
	#define CEC_AP_INFO(fmt, ...)            
	#define CEC_AP_WARNING(fmt, ...)        
#endif

#ifdef CEC_TX_DBG_EN
    #define CEC_TX_DBG(fmt, ...)        __android_log_print(ANDROID_LOG_DEBUG,"[CEC] TX_DBG,", fmt, ##__VA_ARGS__)
    #define CEC_TX_DBG_NO_TITLE(fmt, ...)        __android_log_print(ANDROID_LOG_DEBUG,"", fmt, ##__VA_ARGS__)
#else
    #define CEC_TX_DBG(fmt, ...)
    #define CEC_TX_DBG_NO_TITLE(fmt, ...)
#endif

#ifdef CEC_RX_DBG_EN
    #define CEC_RX_DBG(fmt, ...)         __android_log_print(ANDROID_LOG_DEBUG,"[CEC] RX_DBG,", fmt, ##__VA_ARGS__)
    #define CEC_RX_DBG_NO_TITLE(fmt, ...)        __android_log_print(ANDROID_LOG_DEBUG,"", fmt, ##__VA_ARGS__)
#else
    #define CEC_RX_DBG(fmt, ...)         
    #define CEC_RX_DBG_NO_TITLE(fmt, ...)
#endif

#ifdef CEC_AP_DBG_EN
    #define CEC_AP_DBG(fmt, ...)         __android_log_print(ANDROID_LOG_DEBUG,"[CEC] AP DBG,", fmt, ##__VA_ARGS__)
#else
    #define CEC_AP_DBG(fmt, ...)         
#endif
#endif  //__CEC_DEBUG_H__

