LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)


LOCAL_SRC_FILES := src/tee_api.c

LOCAL_C_INCLUDES += \
		device/realtek/proprietary/libs/Include/TEEapi

LOCAL_SHARED_LIBRARIES := libteec libion
					


LOCAL_MODULE    := libTEEapi
LOCAL_MODULE_TAGS:= optional
include $(BUILD_SHARED_LIBRARY)
