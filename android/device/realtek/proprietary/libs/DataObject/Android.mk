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

ifeq ($(USE_RTK_SETUP_CLASS), YES)

LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)

include device/realtek/proprietary/libs/rtk_libs/system/include/MakeConfig

LOCAL_SRC_FILES := DataObject.cpp DataObjectHdd.cpp DataObjectFlash.cpp DataObjectExtHdd.cpp

LOCAL_MODULE := libDataObject
LOCAL_CFLAGS:= -DLOG_TAG=\"libDataObject\"
LOCAL_MODULE_TAGS := optional

LOCAL_CFLAGS += $(DEFINES) -DSYS_UNIX=1
LOCAL_CFLAGS += -DTEST_ANDROID
LOCAL_CFLAGS += -fPIC
LOCAL_C_INCLUDES += \
	$(LOCAL_PATH)/../Include \
	external/sqlite/dist \
	bionic \
	bionic//libc/include/ \
    system/core/include/cutils \
	$(LOCAL_PATH)/../rtk_libs/common \
	$(LOCAL_PATH)/../rtk_libs/common/IPC/generate/include/system \
	$(LOCAL_PATH)/../rtk_libs/common/IPC/include \
	$(LOCAL_PATH)/../rtk_libs/common/IPC/include/xdr \
	$(LOCAL_PATH)/../rtk_libs/common/IPC/src/xdr \
	$(LOCAL_PATH)/../rtk_libs/OSAL/include \
	$(LOCAL_PATH)/../rtk_libs/OSAL/include/OSAL \
	$(LOCAL_PATH)/../Include/Application/AppClass

include $(BUILD_STATIC_LIBRARY)

endif # USE_RTK_SETUP_CLASS
