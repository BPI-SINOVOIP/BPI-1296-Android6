TOP ?= ../../..

include $(TOP)/make.include

LOCAL_CFLAGS = -D_REENTRANT
LOCAL_INCLUDE = -I$(TOP)/include/SDL2
LOCAL_LIBS = -lSDL2_test -Wl,-rpath,$(TOP)/lib -L$(TOP)/lib -lSDL2 -lpthread

COMMON_FLAGS := $(filter-out -Werror,$(COMMON_FLAGS))
COMMON_FLAGS += -g

INCLUDE_DIR	+= $(LOCAL_INCLUDE)

CFLAGS		+= $(COMMON_FLAGS) $(INCLUDE_DIR) $(LOCAL_CFLAGS)
#---------------------------------------
# Target definition (User)
#---------------------------------------
TARGETS = \
	testdrawchessboard \
	testtimer \
	testthread \


all: $(TARGETS)

testdrawchessboard: testdrawchessboard.c
	$(XCC) -o $@ $@.c $(CFLAGS) $(LOCAL_LIBS)

testtimer:
	$(XCC) -o $@ $@.c $(CFLAGS) $(LOCAL_LIBS)

testthread: testthread.c
	$(XCC) -o $@ $@.c $(CFLAGS) $(LOCAL_LIBS)

clean:
	rm -f $(TARGETS)
