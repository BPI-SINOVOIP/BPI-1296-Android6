#
# Copyright (C) 2007-2011 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk
#include $(INCLUDE_DIR)/kernel.mk

PKG_NAME:=mesh-pathsel
PKG_VERSION:=2016-12-08
PKG_SOURCE_SUBDIR:=$(PKG_NAME)
PKG_RELEASE:=1

include $(INCLUDE_DIR)/package.mk

define Package/mesh-pathsel
  SECTION:=net
  CATEGORY:=Network
  TITLE:=Realtek 802.11s Mesh Path Selection Daemon
endef

define Package/mesh-pathsel/description
	Realtek 802.11s Mesh Path Selection Daemon
endef

define Build/Prepare
	rm -rf $(PKG_BUILD_DIR)
	mkdir -p $(PKG_BUILD_DIR)
	echo "PKG_BUILD_DIR=$(PKG_BUILD_DIR)"
	echo "TARGET_CROSS=$(TARGET_CROSS)"
	echo "LINUX_DIR=$(LINUX_DIR)"
	$(CP) ./files/11s-PathSelection/* $(PKG_BUILD_DIR)
	$(call Build/Prepare/Default)
endef

define Build/Configure
	echo $(PKG_BUILD_DIR)
endef

#MAKE_FLAGS := -DRTK_129X_PLATFORM -D_RTL8187B_ -D_RTL865X_
MAKE_FLAGS := -DPATHSEL_DAEMON_DEBUG

MAKE_OPTS:= CROSS_COMPILE="$(TARGET_CROSS)" MAKE_FLAGS="$(MAKE_FLAGS)"
#		DIR_LINUX="$(LINUX_DIR)" \



define Build/Compile
	(echo $(PKG_BUILD_DIR); cd $(PKG_BUILD_DIR);$(MAKE) $(MAKE_OPTS))
endef

define Package/mesh-pathsel/install
	$(INSTALL_DIR) $(1)/usr/bin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/pathsel $(1)/usr/bin/
endef

$(eval $(call BuildPackage,mesh-pathsel))
