LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)


LOCAL_SRC_FILES:=               \
    test_rtkplugin.cpp

LOCAL_SHARED_LIBRARIES :=       \
    libmediaplayerservice		\
    liblog                      \
    libutils                    \


LOCAL_C_INCLUDES := $(TOP)/frameworks/av/media/libmediaplayerservice \

LOCAL_CFLAGS += -Werror -Wno-error=deprecated-declarations -Wall
LOCAL_CFLAGS += -DRTK_PLATFORM
LOCAL_CLANG := true


LOCAL_MODULE:= librtkplugin_test

LOCAL_MODULE_TAGS := tests

LOCAL_MODULE_TARGET_ARCH := arm x86 mips

include $(BUILD_EXECUTABLE)





