/*
 * Copyright (c) 2014 Realtek Semiconductor Corp.
 */
//#define LOG_NDEBUG 0
//#define LOG_TAG "DPTxSvcJni"
#include <utils/Log.h>

#include <jni.h>
#include <JNIHelp.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <cutils/properties.h>

#include "android_runtime/AndroidRuntime.h"
#include "Platform_Lib/HDMIControl/hdmi.h"
#include <Application/AppClass/VoutUtil.h>
#include <Application/AppClass/SetupClass.h>
#include <Platform_Lib/HDMIControl/DPTXController.h>
#include <Platform_Lib/HDMIControl/HDMIController.h>



namespace android {

static void realtek_server_RtkDPTxService_setTVSystem(JNIEnv *env, jobject obj, jint select)
{
	int ret;
	int videotype = 4;
	ENUM_VIDEO_SYSTEM videoSystem;
	ENUM_VIDEO_STANDARD videoStandard = VIDEO_PROGRESSIVE;

	ALOGD("enter realtek_server_RtkDPTxService_setTVSystem: %d\n", select);
	if (select == -1)
	{
		ret = DPTX_GetAutoVideoFormat();
		if (ret == -1)
			videoSystem = VIDEO_DP_720P_60HZ;
		else
			videoSystem = (ENUM_VIDEO_SYSTEM) ret;		
	}
	else
		videoSystem = (ENUM_VIDEO_SYSTEM)select;
	
	char value[PROPERTY_VALUE_MAX];
	property_get("persist.rtk.hdmitx.dpdiffsource", value, "off");
	if (strcmp(value, "on") == 0)
	{
		videotype = 5;
	}

	if(HDMIController::isSinkConnected())
		VoutUtil::instance().setTvResolution(VIDEO_SYSTEM_NUM, VIDEO_PROGRESSIVE, videoSystem, videoStandard, videotype);
	else
		VoutUtil::instance().setTvResolution(VIDEO_SYSTEM_NUM, VIDEO_PROGRESSIVE, videoSystem, videoStandard, 3); //display port only	videoStandard = VIDEO_PROGRESSIVE;
			
}

static void realtek_server_RtkDPTxService_handleHotPlug(JNIEnv *env, jobject obj, jint state)
{

    {
        char value[PROPERTY_VALUE_MAX];
        memset(value,0x0,PROPERTY_VALUE_MAX);
        property_get("persist.rtk.dp.enabled",value,"1");
        if(strncmp(value,"1",1)==0){
            ALOGD("DP is enabled by properties");
        }else{
            ALOGD("DP is disabled by properties");
            state = 0;
        }
    }

	if(state) {	// dptx is plugged in
		ALOGD("dptx is plugged in\n");
		DPTXController::getSinkCapability();

		DPTXController::setSinkConnected(true);

		int ret;
		int videotype = 4;
		ENUM_VIDEO_SYSTEM videoSystem = VIDEO_SYSTEM_NUM;
		ENUM_VIDEO_STANDARD videoStandard = VIDEO_PROGRESSIVE;

		char value[PROPERTY_VALUE_MAX];
		
		property_get("persist.rtk.hdmitx.dp_user", value, "0");
		/*ref: RtkTVSystem.java
		   TV_SYS_DP_AUTO_DETECT = 0, 
		   TV_SYS_DP_720P_60HZ = 1,
		   TV_SYS_DP_1080P_60HZ = 2,
		   TV_SYS_DP_2160P_30HZ = 3*/
		if (strcmp(value, "1") == 0)
		{
			videoSystem = VIDEO_DP_720P_60HZ;
		}
		else if (strcmp(value, "2") == 0)
		{
			videoSystem = VIDEO_DP_1080P_60HZ;
		}
		else if (strcmp(value, "3") == 0)
		{
			videoSystem = VIDEO_DP_2160P_30HZ;
		}
		else //Auto
		{
			ret = DPTX_GetAutoVideoFormat();
			if (ret == -1)
				videoSystem = VIDEO_DP_720P_60HZ;
			else
				videoSystem = (ENUM_VIDEO_SYSTEM) ret;
		}
		
		property_get("persist.rtk.hdmitx.dpdiffsource", value, "off");
		if (strcmp(value, "on") == 0)
		{
			videotype = 5;
			VoutUtil::instance().setTvResolution(VIDEO_SYSTEM_NUM, VIDEO_PROGRESSIVE, videoSystem, videoStandard, videotype);
		}
		else {
			if(HDMIController::isSinkConnected())
				VoutUtil::instance().setTvResolution(VIDEO_SYSTEM_NUM, VIDEO_PROGRESSIVE, videoSystem, videoStandard, videotype);
			else
				VoutUtil::instance().setTvResolution(VIDEO_SYSTEM_NUM, VIDEO_PROGRESSIVE, videoSystem, videoStandard, 3); //display port only
		}
	}
	else // dptx is plugged out
	{
		ALOGD("dptx is plugged out\n");
		DPTXController::setSinkConnected(false);
		if(HDMIController::isSinkConnected())
			VoutUtil::instance().setTvResolution(VIDEO_SYSTEM_NUM, VIDEO_PROGRESSIVE);
		else
			HDMI_TurnOffVideo();
	}
}

static jint realtek_server_RtkDPTXService_initMediaLinkController(JNIEnv *env, jobject obj) {
	ALOGD("realtek_server_RtkDPTXService_initMediaLinkController\n");

	DPTXController::initDPTXController();
	return 0;
}

static jboolean realtek_server_RtkDPTxService_checkifDPTxPlugged(JNIEnv *env, jobject obj) {
	ALOGD("enter realtek_server_RtkDPTxService_checkifDPTxPlugged\n");
	if(DPTXController::isSinkConnected()) {
		// connected
		return true;
	} else {
		// disconnected
		return false;
	}
}

static jboolean realtek_server_RtkDPTxService_checkIfDPTxEDIDReady(JNIEnv *env, jobject obj) {
	ALOGD("enter realtek_server_RtkDPTxService_checkIfDPTxEDIDReady\n");
	if(DPTXController::isEDIDReady()) {
		// connected
		return true;
	} else {
		// disconnected
		return false;
	}
}

static jobjectArray realtek_server_RtkDPTxService_getVideoFormat(JNIEnv *env, jobject obj) {
	ALOGD("enter realtek_server_RtkDPTxService_getVideoFormat\n");

	jobjectArray arrRtkTVSystem;
	int len = 0;
	struct HDMI_TvSystemAndStandard arrTvSS[64];

	// Get the available TV Video format.
	if(DPTX_GetVideoFormat(&len, (HDMI_TvSystemAndStandard *)&arrTvSS) != 1 || len > 64) {
		len = 0;
	}

	jclass RtkTVSystemCls = env->FindClass("com/realtek/server/RtkTVSystem");
	jmethodID mID = env->GetMethodID(RtkTVSystemCls, "<init>", "()V"); //Constructor

	// Create the array of RtkTVSystem
	arrRtkTVSystem = env->NewObjectArray(len, RtkTVSystemCls, NULL);

	for(int i = 0; i < len; i++) {
		// create RtkTVSystem objects
		jobject RtkTVSystemObj = env->NewObject(RtkTVSystemCls, mID);

		// get the field of RtkTVSystem
		jfieldID fid_intVideoSystem = env->GetFieldID(RtkTVSystemCls, "intVideoSystem", "I");
		jfieldID fid_intVideoStandard = env->GetFieldID(RtkTVSystemCls, "intVideoStandard", "I");
		jfieldID fid_intIsWideScreen = env->GetFieldID(RtkTVSystemCls, "intIsWideScreen", "I");

		// copy the TV system and standard information into RtkTVSystemObject
		env->SetIntField(RtkTVSystemObj, fid_intVideoSystem, arrTvSS[i].videoSystem);
		env->SetIntField(RtkTVSystemObj, fid_intVideoStandard, arrTvSS[i].videoStandard);
		env->SetIntField(RtkTVSystemObj, fid_intIsWideScreen, arrTvSS[i].bIsWidescreen);

		// Set the object to the object array
		env->SetObjectArrayElement(arrRtkTVSystem, i, RtkTVSystemObj);

		env->DeleteLocalRef(RtkTVSystemObj);
	}

	return arrRtkTVSystem;
}


/*
 * JNI registration.
 */
static JNINativeMethod gMethods[] = {
    /* name, signature, funcPtr */
	{"_checkifDPTxPlugged", "()Z", (void*) realtek_server_RtkDPTxService_checkifDPTxPlugged },
	{"_checkIfDPTxEDIDReady", "()Z", (void*) realtek_server_RtkDPTxService_checkIfDPTxEDIDReady },	
	{"_getVideoFormat", "()[Lcom/realtek/server/RtkTVSystem;", (void*) realtek_server_RtkDPTxService_getVideoFormat },
    { "nativeSetTVSystem", "(I)V", (void*) realtek_server_RtkDPTxService_setTVSystem },
    { "handleHotPlug", "(I)V", (void*) realtek_server_RtkDPTxService_handleHotPlug },
	{ "initMediaLinkController", "()I", (void*) realtek_server_RtkDPTXService_initMediaLinkController },
};

int register_realtek_server_RtkDPTxService(JNIEnv* env)
{
	jclass clazz = env->FindClass("com/realtek/server/RtkDPTxService");

	if (clazz == NULL)
	{
		ALOGE("Can't find com/realtek/server/RtkDPTxService");
		return -1;
	}

	if(jniRegisterNativeMethods(env, "com/realtek/server/RtkDPTxService",
		gMethods, NELEM(gMethods)) != 0) {
		ALOGD("Register method to com/realtek/server/RtkDPTxService failed!");
		return -1;
	}

	return 0;
}

} // namespace android
