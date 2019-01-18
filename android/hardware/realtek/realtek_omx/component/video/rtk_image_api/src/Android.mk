
LOCAL_PATH := $(call my-dir)

#Building vpurun binary which will be placed in the /system/bin folder
include $(CLEAR_VARS)

LOCAL_SRC_FILES := \
	main.c \
	jpurun.c
	
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE := jpurun


# LOCAL_CFLAGS +=  -DRTK_FPGA_PLATFORM

LOCAL_C_INCLUDES := \
	$(LOCAL_PATH) \
	$(TOP)/hardware/realtek/realtek_omx/component/video/rtk_image_api/include \
	$(TOP)/hardware/realtek/realtek_omx/component/video/rtk_image_api/jpuapi \
	system/core
 
					

LOCAL_SHARED_LIBRARIES := \
	libjpu \
	libutils \
	libion

	
include $(BUILD_EXECUTABLE)

#Building jpuinit binary which will be placed in the /system/bin folder
include $(CLEAR_VARS)

LOCAL_SRC_FILES := jpuinit.c

LOCAL_MODULE_TAGS := optional
LOCAL_MODULE := jpuinit

LOCAL_C_INCLUDES := \
	$(LOCAL_PATH) \
	$(TOP)/hardware/realtek/realtek_omx/component/video/rtk_image_api/include \
	$(TOP)/hardware/realtek/realtek_omx/component/video/rtk_image_api/jpuapi \
	system/core

LOCAL_SHARED_LIBRARIES := \
	libjpu \
	libutils

include $(BUILD_EXECUTABLE)
