
# the compiler: gcc for C program, define as g++ for C++
CC      ?= gcc

#ARM32
#CC=arm-linux-androideabi-gcc
#ARM64
#CC=aarch64-linux-android-gcc
#x86
#CC=i686-linux-android-gcc
#x86_64
#CC=x86_64-linux-android-gcc
#mips
#CC=mipsel-linux-android-gcc
#mips64
#CC=mips64el-linux-android-gcc


INCLUDE = .
RM = rm -f

# compiler flags:
#  -g    adds debugging information to the executable file
#  -Wall turns on most, but not all, compiler warnings
CFLAGS  = -g -Wall -static

# the build target executable:
TARGET = rtwpriv

all: $(TARGET)
$(TARGET):  rtwpriv.h rtwpriv.o
		$(CC) $(CFLAGS) -o $(TARGET) rtwpriv.o -lm

rtwpriv.o: rtwpriv.h
	$(CC) -I$(INCLUDE) $(CFLAGS) -c rtwpriv.cpp
	
#hwtx.o: rtw_pmact.h
#	$(CC) -I$(INCLUDE) $(CFLAGS) -c hwtx.c

clean:
		$(RM) $(TARGET) *.o
