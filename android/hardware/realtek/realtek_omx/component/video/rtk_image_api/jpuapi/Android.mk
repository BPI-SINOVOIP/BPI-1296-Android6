LOCAL_PATH := $(call my-dir)

# Building the vpuapi
include $(CLEAR_VARS)

LOCAL_SRC_FILES := \
	jpuapi.c \
	jpuapifunc.c \
	../src/jpuhelper.c \
	../src/jpulog.c \
	../src/mixer.c

LOCAL_SRC_FILES += \
	../jdi/linux/jdi.c


LOCAL_MODULE_TAGS := optional
LOCAL_MODULE := libjpu

# LOCAL_CFLAGS +=  -DRTK_FPGA_PLATFORM


LOCAL_SHARED_LIBRARIES := \
	libutils \
	libcutils \
	libdl \
	liblog \
	libRTK_lib

							

LOCAL_C_INCLUDES := \
	$(LOCAL_PATH)/src \
	$(TOP)/hardware/realtek/realtek_omx/component/video/rtk_image_api \
	$(TOP)/hardware/realtek/realtek_omx/component/video/rtk_image_api/include


include $(BUILD_SHARED_LIBRARY)

