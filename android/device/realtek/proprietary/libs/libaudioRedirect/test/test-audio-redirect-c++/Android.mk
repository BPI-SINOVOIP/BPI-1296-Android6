LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)

LOCAL_MODULE := test-audio-redirect
LOCAL_MODULE_TAGS := optional tests
LOCAL_CFLAGS := -DLOG_TAG=\"TestAudioRedirect\"

LOCAL_SHARED_LIBRARIES := liblog
LOCAL_SHARED_LIBRARIES += libutils
LOCAL_SHARED_LIBRARIES += libRtkAudioRedirect
LOCAL_SHARED_LIBRARIES += libRTKAllocator

LOCAL_SHARED_LIBRARIES += libstagefright
LOCAL_SHARED_LIBRARIES += libstagefright_foundation
LOCAL_SHARED_LIBRARIES += libbinder
LOCAL_SHARED_LIBRARIES += libmedia
LOCAL_SHARED_LIBRARIES += libRTKOMX_OSAL_RTK
LOCAL_SHARED_LIBRARIES += libRTK_lib
LOCAL_SHARED_LIBRARIES += libtinyalsa
LOCAL_SHARED_LIBRARIES += libaudioutils

LOCAL_C_INCLUDES += frameworks/av/media/libstagefright
LOCAL_C_INCLUDES += system/core/include
LOCAL_C_INCLUDES += device/realtek/proprietary/libs/libRTKAllocator/include

#LOCAL_C_INCLUDES += frameworks/native/include/media/openmax
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


LOCAL_SRC_FILES := main.cpp
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../..
include $(BUILD_EXECUTABLE)
