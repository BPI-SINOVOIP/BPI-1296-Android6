#
# Copyright (C) 2012 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

define Profile/giraffe-1GB
  NAME:=Giraffe 1GB board
endef

define Profile/giraffe-1GB/Description
	Giraffe board V0.1 1GB
endef

$(eval $(call Profile,giraffe-1GB))

define Profile/giraffe-2GB
  NAME:=Giraffe 2GB board
endef

define Profile/giraffe-2GB/Description
	Giraffe board V0.1 2GB
endef

$(eval $(call Profile,giraffe-2GB))

define Profile/giraffe-2GB-B00
  NAME:=Giraffe V5.0 2GB board
endef

define Profile/giraffe-2GB-B00/Description
	Giraffe board V5.0 2GB with B00 chip
endef

$(eval $(call Profile,giraffe-2GB-B00))
