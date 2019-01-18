LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)

LOCAL_SRC_FILES:= \
    screencap.cpp

LOCAL_SHARED_LIBRARIES := \
    libcutils \
    libutils \
    libbinder \
    libskia \
    libui \
    libgui

LOCAL_C_INCLUDES += $(TARGET_OUT_HEADERS)/libRTKScreenshot
LOCAL_SHARED_LIBRARIES += libRTKScreenshot
LOCAL_CFLAGS += -DUSE_RTK_SCREENSHOT

LOCAL_MODULE:= rtk_screencap

LOCAL_MODULE_TAGS := optional

LOCAL_CFLAGS += -Wall -Werror -Wunused -Wunreachable-code

include $(BUILD_EXECUTABLE)
