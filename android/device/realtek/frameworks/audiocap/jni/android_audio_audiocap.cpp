#include <utils/Mutex.h>
#include <binder/IServiceManager.h>
#include <binder/IInterface.h>
#include <binder/Parcel.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <cutils/log.h>
#include <utils/RefBase.h>
#include <utils/String16.h> 
#include "RedirectCommand.h"
#include "AudioRedirectService.h"
#include "RtkAudioCommon.h"
#include "android_runtime/AndroidRuntime.h"
#include <jni.h>
#include <JNIHelp.h>

namespace android {
/*
 * Class:     android_audio_AudioCap
 * Method:    native_set_cap_mode
 * Signature: (I)V
 */
JNIEXPORT void JNICALL Java_com_realtek_AudioCap_native_set_cap_mode
  (JNIEnv *env, jobject thiz, jint mode)
{
	//ALOGD("Java_android_audio_AudioCap_native_set_cap_mode");
	sp<IBinder> binder = defaultServiceManager()->getService(String16("vendor.realtek.AudioRedirect"));
    if(binder == NULL) 
	{
		ALOGD("failed to get AudioRedirect Service %s %d\n", __func__, __LINE__);
    } 
	else 
	{
		Parcel data, reply;
		data.writeInt32((int)mode);
		binder->transact(SET_CAP_MODE, data, &reply);
    }
}

/*
 * Class:     android_audio_AudioCap
 * Method:    native_start_audio_cap
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_com_realtek_AudioCap_native_start_audio_cap
  (JNIEnv *env, jobject thiz)
{
	//ALOGD("Java_android_audio_AudioCap_native_start_audio_cap");
	sp<IBinder> binder = defaultServiceManager()->getService(String16("vendor.realtek.AudioRedirect"));
    if(binder == NULL) 
	{
		ALOGD("failed to get AudioRedirect Service %s %d\n", __func__, __LINE__);
    } 
	else 
	{
		Parcel data, reply;
		binder->transact(START_AUDIO_CAP, data, &reply);
    }
}

/*
 * Class:     android_audio_AudioCap
 * Method:    native_stop_audio_cap
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_com_realtek_AudioCap_native_stop_audio_cap
  (JNIEnv *env, jobject thiz)
{
	//ALOGD("Java_android_audio_AudioCap_native_stop_audio_cap");
	sp<IBinder> binder = defaultServiceManager()->getService(String16("vendor.realtek.AudioRedirect"));
    if(binder == NULL) 
	{
		ALOGD("failed to get AudioRedirect Service %s %d\n", __func__, __LINE__);
    } 
	else 
	{
		Parcel data, reply;
		binder->transact(STOP_AUDIO_CAP, data, &reply);
    } 
}

/*
 * Class:     android_audio_AudioCap
 * Method:    native_get_one_pcmbuffer
 * Signature: ()[B
 */
JNIEXPORT jbyteArray JNICALL Java_com_realtek_AudioCap_native_get_one_pcmbuffer
  (JNIEnv *env, jobject thiz)
{
	//ALOGD("Java_android_audio_AudioCap_native_get_one_pcmbuffer");

	sp<IBinder> binder = defaultServiceManager()->getService(String16("vendor.realtek.AudioRedirect"));
    if(binder == NULL) 
	{
		ALOGD("failed to get AudioRedirect Service %s %d\n", __func__, __LINE__);
		return NULL;
    } 
	else 
	{
		Parcel data, reply;
		binder->transact(GET_PCM_DATA, data, &reply);
		const unsigned char *buf = reply.data();
		unsigned int len = reply.dataSize();
	    jbyteArray byteArray = env->NewByteArray(len);
		jbyte *bytes = env->GetByteArrayElements(byteArray, NULL);
		memcpy(bytes, buf, len);
        env->ReleaseByteArrayElements(byteArray, bytes, 0);
		//env->SetByteArrayRegion(byteArray, 0, len, buf);
		return byteArray;
	}
}

/*
 * Class:     android_audio_AudioCap
 * Method:    native_flush_pcmbufffer_queue
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_com_realtek_AudioCap_native_flush_pcmbufffer_queue
  (JNIEnv *env, jobject thiz) 
{
	//ALOGD("Java_android_audio_AudioCap_native_flush_pcmbufffer_queue");
	sp<IBinder> binder = defaultServiceManager()->getService(String16("vendor.realtek.AudioRedirect"));
    if(binder == NULL) 
	{
		ALOGD("failed to get AudioRedirect Service %s, %d\n", __func__, __LINE__);
    } 
	else 
	{
		Parcel data, reply;
		binder->transact(FLUSH_PCM_DATA, data, &reply);
    }  
}

static const JNINativeMethod sMethods[] = {
    {"native_set_cap_mode", "(I)V", (void*) Java_com_realtek_AudioCap_native_set_cap_mode },
    {"native_start_audio_cap", "()V", (void*) Java_com_realtek_AudioCap_native_start_audio_cap },
    {"native_stop_audio_cap", "()V", (void*) Java_com_realtek_AudioCap_native_stop_audio_cap },
    {"native_get_one_pcmbuffer", "()[B", (void*) Java_com_realtek_AudioCap_native_get_one_pcmbuffer },
    {"native_flush_pcmbufffer_queue", "()V", (void*) Java_com_realtek_AudioCap_native_flush_pcmbufffer_queue },
};

int register_com_realtek_AudioCap(JNIEnv *env) {
	//ALOGI("[%s][%s][%d], euid=%d\n", __FILE__, __func__, __LINE__,geteuid());
	//ALOGD("register_android_audio_AudioCap");
	jclass clazz = env->FindClass("com/realtek/AudioCap");
    if (clazz == NULL)
	{
		ALOGE("com/realtek/AudioCap clazz is null");
		return -1;
	}
    return jniRegisterNativeMethods(env, "com/realtek/AudioCap",sMethods, NELEM(sMethods));
}

}
