#include <jni.h>
#include <JNIHelp.h>
#include <android/log.h>
#include <utils/Log.h>
#include <android_runtime/AndroidRuntime.h>
#include <ctype.h>
#include "CECAPBridgeImpl.h"

#define TAG "RtkCECJNI: "

namespace android {
	static JavaVM * gVM;
	static jobject gCECServiceObject = NULL;
	//Call back method ID List
	static jmethodID SetMenuLanguage_Tojava;
	static jmethodID GetSystemStandby_Tojava;

	static void CB_SetMenuLanguage(int MenuLanguage)
	{
		JNIEnv *env;
		int getEnvStat;
        bool NewAttached = false;
		getEnvStat = gVM->GetEnv((void **)&env, JNI_VERSION_1_4);
		if (getEnvStat == JNI_EDETACHED){
			if (gVM->AttachCurrentThread(&env, NULL)!=0){
				ALOGD(TAG "Failed to attachCurrentThread for CB_SetMenuLanguage");
			}
            else
            {
                ALOGD(TAG "NewAttached Java Thread");
                NewAttached = true;
            }
		}else if (getEnvStat == JNI_OK){
			ALOGD(TAG "Get Env OK");
		}else{
			ALOGD(TAG "Get Env NG");
		}		
		env->CallVoidMethod(gCECServiceObject, SetMenuLanguage_Tojava, MenuLanguage);
		if (env->ExceptionCheck()) {
        	env->ExceptionDescribe();
    	}
        if (NewAttached)
        {
		    gVM->DetachCurrentThread();
        }
	}
    static void CB_GetSystemStandby()
    {
		JNIEnv *env;
		int getEnvStat;
        bool NewAttached = false;
		getEnvStat = gVM->GetEnv((void **)&env, JNI_VERSION_1_4);
		if (getEnvStat == JNI_EDETACHED){
			if (gVM->AttachCurrentThread(&env, NULL)!=0){
				ALOGD(TAG "Failed to attachCurrentThread for CB_SetMenuLanguage");
			}
            else
            {
                ALOGD(TAG "NewAttached Java Thread");
                NewAttached = true;
            }
		}else if (getEnvStat == JNI_OK){
			ALOGD(TAG "Get Env OK");
		}else{
			ALOGD(TAG "Get Env NG");
		}		
		env->CallVoidMethod(gCECServiceObject, GetSystemStandby_Tojava);
		if (env->ExceptionCheck()) {
        	env->ExceptionDescribe();
    	}
        if (NewAttached)
        {
		    gVM->DetachCurrentThread();
        }
    }
	//
	static CECAPBridgeImpl* g_pCECAPBridgeImpl = NULL;
/*
	static bool IsCECAPBridgeImplAlive()
	{
		bool ret = true;
		if (g_pCECAPBridgeImpl == NULL)
		{
			ret = false;
		}
		return ret;
	}
*/
    //Call back API for CECAPBridge
    static int SetMenuLanguage(CEC_MENU_LANGUAGE cec_language)
    {
        ALOGV(TAG "SetMenuLanguage CEC Language=[%x][Start]",cec_language);
        CB_SetMenuLanguage((int)cec_language);
        ALOGV(TAG "SetMenuLanguage [End]");
        return 0;
    }
    static void GetSystemStandby()
    {
        ALOGV(TAG "GetSystemStandby [Start]");
        CB_GetSystemStandby();
        ALOGV(TAG "GetSystemStandby [End]");
    }

	static jboolean RtkCECService_Init(JNIEnv* env, jobject obj) 
	{
		ALOGV(TAG "CEC Init.");
		// Hold the RtkHDMIServiceObject
		if (gCECServiceObject == NULL)
		{
			gCECServiceObject = env->NewGlobalRef(obj);
		}

		g_pCECAPBridgeImpl = new CECAPBridgeImpl();
        g_pCECAPBridgeImpl->RegCallback_SetMenuLanguage(&SetMenuLanguage);
        g_pCECAPBridgeImpl->RegCallback_GetSystemStandby(&GetSystemStandby);
    	g_pCECAPBridgeImpl->doUpdatePowerState(CEC_POWER_STATUS_ON);

		return true;
	}
	
	static jboolean RtkCECService_doOneTouchPlay(JNIEnv* env, jobject obj, jint Key)
	{
		ALOGV(TAG "RtkCECService_doOneTouchPlay");
    	g_pCECAPBridgeImpl->doOneTouchPlay(3);
		return true;
	}
	static jint RtkCECService_SendRemotePressed(JNIEnv* env, jobject obj, jint DevAddr, jint Key, jboolean SendReleased)
	{
		ALOGV(TAG "RtkCECService_SendRemotePressed");
    	return g_pCECAPBridgeImpl->SendRemotePressed((unsigned short)DevAddr, (unsigned char)Key, (bool)SendReleased);
	}
    static void RtkCECService_HDMIHotPlug(JNIEnv* env, jobject obj, jboolean PlugIn) 
    {
		ALOGV(TAG "RtkCECService_HDMIHotPlug");
        g_pCECAPBridgeImpl->CallBack_hdmi_event(PlugIn);
    }

    static void RtkCECService_SystemStandby(JNIEnv* env, jobject obj, jboolean AllStandby) 
    {
		ALOGV(TAG "RtkCECService_SystemStandby");
        if (AllStandby)
        {
            g_pCECAPBridgeImpl->doAllSystemStandby();
        }
        g_pCECAPBridgeImpl->SetStandbyMode();
        g_pCECAPBridgeImpl->doUpdatePowerState(CEC_POWER_STATUS_ON_TO_STANDBY);
    }

    //JNI table 
	static JNINativeMethod RtkCECServiceMethods[] = {
		{"JNI_Init", "()Z", (void*)RtkCECService_Init},
		{"JNI_doOneTouchPlay", "(I)Z", (void*)RtkCECService_doOneTouchPlay},
		{"JNI_SendRemotePressed", "(IIZ)I", (void*)RtkCECService_SendRemotePressed},
		{"JNI_HDMIHotPlug", "(Z)V", (void*)RtkCECService_HDMIHotPlug},
		{"JNI_SystemStandby", "(Z)V", (void*)RtkCECService_SystemStandby},
	};

/*
* This is called by the VM when the shared library is first loaded.
*/
int register_android_server_RtkCECService(JNIEnv *env) {

	jclass clazz = env->FindClass("com/realtek/server/RtkCECService");

	if (clazz == NULL)
	{
		ALOGE(TAG "Can't find com/realtek/server/RtkCECService");
		return -1;
	}

	// get the VM pointer
	env->GetJavaVM(&gVM);

	// Callbacks for CEC Controller -- start --
	SetMenuLanguage_Tojava = env->GetMethodID(clazz, "SetMenuLanguage", "(I)V");
	GetSystemStandby_Tojava = env->GetMethodID(clazz, "GetSystemStandby", "()V");
	// Callbacks for CEC Controller -- end --

	if(jniRegisterNativeMethods(env, "com/realtek/server/RtkCECService",
		RtkCECServiceMethods, NELEM(RtkCECServiceMethods)) != 0) {
		ALOGD(TAG "Register method to com/realtek/server/RtkCECService failed!");
		return -1;
	}
    //ALOGD(TAG "JNI_Init's address=%x\n", &RtkCECService_Init);
	return 0;
}

}	// namespace android
