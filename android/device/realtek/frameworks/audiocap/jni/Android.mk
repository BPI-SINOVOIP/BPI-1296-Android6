LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)

LOCAL_SRC_FILES += android_audio_audiocap.cpp
LOCAL_SRC_FILES += onload.cpp

LOCAL_C_INCLUDES += \
    $(JNI_H_INCLUDE) \
	device/realtek/proprietary/libs/libaudioRedirect\
	
LOCAL_C_INCLUDES += frameworks/av/media/libstagefright
LOCAL_C_INCLUDES += system/core/include
LOCAL_C_INCLUDES += device/realtek/proprietary/libs/libRTKAllocator/include
LOCAL_C_INCLUDES += system/core/include/cutils
LOCAL_C_INCLUDES += hardware/realtek/realtek_omx/osal_rtk
LOCAL_C_INCLUDES += device/realtek/proprietary/libs/rtk_libs/common/IPC/generate/include/system
LOCAL_C_INCLUDES += device/realtek/proprietary/libs/rtk_libs/common/IPC/include
LOCAL_C_INCLUDES += device/realtek/proprietary/libs/rtk_libs/common/IPC/include/xdr
LOCAL_C_INCLUDES += device/realtek/proprietary/libs/rtk_libs/common/IPC/src/xdr
LOCAL_C_INCLUDES += device/realtek/proprietary/libs/rtk_libs/OSAL/include
LOCAL_C_INCLUDES += device/realtek/proprietary/libs/rtk_libs/OSAL/include/OSAL
LOCAL_C_INCLUDES += external/tinyalsa/include
LOCAL_C_INCLUDES += $(call include-path-for, audio-utils)
LOCAL_C_INCLUDES += system/core/include/ion

LOCAL_SHARED_LIBRARIES := \
    libandroid_runtime \
    libandroidfw \
    libnativehelper \
    libcutils \
    liblog \
    libskia \
    libutils \
    libui \
    libgui \
    libjnigraphics

LOCAL_SHARED_LIBRARIES += libbinder

LOCAL_CFLAGS += -DENABLE_RTK_MIRACAST_SINK_SERVICE
LOCAL_CFLAGS += -DHDCP_REPEATER=0
LOCAL_CFLAGS += -DHDCP2_KEY_PROTECT_WITH_KIMG
LOCAL_CFLAGS += -DHDCP2_1

LOCAL_MODULE := libaudiocap_jni
LOCAL_MODULE_TAGS := optional
include $(BUILD_SHARED_LIBRARY)



