TOP ?= ..

include $(TOP)/make.include

#---------------------------------------
# Target definition (User)
#---------------------------------------
TARGET := $(BIN)/test

SOURCES		+= test.c pthread_mutex.c

#---------------------------------------
# Linking libraries (User)
#---------------------------------------
LIBS += $(LIB_OSAL)

APP=Y
include $(TOP)/Makefile.in
