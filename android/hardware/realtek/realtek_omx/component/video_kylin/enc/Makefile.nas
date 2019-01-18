ANDROID_SRC_PATH=../../../../../..
GENERIC_LINUX_PATH=$(ANDROID_SRC_PATH)/build
include $(GENERIC_LINUX_PATH)/MakeConfig

MYDEFS=-DHAVE_SYS_UIO_H
rm=/bin/rm -f
cp=/bin/cp -f
CC= $(MYCC)
CXX = $(MYCXX)
AR= $(MYAR) cq
RANLIB= $(MYRANLIB)
LIBNAME_VENC= $(GENERIC_LINUX_PATH)/lib/libOMX.realtek.video.enc

LIBNAME_JENC= $(GENERIC_LINUX_PATH)/lib/libOMX.realtek.image.enc

TOP=$(ANDROID_SRC_PATH)

RTK_TOP := $(TOP)/hardware/realtek/realtek_omx
RTK_INC := $(RTK_TOP)/include
RTK_RPC := $(TOP)/device/realtek/proprietary/libs/rtk_libs/common
RTK_VE1:= $(TOP)/hardware/realtek/VideoEngine/VE1
RTK_VE3:= $(TOP)/hardware/realtek/VideoEngine/VE3
RTK_JPG:= $(TOP)/hardware/realtek/VideoEngine/JPEG

OMX_DECODER_TUNNELING_SUPPORT = false

LOCAL_VENC_SRC_FILES := \
	encoder.c \
	codec_venc.c

LOCAL_JENC_SRC_FILES := \
	encoder.c \
	codec_jenc.c
LOCAL_CFLAGS = -DUSE_OUTPUT_CROP 
LOCAL_CFLAGS += -DUSE_RT_ION -D__GENERIC_LINUX__

VENC_LOCAL_CFLAGS = -DOMX_ENCODER_VIDEO_DOMAIN
JENC_LOCAL_CFLAGS = -DOMX_ENCODER_IMAGE_DOMAIN

LOCAL_C_INCLUDES := \
	. \
	$(RTK_TOP)/include/khronos \
	$(RTK_INC)/imagination \
	$(TOP)/hardware/libhardware/include \
	$(TOP)/system/core/include/cutils \
	$(RTK_RPC)/IPC/include \
	$(TOP)/bionic/libc/kernel/common \
	$(RTK_TOP)/osal_rtk \
	$(RTK_TOP)/core \
	$(RTK_VE1)/vpuapi \
	$(RTK_VE1)/sample/helper \
	$(RTK_JPG)/include \
	$(RTK_JPG)/jpuapi \
	$(RTK_JPG) \
	$(TOP)/hardware/realtek/StreamingEngine2 \
	$(GENERIC_LINUX_PATH)/include \
	$(TOP)/system/core/include 

LOCAL_C_INCLUDES += $(TOP)/device/realtek/proprietary/libs/rtk_libs/common/include
LOCAL_C_INCLUDES += $(TOP)/device/realtek/proprietary/libs/rtk_libs/common/IPC/include
LOCAL_C_INCLUDES += $(TOP)/device/realtek/proprietary/libs/rtk_libs/common/IPC/generate/include
LOCAL_C_INCLUDES += $(TOP)/device/realtek/proprietary/libs/rtk_libs/common/IPC/generate/include/system
LOCAL_C_INCLUDES += $(TOP)/device/realtek/proprietary/libs/rtk_libs/OSAL/include
LOCAL_C_INCLUDES += $(TOP)/device/realtek/proprietary/libs/libRTKAllocator/include


LOCAL_C_INCLUDES += $(TOP)/device/realtek/proprietary/libs/Include


#LIBS += -lpthread

OBJ_VENC_FILES = $(addsuffix .vo, $(basename $(LOCAL_VENC_SRC_FILES)))
OBJ_JENC_FILES = $(addsuffix .jo, $(basename $(LOCAL_JENC_SRC_FILES)))

INCLUDES = $(addprefix -I, $(LOCAL_C_INCLUDES))

CFLAGS = $(INCLUDES) $(DEFINES) $(LOCAL_CFLAGS) -DSYS_UNIX=1 $(MYDEFS) -fPIC

LOCAL_SHARED_LIBRARIES = -Wl,--no-undefined \
	-L $(GENERIC_LINUX_PATH)/lib -lRTKOMX_OSAL_RTK -lhardware -lRTKAllocator 

LOCAL_SHARED_LIBRARIES +=  -lvpu -ljpu -lhwse_$(TARGET_BOARD_PLATFORM)\
	-Wl,--whole-archive \
	$(GENERIC_LINUX_PATH)/src/libMisc/generic_misc.o \
	-Wl,--no-whole-archive

#	$(GENERIC_LINUX_PATH)/lib/libhwse_$(TARGET_BOARD_PLATFORM).a \
	$(GENERIC_LINUX_PATH)/lib/libRtkIpc.a \

ENCODER_FORMATS += avc
ENCODER_FORMATS += mpeg4
SYMLINKS := $(addsuffix .so,$(addprefix /usr/local/lib/libOMX.realtek.video.enc.,$(ENCODER_FORMATS)))
all:  $(LIBNAME_VENC).so $(LIBNAME_JENC).so 

$(LIBNAME_VENC).so : $(OBJ_VENC_FILES) 
	$(SHOW_COMMAND)$(rm) $@
	$(SHOW_COMMAND)$(AR) $@ $(OBJ_VENC_FILES)
	$(CC) -shared $(LOCAL_SHARED_LIBRARIES) -o $@ $(OBJ_VENC_FILES)

$(LIBNAME_JENC).so : $(OBJ_JENC_FILES) 
	$(SHOW_COMMAND)$(rm) $@
	$(SHOW_COMMAND)$(AR) $@ $(OBJ_JENC_FILES)
	$(CC) -shared $(LOCAL_SHARED_LIBRARIES) -o $@ $(OBJ_JENC_FILES)
clean:
	$(SHOW_COMMAND)$(rm) $(OBJ_JENC_FILES) $(OBJ_VENC_FILES) $(LIBNAME_JENC) $(LIBNAME_VENC).so core *~

install:
	cp $(LIBNAME_VENC).so $(LIBNAME_JENC).so /usr/local/lib	
		$(foreach t,$(SYMLINKS),ln -sf /usr/local/lib/libOMX.realtek.video.enc.so $(t);)
fix_gralloc::
	cp ./android_ext.h ../../../osal_rtk/android_ext.h

miniclean: clean

%.vo:%.cpp
	$(rm) -f $@
	$(SHOW_COMMAND)echo -=--=--=- [$*.cpp] -=--=--=--=--=--=--=--=--=-
	$(SHOW_COMMAND)$(CXX) $(CFLAGS) $(VENC_LOCAL_CFLAGS) $(WARNING) $(DEBUGFLAG) $(LIBS) -c $*.cpp -o $@

%.vo:%.c
	$(rm) -f $@
	$(SHOW_COMMAND)echo --------- [$*.c] ---------------------------
	$(SHOW_COMMAND)$(CC) $(CFLAGS) $(VENC_LOCAL_CFLAGS) $(WARNING) $(DEBUGFLAG) $(LIBS) -c $*.c -o $@

%.jo:%.cpp
	$(rm) -f $@
	$(SHOW_COMMAND)echo -=--=--=- [$*.cpp] -=--=--=--=--=--=--=--=--=-
	$(SHOW_COMMAND)$(CXX) $(CFLAGS) $(JENC_LOCAL_CFLAGS) $(WARNING) $(DEBUGFLAG) $(LIBS) -c $*.cpp -o $@

%.jo:%.c
	$(rm) -f $@
	$(SHOW_COMMAND)echo --------- [$*.c] ---------------------------
	$(SHOW_COMMAND)$(CC) $(CFLAGS) $(JENC_LOCAL_CFLAGS) $(WARNING) $(DEBUGFLAG) $(LIBS) -c $*.c -o $@

.cpp.o:
	$(rm) -f $@
	$(SHOW_COMMAND)echo -=--=--=- [$*.cpp] -=--=--=--=--=--=--=--=--=-
	$(SHOW_COMMAND)$(CXX) $(CFLAGS) $(WARNING) $(DEBUGFLAG) $(LIBS) -c $*.cpp -o $@

.c.o:
	$(rm) -f $@
	$(SHOW_COMMAND)echo --------- [$*.c] ---------------------------
	$(SHOW_COMMAND)$(CC) $(CFLAGS) $(WARNING) $(DEBUGFLAG) $(LIBS) -c $*.c -o $@
