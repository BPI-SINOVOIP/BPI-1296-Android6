# 
# Copyright (C) 2006-2011 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/kernel.mk

PKG_NAME:=ufsd
PKG_VERSION:=9.2.2
PKG_RELEASE:=1

PKG_SOURCE_PROTO:=git
PKG_SOURCE_URL:=
PKG_SOURCE_SUBDIR:=$(PKG_NAME)-$(PKG_VERSION)
PKG_SOURCE_BRANCH:=master
PKG_SOURCE_VERSION:=c81628
PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION)-$(PKG_SOURCE_VERSION).tar.gz
PKG_BUILD_DIR:=$(KERNEL_BUILD_DIR)/ufsd-$(PKG_VERSION)
PKG_BUILD_PARALLEL:=1
FS_LIB:=lke_9.2.2_r264218_b43


include $(INCLUDE_DIR)/package.mk

define KernelPackage/ufsd
  VERSION:=$(LINUX_VERSION)-$(BOARD)-$(PKG_RELEASE)
  SUBMENU:=Filesystems
  TITLE:=Paragon's File System
endef

define KernelPackage/ufsd/Default/description
  Parago's driver for NTFS/exFAT/HFS+
endef

define Build/Prepare
endef

define Build/Configure
endef

define Build/Compile
endef

define KernelPackage/ufsd/install
endef

$(eval $(call KernelPackage,ufsd))
