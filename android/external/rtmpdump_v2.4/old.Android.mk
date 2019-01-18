LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)

LOCAL_SRC_FILES:= librtmp/log.c librtmp/rtmp.c  librtmp/amf.c librtmp/hashswf.c librtmp/parseurl.c

LOCAL_SHARED_LIBRARIES := libssl libcrypto libz

CRYPTO=OPENSSL
CRYPTO_DEF=$(DEF_$(CRYPTO))
VERSION=v2.4
DEF=-DRTMPDUMP_VERSION=\"$(VERSION)\" $(CRYPTO_DEF) $(XDEF)

LOCAL_C_INCLUDES := externel/boringssl/include externel/boringssl/include/src/include

LOCAL_CFLAGS := -Werror -Wall

LOCAL_CFLAGS += -DSYS_UNIX=1 $(DEF)  -D__u_char_defined -D__daddr_t_defined

LOCAL_MODULE:= librtmp
LOCAL_MODULE_TAGS := optional

include $(BUILD_STATIC_LIBRARY)
#############################################################################3
include $(CLEAR_VARS)
#LOCAL_PATH:= $(call my-dir)

LOCAL_SRC_FILES:= rtmpdump.c

LOCAL_SHARED_LIBRARIES := libz libssl libcrypto
LOCAL_STATIC_LIBRARIES := librtmp 

CRYPTO=OPENSSL
CRYPTO_DEF=$(DEF_$(CRYPTO))
VERSION=v2.4
DEF=-DRTMPDUMP_VERSION=\"$(VERSION)\" $(CRYPTO_DEF) $(XDEF)

LOCAL_C_INCLUDES := 

LOCAL_CFLAGS := -Werror -Wall

LOCAL_CFLAGS += -DSYS_UNIX=1 $(DEF)  -D__u_char_defined -D__daddr_t_defined



LOCAL_MODULE:= rtmpdump
LOCAL_MODULE_TAGS := optional

include $(BUILD_EXECUTABLE)
