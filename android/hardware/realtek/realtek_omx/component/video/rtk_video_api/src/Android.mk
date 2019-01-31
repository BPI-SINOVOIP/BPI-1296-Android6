
LOCAL_PATH := $(call my-dir)

#Building vpurun binary which will be placed in the /system/bin folder
include $(CLEAR_VARS)

LOCAL_SRC_FILES := \
	main.c \
	vpurun.c \
	mixer.c \
	
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE := vpurun


#LOCAL_CFLAGS :=  -DRTK_FPGA_PLATFORM
#LOCAL_CFLAGS +=  -DPASS_VERSION_1
LOCAL_CFLAGS := -DUSE_ION_ALLOC_FB

LOCAL_C_INCLUDES := $(LOCAL_PATH) \
	$(TOP)/hardware/realtek/realtek_omx/component/video/rtk_video_api/include \
	$(TOP)/hardware/realtek/realtek_omx/component/video/rtk_video_api/vpuapi \
	system/core
 
					

LOCAL_SHARED_LIBRARIES := \
	libvpu \
	libutils \
	libion

	
include $(BUILD_EXECUTABLE)


#Building vpuinit binary which will be placed in the /system/bin folder
include $(CLEAR_VARS)

LOCAL_SRC_FILES := vpuinit.c

LOCAL_MODULE_TAGS := optional
LOCAL_MODULE := vpuinit

LOCAL_C_INCLUDES := $(LOCAL_PATH) \
	$(TOP)/hardware/realtek/realtek_omx/component/video/rtk_video_api/include \
	$(TOP)/hardware/realtek/realtek_omx/component/video/rtk_video_api/vpuapi

LOCAL_SHARED_LIBRARIES := \
	libvpu \
	libutils

include $(BUILD_EXECUTABLE)
