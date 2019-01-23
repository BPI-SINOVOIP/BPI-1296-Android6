# Makefile for OpenWrt
#
# Copyright (C) 2007-2015 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

TOPDIR:=${CURDIR}
LC_ALL:=C
LANG:=C
export TOPDIR LC_ALL LANG
export OPENWRT_VERBOSE=s

world:

include $(TOPDIR)/include/host.mk

ifneq ($(OPENWRT_BUILD),1)
  override OPENWRT_BUILD=1
  export OPENWRT_BUILD

  empty:=
  space:= $(empty) $(empty)
  _SINGLE=export MAKEFLAGS=$(space);

  include $(TOPDIR)/include/debug.mk
  include $(TOPDIR)/include/depends.mk

ifeq ($(filter info image distclean, $(MAKECMDGOALS)),)
  include $(TOPDIR)/include/toplevel.mk
endif
else
  include rules.mk
  include $(INCLUDE_DIR)/depends.mk

ifeq ($(filter kernel _call_info _image, $(MAKECMDGOALS)),)
  include $(INCLUDE_DIR)/subdir.mk
  include package/Makefile
endif

ifneq ($(filter _image _call_image package_index package_install copy_files  package_postinst build_image, $(MAKECMDGOALS)),)
include $(INCLUDE_DIR)/version.mk
export REVISION

# override variables from rules.mk
PACKAGE_DIR:=$(TOPDIR)/packages
OPKG:= \
  IPKG_NO_SCRIPT=1 \
  IPKG_TMP="$(TMP_DIR)/ipkgtmp" \
  IPKG_INSTROOT="$(TARGET_DIR)" \
  IPKG_CONF_DIR="$(TMP_DIR)" \
  IPKG_OFFLINE_ROOT="$(TARGET_DIR)" \
  $(STAGING_DIR_HOST)/bin/opkg \
	-f $(TOPDIR)/repositories.conf \
	--force-depends \
	--force-overwrite \
	--force-postinstall \
	--cache $(DL_DIR) \
	--offline-root $(TARGET_DIR) \
	--add-dest root:/ \
	--add-arch all:100 \
	--add-arch $(ARCH_PACKAGES):200
endif

define Profile
  $(eval $(call Profile/Default))
  $(eval $(call Profile/$(1)))
  ifeq ($(CONFIG_TARGET_$(call target_conf,$(BOARD)_$(if $(SUBTARGET),$(SUBTARGET)_))$(1)),y)
  ifeq ($(USER_PROFILE),)
    USER_PROFILE:=$(1)
  endif
  endif
  $(1)_NAME:=$(NAME)
  $(1)_PACKAGES:=$(PACKAGES)
  PROFILE_NAMES += $(1)
  PROFILE_LIST += \
  	echo '$(1):'; [ -z '$(NAME)' ] || echo '	$(NAME)'; echo '	Packages: $(PACKAGES)';
endef

  include $(INCLUDE_DIR)/target.mk

staging_dir/host/.prereq-build: include/prereq-build.mk
	mkdir -p tmp
	rm -f tmp/.host.mk
	@$(_SINGLE)$(NO_TRACE_MAKE) -j1 -r -s -f $(TOPDIR)/include/prereq-build.mk prereq 2>/dev/null || { \
		echo "Prerequisite check failed. Use FORCE=1 to override."; \
		false; \
	}
  ifneq ($(realpath $(TOPDIR)/include/prepare.mk),)
	@$(_SINGLE)$(NO_TRACE_MAKE) -j1 -r -s -f $(TOPDIR)/include/prepare.mk prepare 2>/dev/null || { \
		echo "Preparation failed."; \
		false; \
	}
  endif
	touch $@

_call_info: FORCE
	echo 'Current Target: "$(BOARD)$(if $(SUBTARGET), ($(BOARDNAME)))"'
	echo 'Default Packages: $(DEFAULT_PACKAGES)'
	echo 'Current Profile: $(USER_PROFILE)'
	echo 'Available Profiles:'
	echo; $(PROFILE_LIST)

ifneq ($(USER_PACKAGES),ALL)
BUILD_PACKAGES:=$(sort $(DEFAULT_PACKAGES) $(USER_PACKAGES) $($(USER_PROFILE)_PACKAGES) kernel)
# "-pkgname" in the package list means remove "pkgname" from the package list
BUILD_PACKAGES:=$(filter-out $(filter -%,$(BUILD_PACKAGES)) $(patsubst -%,%,$(filter -%,$(BUILD_PACKAGES))),$(BUILD_PACKAGES))
PACKAGES:=
else
BUILD_PACKAGES:=$(shell $(OPKG) list | cut -f1 -d ' ')
endif

_call_image: staging_dir/host/.prereq-build
	echo 'Building images for $(BOARD)$(if $($(USER_PROFILE)_NAME), - $($(USER_PROFILE)_NAME))'
	echo 'Packages: $(BUILD_PACKAGES)'
	echo
	rm -rf $(TARGET_DIR)
	mkdir -p $(TARGET_DIR) $(BIN_DIR) $(TMP_DIR) $(DL_DIR)
	if [ ! -f "$(PACKAGE_DIR)/Packages" ] || [ ! -f "$(PACKAGE_DIR)/Packages.gz" ] || [ "`find $(PACKAGE_DIR) -cnewer $(PACKAGE_DIR)/Packages.gz`" ]; then \
		echo "Package list missing or not up-to-date, generating it.";\
		$(MAKE) package_index; \
	else \
		mkdir -p $(TARGET_DIR)/tmp; \
		$(OPKG) update || true; \
	fi
	$(MAKE) package_install
ifneq ($(USER_FILES),)
	$(MAKE) copy_files
endif
	$(MAKE) package_postinst
	$(MAKE) build_image

package_index: FORCE
	@echo
	@echo Building package index...
	@mkdir -p $(TMP_DIR) $(TARGET_DIR)/tmp
	(cd $(PACKAGE_DIR); $(SCRIPT_DIR)/ipkg-make-index.sh . > Packages && \
		gzip -9c Packages > Packages.gz \
	) >/dev/null 2>/dev/null
	$(OPKG) update || true

package_install: FORCE
	@echo
	@echo Installing packages...
	$(OPKG) install $(firstword $(wildcard $(PACKAGE_DIR)/libc_*.ipk $(PACKAGE_DIR)/base/libc_*.ipk))
	$(OPKG) install $(firstword $(wildcard $(PACKAGE_DIR)/kernel_*.ipk $(PACKAGE_DIR)/base/kernel_*.ipk))
	$(OPKG) install $(BUILD_PACKAGES)
	rm -f $(TARGET_DIR)/usr/lib/opkg/lists/*

copy_files: FORCE
	@echo
	@echo Copying extra files
	@$(call file_copy,$(USER_FILES)/*,$(TARGET_DIR)/)

package_postinst: FORCE
	@echo
	@echo Cleaning up
	@rm -f $(TARGET_DIR)/tmp/opkg.lock
	@echo
	@echo Activating init scripts
	@mkdir -p $(TARGET_DIR)/etc/rc.d
	@( \
		cd $(TARGET_DIR); \
		for script in ./usr/lib/opkg/info/*.postinst; do \
			IPKG_INSTROOT=$(TARGET_DIR) $$(which bash) $$script; \
		done || true \
	)
	rm -f $(TARGET_DIR)/usr/lib/opkg/info/*.postinst
	$(if $(CONFIG_CLEAN_IPKG),rm -rf $(TARGET_DIR)/usr/lib/opkg)

build_image: FORCE
	@echo
	@echo Building images...
	$(NO_TRACE_MAKE) -C target/linux/$(BOARD)/image install TARGET_BUILD=1 IB=1 \
		$(if $(USER_PROFILE),PROFILE="$(USER_PROFILE)")

$(package/stamp-compile): $(BUILD_DIR)/.prepared
$(BUILD_DIR)/.prepared: Makefile
	@mkdir -p $$(dirname $@)
	@touch $@

clean: FORCE
	+$(NO_TRACE_MAKE) -C target/linux/$(BOARD) mostlyclean TARGET_BUILD=1 IB=1 $(if $(USER_PROFILE),PROFILE="$(USER_PROFILE)")
	rm -rf $(TMP_DIR) $(TARGET_DIR) $(BIN_DIR)
	find -name kmod*.ipk ! -name kmod-ufsd*.ipk -print | xargs rm -rf {}
	(cd $(KERNEL_BUILD_DIR); ls . | grep -v "linux" | xargs rm -rf {})

dirclean: clean
	rm -rf feeds/

kernel:
	@echo Command Goals $(MAKECMDGOALS)
	@echo
	@echo Building kernel...
	+$(NO_TRACE_MAKE) -C target/linux/$(BOARD) compile TARGET_BUILD=1 IB=1 \
		$(if $(USER_PROFILE),PROFILE="$(USER_PROFILE)")

WIFI_DRV := mac80211 ottbtusb ottwifi

modules: kernel
	@$(MAKE) package/linux/install
	@$(MAKE) package/kernel/mali/install
	-+@(for wifi in $(WIFI_DRV); do [ -d package/kernel/$$wifi ] && $(MAKE) package/kernel/$$wifi/install; done)

_image: 
ifneq ($(PROFILE),)
  ifeq ($(filter $(PROFILE),$(PROFILE_NAMES)),)
	@echo 'Profile "$(PROFILE)" does not exist!'
	@echo 'Use "make info" to get a list of available profile names.'
	@exit 1
  endif
endif
	(unset PROFILE FILES PACKAGES MAKEFLAGS; \
	$(MAKE) _call_image \
		$(if $(PROFILE),USER_PROFILE="$(PROFILE)") \
		$(if $(FILES),USER_FILES="$(FILES)") \
		$(if $(PACKAGES),USER_PACKAGES="$(PACKAGES)") \
		$(if $(BIN_DIR),BIN_DIR="$(BIN_DIR)"))


# check prerequisites before starting to build
prereq: $(package/stamp-prereq) ;

world: prepare $(package/stamp-compile) FORCE
	@$(MAKE) package/index
	(cd $(PACKAGE_DIR); $(FIND) -type f -name '*.ipk') | \
		while read path; do \
			mkdir -p "$(TOPDIR)/packages/$${path%/*}"; \
			mv "$(PACKAGE_DIR)/$$path" "$(TOPDIR)/packages/$$path"; \
		done

.PHONY: clean dirclean prereq prepare world

endif # OPENWRT_BUILD

image:
	$(MAKE) -s _image

info:
	(unset PROFILE FILES PACKAGES MAKEFLAGS; $(MAKE) -s _call_info)

distclean:
	@echo
	@echo Remove all downloaded sources in dl folder
	(cd $(TOPDIR)/dl; ls . | grep -E -v "rtl8192|ottwifi|ottbt|rtkwifiu|mali|linux-[34]" | xargs rm -rf {})

.SILENT: info image
