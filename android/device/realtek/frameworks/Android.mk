LOCAL_PATH := $(call my-dir)
# =============================================================
include $(CLEAR_VARS)
LOCAL_SRC_FILES += $(call all-subdir-Iaidl-files)
LOCAL_SRC_FILES += $(call all-subdir-java-files)

LOCAL_MODULE_TAGS := optional
LOCAL_MODULE := realtek

LOCAL_MODULE_CLASS := JAVA_LIBRARIES

# AIDL
LOCAL_AIDL_INCLUDES +=$(LOCAL_PATH)/core/java
LOCAL_AIDL_INCLUDES +=$(LOCAL_PATH)/services/java

#LOCAL_SRC_FILES += \
#	core/java/com/realtek/hardware/IRtkHDMIService.aidl \
#	core/java/com/realtek/hardware/IRtkCECService.aidl \
#	core/java/com/realtek/hardware/IRtkHDMIRxService.aidl \
#	core/java/com/realtek/app/ITvManager.aidl \
#	core/java/com/realtek/app/IAirFunService.aidl \
#	core/java/com/realtek/app/IToGoManager.aidl \
#	core/java/com/realtek/provider/ISettingsService.aidl \
#	core/java/com/realtek/provider/IMediaQueryService.aidl \
#	core/java/com/realtek/hardware/IRtkAoutUtilService.aidl \
#	core/java/com/realtek/hardware/IRtkVoutUtilService.aidl \
#	services/java/com/realtek/server/IToGoProxyService.aidl

#framework_built := $(call java-lib-deps,framework)
#LOCAL_PRELINK_MODULE := false
LOCAL_REQUIRED_MODULES := com.realtek.xml
include $(BUILD_JAVA_LIBRARY)

# This will install the file in /system/etc/permissions
#
include $(CLEAR_VARS)
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE := com.realtek.xml
LOCAL_MODULE_CLASS := ETC
LOCAL_MODULE_PATH := $(TARGET_OUT_ETC)/permissions
LOCAL_SRC_FILES := $(LOCAL_MODULE)
include $(BUILD_PREBUILT)

# The JNI component
include $(CLEAR_VARS)
include $(call all-makefiles-under,$(LOCAL_PATH))
