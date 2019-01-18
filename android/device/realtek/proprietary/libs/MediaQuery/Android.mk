LOCAL_PATH := $(call my-dir)

################################################################################
include $(CLEAR_VARS)

LOCAL_SRC_FILES := libMediaQuery.so
LOCAL_MODULE_TAGS:= optional eng
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_MODULE_PATH := $(TARGET_OUT_SHARED_LIBRARIES)
LOCAL_MODULE_SUFFIX := .so
LOCAL_MODULE := libMediaQuery

include $(BUILD_PREBUILT)
