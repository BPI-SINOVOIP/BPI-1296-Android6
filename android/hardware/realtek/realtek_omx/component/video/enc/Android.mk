LOCAL_PATH := $(call my-dir)
RTK_TOP := hardware/realtek/realtek_omx
RTK_INC := $(RTK_TOP)/include
RTK_RPC := $(TOP)/device/realtek/proprietary/libs/rtk_libs/common
RTK_RTKV:= $(RTK_TOP)/component/video/rtk_video_api
RTK_RTKJ:= $(RTK_TOP)/component/video/rtk_image_api

ENCODER_TYPE = VIDEO
# build video encoder
include $(CLEAR_VARS)

ifeq ($(ENCODER_TYPE), VIDEO)
LOCAL_PRELINK_MODULE := false

LOCAL_SRC_FILES := \
	encoder.c \
	codec_venc.c

LOCAL_MODULE := libOMX.realtek.video.enc
LOCAL_MODULE_TAGS := optional

LOCAL_SHARED_LIBRARIES += libRTKOMX_OSAL

LOCAL_SHARED_LIBRARIES += libc
LOCAL_SHARED_LIBRARIES += libz
LOCAL_SHARED_LIBRARIES += libdl
LOCAL_SHARED_LIBRARIES += libstdc++
LOCAL_SHARED_LIBRARIES += libm
LOCAL_SHARED_LIBRARIES += libcutils
LOCAL_SHARED_LIBRARIES += libhardware
LOCAL_SHARED_LIBRARIES += libhwse.$(TARGET_BOARD_PLATFORM)
LOCAL_SHARED_LIBRARIES += libvpu
LOCAL_SHARED_LIBRARIES += libRtkIpc

LOCAL_CFLAGS = -DOMX_ENCODER_VIDEO_DOMAIN -DUSE_OUTPUT_CROP
LOCAL_CFLAGS += -DUSE_RT_ION

LOCAL_C_INCLUDES := \
	. \
	$(RTK_INC)/khronos \
	$(RTK_INC)/imagination \
	system/core/include/cutils \
	$(RTK_RPC)/IPC/include \
	$(RTK_TOP)/osal \
	$(RTK_TOP)/core \
	$(RTK_RTKV)/include \
	$(RTK_RTKV)/vpuapi \
	hardware/realtek/StreamingEngine2

LOCAL_C_INCLUDES += device/realtek/proprietary/libs/Include

#LOCAL_LDLIBS += -lpthread

#LOCAL_LDLIBS += -lpthread
#
# create symbolic links to each encoders
#
ENCODER_FORMATS += avc
ENCODER_FORMATS += mpeg4

SYMLINKS := $(addsuffix .so,$(addprefix $(TARGET_OUT)/lib/$(LOCAL_MODULE).,$(ENCODER_FORMATS)))

LOCAL_POST_INSTALL_CMD = \
	$(foreach t,$(SYMLINKS),ln -sf $(LOCAL_MODULE).so $(t);)
include $(BUILD_SHARED_LIBRARY)
endif # end of ENCODER_TYPE

ENCODER_TYPE = IMAGE
# build image encoder
include $(CLEAR_VARS)

ifeq ($(ENCODER_TYPE), IMAGE)
LOCAL_PRELINK_MODULE := false

LOCAL_SRC_FILES := \
	encoder.c \
	codec_jenc.c

LOCAL_MODULE := libOMX.realtek.image.enc
LOCAL_MODULE_TAGS := optional

LOCAL_SHARED_LIBRARIES += libRTKOMX_OSAL

LOCAL_SHARED_LIBRARIES += libc
LOCAL_SHARED_LIBRARIES += libz
LOCAL_SHARED_LIBRARIES += libdl
LOCAL_SHARED_LIBRARIES += libstdc++
LOCAL_SHARED_LIBRARIES += libm
LOCAL_SHARED_LIBRARIES += libcutils
LOCAL_SHARED_LIBRARIES += libhardware
LOCAL_SHARED_LIBRARIES += libhwse.$(TARGET_BOARD_PLATFORM)
LOCAL_SHARED_LIBRARIES += libjpu

LOCAL_CFLAGS = -DOMX_ENCODER_IMAGE_DOMAIN -DUSE_OUTPUT_CROP
LOCAL_CFLAGS += -DUSE_RT_ION

LOCAL_C_INCLUDES := \
	. \
	$(RTK_INC)/khronos \
	$(RTK_INC)/imagination \
	system/core/include/cutils \
	$(RTK_RPC)/IPC/include \
	$(RTK_TOP)/osal \
	$(RTK_TOP)/core \
	$(RTK_RTKJ)/include \
	$(RTK_RTKJ)/jpuapi \
	$(RTK_RTKJ) \
	hardware/realtek/StreamingEngine2

#LOCAL_LDLIBS += -lpthread

include $(BUILD_SHARED_LIBRARY)
endif # end of ENCODER_TYPE
