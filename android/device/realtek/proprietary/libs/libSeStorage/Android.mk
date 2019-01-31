LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)

LOCAL_SRC_FILES := src/rtk_storage.cpp \
				   src/rtk_mtd.cpp \
                   src/rtk_common.cpp

LOCAL_C_INCLUDES := device/realtek/proprietary/libs/Include/SeStorage

LOCAL_MODULE    := libSeStorage
include $(BUILD_SHARED_LIBRARY)

