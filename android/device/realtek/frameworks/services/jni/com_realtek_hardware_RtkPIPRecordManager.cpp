/*
 * Copyright (c) 2017 Realtek Semi. co. All Rights Reserved.
 * Comment: JNI used to call Realtek Transcode Service functions.
 */

#define LOG_NDEBUG 0
#define LOG_TAG "RtkPIPRecordManager-JNI"
#include <utils/Log.h>

#include "jni.h"
#include "JNIHelp.h"
#include "android_runtime/AndroidRuntime.h"

#include <cutils/properties.h>
#include <utils/Vector.h>

#include <binder/IServiceManager.h>
#include <binder/ProcessState.h>
#include <binder/IPCThreadState.h>
#include <binder/MemoryBase.h>
#include <binder/MemoryHeapBase.h>
#include <binder/IMemory.h>
#include <gui/GLConsumer.h>
#include <gui/Surface.h>
#include "RtkTranscodeCommon.h"
#include "RtkTranscodePlayer.h"
// added for binder service mode.
#include "RtkIpc/TranscodeService.h"
#include "RtkIpc/IRtkTranscodePlayerServiceClient.h"

// use binder service
#define USE_RTK_TRANSCODEPLAYER_SERVICE

#define LOG_ERROR_LINE()    ALOGE("[%s][%d] TranscodePlayer instance in NULL",__FUNCTION__,__LINE__)

namespace android {

static JavaVM *g_jvm;

#ifdef USE_RTK_TRANSCODEPLAYER_SERVICE

// single instance should be okay since they are exist in different process.
static sp<IRtkTranscodePlayerServiceClient> sTranscodePlayer;
static sp<Camera> mCamera;

#else
static sp<RtkTranscodePlayer> sTranscodePlayer;
#endif


static AV_FORMAT_CONFIG TargetFormat = {
    {
            640,     // width
            480,     // height
            30,      // fps;
            9000000, // bitrate
            1,       // iframeInterval
            1,       // rateControl , default is variable
            0,       // aspectRatio
            0,       // interlace
            0,       // shareWBbuf
    },
    {
            2,      // channelCount;
            0,      // channelMode
            48000,  // sampleRate
            192000, // bitrate
    }
};

static RtkTranscodeConf config;


static void realtek_hardware_RtkPIPRecordManager_native_initiate(JNIEnv *env, jobject thiz)
{
    ALOGV("RtkPIPRecordManager_native_initiate");
#ifdef USE_RTK_TRANSCODEPLAYER_SERVICE
    ProcessState::self()->startThreadPool();
    sp<IRtkTranscodePlayerService> sm(TranscodeService::getTranscodeService());

    int clientCount = sm->getClientsCount();
    ALOGD("RtkTranscodePlayer instance count: %d",clientCount);

    sTranscodePlayer = sm->create(0); // onlyTrack : default 0

#else
    sTranscodePlayer = new RtkTranscodePlayer(true, 0);
#endif 
    
}

static jint realtek_hardware_RtkPIPRecordManager_native_prepare(JNIEnv *env, jobject thiz, jstring clientPackageName, jint type)
{
    ALOGV("RtkPIPRecordManager_native_prepare");
    // Convert jstring to String16
    /*
    const jchar *rawClientName = env->GetStringChars(clientPackageName, NULL);
    jsize rawClientNameLen = env->GetStringLength(clientPackageName);
    String16 clientName((const char16_t *)rawClientName, rawClientNameLen);
    env->ReleaseStringChars(clientPackageName, rawClientName);
    */
    
    jint rst = 0;
    if(type == TranscodeTypeMixer1) {
        ALOGV("Record Mixer1");
        rst = sTranscodePlayer->initiate(TranscodeTypeMixer1);
    }
    else if(type = TranscodeTypeV1) {
        ALOGV("Record V1");
        rst = sTranscodePlayer->initiate(TranscodeTypeV1);
    }
    else {
        ALOGV("default record Mixer1");
        rst = sTranscodePlayer->initiate(TranscodeTypeMixer1);
    }

    return rst;
}

static void realtek_hardware_RtkPIPRecordManager_native_configureTargetFormat
  (JNIEnv *env, jobject thiz, jint w, jint h, jint fps, jint vbitrate,
    jint iframeInterval, jint rateControl, jint aspectRatio,
    jint interlace, jint shareWBbuf,
    jint channelCount, jint channelMode, jint sampleRate, jint abitrate)
{
    ALOGD("RtkPIPRecordManager_native_configureTargetFormat");
    // video parameters
    TargetFormat.videoFmt.width = w;
    TargetFormat.videoFmt.height = h;
    TargetFormat.videoFmt.fps = fps;
    TargetFormat.videoFmt.bitrate = vbitrate;
    TargetFormat.videoFmt.iframeInterval = iframeInterval;
    TargetFormat.videoFmt.rateControl = rateControl;
    TargetFormat.videoFmt.aspectRatio = aspectRatio;
    TargetFormat.videoFmt.interlace = interlace;
    TargetFormat.videoFmt.shareWBbuf = shareWBbuf;
    // audio parameters
    TargetFormat.audioFmt.channelCount = channelCount;
    TargetFormat.audioFmt.channelMode = channelMode;
    TargetFormat.audioFmt.sampleRate = sampleRate;
    TargetFormat.audioFmt.bitrate = abitrate;

    if(sTranscodePlayer.get() == NULL) {
        LOG_ERROR_LINE();
        return;
    }
    //sTranscodePlayer->configureTargetFormat(TargetFormat);
}

static void realtek_hardware_RtkPIPRecordManager_native_setTargetFd
                    (JNIEnv *env, jobject thiz, jint fd, jint format)
{
    ALOGD("RtkPIPRecordManager_native_setTargetFd fd:%d format:%d",fd,format);

    config.outputType = OUTPUT_FD;
    config.outputFormat = format;
    config.fileDesc = fd;
    config.fileName = "";
    config.ashmemData = NULL;
    config.ashmemInfo = NULL;
    config.udpIp = "";
    config.udpPort = 0;
    config.rtpHeader = false;

    if(sTranscodePlayer.get() == NULL) {
        LOG_ERROR_LINE();
        return;
    }

    sTranscodePlayer->setOutputFormat(config);
}

static jint realtek_hardware_RtkPIPRecordManager_startTranscode(JNIEnv *env, jobject thiz)
{
    ALOGV("RtkPIPRecordManager_startTranscode");

    if(sTranscodePlayer.get() == NULL) {
        LOG_ERROR_LINE();
        return -1;
    }
    jint rst = sTranscodePlayer->configureTargetFormat(TargetFormat);
    if(rst != 0) {
        LOG_ERROR_LINE();
        return -1;
    }
    rst = sTranscodePlayer->setTranscode(true);
    return rst;
}

static jint realtek_hardware_RtkPIPRecordManager_stopTranscode(JNIEnv *env, jobject thiz)
{
    ALOGV("RtkPIPRecordManager_stopPreview");

    if(sTranscodePlayer.get() == NULL){
        LOG_ERROR_LINE();
        return -1;
    }

    jint rst = sTranscodePlayer->stop();

    return rst;
}

static void realtek_hardware_RtkPIPRecordManager_release(JNIEnv *env, jobject thiz)
{
    ALOGV("RtkPIPRecordManager_release");
#ifdef USE_RTK_TRANSCODEPLAYER_SERVICE

    // only call release in binder mode
    if(sTranscodePlayer != NULL)
    {
        sTranscodePlayer->release();
    }

#endif
    if (sTranscodePlayer!=NULL)
    {
        sTranscodePlayer = NULL;
    }
}

//------------------------------------------------------------------------------------------------------------------

/*
 * Array of methods.
 *
 * Each entry has three fields: the name of the method,
 * the method signature, and a pointer to the native implementation.
 */
static const JNINativeMethod RtkPIPRecordManagerMethods[] = {
    {"native_initiate", "()V", (void*) realtek_hardware_RtkPIPRecordManager_native_initiate },
    {"native_prepare", "(Ljava/lang/String;I)I", (void*) realtek_hardware_RtkPIPRecordManager_native_prepare },
    {"native_configureTargetFormat", "(IIIIIIIIIIIII)V", (void*) realtek_hardware_RtkPIPRecordManager_native_configureTargetFormat },
    {"native_setTargetFd", "(II)V", (void*) realtek_hardware_RtkPIPRecordManager_native_setTargetFd },
    {"native_startTranscode", "()I", (void*) realtek_hardware_RtkPIPRecordManager_startTranscode },
    {"native_stopTranscode", "()I", (void*) realtek_hardware_RtkPIPRecordManager_stopTranscode },
    {"native_release", "()V", (void*) realtek_hardware_RtkPIPRecordManager_release },
};

void set_realtek_hardware_RtkPIPRecordManager_JavaVMInstance(JavaVM *vm)
{
    g_jvm = vm;
}

/*
* This is called by the VM when the shared library is first loaded.
*/
int register_realtek_hardware_RtkPIPRecordManager(JNIEnv *env) {

    jclass clazz = env->FindClass("com/realtek/hardware/RtkPIPRecordManager");

    if (clazz == NULL)
    {
        ALOGE("Can't find com/realtek/hardware/RtkPIPRecordManager");
        return -1;
    }

    if(jniRegisterNativeMethods(env, "com/realtek/hardware/RtkPIPRecordManager",
        RtkPIPRecordManagerMethods, NELEM(RtkPIPRecordManagerMethods)) != 0) {
        ALOGD("Register method to com/realtek/hardware/RtkPIPRecordManager failed!");
        return -1;
    }

    return 0;
}


} /* namespace android */
