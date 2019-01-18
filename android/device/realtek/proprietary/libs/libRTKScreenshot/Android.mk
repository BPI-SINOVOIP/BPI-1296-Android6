LOCAL_PATH:= $(call my-dir)

include $(CLEAR_VARS)
LOCAL_COPY_HEADERS_TO := libRTKScreenshot
LOCAL_COPY_HEADERS := inc/rtk-screenshot.h
include $(BUILD_COPY_HEADERS)


include $(CLEAR_VARS)
LOCAL_SHARED_LIBRARIES := \
    libcutils \
    libutils \
    libbinder \
    libskia \
    libui \
    libgui

LOCAL_SHARED_LIBRARIES += libRtkScreenRecord
LOCAL_SHARED_LIBRARIES += libRTKAllocator
LOCAL_C_INCLUDES += $(LOCAL_PATH)/inc
LOCAL_C_INCLUDES += device/realtek/proprietary/libs/libRTKAllocator/include
LOCAL_C_INCLUDES += device/realtek/proprietary/libs/libRTKScreenrecord/include
LOCAL_SRC_FILES := src/rtk-screenshot.cpp
LOCAL_MODULE:= libRTKScreenshot
LOCAL_MODULE_TAGS := optional
LOCAL_CFLAGS += -Wall -Werror -Wunused -Wunreachable-code
LOCAL_CFLAGS := -DLOG_TAG=\"RTK_SCREENSHOT\"

include $(BUILD_SHARED_LIBRARY)

-include $(LOCAL_PATH)/cmds/rtk_screencap/Android.mk
