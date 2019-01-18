-include ../../../../../branch_src_sharedMemory_integration/Tools/MConfig/.config
include ../../../../../include/MakeConfig

MYDEFS=
rm=/bin/rm -f
cp=/bin/cp -f
CC= $(MYCXX)
AR= $(MYAR) cq
RANLIB=$(MYRANLIB)
LIBNAME= ../../../../../lib/libDataObject.a
#ifneq ($(filter APCFG_PRJ_SCORPIO,$(subst |, ,$(APCFG_FLAGS))),)
#else 
#  ifneq ($(filter APCFG_PRJ_AVHDD,$(subst |, ,$(APCFG_FLAGS))),)
#  else
#    MYDEFS+= -DDB_IN_DVDPLAYER
#  endif
#endif
#INCLUDES=  -I../../../../Include/ -I../../../../Include/freetype2/
INCLUDES=  -I../../../../Include/ -I../../../../Include/freetype2/ -I../../../../../../common/IPC/include -I../../../../../../common/IPC/generate/include/system

ifeq ($(BUILD_CMDLINE_DVDPLAYER), YES)
ifeq ($(ENABLE_ANDROID), YES)
  MYDEFS+= -DTEST_ANDROID
endif
endif

CFLAGS= $(INCLUDES) $(DEFINES) -DSYS_UNIX=1 $(MYDEFS)
ifeq ($(ENABLE_DFB_VKBD), YES)
#for compile dfb_vkd
CFLAGS += -fPIC
endif

SRCS = DataObject.cpp DataObjectHdd.cpp DataObjectFlash.cpp DataObjectExtHdd.cpp

OBJS = DataObject.o DataObjectHdd.o DataObjectFlash.o DataObjectExtHdd.o

.cpp.o:
	$(rm) -f $@
	$(SHOW_COMMAND)echo -=--=--=- [$*.cpp] -=--=--=--=--=--=--=--=--=-
	$(SHOW_COMMAND)$(CC) $(CFLAGS) $(DEBUGFLAG) $(WARNING) -c $*.cpp

all: $(LIBNAME)

$(LIBNAME) : $(OBJS)
	$(rm) $@
	$(AR) $@ $(OBJS)
	$(RANLIB) $@

clean:
	$(rm) $(OBJS) $(LIBNAME) core *~
