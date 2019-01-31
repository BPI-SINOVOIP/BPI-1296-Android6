#
# Copyright (C) 2008 The Android Open Source Project
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#

# This makefile supplies the rules for building a library of JNI code for
# use by our example platform shared library.

LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)

#include hardware/realtek/realtek_omx/rtk_libs/system/include/MakeConfig
include device/realtek/proprietary/libs/rtk_libs/system/include/MakeConfig

# [optional, user, eng] 
# eng = required
# optinal = no install on target
LOCAL_MODULE_TAGS := optional eng

# This is the target being built.
LOCAL_MODULE:= librealtek_runtime
LOCAL_MULTILIB := both

# Target install path.
#LOCAL_MODULE_PATH := $(TARGET_OUT_SHARED_LIBRARIES)

# All of the source files that we will compile.
LOCAL_SRC_FILES:= \
	onload.cpp
	
# All of the shared libraries we link against.
LOCAL_SHARED_LIBRARIES := \
	libandroid_runtime \
	libnativehelper \
	libcutils \
	libutils \
	libbinder \
	libhardware \
	libRTK_lib \
	libRtkRpcClientServer \
	libgui \
	libsurfaceflinger	

# No static libraries.
LOCAL_STATIC_LIBRARIES :=

# Also need the JNI headers.
base := $(LOCAL_PATH)/../..
LOCAL_C_INCLUDES += \
	$(JNI_H_INCLUDE) \
	$(base)/include \
	bionic \
	system/core/include/cutils \
	external/sqlite/dist \
	device/realtek/proprietary/libs/rtk_libs/common \
	device/realtek/proprietary/libs/rtk_libs/common/IPC/generate/include/system \
	device/realtek/proprietary/libs/rtk_libs/common/IPC/include \
	device/realtek/proprietary/libs/rtk_libs/common/IPC/include/xdr \
	device/realtek/proprietary/libs/rtk_libs/common/IPC/src/xdr \
	device/realtek/proprietary/libs/rtk_libs/OSAL/include \
	device/realtek/proprietary/libs/rtk_libs/OSAL/include/OSAL \
	device/realtek/proprietary/libs/Include


#	$(RTK_HARDWARE_INCLUDE) \
#	$(RTK_DEVICE_INCLUDE)
# No specia compiler flags.
ifneq ($(TARGET_ARCH), mips)
DEFINES := $(filter-out -mips32r2,$(DEFINES))
endif
LOCAL_CFLAGS += $(DEFINES)
LOCAL_CFLAGS +=-DLOG_TAG=\"librealtek_runtime\"
LOCAL_CFLAGS += -DLOG_NDEBUG=0
# Don't prelink this library.  For more efficient code, you may want
# to add this library to the prelink map and set this to true.
LOCAL_PRELINK_MODULE := false

ifeq ($(USE_RTK_SETUP_CLASS), YES)
LOCAL_SHARED_LIBRARIES += libRTKSetupClass
endif

ifeq ($(TARGET_BOARD_PLATFORM), kylin)
LOCAL_CFLAGS += -DKYLIN_TARGET_BOARD
endif

ifeq ($(ENABLE_RTK_HDMIRX_SERVICE), true)
	LOCAL_SRC_FILES += com_realtek_server_RtkHDMIRxService.cpp
	#LOCAL_C_INCLUDES += device/realtek/proprietary/libs/libMCPControl/inc
	#LOCAL_SHARED_LIBRARIES += libMCPControl
	LOCAL_C_INCLUDES += device/realtek/proprietary/libs/RtkHWMBinderapi
	LOCAL_SHARED_LIBRARIES += libRTKHWMBinderapi
endif

ifeq ($(ENABLE_RTK_DPTX_SERVICE), true)
        LOCAL_SRC_FILES += com_realtek_server_RtkDPTxService.cpp
LOCAL_CFLAGS += -DENABLE_RTK_DPTX_SERVICE
endif


ifeq ($(ENABLE_RTK_HDMI_SERVICE), true)
LOCAL_SRC_FILES+= \
	com_realtek_server_RtkHDMIService.cpp

LOCAL_C_INCLUDES += \
    hardware/realtek \
    hardware/realtek/hdmitx \
    hardware/realtek/mhltx \
    device/realtek/proprietary/libs

LOCAL_CFLAGS += -DENABLE_RTK_HDMI_SERVICE

LOCAL_SHARED_LIBRARIES += libRTKHDMIControl \
                          libRTKVoutUtil
                     #     libRTKAoutUtil

endif

ifeq ($(ENABLE_RTK_CEC_SERVICE), true)
LOCAL_SRC_FILES+= \
       CEC/CECAPBridgeImpl.cpp \
       com_realtek_server_RtkCECService.cpp

LOCAL_CFLAGS += -DENABLE_RTK_CEC_SERVICE

LOCAL_C_INCLUDES += \
       device/realtek/proprietary/libs/Include/Platform_Lib/CECControl \
    device/realtek/proprietary/libs/Include \
       $(LOCAL_PATH)/CEC

LOCAL_SHARED_LIBRARIES += \
       libRTKCECControl
endif

ifeq ($(ENABLE_RTK_HDMIRX_SERVICE), true)
LOCAL_SRC_FILES += \
	com_realtek_hardware_RtkHDMIRxManager.cpp

LOCAL_C_INCLUDES += \
	$(TOP)/device/realtek/proprietary/libs/transcode \
	$(TOP)/device/realtek/proprietary/libs/Include/RtkIpc

LOCAL_SHARED_LIBRARIES += \
	libRtkTranscodePlayer \
	libRtkIpc \
	libbinder

LOCAL_CFLAGS += -DENABLE_RTK_HDMIRX_SERVICE

endif #ENABLE_RTK_HDMIRX_SERVICE

LOCAL_SRC_FILES += \
    com_realtek_hardware_RtkPIPRecordManager.cpp

ifeq ($(ENABLE_RT_WIFIDISPLAY),true)
LOCAL_SRC_FILES += com_realtek_server_RtkMiracastSinkJNI.cpp

LOCAL_C_INCLUDES += $(TOP)/device/realtek/proprietary/libs/wfdisplay
LOCAL_C_INCLUDES += $(TOP)/device/realtek/proprietary/libs/wfdisplay/inband
LOCAL_C_INCLUDES += $(TOP)/device/realtek/proprietary/libs/libRTMediaPlayer

LOCAL_SHARED_LIBRARIES += libc
LOCAL_SHARED_LIBRARIES += libstdc++
LOCAL_SHARED_LIBRARIES += libcrypto
LOCAL_SHARED_LIBRARIES += libhdcp2
LOCAL_SHARED_LIBRARIES += libwfdisplay
LOCAL_SHARED_LIBRARIES += libRTMediaPlayer
LOCAL_SHARED_LIBRARIES += libRT_MediaPlayClient
LOCAL_SHARED_LIBRARIES += libgui
LOCAL_SHARED_LIBRARIES += libstagefright_foundation
LOCAL_SHARED_LIBRARIES += libutils
LOCAL_SHARED_LIBRARIES += libinput
LOCAL_SHARED_LIBRARIES += libinputservice
LOCAL_SHARED_LIBRARIES += liblog
LOCAL_SHARED_LIBRARIES += libwfdinband

LOCAL_CFLAGS += -DENABLE_RTK_MIRACAST_SINK_SERVICE
LOCAL_CFLAGS += -DHDCP_REPEATER=0
LOCAL_CFLAGS += -DHDCP2_KEY_PROTECT_WITH_KIMG
LOCAL_CFLAGS += -DHDCP2_1
#                         -DREALTEK_WFDISPLAY_SIGMA
endif


ifeq ($(USE_TV_MANAGER), YES)
LOCAL_SRC_FILES+= \
	com_realtek_server_TvManagerService.cpp

LOCAL_C_INCLUDES += \
    device/realtek/frameworks/services/jni/tv \
    device/realtek/proprietary/libs/RtkRpcClientServer \
    device/realtek/proprietary/libs/RtkRpcClientServer/rpc

LOCAL_CFLAGS += -DUSE_TV_MANAGER
endif

ifeq ($(USE_AIRFUN_MANAGER), YES)
LOCAL_SRC_FILES+= \
	com_realtek_server_AirFunService.cpp \
	airfun/AirFunIpc.cpp

LOCAL_C_INCLUDES += \
    device/realtek/frameworks/services/jni/airfun

LOCAL_CFLAGS += -DUSE_AIRFUN_MANAGER
endif

ifeq ($(USE_TOGO_MANAGER), YES)

ifeq ($(PLATFORM_VERSION),4.2.2)
LOCAL_CFLAGS += -DAndroid_JB
endif
ifeq ($(PLATFORM_VERSION),4.4)
LOCAL_CFLAGS += -DAndroid_KitKate
else ifeq ($(PLATFORM_VERSION),4.4.4)
LOCAL_CFLAGS += -DAndroid_KitKate
endif

LOCAL_SRC_FILES+= \
	com_realtek_server_ToGoService.cpp \
	com_realtek_server_ToGoProxyService.cpp \
	RtkToGo.cpp

LOCAL_SHARED_LIBRARIES += libstagefright
LOCAL_SHARED_LIBRARIES += libutils
LOCAL_SHARED_LIBRARIES += libbinder
LOCAL_SHARED_LIBRARIES += libstagefright_foundation
LOCAL_SHARED_LIBRARIES += libRtkIpc
LOCAL_SHARED_LIBRARIES += libmedia
LOCAL_SHARED_LIBRARIES += libcamera_client
LOCAL_SHARED_LIBRARIES += libgui
LOCAL_SHARED_LIBRARIES += libRtkTranscodePlayer
LOCAL_SHARED_LIBRARIES += libandroid_runtime
LOCAL_SHARED_LIBRARIES += libRTMediaPlayer
LOCAL_SHARED_LIBRARIES += libRT_IPC
LOCAL_SHARED_LIBRARIES += libTvProxy
LOCAL_SHARED_LIBRARIES += libRTDtvMediaPlayer

LOCAL_C_INCLUDES += $(TOP)/frameworks/av/media/libstagefright \
	$(TOP)/frameworks/native/include/media/openmax \
	$(TOP)/frameworks/native/include/utils \
	$(TOP)/device/realtek/proprietary/libs/transcode \
	$(TOP)/device/realtek/proprietary/libs/libRTMediaPlayer \
	$(TOP)/device/realtek/proprietary/libs/libRTDtvMediaPlayer \
	$(TOP)/device/realtek/proprietary/libs/RT_IPC \
	$(TOP)/device/realtek/proprietary/libs/libTvProxy \
	$(TOP)/device/realtek/proprietary/libs/Include \
	$(TOP)/device/realtek/proprietary/libs \
	$(TOP)/frameworks/av/include \
	$(TOP)/frameworks/av/include/media/stagefright/foundation \
	$(TOP)/frameworks/base/include/android_runtime \
	$(TOP)/frameworks/native/include \
	$(TOP)/frameworks/base/core/jni \
	$(TOP)/device/realtek/frameworks/services/jni/tv

LOCAL_CFLAGS += -DUSE_TOGO_MANAGER
LOCAL_CFLAGS += -DUSE_RT_MEDIA_PLAYER

ifeq ($(ENABLE_1195_DTV_FLOW), YES)
LOCAL_CFLAGS += -DENABLE_1195_DTV_FLOW
endif
endif

ifeq ($(USE_RTK_AOUT_UTIL), YES)
LOCAL_SRC_FILES+= \
	com_realtek_server_RtkAoutUtilService.cpp

#LOCAL_C_INCLUDES += \
#	device/realtek/proprietary/libs/Include

LOCAL_CFLAGS += -DUSE_RTK_AOUT_UTIL

LOCAL_SHARED_LIBRARIES += libRTKAoutUtil
endif

ifeq ($(USE_RTK_VOUT_UTIL), YES)
LOCAL_SRC_FILES += com_realtek_server_RtkVoutUtilService.cpp

LOCAL_C_INCLUDES += \
	device/realtek/proprietary/libs/Include \
	external/skia/include/lazy \
	external/skia/include/core \
	frameworks/base/core/jni/android/graphics \
	frameworks/base/libs/hwui \
	LOCAL_SRC_FILES += $(TOP)/device/realtek/proprietary/libs/wfdisplay

LOCAL_CFLAGS += -DUSE_RTK_VOUT_UTIL

LOCAL_SHARED_LIBRARIES += libRTKVoutUtil
endif

ifeq ($(USE_RTK_HDCP1x_CONTROL), YES)
LOCAL_CFLAGS += -DHDCP1x_ENABLED
# HDCP 2.2
ifeq ($(USE_RTK_HDCP22_CONTROL), YES)
LOCAL_CFLAGS += -DHDCP22_ENABLED
endif
endif

include $(BUILD_SHARED_LIBRARY)
