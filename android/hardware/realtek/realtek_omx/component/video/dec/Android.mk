LOCAL_PATH := $(call my-dir)
RTK_TOP	:= hardware/realtek/realtek_omx
RTK_INC := $(RTK_TOP)/include
RTK_RPC := $(TOP)/device/realtek/proprietary/libs/rtk_libs/common
RTK_RTKV:= $(RTK_TOP)/component/video/rtk_video_api
RTK_RTKJ:= $(RTK_TOP)/component/video/rtk_image_api

DECODER_TYPE = VIDEO
OMX_DECODER_TUNNELING_SUPPORT = true
# build video decoder
include $(CLEAR_VARS)

ifeq ($(DECODER_TYPE), VIDEO)
LOCAL_PRELINK_MODULE := false

LOCAL_SRC_FILES := \
	decoder.c \
	codec_vdec.c \
	codec_jdec.c

LOCAL_MODULE := libOMX.realtek.video.dec
LOCAL_32_BIT_ONLY := true

LOCAL_MODULE_TAGS := optional

LOCAL_SHARED_LIBRARIES += libRTKOMX_OSAL
LOCAL_SHARED_LIBRARIES += libRTK_lib

LOCAL_SHARED_LIBRARIES += libc
LOCAL_SHARED_LIBRARIES += libz
LOCAL_SHARED_LIBRARIES += libdl
LOCAL_SHARED_LIBRARIES += libstdc++
LOCAL_SHARED_LIBRARIES += libm
LOCAL_SHARED_LIBRARIES += libcutils
LOCAL_SHARED_LIBRARIES += libhardware
LOCAL_SHARED_LIBRARIES += libhwse.$(TARGET_BOARD_PLATFORM)
LOCAL_SHARED_LIBRARIES += libvpu
LOCAL_SHARED_LIBRARIES += libjpu
LOCAL_SHARED_LIBRARIES += libRtkIpc
LOCAL_SHARED_LIBRARIES += libion

LOCAL_CFLAGS = -DOMX_DECODER_VIDEO_DOMAIN -DUSE_OUTPUT_CROP
LOCAL_CFLAGS += -DUSE_RT_ION

ifeq ($(OMX_DECODER_TUNNELING_SUPPORT), true)
LOCAL_SRC_FILES += sidebandOutput.c Sideband_C.cpp
LOCAL_SHARED_LIBRARIES += libutils
LOCAL_SHARED_LIBRARIES += libskia
LOCAL_SHARED_LIBRARIES += libgui
LOCAL_SHARED_LIBRARIES += libui
LOCAL_SHARED_LIBRARIES += libbinder
LOCAL_CFLAGS += -DOMX_DECODER_TUNNELING_SUPPORT
endif

LOCAL_C_INCLUDES := \
	. \
	$(RTK_INC)/khronos \
	$(RTK_INC)/imagination \
	system/core/include \
	system/core/include/cutils \
	$(RTK_RPC)/IPC/include \
	$(RTK_TOP)/osal \
	$(RTK_TOP)/core \
	$(RTK_RTKV)/include \
	$(RTK_RTKV)/vpuapi \
	$(RTK_RTKJ)/include \
	$(RTK_RTKJ)/jpuapi \
	$(RTK_RTKJ) \
	hardware/realtek/StreamingEngine2

LOCAL_C_INCLUDES += device/realtek/proprietary/libs/Include

ifeq ($(ENABLE_TEE_DRM_FLOW),true)
LOCAL_CFLAGS += -DENABLE_TEE_DRM_FLOW
LOCAL_C_INCLUDES += \
	$(TOP)/device/realtek/proprietary/libs/Include/TEEapi
LOCAL_SHARED_LIBRARIES += libTEEapi
endif

#LOCAL_LDLIBS += -lpthread
#
# create symbolic links to each decoders
#
DECODER_FORMATS += avc
DECODER_FORMATS += mpeg4
DECODER_FORMATS += vp8
DECODER_FORMATS += 3gpp
DECODER_FORMATS += mpeg2
DECODER_FORMATS += raw
DECODER_FORMATS += vc1
DECODER_FORMATS += rv
DECODER_FORMATS += divx3
DECODER_FORMATS += avs
DECODER_FORMATS += mjpg
DECODER_FORMATS += hevc
DECODER_FORMATS += wvc1
DECODER_FORMATS += wmv3
DECODER_FORMATS += wmv
DECODER_FORMATS += mp43
DECODER_FORMATS += flv
DECODER_FORMATS += rv30
DECODER_FORMATS += rv40

SYMLINKS := $(addsuffix .so,$(addprefix $(TARGET_OUT)/lib/$(LOCAL_MODULE).,$(DECODER_FORMATS)))

LOCAL_POST_INSTALL_CMD = \
	$(hide) $(foreach t,$(SYMLINKS),ln -sf $(LOCAL_MODULE).so $(t);)

include $(BUILD_SHARED_LIBRARY)
endif # end of DECODER_TYPE

###########################################################################

DECODER_TYPE = IMAGE
OMX_DECODER_TUNNELING_SUPPORT = false
# build image decoder
include $(CLEAR_VARS)

ifeq ($(DECODER_TYPE), IMAGE)
LOCAL_PRELINK_MODULE := false

LOCAL_SRC_FILES := \
	decoder.c \
	codec_jdec.c

LOCAL_MODULE := libOMX.realtek.image.dec
LOCAL_32_BIT_ONLY := true

LOCAL_MODULE_TAGS := optional

LOCAL_SHARED_LIBRARIES += libRTKOMX_OSAL
LOCAL_SHARED_LIBRARIES += libRTK_lib

LOCAL_SHARED_LIBRARIES += libc
LOCAL_SHARED_LIBRARIES += libz
LOCAL_SHARED_LIBRARIES += libdl
LOCAL_SHARED_LIBRARIES += libstdc++
LOCAL_SHARED_LIBRARIES += libm
LOCAL_SHARED_LIBRARIES += libcutils
LOCAL_SHARED_LIBRARIES += libhardware
LOCAL_SHARED_LIBRARIES += libhwse.$(TARGET_BOARD_PLATFORM)
LOCAL_SHARED_LIBRARIES += libjpu
LOCAL_SHARED_LIBRARIES += libRtkIpc
LOCAL_SHARED_LIBRARIES += libion

LOCAL_CFLAGS = -DOMX_DECODER_IMAGE_DOMAIN -DUSE_OUTPUT_CROP
LOCAL_CFLAGS += -DUSE_RT_ION

LOCAL_C_INCLUDES := \
	. \
	$(RTK_INC)/khronos \
	$(RTK_INC)/imagination \
	system/core/include \
	system/core/include/cutils \
	$(RTK_RPC)/IPC/include \
	$(RTK_TOP)/osal \
	$(RTK_TOP)/core \
	$(RTK_RTKJ)/include \
	$(RTK_RTKJ)/jpuapi \
	$(RTK_RTKJ) \
	hardware/realtek/StreamingEngine2

LOCAL_C_INCLUDES += device/realtek/proprietary/libs/Include
#LOCAL_LDLIBS += -lpthread

include $(BUILD_SHARED_LIBRARY)
endif # end of DECODER_TYPE

###########################################################################
# For DRM used
###########################################################################
include $(CLEAR_VARS)

OMX_DECODER_TUNNELING_SUPPORT = false
ifeq ($(ENABLE_DRM_PLUGIN_SERVICE), true)
LOCAL_PRELINK_MODULE := false

LOCAL_SRC_FILES := \
	decoder.c \
	codec_vdec.c \
	codec_jdec.c

LOCAL_MODULE := libOMX.realtek.video.dec.secure
LOCAL_32_BIT_ONLY := true

LOCAL_MODULE_TAGS := optional

LOCAL_SHARED_LIBRARIES += libRTKOMX_OSAL
LOCAL_SHARED_LIBRARIES += libRTK_lib

LOCAL_SHARED_LIBRARIES += libc
LOCAL_SHARED_LIBRARIES += libz
LOCAL_SHARED_LIBRARIES += libdl
LOCAL_SHARED_LIBRARIES += libstdc++
LOCAL_SHARED_LIBRARIES += libm
LOCAL_SHARED_LIBRARIES += libcutils
LOCAL_SHARED_LIBRARIES += libhardware
LOCAL_SHARED_LIBRARIES += libhwse.$(TARGET_BOARD_PLATFORM)
LOCAL_SHARED_LIBRARIES += libvpu
LOCAL_SHARED_LIBRARIES += libjpu
LOCAL_SHARED_LIBRARIES += libRtkIpc
LOCAL_SHARED_LIBRARIES += libion

LOCAL_CFLAGS = -DOMX_DECODER_VIDEO_DOMAIN -DUSE_OUTPUT_CROP
LOCAL_CFLAGS += -DUSE_RT_ION

ifeq ($(OMX_DECODER_TUNNELING_SUPPORT), true)
LOCAL_SRC_FILES += sidebandOutput.c Sideband_C.cpp
LOCAL_SHARED_LIBRARIES += libutils
LOCAL_SHARED_LIBRARIES += libskia
LOCAL_SHARED_LIBRARIES += libgui
LOCAL_SHARED_LIBRARIES += libui
LOCAL_SHARED_LIBRARIES += libbinder
LOCAL_CFLAGS += -DOMX_DECODER_TUNNELING_SUPPORT
endif

LOCAL_C_INCLUDES := \
	. \
	$(RTK_INC)/khronos \
	$(RTK_INC)/imagination \
	system/core/include \
	system/core/include/cutils \
	$(RTK_RPC)/IPC/include \
	$(RTK_TOP)/osal \
	$(RTK_TOP)/core \
	$(RTK_RTKV)/include \
	$(RTK_RTKV)/vpuapi \
	$(RTK_RTKJ)/include \
	$(RTK_RTKJ)/jpuapi \
	$(RTK_RTKJ) \
	hardware/realtek/StreamingEngine2

LOCAL_C_INCLUDES += device/realtek/proprietary/libs/Include

ifeq ($(ENABLE_TEE_DRM_FLOW),true)
LOCAL_CFLAGS += -DENABLE_TEE_DRM_FLOW
LOCAL_C_INCLUDES += \
	$(TOP)/device/realtek/proprietary/libs/Include/TEEapi
LOCAL_SHARED_LIBRARIES += libTEEapi
endif

include $(BUILD_SHARED_LIBRARY)
endif # end of ifeq $(ENABLE_DRM_PLUGIN_SERVICE)
