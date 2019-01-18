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

LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)
include device/realtek/proprietary/libs/rtk_libs/system/include/MakeConfig
LOCAL_SHARED_LIBRARIES := liblog \
						  libutils \
						  libbinder \
						  libhardware \
						  libRTK_lib

CEC_BRIDGE_FILES :=$(wildcard $(LOCAL_PATH)/bridge/src/*.cpp)
CEC_BRIDGE_FILES :=$(CEC_BRIDGE_FILES:$(LOCAL_PATH)/%=%)

CEC_CONTROLLER_FILES :=$(wildcard $(LOCAL_PATH)/controller/src/*.cpp)
CEC_CONTROLLER_FILES :=$(CEC_CONTROLLER_FILES:$(LOCAL_PATH)/%=%)

CEC_ADAPTER_FILES :=$(wildcard $(LOCAL_PATH)/adapter/src/*.cpp)
CEC_ADAPTER_FILES :=$(CEC_ADAPTER_FILES:$(LOCAL_PATH)/%=%)

CEC_COMMON_FILES :=$(wildcard $(LOCAL_PATH)/common/src/*.cpp)
CEC_COMMON_FILES :=$(CEC_COMMON_FILES:$(LOCAL_PATH)/%=%)

LOCAL_SRC_FILES := $(CEC_ADAPTER_FILES)
LOCAL_SRC_FILES += $(CEC_CONTROLLER_FILES)
LOCAL_SRC_FILES += $(CEC_BRIDGE_FILES)
LOCAL_SRC_FILES += $(CEC_COMMON_FILES)

LOCAL_MODULE := libRTKCECControl

LOCAL_CFLAGS:= -DLOG_TAG=\"libRTKCECControl\"
LOCAL_MODULE_TAGS := optional

ifneq ($(TARGET_ARCH), mips)
    DEFINES := $(filter-out -mips32r2,$(DEFINES))
endif
LOCAL_CFLAGS += $(DEFINES)
LOCAL_CFLAGS += -fPIC
LOCAL_CFLAGS += -DTX_DBG
LOCAL_CFLAGS += -DRX_DBG
LOCAL_C_INCLUDES := $(LOCAL_PATH)/../Include/Platform_Lib/CECControl
LOCAL_C_INCLUDES += \
    system/core/include/cutils \
	hardware/realtek \
    $(LOCAL_PATH)/../rtk_libs/common \
    $(LOCAL_PATH)/../rtk_libs/common/IPC/generate/include/system \
    $(LOCAL_PATH)/../rtk_libs/common/IPC/include \
    $(LOCAL_PATH)/../rtk_libs/common/IPC/include/xdr \
    $(LOCAL_PATH)/../rtk_libs/common/IPC/src/xdr \
    $(LOCAL_PATH)/../rtk_libs/OSAL/include \
    $(LOCAL_PATH)/../rtk_libs/OSAL/include/OSAL
include $(BUILD_SHARED_LIBRARY)

