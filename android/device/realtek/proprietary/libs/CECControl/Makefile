

##############################################
# Tool Chain
##############################################
include ../../../include/MakeConfig
MAKEFLAGS = --no-print-directory
CFLAGS = $(DEBUGFLAG)
CC = $(MYCC)
AR = $(MYAR)

##############################################
# PATH
##############################################
MYLIB            := $(CURDIR)/../../../lib/libCECControl.a
EXPORT_INC_PATH  := $(CURDIR)/../../Include/Platform_Lib/CECControl/
OSAL_INC         := $(CURDIR)/../../Include/
COMMON_INC       := $(CURDIR)/common/inc/
ADAPTER_INC      := $(CURDIR)/adapter/inc/
CONTROLLER_INC   := $(CURDIR)/controller/inc/
BRIDGE_INC       := $(CURDIR)/bridge/inc/
PLATFORM_COMM_INC:= $(CURDIR)/../../Include//Platform_Lib/PlatformCommon


##############################################
# Environment
##############################################
SRC     = 
OBJ     = $(SRC:.cpp=.o)
INC     = $(SRC:.cpp=.h)
INC_DEF = -I inc/ -I $(OSAL_INC) -I $(PLATFORM_COMM_INC)
SUBDIRS = common adapter controller bridge
VPATH   = obj inc src 


##############################################
# Depandency
##############################################
 
ifeq ($(USE_CEC),YES)
#CEC_MODULE = MARS_CEC
#CEC_MODULE = VERONA_CEC
endif

##############################################
# export control
##############################################
export   

##############################################
# Make
##############################################
.PHONY: test
all : mk_dir rm_lib subdir	
		
subdir:
	@set -e; for i in $(SUBDIRS); do $(MAKE) -C $$i; done;				

mk_dir:
	@mkdir -p $(EXPORT_INC_PATH)

rm_lib:
	@$(RM) -f $(EXPORT_INC_PATH)/*
	@$(RM) -vf $(MYLIB)

test:
	@ $(MAKE) link -C test/
	
clean: 	
	@set -e; for i in $(SUBDIRS); do $(MAKE) clean -C $$i; done;	
	@$(RM) -vf src/*~ src/*.bak 	
	@$(RM) -vf inc/*~ inc/*.bak
	@$(RM) -vf obj/*	
	@$(RM) -vf *~ *.bak 	
	
