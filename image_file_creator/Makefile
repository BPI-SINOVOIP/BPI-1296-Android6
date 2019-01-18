#
# This Makefile is to maintain all the versions of the sub-images in an Image file.
# Sub-images include Linux, Video ROS, Audio ROS, and Application images.
#

include Makefile.in

all:
	@echo Usage for Image File Creator:
	@echo -e "\tmake image:"
	@echo -e "\t\tBuild the image file."
#	@echo -e "\tdefault flag value:\tlayout_type=nand layout_size=8gb install_dtb=0 install_ap(AP)=0 install_bootloader(bootloader.tar)=0 install_factory(factory.tar)=1"
	@echo -e "\tdefault flag value:\tlayout_type=emmc layout_size=8gb install_dtb=0 install_bootloader(bootloader.tar)=0 install_factory(factory.tar)=1"
	@echo -e "\t"
#	@echo -e "\tmake image install_ap=1:"
#	@echo -e "\t\tBuild the image file that including applications in "
#	@echo -e "\t\tcomponents/packages/[dir]/AP/."
#	@echo -e "\t"
#	@echo -e "\tmake image hash_imgfile=1:"
#	@echo -e "\t\tBuild the image file that includes hash value"
#	@echo -e "\t"
	@echo -e "\tmake image PACKAGES=[dir] layout_type=[nand/emmc] layout_size=[4gb/8gb]:"
	@echo -e "\t\tBuild the image file that only including packages in "
	@echo -e "\t\tcomponents/packages/[dir] directory."
	@echo -e "\t\tAnd specify the storage layout type you are using [nand/emmc], default is nand."
	@echo -e "\t\tAnd specify the emmc storage layout size you are using [4gb/8gb], only working for emmc case, default is 8gb."
	@echo -e "\t"
	@echo -e "\tmake image install_bootloader=1:"
	@echo -e "\t\tThe installation includes installing bootloader."	
	@echo -e "\t"
	@echo -e "\tmake image install_nor_bootloader=1:"
	@echo -e "\t\tThe installation includes installing nor bootloader."	
	@echo -e "\t"
	@echo -e "\tmake image install_nor_bootloader2=1:"
	@echo -e "\t\tThe installation includes installing nor bootloader2."	
	@echo -e "\t"
	@echo -e "\tmake image efuse_key=1:"
	@echo -e "\t\tCopy encryption key."
	@echo -e "\t"
	@echo -e "\tmake image offline_gen=y:"
	@echo -e "\t\tGenerate the offline stuffs for mp tool."	
	@echo -e "\t\tFor detail, please read the components/installer_x86/readme.txt"	
	@echo -e "\t"
	@echo -e "\tmake image PURE_LINUX_IMGS=y:"
	@echo -e "\t\tOffline gen utility support for pure linux case."
	@echo -e "\t"
	@echo -e "\tmake image efuse_fw=1:"
	@echo -e "\t\tCopy efuse fw to install.img for writing."
	@echo -e "\t"
	@echo -e "\tmake clean PACKAGES=[dir]:"
	@echo -e "\t\tClean the temp file."	
	@echo -e "\t"
	@echo -e "\tmake image gen_install_binary=0 or 1:"
	@echo -e "\t\tgen install.img binary file."
#	@echo -e "\t"	
#	@echo -e "\tmake image update_etc=1:"
#	@echo -e "\t\tThe installation includes update etc partition (user data)."
#	@echo -e "\t"
#	@echo -e "\tmake image stop_reboot=1:"
#	@echo -e "\t\tSystem will not auto reboot after finish installing firmware."
#	@echo -e "\t"
#	@echo -e "\tmake image reboot_delay=N:"
#	@echo -e "\t\tSystem will delay N seconds and reboot after finish installing firmware when stop_reboot is not set."
#	@echo -e "\t"	
#	@echo -e "\t\tNote: It only supports for making Android image(ANDROID_IMGS=1) & tv001 so far"
#	@echo -e "\t\tNote: This feature works on DDR size should be more than/eqaul 1G recommanded"
#	@echo -e "\t"
#	@echo -e "\tmake svnup:"
#	@echo -e "\t\tUpdate images according to the version information in "
#	@echo -e "\t\tcomponents/packages/[dir]/Makefile.in."
#	@echo -e "\t"
#	@echo -e "\tmake image logger_level=n"
#	@echo -e "\t\tThis value is used for control install_a logger level while upgrading image"
#	@echo -e "\t\t 0:default by program;Fail:1/INFO:2/LOG:4/DEBUG:8/Warning:16/UI:32/TarLog:256/MemInfo:512"
#	@echo -e "\t\tAggregate all level log you want to read.Ex: n=288=32+256 means you only enable UI&Tar log while upgrading"
#	@echo -e "\t\tIf modified, INSTALL_FAIL_LEVEL | INSTALL_INFO_LEVEL | INSTALL_UI_LEVEL is recommanded to included basically"
#	@echo -e "\t"

image:
	rm -f $(IMGFILE_NAME)
	if [ '$(PACKAGES)' = '' ]; then	\
		echo Please specify a package to build, ex. PACKAGES=package2;	\
	else	\
		for d in $(PACKAGES);	\
		do	\
			cd components; $(MAKE) TARGET=$$d layout_type=$(layout_type) layout_size=$(layout_size) install_ap=$(install_ap) install_bootloader=$(install_bootloader) install_factory=$(install_factory) update_etc=$(update_etc) logger_level=$(logger_level) layout_use_emmc_swap=$(layout_use_emmc_swap);	\
		done;	\
	fi

install_ap:
	@echo We no longer support command, \"make install_ap\".
	@echo Use \"make install_ap=1\" instead.

svnup:
	for d in `ls components/packages`;					\
	do									\
		cd components; $(MAKE) svnup TARGET=$$d;			\
	done

clean:
	rm -f $(IMGFILE_NAME)
	if [ '$(PACKAGES)' = '' ]; then	\
		echo Please specify a package to clean, ex. PACKAGES=package2;	\
	else	\
		for d in $(PACKAGES);	\
		do	\
			cd components; $(MAKE) TARGET=$$d clean;	\
		done;	\
	fi
