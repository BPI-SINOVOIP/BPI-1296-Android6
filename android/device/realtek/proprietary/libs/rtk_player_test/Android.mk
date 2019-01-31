LOCAL_PATH:= $(call my-dir)

include $(CLEAR_VARS)

LOCAL_SRC_FILES:= \
		main.cpp

LOCAL_C_INCLUDES:= \
		bionic							\
		external/stlport/stlport		\
		external/openssl/include		\
		${TOP}/frameworks/av/include	\
		${TOP}/device/realtek/proprietary/libs/RT_IPC \
		$(TOP)/frameworks/native/include/gui			\
		${TOP}/frameworks/av/media/libmediaplayerservice \
		$(LOCAL_PATH)/../Include \
		$(TOP)/device/realtek/proprietary/libs	\
		$(TOP)/device/realtek/proprietary/libs/libRTMediaPlayer	\
		$(TOP)/device/realtek/proprietary/libs/libRTMediaPlayer/DataTransportServer

#LOCAL_SHARED_LIBRARIES:= \
#		libstlport						\

LOCAL_SHARED_LIBRARIES:= \
		libc							\
		libstdc++						\
		libcrypto						\
		libRTMediaPlayer				\
        libutils                        \
        liblog							\
		libgui libstagefright_foundation

#LOCAL_CFLAGS:= -DHDCP_REPEATER=0		\

LOCAL_MODULE:= rtk_player_test
#LOCAL_32_BIT_ONLY := true


ifeq ($(USE_RT_MEDIA_PLAYER),true)
LOCAL_CFLAGS += -DUSE_RT_MEDIA_PLAYER
LOCAL_CFLAGS += -DUSE_RTK_VOUT_UTIL
endif

LOCAL_MODULE_TAGS := optional

include $(BUILD_EXECUTABLE)
