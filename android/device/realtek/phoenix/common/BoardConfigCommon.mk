#
# Copyright (C) 2011 The Android Open-Source Project
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

#Target and compiler options
TARGET_CPU_ABI := armeabi-v7a
TARGET_CPU_ABI2 := armeabi
TARGET_CPU_SMP := true
TARGET_ARCH := arm
TARGET_CPU_VARIANT := cortex-a7

#Enable NEON feature
TARGET_ARCH_VARIANT := armv7-a-neon
ARCH_ARM_HAVE_TLS_REGISTER := true

TARGET_BOARD_PLATFORM := phoenix
               
BOARD_FLASH_BLOCK_SIZE := 4096
TARGET_USERIMAGES_USE_EXT4 := true
BOARD_SYSTEMIMAGE_PARTITION_SIZE := 1073741824

#System
TARGET_PRELINK_MODULE := true
TARGET_NO_BOOTLOADER := true
TARGET_NO_RECOVERY := true
TARGET_NO_KERNEL := true
TARGET_NO_RADIOIMAGE := true
USE_OPENGL_RENDERER := true
BOARD_USES_GENERIC_AUDIO := true
TARGET_PROVIDES_INIT_RC := true

TARGET_RUNNING_WITHOUT_SYNC_FRAMEWORK := true

# ---------------------------------------------------> Time
#
# |----------------------------|----------------------
# |<- A ->|----------------------------|--------------
#         |<- B ->|----------------------------|------
# ^       ^       ^
# |       |       |_ VSync for SurfaceFlinger
# |       |_ VSync for App UI
# |_ Hardware VSync
#
# A: VSYNC_EVENT_PHASE_OFFSET_NS
# B: SF_VSYNC_EVENT_PHASE_OFFSET_NS
VSYNC_EVENT_PHASE_OFFSET_NS := 0
SF_VSYNC_EVENT_PHASE_OFFSET_NS := 0

# SELinux
USE_SEPOLICY_CUSTOMIZED := true
ifeq ($(PLATFORM_VERSION), 6.0.1)
USE_SEPOLICY_CUSTOMIZED := false
else ifeq ($(PLATFORM_VERSION), 6.0)
USE_SEPOLICY_CUSTOMIZED := false
else ifeq ($(PLATFORM_VERSION), 5.1.1)
USE_SEPOLICY_CUSTOMIZED := true
else ifeq ($(PLATFORM_VERSION), 4.4)
USE_SEPOLICY_CUSTOMIZED := true
else
USE_SEPOLICY_CUSTOMIZED := true
endif

ifeq ($(USE_SEPOLICY_CUSTOMIZED), true)
BOARD_SEPOLICY_DIRS += device/realtek/phoenix/common/sepolicy
BOARD_SEPOLICY_UNION += fs_use \
                        genfs_contexts \
                        file.te \
                        vold.te
endif
