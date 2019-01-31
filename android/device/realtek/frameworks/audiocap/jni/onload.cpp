#include <JNIHelp.h>
#include <jni.h>
#include <utils/Log.h>
#include <utils/misc.h>

namespace android {
int register_com_realtek_AudioCap(JNIEnv* env);
};

using namespace android;

extern "C" jint JNI_OnLoad(JavaVM* vm, void* reserved)
{
    JNIEnv* env = NULL;
    jint result = -1;

    if (vm->GetEnv((void**) &env, JNI_VERSION_1_4) != JNI_OK) {
        ALOGE("GetEnv failed!");
        return result;
    }
    ALOG_ASSERT(env, "Could not retrieve the env!");

    if (register_com_realtek_AudioCap(env) < 0) {
        ALOGE("ERROR: AudioCap native registration failed\n");
        goto bail;
    }


    result = JNI_VERSION_1_4;

    ALOGD("Successfully load JNI library");

    bail:
        return result;
}

