PWD=$(shell pwd)
ifndef TOP
	TOP = ../..
endif
include $(TOP)/make.include

GPU_PATH := $(PWD)/../gpu/mali
DRIVER_PATH := $(GPU_PATH)/drivers/gpu/arm/midgard
#MALI_SUPPORT := GATOR_WITH_MALI_SUPPORT=MALI_MIDGARD GATOR_MALI_MIDGARD_PATH=$(DRIVER_PATH) DDK_DIR=$(GPU_PATH)


all: install

install:
	$(MAKE) -C $(KERNELDIR) M=$(PWD)/driver $(MALI_SUPPORT) modules

clean:
	$(MAKE) -C $(KERNELDIR) M=$(PWD)/driver $(MALI_SUPPORT) clean
