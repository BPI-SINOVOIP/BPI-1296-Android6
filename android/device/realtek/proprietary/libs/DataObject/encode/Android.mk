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

ifeq ($(USE_RTK_VOUT_UTIL), YES)

LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)

include hardware/realtek/realtek_omx/rtk_libs/system/include/MakeConfig

LOCAL_MODULE_PATH := $(TARGET_OUT_OPTIONAL_EXECUTABLES)
LOCAL_SHARED_LIBRARIES := liblog
LOCAL_SRC_FILES := db_transfer.cpp

LOCAL_MODULE := db_transfer
LOCAL_CFLAGS:= -DLOG_TAG=\"db_transfer\"
LOCAL_MODULE_TAGS := optional

LOCAL_CFLAGS += $(DEFINES) -DSYS_UNIX=1
LOCAL_CFLAGS += -fPIC
LOCAL_CFLAGS += -Wl,--start-group -Wl,--end-group

include $(BUILD_EXECUTABLE)

endif # USE_RTK_VOUT_UTIL
