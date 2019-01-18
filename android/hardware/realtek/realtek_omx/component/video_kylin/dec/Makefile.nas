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
LIBNAME= $(GENERIC_LINUX_PATH)/lib/libOMX.realtek.video.dec.so
LIBNAME_VP9= $(GENERIC_LINUX_PATH)/lib/libOMX.realtek.video.dec.vp9.so

TOP=$(ANDROID_SRC_PATH)

RTK_TOP := $(TOP)/hardware/realtek/realtek_omx
RTK_INC := $(RTK_TOP)/include
RTK_RPC := $(TOP)/device/realtek/proprietary/libs/rtk_libs/common
RTK_VE1:= $(TOP)/hardware/realtek/VideoEngine/VE1
RTK_VE3:= $(TOP)/hardware/realtek/VideoEngine/VE3
RTK_JPG:= $(TOP)/hardware/realtek/VideoEngine/JPEG

OMX_DECODER_TUNNELING_SUPPORT = false

LOCAL_SRC_FILES := \
	decoder.c \
	codec.c \
	codec_vdec.c \
	codec_jdec.c

LOCAL_SRC_FILES_VP9 := \
	decoder.c \
	codec.c \
	codec_vp9.c 

LOCAL_CFLAGS = -DOMX_DECODER_VIDEO_DOMAIN -DUSE_OUTPUT_CROP -D__GENERIC_LINUX__
LOCAL_CFLAGS += -DUSE_RT_ION

# For HEVC 10bit and compression mode
LOCAL_CFLAGS += -DOMX_HEVC_10BIT_HDR_SUPPORT
LOCAL_SRC_FILES += codec_HEVCHdrParameterParser.c

ifeq ($(OMX_DECODER_TUNNELING_SUPPORT), true)
LOCAL_SRC_FILES += sidebandOutput.c Sideband_C.cpp
LOCAL_CFLAGS += -DOMX_DECODER_TUNNELING_SUPPORT
endif


LOCAL_SRC_FILES += vo_writeback/codec_VORPC.c
LOCAL_CFLAGS += -DCODEC_VO_WRITEBACK
LOCAL_C_INCLUDES += $(RTK_RPC)/IPC/generate/include/system


LOCAL_C_INCLUDES := \
	. \
	$(RTK_TOP)/include/khronos \
	$(RTK_INC)/imagination \
	$(TOP)/system/core/include \
	$(TOP)/system/core/include/cutils \
	$(TOP)/bionic/libc/kernel/common \
	$(TOP)/hardware/libhardware/include \
	$(RTK_TOP)/osal_rtk \
	$(RTK_TOP)/core \
	$(RTK_VE1)/vpuapi \
	$(RTK_VE1)/sample/helper \
	$(RTK_JPG)/include \
	$(RTK_JPG)/jpuapi \
	$(RTK_JPG) \
	$(TOP)/hardware/realtek/StreamingEngine2 \
	$(GENERIC_LINUX_PATH)/include \
	$(TOP)/hardware/realtek/VideoEngine/VE3 \
	$(TOP)/hardware/realtek/VideoEngine/VE3/software/source/inc

LOCAL_C_INCLUDES += $(TOP)/device/realtek/proprietary/libs/rtk_libs/common/include
LOCAL_C_INCLUDES += $(TOP)/device/realtek/proprietary/libs/rtk_libs/common/IPC/include
LOCAL_C_INCLUDES += $(TOP)/device/realtek/proprietary/libs/rtk_libs/common/IPC/generate/include
LOCAL_C_INCLUDES += $(TOP)/device/realtek/proprietary/libs/rtk_libs/common/IPC/generate/include/system
LOCAL_C_INCLUDES += $(TOP)/device/realtek/proprietary/libs/rtk_libs/OSAL/include
LOCAL_C_INCLUDES += $(TOP)/device/realtek/proprietary/libs/libRTKAllocator/include


LOCAL_C_INCLUDES += $(TOP)/device/realtek/proprietary/libs/Include

ifeq ($(ENABLE_TEE_DRM_FLOW),true)
LOCAL_CFLAGS += -DENABLE_TEE_DRM_FLOW
LOCAL_C_INCLUDES += \
	$(TOP)/device/realtek/proprietary/libs/Include/TEEapi
endif


#LIBS += -lpthread

OBJ_FILES = $(addsuffix .o, $(basename $(LOCAL_SRC_FILES)))
OBJ_FILES_VP9 = $(addsuffix .vp9o, $(basename $(LOCAL_SRC_FILES_VP9)))

INCLUDES = $(addprefix -I, $(LOCAL_C_INCLUDES))

CFLAGS = $(INCLUDES) $(DEFINES) $(LOCAL_CFLAGS) -DSYS_UNIX=1 $(MYDEFS) -fPIC -DFIFO_DATATYPE=void*

LOCAL_SHARED_LIBRARIES = -Wl,--no-undefined\
	-L $(GENERIC_LINUX_PATH)/lib  -lRTKOMX_OSAL_RTK -lRTK_lib -lRTKAllocator -lvpu -ljpu -lion -lhwse_$(TARGET_BOARD_PLATFORM)\
	-Wl,--whole-archive \
	$(GENERIC_LINUX_PATH)/src/libMisc/generic_misc.o \
	-Wl,--no-whole-archive

 VP9_LOCAL_CFLAGS = -D__LINUX_MEDIA_NAS_VP9__ -DOMX_DECODER_VIDEO_DOMAIN -DOMX_DECODER_VP9_DOMAIN -DUSE_OUTPUT_CROP
 VP9_LOCAL_CFLAGS += -DUSE_RT_ION
 
 VP9_LOCAL_CFLAGS += -DFIFO_DATATYPE=void*
 VP9_LOCAL_CFLAGS += -DDOWN_SCALER
 VP9_LOCAL_CFLAGS += -D_ASSERT_USED
 VP9_LOCAL_CFLAGS += -DDWL_DISABLE_REG_PRINTS
 VP9_LOCAL_CFLAGS += -DENABLE_VP9_SUPPORT
 VP9_LOCAL_CFLAGS += -DDWL_USE_DEC_IRQ
 VP9_LOCAL_CFLAGS += -DUSE_64BIT_ENV
 VP9_LOCAL_CFLAGS += -DUSE_EXTERNAL_BUFFER

LOCAL_SRC_FILES_VP9 += vo_writeback/codec_VORPC.c
VP9_LOCAL_CFLAGS += -DCODEC_VO_WRITEBACK


	DECODER_FORMATS += avc
	DECODER_FORMATS += mpeg4
	DECODER_FORMATS += vp8
	DECODER_FORMATS += 3gpp
	DECODER_FORMATS += mpeg2
	DECODER_FORMATS += raw
	DECODER_FORMATS += vc1
	DECODER_FORMATS += rv
	DECODER_FORMATS += divx3
	DECODER_FORMATS += avs
	DECODER_FORMATS += mjpg
	DECODER_FORMATS += hevc
	DECODER_FORMATS += wvc1
	DECODER_FORMATS += wmv3
	DECODER_FORMATS += wmv
	DECODER_FORMATS += mp43
	DECODER_FORMATS += flv
	DECODER_FORMATS += rv30
	DECODER_FORMATS += rv40
SYMLINKS := $(addsuffix .so,$(addprefix /usr/local/lib/libOMX.realtek.video.dec.,$(DECODER_FORMATS)))
all: $(LIBNAME) $(LIBNAME_VP9)

$(LIBNAME) : $(OBJ_FILES) 
	$(SHOW_COMMAND)$(rm) $@
	$(SHOW_COMMAND)$(AR) $@ $(OBJ_FILES)
	$(CC) -shared $(LOCAL_SHARED_LIBRARIES) -o $@ $(OBJ_FILES)

$(LIBNAME_VP9) : $(OBJ_FILES_VP9) 
	$(SHOW_COMMAND)$(rm) $@
	$(SHOW_COMMAND)$(AR) $@ $(OBJ_FILES_VP9)
	$(CC) -shared $(LOCAL_SHARED_LIBRARIES) -o $@ $(OBJ_FILES_VP9) -L/usr/local/lib -lvp9
clean:
	$(SHOW_COMMAND)$(rm) $(OBJ_FILES) $(OBJ_FILES_VP9) $(LIBNAME_VP9)  $(LIBNAME) core *~
install:
	cp $(LIBNAME) $(LIBNAME_VP9) /usr/local/lib	
	$(foreach t,$(SYMLINKS),ln -sf /usr/local/lib/libOMX.realtek.video.dec.so $(t);)

miniclean: clean
%.vp9o:%.cpp
	$(rm) -f $@
	$(SHOW_COMMAND)echo -=--=--=- [$*.cpp] -=--=--=--=--=--=--=--=--=-
	$(SHOW_COMMAND)$(CXX) $(CFLAGS) $(VP9_LOCAL_CFLAGS) $(WARNING) $(DEBUGFLAG) $(LIBS) -c $*.cpp -o $@

%.vp9o:%.c
	$(rm) -f $@
	$(SHOW_COMMAND)echo --------- [$*.c] ---------------------------
	$(SHOW_COMMAND)$(CC) $(CFLAGS) $(VP9_LOCAL_CFLAGS) $(WARNING) $(DEBUGFLAG) $(LIBS) -c $*.c -o $@

.cpp.o:
	$(rm) -f $@
	$(SHOW_COMMAND)echo -=--=--=- [$*.cpp] -=--=--=--=--=--=--=--=--=-
	$(SHOW_COMMAND)$(CXX) $(CFLAGS) $(WARNING) $(DEBUGFLAG) $(LIBS) -c $*.cpp -o $@

.c.o:
	$(rm) -f $@
	$(SHOW_COMMAND)echo --------- [$*.c] ---------------------------
	$(SHOW_COMMAND)$(CC) $(CFLAGS) $(WARNING) $(DEBUGFLAG) $(LIBS) -c $*.c -o $@
