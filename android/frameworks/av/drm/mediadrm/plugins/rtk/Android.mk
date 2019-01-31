#
# Copyright (C) 2013 The Android Open Source Project
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
LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)

LOCAL_SRC_FILES:= \
    RtkDrmCryptoPlugin.cpp \
    RtkPluginTee.c

LOCAL_MODULE := librtkdrmcryptoplugin

LOCAL_PROPRIETARY_MODULE := true
LOCAL_MODULE_RELATIVE_PATH := mediadrm

LOCAL_SHARED_LIBRARIES := \
    libutils liblog libteec libstagefright_foundation

LOCAL_C_INCLUDES += \
    $(TOP)/frameworks/av/include \
    $(TOP)/frameworks/native/include/media \
    $(TOP)/device/realtek/proprietary/libs/Include/TEEapi \
    $(TOP)/device/realtek/kylin/common/prebuilt/tee/include

# Set the following flag to enable the decryption passthru flow
#LOCAL_CFLAGS += -DENABLE_PASSTHRU_DECRYPTION

LOCAL_MODULE_TAGS := optional

include $(BUILD_SHARED_LIBRARY)
