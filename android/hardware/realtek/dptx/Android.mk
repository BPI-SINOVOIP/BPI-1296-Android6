LOCAL_PATH:= $(call my-dir)

################################################################################
include $(CLEAR_VARS)

LOCAL_SRC_FILES := dptx.$(TARGET_BOARD_PLATFORM).so
LOCAL_MODULE_TAGS:= optional
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_MODULE_PATH := $(TARGET_OUT_SHARED_LIBRARIES)/hw
LOCAL_MODULE_SUFFIX := .so
LOCAL_MODULE := dptx.$(TARGET_BOARD_PLATFORM)

include $(BUILD_PREBUILT)

