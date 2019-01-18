LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)

LOCAL_SRC_FILES := RTK_OMX_Core.c

LOCAL_CFLAGS := $(PV_CFLAGS_MINUS_VISIBILITY)

LOCAL_PRELINK_MODULE := false

LOCAL_C_INCLUDES += hardware/realtek/realtek_omx/include/khronos

LOCAL_C_INCLUDES += device/realtek/proprietary/libs/rtk_libs/common/include
LOCAL_C_INCLUDES += device/realtek/proprietary/libs/rtk_libs/common/IPC/include
LOCAL_C_INCLUDES += device/realtek/proprietary/libs/rtk_libs/common/IPC/generate/include
LOCAL_C_INCLUDES += device/realtek/proprietary/libs/rtk_libs/common/IPC/generate/include/system
LOCAL_C_INCLUDES += device/realtek/proprietary/libs/rtk_libs/OSAL/include
	
LOCAL_SHARED_LIBRARIES += libRTK_lib

LOCAL_SHARED_LIBRARIES += libdl
LOCAL_SHARED_LIBRARIES += liblog

LOCAL_MODULE := libOMX_Core

LOCAL_MODULE_TAGS := optional

include $(BUILD_SHARED_LIBRARY)
