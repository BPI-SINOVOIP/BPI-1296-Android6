###########################################################################
# Edit the following line to indicate where libdl.so is found on your system
###########################################################################
#LIBDL = /usr/lib64/libdl.so  # 64-bit systems
#LIBDL = /usr/lib/libdl.so   # 32-bit systems

# No need to change anything below here

#BUILD = test
ifeq ($(BUILD),test)
CC = gcc
CFLAGS = -DTEST_CASE
else
CC = arm-linux-gnueabihf-gcc
endif

CFLAGS += -static -lpthread
#CFLAGS += -lpthread

all: Speed-Monitor 

Speed-Monitor: Speed-Monitor.c
	$(CC) -o Speed-Monitor Speed-Monitor.c $(CFLAGS)

clean:
	rm -f *~ Speed-Monitor *.so *.o
