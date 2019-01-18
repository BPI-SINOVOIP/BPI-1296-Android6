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

ifeq ($(USE_RTK_HDCP1x_CONTROL),YES)

LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_SRC_FILES := rtk_hdcp.cpp


LOCAL_C_INCLUDES += \
	device/realtek/proprietary/libs/rtk_libs/common\
        device/realtek/proprietary/libs/rtk_libs/common/IPC/generate/include/system\
        device/realtek/proprietary/libs/rtk_libs/common/IPC/include\
		
	
LOCAL_SHARED_LIBRARIES += liblog
LOCAL_SHARED_LIBRARIES += libcutils
LOCAL_SHARED_LIBRARIES += libRTK_lib
		

LOCAL_MODULE_PATH_32 := $(TARGET_OUT)/lib/hw
LOCAL_MODULE_PATH_64 := $(TARGET_OUT)/lib64/hw
LOCAL_MULTILIB := both
LOCAL_MODULE := hdcp.$(TARGET_BOARD_PLATFORM)
LOCAL_MODULE_TAGS := optional	

	
include $(BUILD_SHARED_LIBRARY)

endif
