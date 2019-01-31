LOCAL_PATH := $(call my-dir)

# Building the vpuapi
include $(CLEAR_VARS)

LOCAL_SRC_FILES := \
	vpuapi.c \
	product.c \
	coda9/coda9.c \
	wave4/wave4.c \
	vpuapifunc.c \
	../src/vpuhelper.c \
	../src/vpuio.c 

LOCAL_SRC_FILES += \
	../vdi/linux/vdi.c \
	../vdi/linux/vdi_osal.c


# LOCAL_SRC_FILES += \
#  		../vdi/socket/vdi.c \
#  		../vdi/socket/vdi_osal.c \
# 		../vdi/socket/hpi_client.c \
# 		../vdi/mm.c


LOCAL_MODULE_TAGS := optional
LOCAL_MODULE := libvpu

LOCAL_CFLAGS := -DCONFIG_DEBUG_LEVEL=255
# LOCAL_CFLAGS += -DRTK_FPGA_PLATFORM

LOCAL_STATIC_LIBRARIES := 

LOCAL_SHARED_LIBRARIES := \
	libutils \
	libcutils \
	libdl \
	liblog \
	libRTK_lib

#	libdvm \
		
LOCAL_C_INCLUDES := $(LOCAL_PATH)/src \
	$(TOP)/hardware/realtek/realtek_omx/component/video/rtk_video_api \
	$(TOP)/hardware/realtek/realtek_omx/component/video/rtk_video_api/include				


include $(BUILD_SHARED_LIBRARY)

