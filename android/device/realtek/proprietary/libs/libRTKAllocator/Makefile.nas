ANDROID_SRC_PATH=../../../../..
GENERIC_LINUX_PATH=$(ANDROID_SRC_PATH)/build
include $(GENERIC_LINUX_PATH)/MakeConfig

MYDEFS=
rm=/bin/rm -f
cp=/bin/cp -f
CC= $(MYCC)
CXX = $(MYCXX)
AS=$(MYAS)
AR= $(MYAR) cq
RANLIB= $(MYRANLIB)
LIBNAME= $(GENERIC_LINUX_PATH)/lib/libRTKAllocator.so

TOP=$(ANDROID_SRC_PATH)
LOCAL_PATH=.

LOCAL_SRC_FILES := src/allocator/Allocator.cpp
LOCAL_SRC_FILES += src/allocator/IonAllocator.cpp
LOCAL_SRC_FILES += src/allocator/MallocAllocator.cpp

LOCAL_SRC_FILES += src/hardware/GrallocWrapper.cpp
LOCAL_SRC_FILES += src/system/SystemMemory.cpp

LOCAL_SRC_FILES += src/system/RingBuffer.cpp
LOCAL_SRC_FILES += src/system/RingBufferBody.cpp
LOCAL_SRC_FILES += src/system/RingBufferHeader.cpp

LOCAL_CFLAGS = -DUSE_RT_ION=1

LOCAL_C_INCLUDES = $(LOCAL_PATH)/include $(TOP)/hardware/libhardware/include
LOCAL_C_INCLUDES += $(TOP)/system/core/include $(TOP)/bionic/libc/kernel/common 

# for rpc_common.h at RingBufferHeader.cpp
LOCAL_C_INCLUDES += $(TOP)/device/realtek/proprietary/libs/rtk_libs/common/IPC/include



OBJ_FILES = $(addsuffix .o, $(basename $(LOCAL_SRC_FILES)))

INCLUDES = $(addprefix -I, $(LOCAL_C_INCLUDES) )

CFLAGS = $(INCLUDES) $(DEFINES) $(LOCAL_CFLAGS)  $(MYDEFS) -fPIC

all: $(LIBNAME)

$(LIBNAME) : $(OBJ_FILES) 
	$(SHOW_COMMAND)$(rm) $@
	$(CC) -shared  -o $@ $(OBJ_FILES) -L$(GENERIC_LINUX_PATH)/lib -lstdc++ -lhardware -lion -Wl,--no-undefined


clean:
	$(SHOW_COMMAND)$(rm) $(OBJ_FILES) $(LIBNAME)  core *~

miniclean: clean

.cpp.o:
	$(rm) -f $@
	$(SHOW_COMMAND)echo -=--=--=- [$*.cpp] -=--=--=--=--=--=--=--=--=-
	$(SHOW_COMMAND)$(CXX) $(CFLAGS) $(WARNING) -std=c++11 $(DEBUGFLAG) -c $*.cpp -o $@

.c.o:
	$(rm) -f $@
	$(SHOW_COMMAND)echo --------- [$*.c] ---------------------------
	$(SHOW_COMMAND)$(CC) $(CFLAGS) $(WARNING) $(DEBUGFLAG) -c $*.c -o $@

.S.o:
	$(rm) -f $@
	$(SHOW_COMMAND)echo --------- [$*.S] ---------------------------
	$(SHOW_COMMAND)$(AS) $(WARNING) -c $*.S -o $@
