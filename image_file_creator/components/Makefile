#
# According to Makefile.in, update all images and then produce Image File
#

include ../Makefile.in
include packages/$(TARGET)/Makefile.in
include configuration.in

SHELL=/bin/bash

ifeq ($(layout_type), '')
LAYOUT:=nand
else
LAYOUT:=$(layout_type)
endif

ifeq ($(layout_use_emmc_swap), '')
LAYOUT_USE_EMMC_SWAP:=emmc_swap_false
else
LAYOUT_USE_EMMC_SWAP:=emmc_swap_$(layout_use_emmc_swap)
endif

TMP:=$(CURDIR)/tmp
ifeq ($(LAYOUT), )
  $(error No LAYOUT found.)
endif

ifneq ($(CUSTOMER_ID), )
  CUSTOMER_FOLDER := packages/$(TARGET)/customer/$(CUSTOMER_ID)
else
  ifeq ($(LAYOUT), nand)
    ifeq ($(ANDROID_IMGS), y)
    CUSTOMER_ID := rtk_generic
    else ifeq ($(NAS_IMGS), y)
    CUSTOMER_ID := rtk_generic_slc
    else ifeq ($(LNX_IMGS), y)
    CUSTOMER_ID := rtk_generic
    endif
  else ifeq ($(LAYOUT), emmc) # emmc 4GB
    ifeq ($(layout_size), 4gb)
    CUSTOMER_ID := rtk_generic_emmc
    else ifeq ($(layout_size), 8gb)
    CUSTOMER_ID := rtk_generic_emmc_8gb
    else ifeq ($(layout_size), 16gb)
    CUSTOMER_ID := rtk_generic_emmc_16gb
    else ifeq ($(layout_size), 32gb)
    CUSTOMER_ID := rtk_generic_emmc_32gb
    endif
    ifeq ($(ANDROID_IMGS), n)
      ifeq ($(NAS_IMGS), y)
      CUSTOMER_ID := rtk_generic_emmc_nas
      endif
    endif
  else ifeq ($(LAYOUT), sata)
    CUSTOMER_ID := rtk_generic_sata
  else ifeq ($(LAYOUT), spi)
    CUSTOMER_ID := rtk_generic_spi
  endif
  CUSTOMER_FOLDER := packages/$(TARGET)/customer/$(CUSTOMER_ID)
endif

#### Utilities
OPENSSL = openssl
DUMMY_TARGET       = $(LAYOUT).uImage
GEN_RSA_PATTERN     = $(CURDIR)/bin/gen_rsa_pattern.pl
REAL_RSA_OUT        = msg.bin
RRMODN              = RRModN.bin
NP_INV32            = np_inv32.bin
TMP_RSA_OUT         = rsa_enc.bin
#### 
LIBFILE=`cat packages/$(TARGET)/system/vendor/lib/mediadrm/lib.List`
MYZLIB_PATH = $(CURDIR)/bin/myzlib
E2FSCK_PATH = $(CURDIR)/bin/e2fsck
MKE2FS_PATH = $(CURDIR)/bin/mke2fs
MAKE_EXT4FS = $(CURDIR)/bin/make_ext4fs
SIMG2IMG = $(CURDIR)/bin/simg2img
RESIZE2FS_PATH = $(CURDIR)/bin/resize2fs
MKSQUASHFS_PATH = $(CURDIR)/bin/mksquashfs
MKYAFFS2IMAGE_PATH = $(CURDIR)/bin/mkyaffs2image
RTSSL_PATH = $(CURDIR)/bin/RtSSL
MKUBIFS_PATH := $(CURDIR)/bin/mkfs.ubifs
UBINIZE_PATH := $(CURDIR)/bin/ubinize
DO_SHA256_PATH := $(CURDIR)/bin/do_sha256
SHA_REVERSE_PATH := $(CURDIR)/bin/reverse_rsa_data.pl
REVERSE_PL := $(CURDIR)/bin/reverse.pl
STR2BIN_PATH := $(CURDIR)/bin/str2bin.pl
RUNCMD_PATH := $(CURDIR)/bin/runCmd.pl
FDTGET_PATH := $(CURDIR)/bin/fdtget
LIB_ENC_PATH := $(CURDIR)/bin/lib_encryptor
OBFUSE_PATH := $(CURDIR)/bin/obfuse
GZIP_PATH := $(CURDIR)/bin/minigzip
LZMA_PATH := $(CURDIR)/bin/lzma

#############################

IMGFILE_PATH = $(CURDIR)/../$(IMGFILE_NAME)
IMGFILE_AP_PATH = $(CURDIR)/../$(PROJECT_NAME).$(LAYOUT).ap.img
EFUSE_FW_FILE = efuse_programmer.bin
EFUSE_FW_FILE_PATH = $(CURDIR)/../$(EFUSE_FW_FILE)
SECURE_FW_KEY_FILE = $(CURDIR)/../aes_128bit_key.bin.enc
SECURE_TEE_KEY_FILE = $(CURDIR)/../aes_128bit_key_2.bin.enc
SECURE_KEY_FILE = $(CURDIR)/../aes_128bit_key.bin
SECURE_KEY1_FILE = $(CURDIR)/../aes_128bit_key_1.bin
SECURE_KEY2_FILE = $(CURDIR)/../aes_128bit_key_2.bin
SECURE_KEY3_FILE = $(CURDIR)/../aes_128bit_key_3.bin
SECURE_KEY_SEED_FILE = $(CURDIR)/../aes_128bit_seed.bin
SECURE_KEY_FILE_REV = $(CURDIR)/../aes_128bit_key_rev.bin
SECURE_KEY_SEED_FILE_REV = $(CURDIR)/../aes_128bit_seed_rev.bin
RSA_PRIVATE_KEY = $(CURDIR)/../rsa_key_2048.pem
RSA_FW_PRIVATE_KEY = $(CURDIR)/../rsa_key_2048.fw.pem
RSA_TEE_PRIVATE_KEY = $(CURDIR)/../rsa_key_2048.tee.pem
RSA_LIB_PRIVATE_KEY = $(CURDIR)/../rsa_lib_2048.pem
RSA_LIB_PRIVATE_OTP_KEY = $(CURDIR)/../rsa_lib_otp_2048.pem
RSA_LIB_PUB_KEY = packages/$(TARGET)/system/vendor/lib/mediadrm/rsa_lib_2048.pub
EFUSE_PROGRAMMER_FILE = efuse_programmer.complete.enc
EFUSE_PROGRAMMER_FILE_PATH = $(CURDIR)/../$(EFUSE_PROGRAMMER_FILE)
EFUSE_VERIFY_FILE = efuse_verify.bin
EFUSE_VERIFY_FILE_PATH = $(CURDIR)/../$(EFUSE_VERIFY_FILE)

#---------------------------------------------------------------------
#$(call hwrsa-sign, rsa_key_text, clear_data, enc_data)
hwrsa-sign =                                                                                          \
        $(OPENSSL) rsa -text -in $(1) -out $(1).text;                                           \
        dd if=$(DUMMY_TARGET) of=224b.bin bs=224 count=1;                                    \
        cat 224b.bin $(2) > clear_data.bin;                                                           \
        $(OPENSSL) rsautl -inkey $(1) -sign -raw -in clear_data.bin -out $(TMP_RSA_OUT);   \
        $(GEN_RSA_PATTERN) --key $(1).text --msg $(TMP_RSA_OUT) --binary;                                  \
        cat $(REAL_RSA_OUT) $(RRMODN) $(NP_INV32) > $(3)

###############


ifeq ($(offline_gen), y)
ifeq ($(wildcard installer_x86/setting.txt),)
$(error installer_x86/setting.txt does not exist)
endif
endif

ifeq ($(wildcard gen_binary/gen_binary_tool),)
$(error gen_binary/gen_binary_tool does not exist)
endif

ifeq ($(wildcard packages/$(TARGET)/System.map.audio),)
$(error packages/$(TARGET)/System.map.audio does not exist)
endif

#AUDIOADDR=0x$(shell grep -w _osboot packages/$(TARGET)/System.map.audio | awk -F' ' '{print $$1}')
AUDIOADDR=0x01b00000
#LINUXADDR=0x$(shell grep -w stext packages/$(TARGET)/System.map | awk -F' ' '{print $$1}')
ifeq ($(NAS_IMGS), y)
  ifeq ($(HYPERVISOR), y)
  LINUXADDR=0x04000000
  else
  LINUXADDR=0x03000000
  endif
else
LINUXADDR=0x03000000
endif
KERNELDT_ADDR=0x02100000
RESCUEDT_ADDR=0x02140000
KERNELROOTFSADDR=0x02200000
RESCUEROOTFSADDR=0x30000000
TEEADDR=0x10200000
BL31ADDR=0x10120000
XENADDR=0x03000000
ifeq ($(genAll), y)
    FLASH_SIZE= 8gb 16gb
else
    FLASH_SIZE= $(layout_size)
endif
#$(shell $(FDTGET_PATH) -t x ./packages/$(TARGET)/android.$(LAYOUT).dtb /fb reg | sed -e 's/\s.*//g' > $(TMP)/bootfile_image_addr)  # get fb address from dtb
TMP_BOOTFILE_IMAGE_ADDR=
ifeq ($(TMP_BOOTFILE_IMAGE_ADDR),)  # empty
BOOTFILE_IMAGE_ADDR=0x1e800000  # If nothing we can get from dtb, use this default value. 
else
BOOTFILE_IMAGE_ADDR=0x$(TMP_BOOTFILE_IMAGE_ADDR)
endif

############################

all:
	@echo Setting: LAYOUT=$(LAYOUT) PROJECT_NAME=$(PROJECT_NAME) CUSTOMER_ID=$(CUSTOMER_ID)
	@echo
ifeq ($(ANDROID_IMGS), y)
ifneq ($(LIVEUPDATE_URL), )
	$(error No change LIVEUPDATE_URL for prebuilt images.)
endif
ifeq ($(AV_IN_ROOT), y)
	$(error We havn't supported prebuilt images with AV in root.)
endif
endif

#	(sudo umount tmp/rootfs || true)
	rm -rf tmp/* && mkdir -p tmp/pkgfile/$(TARGET)
	if [ -d $(CUSTOMER_FOLDER)/info ]; then cp $(CUSTOMER_FOLDER)/info/* $(CUSTOMER_FOLDER)/../../factory/; fi
#for only-install_factory package
ifeq ($(only_install_factory), 1)
	echo "only_factory=y" >> tmp/pkgfile/config.txt
	echo "ifcmd0 = \"$(IFCMD0)\"" >> tmp/pkgfile/config.txt
	echo "ifcmd1 = \"$(IFCMD1)\"" >> tmp/pkgfile/config.txt
	if [ '$(stop_reboot)' = '1' ]; then echo "stop_reboot=y" >> tmp/pkgfile/config.txt; fi
	if [ '$(logger_level)' != '0' ]; then \
		echo "logger_level="$(logger_level) >> tmp/pkgfile/config.txt; \
	fi
	cd packages/$(TARGET)/factory$(FACTORY_SUFFIX) && tar cf ../factory.tar *
	mv packages/$(TARGET)/factory.tar tmp/pkgfile/$(TARGET)/
	cp installer/install_a tmp/pkgfile/
	-grep -q 'start_customer=y' tmp/pkgfile/config.txt && \
	cp installer/customer.tar tmp/pkgfile/
	cd tmp/pkgfile/ && tar cvf $(IMGFILE_PATH) *
else ifeq ($(only_install_bootcode), 1)
	echo "only_bootcode=y" >> tmp/pkgfile/config.txt
	echo "bootcode=y" >> tmp/pkgfile/config.txt
	if [ '$(verify)' = '1' ]; then echo "verify=y" >> tmp/pkgfile/config.txt; fi
	if [ '$(stop_reboot)' = '1' ]; then echo "stop_reboot=y" >> tmp/pkgfile/config.txt; fi
	if [ '$(logger_level)' != '0' ]; then \
		echo "logger_level="$(logger_level) >> tmp/pkgfile/config.txt; \
	fi
	cp packages/$(TARGET)/bootloader.tar tmp/pkgfile/$(TARGET)
	cp installer/install_a tmp/pkgfile/
	-grep -q 'start_customer=y' tmp/pkgfile/config.txt && \
	cp installer/customer.tar tmp/pkgfile/
	if [ "`ls -A packages/$(TARGET)/installer`" != ".svn" ] && [ "`ls -A packages/$(TARGET)/installer`" != "" ]; then \
		cp -R packages/$(TARGET)/installer/* tmp/pkgfile/; \
	fi
	if [ '$(LAYOUT)' = 'nand' ]; then \
		cp $(CURDIR)/bin/rootfs/nandwrite tmp/pkgfile/; \
		cp $(CURDIR)/bin/rootfs/flash_erase tmp/pkgfile/; \
	fi
	cd tmp/pkgfile/ && tar cvf $(IMGFILE_PATH) *
	if [ '$(NAS_IMGS)' = 'y' ]; then \
		echo Original image size is `ls -lG $(IMGFILE_PATH) | awk -F' ' '{print $$4}'` && \
		if [ '$(hash_imgfile)' = 1 ]; then \
			if [ '$(SECURE_BOOT)' = y ]; then \
				dd if=/dev/zero  bs=1 count=`expr \( 4096000000 - \`ls -lG $(IMGFILE_PATH) | awk -F' ' '{print $$4}'\` - 128 \) % 4096` >> $(IMGFILE_PATH); \
				$(RTSSL_PATH) dgst -mars_sha1 -a -i $(IMGFILE_PATH) -sign -rsa -k $(RSA_KEY_FILE); \
			else \
				dd if=/dev/zero  bs=1 count=`expr \( 4096000000 - \`ls -lG $(IMGFILE_PATH) | awk -F' ' '{print $$4}'\` - 20 \) % 4096` >> $(IMGFILE_PATH); \
				$(RTSSL_PATH) dgst -mars_sha1 -a -i $(IMGFILE_PATH) -sign -aes_128_ecb -k 0; \
			fi; \
		else \
			dd if=/dev/zero  bs=1 count=`expr \( 4096000000 - \`ls -lG $(IMGFILE_PATH) | awk -F' ' '{print $$4}'\` \) % 4096` >> $(IMGFILE_PATH); \
		fi; \
	fi
else

############### new parameter  framework in config.txt
	echo "# Package Information" >> tmp/pkgfile/config.txt;
	echo "company=\"$(COMPANY)\"" >> tmp/pkgfile/config.txt;
	echo "description=\"$(DESCRIPTION)\"" >> tmp/pkgfile/config.txt;
	echo "modelname=\"$(MODELNAME)\"" >> tmp/pkgfile/config.txt;
	echo "version=\"$(VERSION)\"" >> tmp/pkgfile/config.txt;
	echo "releaseDate=\"$(RELEASEDATE)\"" >> tmp/pkgfile/config.txt;
	echo "signature=\"$(SIGNATURE)\"" >> tmp/pkgfile/config.txt;
	echo "# Package Configuration" >> tmp/pkgfile/config.txt;
	-[ '$(NAS_IMGS)' = 'y' ] && [ '$(LAYOUT)' = 'spi' ] || \
	echo "start_customer=y" >> tmp/pkgfile/config.txt;
	if [ '$(verify)' = '1' ]; then echo "verify=y" >> tmp/pkgfile/config.txt; fi
	if [ '$(install_bootloader)' = '1' ]; then echo "bootcode=y" >> tmp/pkgfile/config.txt; fi
	if [ '$(install_factory)' = '1' ]; then echo "install_factory=y" >> tmp/pkgfile/config.txt; fi
	if [ '$(install_dtb)' = '1' ]; then echo "install_dtb=y" >> tmp/pkgfile/config.txt; else echo "install_dtb=n" >> tmp/pkgfile/config.txt; fi
	if [ '$(nas_rescue)' = '1' ]; then echo "nas_rescue=y" >> tmp/pkgfile/config.txt; fi
	if [ '$(update_etc)' = '1' ]; then echo "update_etc=y" >> tmp/pkgfile/config.txt; fi
	if [ '$(jffs2_nocleanmarker)' = '1' ]; then echo "jffs2_nocleanmarker=y" >> tmp/pkgfile/config.txt; fi
	if [ '$(stop_reboot)' = '1' ]; then echo "stop_reboot=y" >> tmp/pkgfile/config.txt; fi 
	echo "install_avfile_count=$(install_avfile_count)" >> tmp/pkgfile/config.txt;
	if [ $(install_avfile_count) > 1 ] && [ -f ./packages/$(TARGET)/bluecore.video.zip ]; then echo "install_avfile_video_size=$(install_avfile_video_size)" >> tmp/pkgfile/config.txt; fi
	if [ '$(LAYOUT)' = 'nand' ]; then echo "rba_percentage=$(rba_percentage)" >> tmp/pkgfile/config.txt; fi
	if [ '$(stop_reboot)' = '0' ]; then echo "reboot_delay=$(reboot_delay)" >> tmp/pkgfile/config.txt; fi
	if [ '$(logger_level)' != '0' ]; then echo "logger_level="$(logger_level) >> tmp/pkgfile/config.txt; fi
	if [ '$(install_factory)' = '1' ]; then echo "ifcmd0 = \"$(IFCMD0)\"" >> tmp/pkgfile/config.txt; fi
	if [ '$(install_factory)' = '1' ]; then echo "ifcmd1 = \"$(IFCMD1)\"" >> tmp/pkgfile/config.txt; fi	
	if [ '$(efuse_key)' = '1' ]; then echo "efuse_key=1" >> tmp/pkgfile/config.txt; else echo "efuse_key=0" >> tmp/pkgfile/config.txt; fi
	if [ '$(efuse_fw)' = '1' ]; then echo "efuse_fw=1" >> tmp/pkgfile/config.txt; else echo "efuse_fw=0" >> tmp/pkgfile/config.txt; fi
	if [ '$(SECURE_BOOT)' = y ]; then \
		echo "secure_boot=1" >> tmp/pkgfile/config.txt; \
		echo "fw = GOLDrescueDT $(TARGET)/gold.rescue.$(LAYOUT).dtb $(RESCUEDT_ADDR)" >> tmp/pkgfile/config.txt; \
		echo "fw = GOLDRootFS $(TARGET)/gold.rescue.root.$(LAYOUT).cpio.gz_pad.img$(F_EXT).aes  $(RESCUEROOTFSADDR)" >> tmp/pkgfile/config.txt; \
		echo "fw = GOLDKernel $(TARGET)/gold.$(LAYOUT).uImage$(F_EXT).aes $(LINUXADDR)" >> tmp/pkgfile/config.txt; \
		echo "fw = GOLDaudio $(TARGET)/gold.bluecore.audio$(F_EXT).aes $(AUDIOADDR)" >> tmp/pkgfile/config.txt; \
		if [ '$(TEE_FW)' = 'y' ]; then \
		echo "fw = tee $(TARGET)/tee.bin.aes $(TEEADDR)" >> tmp/pkgfile/config.txt; \
		echo "fw = BL31 $(TARGET)/bl31.bin.aes $(BL31ADDR)" >> tmp/pkgfile/config.txt; \
		fi; \
		echo "fw = rescueDT $(TARGET)/rescue.$(LAYOUT).dtb $(RESCUEDT_ADDR)" >> tmp/pkgfile/config.txt; \
		echo "fw = kernelDT $(TARGET)/android.$(LAYOUT).dtb $(KERNELDT_ADDR)" >> tmp/pkgfile/config.txt; \
		echo "fw = rescueRootFS $(TARGET)/rescue.root.$(LAYOUT).cpio.gz_pad.img$(F_EXT).aes $(RESCUEROOTFSADDR)" >> tmp/pkgfile/config.txt; \
		if [ '$(NAS_IMGS)' != 'y' ] && [ '$(ANDROID_IMGS)' = 'y' ]; then \
		echo "fw = kernelRootFS $(TARGET)/android.root.$(LAYOUT).cpio.gz_pad.img$(F_EXT).aes $(KERNELROOTFSADDR)" >> tmp/pkgfile/config.txt; \
		fi; \
		echo "fw = linuxKernel $(TARGET)/$(LAYOUT).uImage$(F_EXT).aes $(LINUXADDR)" >> tmp/pkgfile/config.txt; \
        	echo "fw = audioKernel $(TARGET)/bluecore.audio$(F_EXT).aes $(AUDIOADDR)" >> tmp/pkgfile/config.txt; \
		if [ '$(HYPERVISOR)' = 'y' ] && [ '$(NAS_IMGS)' = 'y' ]; then \
		echo "fw = XenOS $(TARGET)/xen.img.aes $(XENADDR)" >> tmp/pkgfile/config.txt; \
		fi; \
	else \
		echo "secure_boot=0" >> tmp/pkgfile/config.txt; \
		if [ '$(NAS_IMGS)' = 'y' ] && [ '$(LAYOUT)' = 'spi' ]; then \
			echo "[NAS]Skip golden fw in NAS SPI + HDD image" ; \
		else \
		echo "fw = GOLDrescueDT $(TARGET)/gold.rescue.$(LAYOUT).dtb $(RESCUEDT_ADDR)" >> tmp/pkgfile/config.txt; \
		echo "fw = GOLDRootFS $(TARGET)/gold.rescue.root.$(LAYOUT).cpio.gz_pad.img$(F_EXT) $(RESCUEROOTFSADDR)" >> tmp/pkgfile/config.txt; \
		echo "fw = GOLDKernel $(TARGET)/gold.$(LAYOUT).uImage$(F_EXT) $(LINUXADDR)" >> tmp/pkgfile/config.txt; \
		echo "fw = GOLDaudio $(TARGET)/gold.bluecore.audio$(F_EXT) $(AUDIOADDR)" >> tmp/pkgfile/config.txt; \
		fi; \
		echo "fw = rescueDT $(TARGET)/rescue.$(LAYOUT).dtb $(RESCUEDT_ADDR)" >> tmp/pkgfile/config.txt; \
		echo "fw = kernelDT $(TARGET)/android.$(LAYOUT).dtb $(KERNELDT_ADDR)" >> tmp/pkgfile/config.txt; \
		echo "fw = rescueRootFS $(TARGET)/rescue.root.$(LAYOUT).cpio.gz_pad.img$(F_EXT) $(RESCUEROOTFSADDR)" >> tmp/pkgfile/config.txt; \
		if [ '$(NAS_IMGS)' != 'y' ] && [ '$(ANDROID_IMGS)' = 'y' ]; then \
			echo "fw = kernelRootFS $(TARGET)/android.root.$(LAYOUT).cpio.gz_pad.img$(F_EXT) $(KERNELROOTFSADDR)" >> tmp/pkgfile/config.txt; \
		fi; \
		if [ -f ./packages/$(TARGET)/$(LAYOUT).uImage ]; then echo "fw = linuxKernel $(TARGET)/$(LAYOUT).uImage $(LINUXADDR)" >> tmp/pkgfile/config.txt; fi; \
		echo "fw = audioKernel $(TARGET)/bluecore.audio$(F_EXT) $(AUDIOADDR)" >> tmp/pkgfile/config.txt; \
		if [ '$(TEE_FW)' = 'y' ]; then \
		echo "fw = tee $(TARGET)/tee.bin $(TEEADDR)" >> tmp/pkgfile/config.txt; \
		echo "fw = BL31 $(TARGET)/bl31.bin $(BL31ADDR)" >> tmp/pkgfile/config.txt; \
		fi; \
		if [ '$(HYPERVISOR)' = 'y' ] && [ '$(NAS_IMGS)' = 'y' ]; then \
			echo "fw = XenOS $(TARGET)/xen.img $(XENADDR)" >> tmp/pkgfile/config.txt; \
		fi; \
		if [ '$(nas_rescue)' = '1' ] && [ '$(NAS_IMGS)' = 'y' ]; then \
			if [ -f ./packages/$(TARGET)/nas.rescue.$(LAYOUT).dtb ]; then echo "fw = NASrescueDT $(TARGET)/nas.rescue.$(LAYOUT).dtb $(RESCUEDT_ADDR)" >> tmp/pkgfile/config.txt; fi; \
			if [ -f ./packages/$(TARGET)/nas.$(LAYOUT).uImage ]; then echo "fw = NASrescueKernel $(TARGET)/nas.$(LAYOUT).uImage $(LINUXADDR)" >> tmp/pkgfile/config.txt; fi; \
			if [ -f ./packages/$(TARGET)/nas.rescue.root.$(LAYOUT).cpio.gz_pad.img ]; then echo "fw = NASrescueRootFS $(TARGET)/nas.rescue.root.$(LAYOUT).cpio.gz_pad.img $(RESCUEROOTFSADDR)" >> tmp/pkgfile/config.txt; fi; \
		fi; \
	fi
	if [ -f ./packages/$(TARGET)/uboot.bin ]; then \
	   echo "fw = UBOOT $(TARGET)/uboot.bin 0x00020000" >> tmp/pkgfile/config.txt; \
	fi
	# the entry address of bootfile are decided by bootcode
	if [ -f ./packages/$(TARGET)/bootfile.audio ] && [ $(install_avfile_count) -gt 0 ]; then \
           echo "fw = audioFile $(TARGET)/bootfile.audio 0xdeaddead" >> tmp/pkgfile/config.txt; \
        fi
	if [ -f ./packages/$(TARGET)/bootfile.video ] && [ $(install_avfile_count) -gt 0 ]; then \
           echo "fw = videoFile $(TARGET)/bootfile.video 0xdeaddead" >> tmp/pkgfile/config.txt; \
        fi
	if [ -f ./packages/$(TARGET)/bootfile.image ] && [ $(install_avfile_count) -gt 0 ]; then \
           echo "fw = imageFile $(TARGET)/bootfile.image $(BOOTFILE_IMAGE_ADDR)" >> tmp/pkgfile/config.txt; \
        fi
	if [ '$(LAYOUT)' = 'emmc' ] || [ '$(LAYOUT)' = 'nand' ] || [ '$(LAYOUT)' = 'spi' ] || [ '$(LAYOUT)' = 'sata' ]; then \
        if [ '$(LAYOUT_USE_EMMC_SWAP)' = 'emmc_swap_true' ] ; then \
           cat $(CUSTOMER_FOLDER)/partition.emmc_swap_700MB.txt | sed s/target_pkg/$(TARGET)/ >> tmp/pkgfile/config.txt; \
        else \
           cat $(CUSTOMER_FOLDER)/partition.txt | sed s/target_pkg/$(TARGET)/ >> tmp/pkgfile/config.txt; \
        fi; \
	fi
	if [ '$(LAYOUT)' = 'emmc' ] || [ '$(LAYOUT)' = 'nand' ] || [ '$(LAYOUT)' = 'sata' ]; then \
		if [ '$(HYPERVISOR)' = 'y' ] && [ '$(NAS_IMGS)' = 'y' ]; then \
			if [ '$(LAYOUT)' = 'emmc' ]; then \
				echo "part = xen /domu ext4 $(TARGET)/xen.bin 67108864" >> tmp/pkgfile/config.txt; \
			fi; \
			if [ '$(LAYOUT)' = 'sata' ]; then \
				echo "part = xen /domu ext4 $(TARGET)/xen.bin 67108864" >> tmp/pkgfile/config.txt; \
			fi; \
		fi; \
		if [ -f $(CUSTOMER_FOLDER)/partition_customize.txt ]; then \
			cat $(CUSTOMER_FOLDER)/partition_customize.txt >> tmp/pkgfile/config.txt; \
		fi; \
	fi

############### new parameter framework in config.txt

	cp -R installer/install_a tmp/pkgfile/
	-grep -q 'start_customer=y' tmp/pkgfile/config.txt && \
	cp -R installer/customer.tar tmp/pkgfile/
	-grep -q 'start_customer=y' tmp/pkgfile/config.txt && \
	cp -R installer/ALSADaemon tmp/pkgfile/

	if [ '$(LAYOUT)' = 'nand' ]; then \
		cp $(CURDIR)/bin/rootfs/ubiformat tmp/pkgfile/; \
		cp $(CURDIR)/bin/rootfs/nandwrite tmp/pkgfile/; \
		cp $(CURDIR)/bin/rootfs/flash_erase tmp/pkgfile/; \
	elif [ '$(LAYOUT)' = 'emmc' ]; then \
		cp $(CURDIR)/bin/rootfs/mke2fs tmp/pkgfile/; \
		if [ '$(LAYOUT_USE_EMMC_SWAP)' = 'emmc_swap_true' ] ; then \
			cp $(CURDIR)/bin/rootfs/mkswap tmp/pkgfile/; \
		fi; \
	elif [ '$(LAYOUT)' = 'sata' ]; then \
		cp $(CURDIR)/bin/rootfs/mke2fs tmp/pkgfile/; \
	elif [ '$(LAYOUT)' = 'spi' ]; then \
		cp $(CURDIR)/bin/rootfs/flash_erase tmp/pkgfile/; \
		cp $(CURDIR)/bin/rootfs/resize2fs tmp/pkgfile/; \
	fi
	if [ "`ls -A packages/$(TARGET)/installer`" != ".svn" ] && [ "`ls -A packages/$(TARGET)/installer`" != "" ]; then \
		cp -R packages/$(TARGET)/installer/* tmp/pkgfile/; \
	fi

	rm -f $(IMGFILE_AP_PATH);

	# untar rootfs
	mkdir -p tmp/rootfs 

	# copy android rootfs / rescue rootfs / rescue DT / linux DT to package
	if [ -f ./packages/$(TARGET)/bootfile.audio ] && [ $(install_avfile_count) -gt 0 ]; then \
		cp packages/$(TARGET)/bootfile.audio tmp/pkgfile/$(TARGET)/; \
	fi;
	if [ -f ./packages/$(TARGET)/bootfile.video ] && [ $(install_avfile_count) -gt 0 ]; then \
		cp packages/$(TARGET)/bootfile.video tmp/pkgfile/$(TARGET)/; \
	fi;
	if [ -f ./packages/$(TARGET)/bootfile.image ] && [ $(install_avfile_count) -gt 0 ]; then \
		cp packages/$(TARGET)/bootfile.image tmp/pkgfile/$(TARGET)/; \
	fi;

	# unzip AV firmwares
		cd tmp/ && cp ../packages/$(TARGET)/System.map.audio ./ && unzip -o ../packages/$(TARGET)/bluecore.audio.zip > /dev/null && cd ../; \

	# rootfs
	# Precedence of Android rootfs on NAS:
	#   package5/root, android.$(LAYOUT).tar.bz2, /mnt/android in NAS rootfs
	if [ '$(NAS_IMGS)' = 'y' ]; then \
		echo "extracting rootfs from root.$(LAYOUT).tar.bz2" && \
		cd tmp && \
		if [ '$(LAYOUT)' = 'nand' ] || [ '$(LAYOUT)' = 'emmc' ] || [ '$(LAYOUT)' = 'sata' ]; then \
			echo "Creating NAS $(LAYOUT) images ....." && \
			$(RUNCMD_PATH) $(layout_type) $(layout_size) $(TARGET) $(CUSTOMER_ID) $(LAYOUT_USE_EMMC_SWAP); \
			echo "Runnning runCmd.pl ....."; \
		elif [ '$(LAYOUT)' = 'spi' ]; then \
			echo "Creating NAS SPI + HDD images ....." && \
			fakeroot ../packages/$(TARGET)/mkrootfs.sh $(LAYOUT); \
		fi; \
		cd ../; \
	elif [ '$(ANDROID_IMGS)' = 'y' ]; then \
		echo "run rtssl to enc file" && \
		if [ -f $(RSA_LIB_PRIVATE_KEY) ]; then \
		openssl rsa -in $(RSA_LIB_PRIVATE_KEY) -pubout -out $(RSA_LIB_PUB_KEY); \
		fi; \
		for file in $(LIBFILE); do \
		if [ -f ./packages/$(TARGET)/$${file} ] && [ -f $(RSA_LIB_PRIVATE_KEY) ]; then \
		$(DO_SHA256_PATH) ./packages/$(TARGET)/$${file} ./packages/$(TARGET)/$${file}.padding ./packages/$(TARGET)/$${file}.sig; \
		$(RTSSL_PATH) enc -e -rsa -k $(RSA_LIB_PRIVATE_KEY) -i ./packages/$(TARGET)/$${file}.sig -o ./packages/$(TARGET)/$${file}.rsa; \
		$(LIB_ENC_PATH) ./packages/$(TARGET)/$${file} ./packages/$(TARGET)/$${file}.enc;  \
		if [ '$(SECURE_BOOT)' = y ]; then \
		$(DO_SHA256_PATH) ./packages/$(TARGET)/$${file}.enc ./packages/$(TARGET)/$${file}.otp.padding ./packages/$(TARGET)/$${file}.otp.sig; \
		$(RTSSL_PATH) enc -e -rsa -k $(RSA_LIB_PRIVATE_KEY) -i ./packages/$(TARGET)/$${file}.otp.sig -o ./packages/$(TARGET)/$${file}.otp.rsa; \
		#$(LIB_ENC_PATH) ./packages/$(TARGET)/$${file} ./packages/$(TARGET)/$${file}.enc;  \
		$(RTSSL_PATH) enc -e -aes_128_ecb -k `hexdump -e '8/1 "%02x"' $(SECURE_KEY_FILE)` -i ./packages/$(TARGET)/$${file}.enc -o ./packages/$(TARGET)/$${file}.otp.enc; \
		else \
			cp ./packages/$(TARGET)/$${file}.enc ./packages/$(TARGET)/$${file}.otp.enc; \
		fi; \
		rm ./packages/$(TARGET)/$${file}.padding ./packages/$(TARGET)/$${file}.sig ./packages/$(TARGET)/$${file}.enc ./packages/$(TARGET)/$${file} -f;sync;sync; \
		fi; \
		done ; \
		echo "run mkrootfs.sh to pack root image with padding" && \
		./packages/$(TARGET)/mkrootfs.sh $(LAYOUT); \
	fi

	if [ -f ./packages/$(TARGET)/android.root.$(LAYOUT).cpio.gz_pad.img ]; then \
		cp packages/$(TARGET)/android.root.$(LAYOUT).cpio.gz_pad.img tmp/; \
	fi;
	if [ -f ./packages/$(TARGET)/rescue.root.$(LAYOUT).cpio.gz_pad.img ]; then \
		cp packages/$(TARGET)/rescue.root.$(LAYOUT).cpio.gz_pad.img tmp/; \
	fi;
	# Use nand rescue rootfs for Pure NAS on eMMC
	if [ '$(NAS_IMGS)' = 'y' ] && [ '$(ANDROID_IMGS)' != 'y' ] && [ '$(LAYOUT)' = 'emmc' ]; then \
		cp packages/$(TARGET)/rescue.root.nand.cpio.gz_pad.img tmp/rescue.root.$(LAYOUT).cpio.gz_pad.img; \
	fi;

	# device tree [DT]
	if [ -f ./packages/$(TARGET)/android.$(LAYOUT).dtb ]; then \
		cp packages/$(TARGET)/android.$(LAYOUT).dtb tmp/pkgfile/$(TARGET)/; \
	fi;
	if [ -f ./packages/$(TARGET)/rescue.$(LAYOUT).dtb ]; then \
		cp packages/$(TARGET)/rescue.$(LAYOUT).dtb tmp/pkgfile/$(TARGET)/; \
	fi;

	# uImage, linux kernel
	if [ -f ./packages/$(TARGET)/$(LAYOUT).uImage ]; then \
		cp packages/$(TARGET)/$(LAYOUT).uImage tmp/; \
	fi;

	# uboot.bin, uboot 64.
	if [ -f ./packages/$(TARGET)/uboot.bin ]; then \
		cp packages/$(TARGET)/uboot.bin tmp/; \
	fi;

	# NAS redundant rescue copies
	if [ '$(nas_rescue)' = '1' ] && [ '$(NAS_IMGS)' = 'y' ]; then \
		if [ -f ./packages/$(TARGET)/nas.rescue.$(LAYOUT).dtb ]; then cp packages/$(TARGET)/nas.rescue.$(LAYOUT).dtb tmp/pkgfile/$(TARGET)/; fi; \
		if [ -f ./packages/$(TARGET)/nas.$(LAYOUT).uImage ]; then cp packages/$(TARGET)/nas.$(LAYOUT).uImage tmp/pkgfile/$(TARGET)/; fi; \
		if [ -f ./packages/$(TARGET)/nas.rescue.root.$(LAYOUT).cpio.gz_pad.img ]; then cp packages/$(TARGET)/nas.rescue.root.$(LAYOUT).cpio.gz_pad.img tmp/pkgfile/$(TARGET)/; fi; \
	fi;

	if [ '$(TEE_FW)' = 'y' ]; then \
		cp ./packages/$(TARGET)/tee.bin ./tmp; \
		cp ./packages/$(TARGET)/bl31.bin ./tmp; \
	fi

	if [ '$(HYPERVISOR)' = 'y' ] && [ '$(NAS_IMGS)' = 'y' ]; then \
		if [ -f ./packages/$(TARGET)/xen.img ]; then cp packages/$(TARGET)/xen.img tmp/pkgfile/$(TARGET)/; fi; \
	fi;

	#install factory
	if [ '$(install_factory)' = 1 ]; then \
		cd packages/$(TARGET)/factory$(FACTORY_SUFFIX) && tar cvf ../factory.tar *; \
		cd ../../../ && mv packages/$(TARGET)/factory.tar tmp/pkgfile/$(TARGET)/; \
	fi

	# install AP
	if [ '$(install_ap)' = 1 ]; then \
		cd packages/$(TARGET)/ap/bin && \
		date +"%Y%m%d_%H:%M" > release.date; \
	fi

	# re-compress kernel images, and pack rootfs, usr/local/etc image
	if [ '$(NAS_IMGS)' != 'y' ]; then \
	if [ '$(LAYOUT)' = 'spi' ]; then \
		echo "Now we don't support nor flash ....."; \
	elif [ '$(LAYOUT)' = 'nand' ]; then \
		cd tmp && \
		if [ '$(ANDROID_IMGS)' = 'y' ] || [ '$(LNX_IMGS)' = 'y' ]; then \
			cd $(TMP) && \
			echo "Creating ubifs image ....." && \
			$(RUNCMD_PATH) $(layout_type) $(layout_size) $(TARGET); \
			echo "Runnning runCmd.pl ....."; \
		fi; \
	elif [ '$(LAYOUT)' = 'emmc' ] || [ '$(LAYOUT)' = 'sata' ]; then \
        if [ '$(ANDROID_IMGS)' = 'y' ]; then \
			cd $(TMP) && \
			echo "Creating ext4 image (max system partition pretend is SYSTEM_MINSIZE) ....." ; \
			cd $(TMP) && \
			$(RUNCMD_PATH) $(layout_type) $(layout_size) $(TARGET) $(CUSTOMER_ID) $(LAYOUT_USE_EMMC_SWAP); \
			echo "Runnning runCmd.pl ....."; \
		elif [ '$(LNX_IMGS)' = 'y' ]; then \
			echo "copy ext4 rootfs image ....."; \
			cp $(CURDIR)/packages/$(TARGET)/$(EXT4FS_ROOTFSIMG) $(TMP)/.;sync; \
			cd $(TMP) && \
			echo "Creating ext4 image (max system partition pretend is SYSTEM_MINSIZE) ....." ; \
			cd $(TMP) && \
			$(RUNCMD_PATH) $(layout_type) $(layout_size) $(TARGET); \
			echo "Runnning runCmd.pl ....."; \
        fi ; \
        fi ; \
	fi
			
	###### Handle bootloader, bootfile.audio, and bootfile.video
	if [ -f packages/$(TARGET)/bootloader.tar ]; then cp packages/$(TARGET)/bootloader.tar tmp/pkgfile/$(TARGET)/; fi

	if [ -f $(CUSTOMER_FOLDER)/bootfile.video ] && [ -f $(CUSTOMER_FOLDER)/bootfile.image ]; then \
       echo "bootfile.image and bootfile.video cannot be co-existed !"; \
       exit 1; \
    fi
	if [ -f $(CUSTOMER_FOLDER)/bootfile.audio ] && [ $(install_avfile_count) -gt 0 ]; then \
            cp $(CUSTOMER_FOLDER)/bootfile.audio tmp/pkgfile/$(TARGET)/; \
    fi
	if [ -f $(CUSTOMER_FOLDER)/bootfile.video ] && [ $(install_avfile_count) -gt 0 ]; then \
            cp $(CUSTOMER_FOLDER)/bootfile.video tmp/pkgfile/$(TARGET)/; \
    fi
	if [ -f $(CUSTOMER_FOLDER)/bootfile.image ] && [ $(install_avfile_count) -gt 0 ]; then \
           cp $(CUSTOMER_FOLDER)/bootfile.image tmp/pkgfile/$(TARGET)/; \
    fi
	if [ '$(efuse_fw)' = 1 ]; then \
		if [ ! -f $(EFUSE_PROGRAMMER_FILE_PATH) ]; then \
                	echo "We cannot find efuse fw! (efuse_programmer.complete.enc)"; \
                	exit 1; \
            	fi; \
		if [ ! -f $(EFUSE_VERIFY_FILE_PATH) ]; then \
                	echo "We cannot find efuse fw! (efuse_verify.bin)"; \
                	exit 1; \
            	fi; \
        fi
	if [ '$(LAYOUT)' = 'spi' ]; then \
		if [ '$(NAS_IMGS)' = 'y' ]; then \
			cd tmp && \
			$(GZIP_PATH) -9 $(LAYOUT).uImage && mv $(LAYOUT).uImage.gz pkgfile/$(TARGET)/$(LAYOUT).uImage; \
			$(LZMA_PATH) e bluecore.audio pkgfile/$(TARGET)/bluecore.audio$(F_EXT); \
			$(LZMA_PATH) e rescue.root.$(LAYOUT).cpio.gz_pad.img pkgfile/$(TARGET)/rescue.root.$(LAYOUT).cpio.gz_pad.img$(F_EXT); \
			cd -; \
		fi; \
		if [ '$(PKG_TYPE)' = 'TAR' ]; then \
			cd tmp/pkgfile/ && tar cvf $(IMGFILE_PATH) *; \
		fi; \
	elif [ '$(LAYOUT)' = 'nand' ]; then \
		cd tmp && \
		if [ '$(SECURE_BOOT)' = y ]; then \
			if [ ! -f $(SECURE_KEY_FILE) ] || [ ! -f $(SECURE_KEY1_FILE) ] || [ ! -f $(SECURE_KEY2_FILE) ] || [ ! -f $(SECURE_KEY3_FILE) ] || [ ! -f $(RSA_FW_PRIVATE_KEY) ] || [ ! -f $(RSA_TEE_PRIVATE_KEY) ]; then \
                echo "We cannot find the keys!"; \
                exit 1; \
            fi; \
			$(DO_SHA256_PATH) $(LAYOUT).uImage$(F_EXT) $(LAYOUT).uImage$(F_EXT)_padding.bin $(LAYOUT).uImage$(F_EXT)_signature.bin; \
			cat $(LAYOUT).uImage$(F_EXT) $(LAYOUT).uImage$(F_EXT)_padding.bin > $(LAYOUT).uImage$(F_EXT)_padding_final.bin; \
			$(call hwrsa-sign, $(RSA_FW_PRIVATE_KEY), $(LAYOUT).uImage$(F_EXT)_signature.bin, $(LAYOUT).uImage$(F_EXT)_signature_enc.bin);\
			$(OBFUSE_PATH) $(RTSSL_PATH) 92 $(SECURE_KEY1_FILE) $(SECURE_KEY_FILE) $(SECURE_KEY2_FILE) $(SECURE_KEY3_FILE) $(LAYOUT).uImage$(F_EXT)_padding_final.bin $(LAYOUT).uImage$(F_EXT)_aes_tmp.bin; \
			cat $(LAYOUT).uImage$(F_EXT)_aes_tmp.bin $(LAYOUT).uImage$(F_EXT)_signature_enc.bin > $(LAYOUT).uImage$(F_EXT).aes; \
			$(DO_SHA256_PATH) bluecore.audio bluecore.audio_padding.bin bluecore.audio_signature.bin; \
			cat bluecore.audio bluecore.audio_padding.bin > bluecore.audio_padding_final.bin; \
			$(call hwrsa-sign, $(RSA_FW_PRIVATE_KEY), bluecore.audio_signature.bin, bluecore.audio_signature_enc.bin);\
			$(OBFUSE_PATH) $(RTSSL_PATH) 92 $(SECURE_KEY1_FILE) $(SECURE_KEY_FILE) $(SECURE_KEY2_FILE) $(SECURE_KEY3_FILE) bluecore.audio_padding_final.bin bluecore.audio_aes_tmp.bin; \
			cat bluecore.audio_aes_tmp.bin bluecore.audio_signature_enc.bin > bluecore.audio.aes; \
			if [ '$(HYPERVISOR)' = 'y' ] && [ '$(NAS_IMGS)' = 'y' ]; then \
			$(DO_SHA256_PATH) xen.img xen.img_padding.bin xen.img_signature.bin; \
			cat xen.img xen.img_padding.bin > xen.img_padding_final.bin; \
			$(call hwrsa-sign, $(RSA_FW_PRIVATE_KEY), xen.img_signature.bin, xen.img_signature_enc.bin);\
			$(OBFUSE_PATH) $(RTSSL_PATH) 92 $(SECURE_KEY1_FILE) $(SECURE_KEY_FILE) $(SECURE_KEY2_FILE) $(SECURE_KEY3_FILE) xen.img_padding_final.bin xen.img_aes_tmp.bin; \
			cat xen.img_aes_tmp.bin xen.img_signature_enc.bin > xen.img.aes; \
			cp xen.img.aes pkgfile/$(TARGET); \
			fi; \
		if [ '$(NAS_IMGS)' != 'y' ] && [ '$(ANDROID_IMGS)' = 'y' ]; then \
			$(DO_SHA256_PATH) android.root.$(LAYOUT).cpio.gz_pad.img android.root.$(LAYOUT).cpio.gz_pad.img_padding.bin android.root.$(LAYOUT).cpio.gz_pad.img_signature.bin; \
			cat android.root.$(LAYOUT).cpio.gz_pad.img android.root.$(LAYOUT).cpio.gz_pad.img_padding.bin > android.root.$(LAYOUT).cpio.gz_pad.img_padding_final.bin; \
			$(call hwrsa-sign, $(RSA_FW_PRIVATE_KEY), android.root.$(LAYOUT).cpio.gz_pad.img_signature.bin, android.root.$(LAYOUT).cpio.gz_pad.img_signature_enc.bin);\
			$(OBFUSE_PATH) $(RTSSL_PATH) 92 $(SECURE_KEY1_FILE) $(SECURE_KEY_FILE) $(SECURE_KEY2_FILE) $(SECURE_KEY3_FILE) android.root.$(LAYOUT).cpio.gz_pad.img_padding_final.bin android.root.$(LAYOUT).cpio.gz_pad.img_aes_tmp.bin; \
			cat android.root.$(LAYOUT).cpio.gz_pad.img_aes_tmp.bin android.root.$(LAYOUT).cpio.gz_pad.img_signature_enc.bin > android.root.$(LAYOUT).cpio.gz_pad.img.aes; \
		fi; \
			$(DO_SHA256_PATH) rescue.root.$(LAYOUT).cpio.gz_pad.img rescue.root.$(LAYOUT).cpio.gz_pad.img_padding.bin rescue.root.$(LAYOUT).cpio.gz_pad.img_signature.bin; \
			cat rescue.root.$(LAYOUT).cpio.gz_pad.img rescue.root.$(LAYOUT).cpio.gz_pad.img_padding.bin > rescue.root.$(LAYOUT).cpio.gz_pad.img_padding_final.bin; \
			$(call hwrsa-sign, $(RSA_FW_PRIVATE_KEY), rescue.root.$(LAYOUT).cpio.gz_pad.img_signature.bin, rescue.root.$(LAYOUT).cpio.gz_pad.img_signature_enc.bin);\
			$(OBFUSE_PATH) $(RTSSL_PATH) 92 $(SECURE_KEY1_FILE) $(SECURE_KEY_FILE) $(SECURE_KEY2_FILE) $(SECURE_KEY3_FILE) rescue.root.$(LAYOUT).cpio.gz_pad.img_padding_final.bin rescue.root.$(LAYOUT).cpio.gz_pad.img_aes_tmp.bin; \
			cat rescue.root.$(LAYOUT).cpio.gz_pad.img_aes_tmp.bin rescue.root.$(LAYOUT).cpio.gz_pad.img_signature_enc.bin > rescue.root.$(LAYOUT).cpio.gz_pad.img.aes;\
			if [ '$(TEE_FW)' = 'y' ]; then \
			$(DO_SHA256_PATH) tee.bin tee_padding.bin tee_signature.bin; \
			cat tee.bin tee_padding.bin > tee_padding_final.bin; \
			$(call hwrsa-sign, $(RSA_TEE_PRIVATE_KEY), tee_signature.bin, tee_signature_enc.bin);\
			$(OBFUSE_PATH) $(RTSSL_PATH) 93 $(SECURE_KEY1_FILE) $(SECURE_KEY_FILE) $(SECURE_KEY2_FILE) $(SECURE_KEY3_FILE) tee_padding_final.bin tee_aes_tmp.bin; \
			cat tee_aes_tmp.bin tee_signature_enc.bin > tee.bin.aes;\
			$(DO_SHA256_PATH) bl31.bin bl31_padding.bin bl31_signature.bin; \
			cat bl31.bin bl31_padding.bin > bl31_padding_final.bin; \
			$(call hwrsa-sign, $(RSA_TEE_PRIVATE_KEY), bl31_signature.bin, bl31_signature_enc.bin);\
			$(OBFUSE_PATH) $(RTSSL_PATH) 93 $(SECURE_KEY1_FILE) $(SECURE_KEY_FILE) $(SECURE_KEY2_FILE) $(SECURE_KEY3_FILE) bl31_padding_final.bin bl31_aes_tmp.bin; \
			cat bl31_aes_tmp.bin bl31_signature_enc.bin > bl31.bin.aes;\
			fi; \
			touch otp_key_verify && \
			echo "Welcome to RTK!" > otp_key_verify; \
			$(OBFUSE_PATH) $(RTSSL_PATH) 97 $(SECURE_KEY_SEED_FILE) $(SECURE_KEY_FILE) otp_key_verify otp_key_verify.aes; \
			cp otp_key_verify.aes pkgfile/; \
		fi; \
		if [ '$(NAS_IMGS)' != 'y' ] && [ '$(SQUASHFS_ROOT)' = 'y' ]; then \
			cp `if [ '$(SECURE_BOOT)' = y ]; then echo $(LAYOUT).uImage$(F_EXT).aes bluecore.audio.aes android.root.$(LAYOUT).cpio.gz_pad.img.aes rescue.root.$(LAYOUT).cpio.gz_pad.img.aes; else echo $(LAYOUT).uImage$(F_EXT) bluecore.audio android.root.$(LAYOUT).cpio.gz_pad.img rescue.root.$(LAYOUT).cpio.gz_pad.img; fi` pkgfile/$(TARGET); \
		else \
			mv -f $(TMP)/$(ETC_IMG_NAME) $(TMP)/pkgfile/$(TARGET)/; \
			cp `if [ '$(SECURE_BOOT)' = y ]; then echo $(LAYOUT).uImage$(F_EXT).aes bluecore.audio.aes android.root.$(LAYOUT).cpio.gz_pad.img.aes rescue.root.$(LAYOUT).cpio.gz_pad.img.aes; else echo $(LAYOUT).uImage$(F_EXT) bluecore.audio android.root.$(LAYOUT).cpio.gz_pad.img rescue.root.$(LAYOUT).cpio.gz_pad.img; fi` pkgfile/$(TARGET); \
			if [ '$(TEE_FW)' = 'y' ]; then \
			if [ '$(SECURE_BOOT)' = 'y' ]; then \
				cp tee.bin.aes pkgfile/$(TARGET); \
				cp bl31.bin.aes pkgfile/$(TARGET); \
			else \
				cp tee.bin pkgfile/$(TARGET); \
				cp bl31.bin pkgfile/$(TARGET); \
			fi; \
			fi; \
		fi; \
		if [ '$(SECURE_BOOT)' == y ] && [ '$(efuse_fw)' == '1' ]; then \
			if [ ! -f $(EFUSE_VERIFY_FILE_PATH) ] || [ ! -f $(EFUSE_PROGRAMMER_FILE_PATH) ] || [ ! -f $(RSA_FW_PRIVATE_KEY) ] || [ ! -f $(RSA_TEE_PRIVATE_KEY) ]; then \
                echo "We cannot find the efuse utilities"; \
                exit 1; \
            fi; \
            cp $(EFUSE_VERIFY) $(TMP)/pkgfile/ && \
            cp $(EFUSE_PROGRAMMER) $(TMP)/pkgfile/ && \
            $(STR2BIN_PATH) `$(RTSSL_PATH) rsa -print -modulous -i $(RSA_FW_PRIVATE_KEY)` > rsa_key_pub.fw.bin && \
            $(REVERSE_PL) rsa_key_pub.fw.bin > rsa_key_pub.fw.bin.rev && \
            mv rsa_key_pub.fw.bin.rev $(TMP)/pkgfile/ && \
            $(STR2BIN_PATH) `$(RTSSL_PATH) rsa -print -modulous -i $(RSA_TEE_PRIVATE_KEY)` > rsa_key_pub.tee.bin && \
		$(REVERSE_PL) rsa_key_pub.tee.bin > rsa_key_pub.tee.bin.rev; \
            mv rsa_key_pub.tee.bin.rev $(TMP)/pkgfile/; \
		elif [ '$(SECURE_BOOT)' == n ] && [ '$(efuse_fw)' == '1' ]; then \
			if [ ! -f $(EFUSE_VERIFY_FILE_PATH) ] || [ ! -f $(EFUSE_PROGRAMMER_FILE_PATH) ]; then \
                echo "We cannot find the efuse utilities!"; \
                exit 1; \
            fi; \
            cp $(EFUSE_VERIFY_FILE_PATH) $(TMP)/pkgfile/ && \
            cp $(EFUSE_PROGRAMMER_FILE_PATH) $(TMP)/pkgfile/; \
		else \
			echo "No need to copy key."; \
		fi; \
		if [ '$(PKG_TYPE)' = 'TAR' ]; then \
			cd pkgfile/ && tar cvf $(IMGFILE_PATH) *; \
		fi; \
	elif [ '$(LAYOUT)' = 'emmc' ] || [ '$(LAYOUT)' = 'sata' ]; then \
		cd tmp && \
		if [ '$(SECURE_BOOT)' = y ]; then \
			if [ ! -f $(SECURE_KEY_FILE) ] || [ ! -f $(SECURE_KEY1_FILE) ] || [ ! -f $(SECURE_KEY2_FILE) ] || [ ! -f $(SECURE_KEY3_FILE) ] || [ ! -f $(RSA_FW_PRIVATE_KEY) ] || [ ! -f $(RSA_TEE_PRIVATE_KEY) ]; then \
                echo "We cannot find the keys!"; \
                exit 1; \
            fi; \
			$(DO_SHA256_PATH) $(LAYOUT).uImage$(F_EXT) $(LAYOUT).uImage$(F_EXT)_padding.bin $(LAYOUT).uImage$(F_EXT)_signature.bin; \
			cat $(LAYOUT).uImage$(F_EXT) $(LAYOUT).uImage$(F_EXT)_padding.bin > $(LAYOUT).uImage$(F_EXT)_padding_final.bin; \
			$(call hwrsa-sign, $(RSA_FW_PRIVATE_KEY), $(LAYOUT).uImage$(F_EXT)_signature.bin, $(LAYOUT).uImage$(F_EXT)_signature_enc.bin);\
			$(OBFUSE_PATH) $(RTSSL_PATH) 92 $(SECURE_KEY1_FILE) $(SECURE_KEY_FILE) $(SECURE_KEY2_FILE) $(SECURE_KEY3_FILE) $(LAYOUT).uImage$(F_EXT)_padding_final.bin $(LAYOUT).uImage$(F_EXT)_aes_tmp.bin; \
			cat $(LAYOUT).uImage$(F_EXT)_aes_tmp.bin $(LAYOUT).uImage$(F_EXT)_signature_enc.bin > $(LAYOUT).uImage$(F_EXT).aes; \
			$(DO_SHA256_PATH) bluecore.audio bluecore.audio_padding.bin bluecore.audio_signature.bin; \
			cat bluecore.audio bluecore.audio_padding.bin > bluecore.audio_padding_final.bin; \
			$(call hwrsa-sign, $(RSA_FW_PRIVATE_KEY), bluecore.audio_signature.bin, bluecore.audio_signature_enc.bin);\
			$(OBFUSE_PATH) $(RTSSL_PATH) 92 $(SECURE_KEY1_FILE) $(SECURE_KEY_FILE) $(SECURE_KEY2_FILE) $(SECURE_KEY3_FILE) bluecore.audio_padding_final.bin bluecore.audio_aes_tmp.bin; \
			cat bluecore.audio_aes_tmp.bin bluecore.audio_signature_enc.bin > bluecore.audio.aes; \
			$(DO_SHA256_PATH) bluecore.audio bluecore.audio_padding.bin bluecore.audio_signature.bin; \
			cat bluecore.audio bluecore.audio_padding.bin > bluecore.audio_padding_final.bin; \
			$(call hwrsa-sign, $(RSA_FW_PRIVATE_KEY), bluecore.audio_signature.bin, bluecore.audio_signature_enc.bin);\
			$(OBFUSE_PATH) $(RTSSL_PATH) 92 $(SECURE_KEY1_FILE) $(SECURE_KEY_FILE) $(SECURE_KEY2_FILE) $(SECURE_KEY3_FILE) bluecore.audio_padding_final.bin bluecore.audio_aes_tmp.bin; \
			cat bluecore.audio_aes_tmp.bin bluecore.audio_signature_enc.bin > bluecore.audio.aes; \
			if [ '$(HYPERVISOR)' = 'y' ] && [ '$(NAS_IMGS)' = 'y' ]; then \
			$(DO_SHA256_PATH) xen.img xen.img_padding.bin xen.img_signature.bin; \
			cat xen.img xen.img_padding.bin > xen.img_padding_final.bin; \
			$(call hwrsa-sign, $(RSA_FW_PRIVATE_KEY), xen.img_signature.bin, xen.img_signature_enc.bin);\
			$(OBFUSE_PATH) $(RTSSL_PATH) 92 $(SECURE_KEY1_FILE) $(SECURE_KEY_FILE) $(SECURE_KEY2_FILE) $(SECURE_KEY3_FILE) xen.img_padding_final.bin xen.img_aes_tmp.bin; \
			cat xen.img_aes_tmp.bin xen.img_signature_enc.bin > xen.img.aes; \
			cp xen.img.aes pkgfile/$(TARGET); \
			fi; \
		if [ '$(NAS_IMGS)' != 'y' ] && [ '$(ANDROID_IMGS)' = 'y' ]; then \
			$(DO_SHA256_PATH) android.root.$(LAYOUT).cpio.gz_pad.img android.root.$(LAYOUT).cpio.gz_pad.img_padding.bin android.root.$(LAYOUT).cpio.gz_pad.img_signature.bin; \
			cat android.root.$(LAYOUT).cpio.gz_pad.img android.root.$(LAYOUT).cpio.gz_pad.img_padding.bin > android.root.$(LAYOUT).cpio.gz_pad.img_padding_final.bin; \
			$(call hwrsa-sign, $(RSA_FW_PRIVATE_KEY), android.root.$(LAYOUT).cpio.gz_pad.img_signature.bin, android.root.$(LAYOUT).cpio.gz_pad.img_signature_enc.bin);\
			$(OBFUSE_PATH) $(RTSSL_PATH) 92 $(SECURE_KEY1_FILE) $(SECURE_KEY_FILE) $(SECURE_KEY2_FILE) $(SECURE_KEY3_FILE) android.root.$(LAYOUT).cpio.gz_pad.img_padding_final.bin android.root.$(LAYOUT).cpio.gz_pad.img_aes_tmp.bin; \
			cat android.root.$(LAYOUT).cpio.gz_pad.img_aes_tmp.bin android.root.$(LAYOUT).cpio.gz_pad.img_signature_enc.bin > android.root.$(LAYOUT).cpio.gz_pad.img.aes; \
		fi; \
			$(DO_SHA256_PATH) rescue.root.$(LAYOUT).cpio.gz_pad.img rescue.root.$(LAYOUT).cpio.gz_pad.img_padding.bin rescue.root.$(LAYOUT).cpio.gz_pad.img_signature.bin; \
			cat rescue.root.$(LAYOUT).cpio.gz_pad.img rescue.root.$(LAYOUT).cpio.gz_pad.img_padding.bin > rescue.root.$(LAYOUT).cpio.gz_pad.img_padding_final.bin; \
			$(call hwrsa-sign, $(RSA_FW_PRIVATE_KEY), rescue.root.$(LAYOUT).cpio.gz_pad.img_signature.bin, rescue.root.$(LAYOUT).cpio.gz_pad.img_signature_enc.bin);\
			$(OBFUSE_PATH) $(RTSSL_PATH) 92 $(SECURE_KEY1_FILE) $(SECURE_KEY_FILE) $(SECURE_KEY2_FILE) $(SECURE_KEY3_FILE) rescue.root.$(LAYOUT).cpio.gz_pad.img_padding_final.bin rescue.root.$(LAYOUT).cpio.gz_pad.img_aes_tmp.bin; \
			cat rescue.root.$(LAYOUT).cpio.gz_pad.img_aes_tmp.bin rescue.root.$(LAYOUT).cpio.gz_pad.img_signature_enc.bin > rescue.root.$(LAYOUT).cpio.gz_pad.img.aes;\
			if [ '$(TEE_FW)' = 'y' ]; then \
			$(DO_SHA256_PATH) tee.bin tee_padding.bin tee_signature.bin; \
			cat tee.bin tee_padding.bin > tee_padding_final.bin; \
			$(call hwrsa-sign, $(RSA_TEE_PRIVATE_KEY), tee_signature.bin, tee_signature_enc.bin);\
			$(OBFUSE_PATH) $(RTSSL_PATH) 93 $(SECURE_KEY1_FILE) $(SECURE_KEY_FILE) $(SECURE_KEY2_FILE) $(SECURE_KEY3_FILE) tee_padding_final.bin tee_aes_tmp.bin; \
			cat tee_aes_tmp.bin tee_signature_enc.bin > tee.bin.aes;\
			cp tee.bin.aes pkgfile/$(TARGET); \
			$(DO_SHA256_PATH) bl31.bin bl31_padding.bin bl31_signature.bin; \
			cat bl31.bin bl31_padding.bin > bl31_padding_final.bin; \
			$(call hwrsa-sign, $(RSA_TEE_PRIVATE_KEY), bl31_signature.bin, bl31_signature_enc.bin);\
			$(OBFUSE_PATH) $(RTSSL_PATH) 93 $(SECURE_KEY1_FILE) $(SECURE_KEY_FILE) $(SECURE_KEY2_FILE) $(SECURE_KEY3_FILE) bl31_padding_final.bin bl31_aes_tmp.bin; \
			cat bl31_aes_tmp.bin bl31_signature_enc.bin > bl31.bin.aes;\
			cp bl31.bin.aes pkgfile/$(TARGET); \
			fi; \
			cp $(LAYOUT).uImage$(F_EXT).aes bluecore.audio.aes android.root.$(LAYOUT).cpio.gz_pad.img.aes rescue.root.$(LAYOUT).cpio.gz_pad.img.aes pkgfile/$(TARGET); \
			cp pkgfile/$(TARGET)/$(LAYOUT).uImage$(F_EXT).aes pkgfile/$(TARGET)/gold.$(LAYOUT).uImage$(F_EXT).aes; \
			cp pkgfile/$(TARGET)/bluecore.audio$(F_EXT).aes pkgfile/$(TARGET)/gold.bluecore.audio$(F_EXT).aes; \
			cp pkgfile/$(TARGET)/rescue.$(LAYOUT).dtb pkgfile/$(TARGET)/gold.rescue.$(LAYOUT).dtb; \
			cp pkgfile/$(TARGET)/rescue.root.$(LAYOUT).cpio.gz_pad.img.aes pkgfile/$(TARGET)/gold.rescue.root.$(LAYOUT).cpio.gz_pad.img.aes; \
			touch otp_key_verify && \
			echo "Welcome to RTK!" > otp_key_verify; \
			$(OBFUSE_PATH) $(RTSSL_PATH) 97 $(SECURE_KEY_SEED_FILE) $(SECURE_KEY_FILE) otp_key_verify otp_key_verify.aes; \
			cp otp_key_verify.aes pkgfile/; \
			cp uboot.bin pkgfile/$(TARGET); \
		else \
			if [ '$(TEE_FW)' = 'y' ]; then \
			cp tee.bin pkgfile/$(TARGET); \
			cp bl31.bin pkgfile/$(TARGET); \
			fi; \
			cp $(LAYOUT).uImage$(F_EXT) bluecore.audio$(F_EXT) android.root.$(LAYOUT).cpio.gz_pad.img rescue.root.$(LAYOUT).cpio.gz_pad.img pkgfile/$(TARGET); \
			cp pkgfile/$(TARGET)/$(LAYOUT).uImage$(F_EXT) pkgfile/$(TARGET)/gold.$(LAYOUT).uImage$(F_EXT); \
			cp pkgfile/$(TARGET)/bluecore.audio$(F_EXT) pkgfile/$(TARGET)/gold.bluecore.audio$(F_EXT); \
			cp pkgfile/$(TARGET)/rescue.$(LAYOUT).dtb pkgfile/$(TARGET)/gold.rescue.$(LAYOUT).dtb; \
			cp pkgfile/$(TARGET)/rescue.root.$(LAYOUT).cpio.gz_pad.img pkgfile/$(TARGET)/gold.rescue.root.$(LAYOUT).cpio.gz_pad.img; \
			cp uboot.bin pkgfile/$(TARGET); \
		fi; \
		if [ '$(SECURE_BOOT)' == y ]; then \
			echo "Secure boot is not support fastboot mode."; \
		else \
			dd if=/dev/zero of=kernel-20m bs=1M count=20; \
			dd if=pkgfile/$(TARGET)/$(LAYOUT).uImage of=kernel-20m conv=notrunc; \
			cat kernel-20m  pkgfile/$(TARGET)/rescue.$(LAYOUT).dtb > kernel-dtb; \
			mkbootimg --kernel kernel-dtb --ramdisk pkgfile/$(TARGET)/rescue.root.$(LAYOUT).cpio.gz_pad.img --second pkgfile/$(TARGET)/bluecore.audio -o pkgfile/$(TARGET)/rescue.boot.img; \
			rm kernel-20m; rm kernel-dtb; \
		fi; \
		if [ '$(SECURE_BOOT)' == y ] && [ '$(efuse_fw)' == '1' ]; then \
			if [ ! -f $(EFUSE_VERIFY_FILE_PATH) ] || [ ! -f $(EFUSE_PROGRAMMER_FILE_PATH) ]; then \
                echo "We cannot find the efuse utilities!"; \
                exit 1; \
            fi; \
                cp $(EFUSE_VERIFY) $(TMP)/pkgfile/ && \
                cp $(EFUSE_PROGRAMMER) $(TMP)/pkgfile/ && \
                $(STR2BIN_PATH) `$(RTSSL_PATH) rsa -print -modulous -i $(RSA_FW_PRIVATE_KEY)` > rsa_key_pub.fw.bin && \
                $(REVERSE_PL) rsa_key_pub.fw.bin > rsa_key_pub.fw.bin.rev && \
                mv rsa_key_pub.fw.bin.rev $(TMP)/pkgfile/ && \
                $(STR2BIN_PATH) `$(RTSSL_PATH) rsa -print -modulous -i $(RSA_TEE_PRIVATE_KEY)` > rsa_key_pub.tee.bin && \
			$(REVERSE_PL) rsa_key_pub.tee.bin > rsa_key_pub.tee.bin.rev; \
                mv rsa_key_pub.tee.bin.rev $(TMP)/pkgfile/; \
		elif [ '$(SECURE_BOOT)' == n ] && [ '$(efuse_fw)' == '1' ]; then \
			if [ ! -f $(EFUSE_VERIFY_FILE_PATH) ] || [ ! -f $(EFUSE_PROGRAMMER_FILE_PATH) ]; then \
                echo "We cannot find the efuse utilities!"; \
                exit 1; \
            fi; \
            touch enc_kn.bin && \
            $(STR2BIN_PATH) `hexdump -e '1/4 "%08x"' $(SECURE_KEY_FILE)` > $(SECURE_KEY_FILE_REV) && \
            $(RTSSL_PATH) enc -e -aes_128_ecb -k 3da02388c8a1535f798d60f2d0c8a8ab -i $(SECURE_KEY_FILE_REV) -o enc_kn.bin && \
            mv enc_kn.bin $(TMP)/pkgfile/ && \
            $(STR2BIN_PATH) `$(RTSSL_PATH) rsa -print -modulous -i $(RSA_PRIVATE_KEY)` > rsa_key_pub.bin && \
                       $(SHA_REVERSE_PATH) rsa_key_pub.bin; \
            mv rsa_key_pub.bin.rev $(TMP)/pkgfile/; \
		fi && \
		if [ '$(ANDROID_IMGS)' = 'y' ]; then \
	        echo "Start copying ext4fs images ....."; \
		elif [ '$(LNX_IMGS)' = 'y' ]; then \
	        echo "Start copying ext4fs images ....."; \
			if [ '$(ROOTFS_TYPE)' = 'ext4fs' ]; then \
				cp $(TMP)/$(EXT4FS_ROOTFSIMG) $(TMP)/pkgfile/$(TARGET)/; \
			fi; \
		fi; \
		if [ '$(PKG_TYPE)' = 'TAR' ]; then \
            cd pkgfile/ && tar cvf $(IMGFILE_PATH) *; \
        fi; \
	fi
	if [ '$(NAS_IMGS)' = 'y' ]; then \
		echo Original image size is `ls -lG $(IMGFILE_PATH) | awk -F' ' '{print $$4}'` && \
		if [ '$(hash_imgfile)' = 1 ]; then \
			if [ '$(SECURE_BOOT)' = y ]; then \
				dd if=/dev/zero  bs=1 count=`expr \( 4096000000 - \`ls -lG $(IMGFILE_PATH) | awk -F' ' '{print $$4}'\` - 128 \) % 4096` >> $(IMGFILE_PATH); \
				$(RTSSL_PATH) dgst -mars_sha1 -a -i $(IMGFILE_PATH) -sign -rsa -k $(RSA_KEY_FILE); \
			else \
				dd if=/dev/zero  bs=1 count=`expr \( 4096000000 - \`ls -lG $(IMGFILE_PATH) | awk -F' ' '{print $$4}'\` - 20 \) % 4096` >> $(IMGFILE_PATH); \
				$(RTSSL_PATH) dgst -mars_sha1 -a -i $(IMGFILE_PATH) -sign -aes_128_ecb -k 0; \
			fi; \
		else \
			dd if=/dev/zero  bs=1 count=`expr \( 4096000000 - \`ls -lG $(IMGFILE_PATH) | awk -F' ' '{print $$4}'\` \) % 4096` >> $(IMGFILE_PATH); \
		fi; \
	fi
	if [ '$(offline_gen)' = 'y' ]; then \
          echo "offline gen. start....."; \
          rm -rf installer_x86/tmp; sync; \
          mkdir installer_x86/tmp; \
          chmod 775 installer_x86/tmp; \
	if [ '$(LAYOUT)' = 'emmc' ]; then \
          touch installer_x86/tmp/mmcblk0; \
          chmod 777 installer_x86/tmp/mmcblk0; \
	elif [ '$(LAYOUT)' = 'sata' ]; then \
          touch installer_x86/tmp/sataa0; \
          chmod 777 installer_x86/tmp/sataa0; \
	else \
          touch installer_x86/tmp/mtdblock0 installer_x86/tmp/mtd0; \
          chmod 777 installer_x86/tmp/mtdblock0 installer_x86/tmp/mtd0; \
	fi; \
          cat installer_x86/setting.txt|grep CONFIG_FLASH_PARTNAME >> tmp/pkgfile/config.txt; \
	if [ '$(NAS_IMGS)' = 'y' ]; then \
          cd installer_x86;./install_a.pc.nas ../../install.img $(FLASH_SIZE) $(PACKAGES) > /dev/null; \
	elif [ '$(PURE_LINUX_IMGS)' = 'y' ]; then \
          cd installer_x86;./install_a.pc.linux ../../install.img $(FLASH_SIZE) $(PACKAGES) > /dev/null; \
	else \
          cd installer_x86;./install_a.pc ../../install.img $(FLASH_SIZE) $(PACKAGES) > /dev/null; \
	fi; \
          rm -rf $(PACKAGES);mkdir $(PACKAGES); \
		if [ '$(LAYOUT)' = 'nand' ]; then \
                  cd $(PACKAGES); \
		  if [ -f $(EFUSE_PROGRAMMER_FILE_PATH) ]; then \
                    cp $(EFUSE_PROGRAMMER_FILE_PATH) .; \
		  fi; \
		  if [ -f $(EFUSE_VERIFY_FILE_PATH) ]; then \
                    cp $(EFUSE_VERIFY_FILE_PATH) .; \
		  fi; \
                  ../../bin/nf_profiler `cat $(CURDIR)/installer_x86/setting.txt|grep CONFIG_FLASH_PARTNAME | awk -F' ' '{print $$2}' | sed 's/\"//'`; \
                  cp ../tmp/fw_tbl.bin .;cd ..;cp tmp/factory/layout.txt .; \
                  echo "#define FW_PROFILE \" target=0 offset=0 size=`ls -lG $(CURDIR)/installer_x86/$(PACKAGES)/nf_profile.bin | awk -F' ' '{printf("%x",$$4)}'` type=bin name=$(PACKAGES)/nf_profile.bin nf_id=`cat $(CURDIR)/installer_x86/setting.txt|grep CONFIG_FLASH_PARTNAME | awk -F' ' '{print $$2}' | sed 's/\"//'` \"" >> layout.txt; \
		  if [ '$(efuse_fw)' = 1 ]; then \
                        echo "We find efuse fw! (efuse_verify.bin)"; \
                        echo "#define FW_EFUSE_VERIFY \" target=0x01610000 offset=0 size=`ls -lG $(CURDIR)/installer_x86/$(PACKAGES)/$(EFUSE_VERIFY_FILE) | awk -F' ' '{printf("%x",     $$4)}'` type=bin name=$(PACKAGES)/$(EFUSE_VERIFY_FILE) \"" >> layout.txt; \
                        echo "We find efuse fw! (efuse_programmer.complete.enc)"; \
                        echo "#define FW_EFUSE_PROGRAMMER \" target=0x01700000 offset=0 size=`ls -lG $(CURDIR)/installer_x86/$(PACKAGES)/$(EFUSE_PROGRAMMER_FILE) | awk -F' ' '{printf("%x",     $$4)}'` type=bin name=$(PACKAGES)/$(EFUSE_PROGRAMMER_FILE) \"" >> layout.txt; \
                        tar rvf $(IMGFILE_PATH) layout.txt $(PACKAGES)/fw_tbl.bin $(PACKAGES)/nf_profile.bin $(PACKAGES)/$(EFUSE_VERIFY_FILE) $(PACKAGES)/$(EFUSE_PROGRAMMER_FILE); \
		  else   \
                        tar rvf $(IMGFILE_PATH) layout.txt $(PACKAGES)/fw_tbl.bin $(PACKAGES)/nf_profile.bin; \
		  fi; \
		fi; \
		if [ '$(LAYOUT)' = 'emmc' ] || [ '$(LAYOUT)' = 'sata' ]; then \
                  rm -f layout.txt;cp tmp/factory/layout.txt .; \
                  cp tmp/fw_tbl.bin ./$(PACKAGES); \
                  cp tmp/mbr*.bin ./$(PACKAGES); \
		  if [ '$(efuse_fw)' = 1 ]; then \
                        cp $(EFUSE_VERIFY_FILE_PATH) ./$(PACKAGES); \
                        echo "We find efuse fw! (efuse_verify.bin)"; \
                        echo "#define FW_EFUSE_VERIFY \" target=0x01610000 offset=0 size=`ls -lG $(CURDIR)/installer_x86/$(PACKAGES)/$(EFUSE_VERIFY_FILE) | awk -F' ' '{printf("%x",     $$4)}'` type=bin name=$(PACKAGES)/$(EFUSE_VERIFY_FILE) \"" >> layout.txt; \
                        cp $(EFUSE_PROGRAMMER_FILE_PATH) ./$(PACKAGES); \
                        echo "We find efuse fw! (efuse_programmer.complete.enc)"; \
                        echo "#define FW_EFUSE_PROGRAMMER \" target=0x01700000 offset=0 size=`ls -lG $(CURDIR)/installer_x86/$(PACKAGES)/$(EFUSE_PROGRAMMER_FILE) | awk -F' ' '{printf("%x",     $$4)}'` type=bin name=$(PACKAGES)/$(EFUSE_PROGRAMMER_FILE) \"" >> layout.txt; \
                        tar rvf $(IMGFILE_PATH) layout.txt $(PACKAGES)/fw_tbl.bin $(PACKAGES)/mbr*.bin $(PACKAGES)/$(EFUSE_VERIFY_FILE) $(PACKAGES)/$(EFUSE_PROGRAMMER_FILE); \
		  else   \
                        tar rvf $(IMGFILE_PATH) layout.txt $(PACKAGES)/fw_tbl.bin $(PACKAGES)/mbr*.bin; \
			if [ '$(LAYOUT_USE_EMMC_SWAP)' = 'emmc_swap_true' ] ; then \
				dd if=tmp/swap_p.bin of=$(PACKAGES)/swap_p.bin bs=4096 count=1; \
				tar rvf $(IMGFILE_PATH) $(PACKAGES)/swap_p.bin; \
			fi; \
		  fi; \
		fi; \
	fi
	
	if [ '$(gen_install_binary)' = '0' ]; then \
		echo "gen install binary file"; \
		echo "start."; \
		if [ '$(LAYOUT)' = 'emmc' ] ; then \
          		cd ./gen_binary/;./gen_binary_tool ../../install.img 2 $(gen_install_binary); \
		else \
        		echo " nand or nor flash is not ready. exit."; \
		fi; \
	elif [ '$(gen_install_binary)' = '1' ]; then \
                echo "gen install binary file"; \
                echo "start."; \
                if [ '$(LAYOUT)' = 'emmc' ] ; then \
                        cd ./gen_binary/;./gen_binary_tool ../../install.img 2 $(gen_install_binary); \
                else \
                        echo " nand or nor flash is not ready. exit."; \
                fi; \
	fi;
endif
	
clean:
	rm -rf tmp/ ./gen_binary/complete
