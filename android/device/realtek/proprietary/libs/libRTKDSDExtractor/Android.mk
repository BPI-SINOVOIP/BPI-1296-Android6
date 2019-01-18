LOCAL_PATH:= $(call my-dir)

include $(CLEAR_VARS)

LOCAL_CFLAGS += -DRTK_PLATFORM

LOCAL_SRC_FILES += DSDExtractor.cpp

LOCAL_SHARED_LIBRARIES += liblog
LOCAL_SHARED_LIBRARIES += libutils
LOCAL_SHARED_LIBRARIES += libstagefright_foundation
LOCAL_SHARED_LIBRARIES += libstagefright

LOCAL_MODULE:= libRTKDSDExtractor

LOCAL_MODULE_TAGS := optional

include $(BUILD_SHARED_LIBRARY)

