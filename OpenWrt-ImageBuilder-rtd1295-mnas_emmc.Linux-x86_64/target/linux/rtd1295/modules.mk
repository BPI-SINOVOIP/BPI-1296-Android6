#
# Copyright (C) 2012 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#


define KernelPackage/rtksdmmc
  SUBMENU:=$(OTHER_MENU)
  TITLE:=Realtek SD/MMC host driver
  KCONFIG:=CONFIG_MMC_RTK_SDMMC
  FILES:=$(LINUX_DIR)/drivers/mmc/host/rtk-sdmmc.ko
  AUTOLOAD:=$(call AutoLoad,30,rtk-sdmmc)
  DEPENDS:=@TARGET_rtd1295
endef

define KernelPackage/rtksdmmc/description
  This package contains the Realtek SD/MMC host driver
endef

$(eval $(call KernelPackage,rtksdmmc))


define KernelPackage/rtl815x
  SUBMENU:=$(USB_MENU)
  TITLE:=Realtek RTL8152/RTL8153 USB driver
  KCONFIG:=CONFIG_USB_RTL8152 
  FILES:= \
	$(LINUX_DIR)/drivers/net/usb/rtl8152/r8152.ko \
	$(LINUX_DIR)/drivers/net/mii.ko
  AUTOLOAD:=$(call AutoLoad,25,r8152)
  DEPENDS:=@TARGET_rtd1295
endef

define KernelPackage/rtl815x/description
  This package contains the Realtek 8152/8153 USB NIC driver
endef

$(eval $(call KernelPackage,rtl815x))

define KernelPackage/rtl8169soc
  SUBMENU:=$(NETWORK_DEVICES_MENU)
  TITLE:=Realtek Gigabit Ethernet driver
  KCONFIG:= \
	CONFIG_R8169SOC=y \
	CONFIG_NET_VENDOR_REALTEK=y
  FILES:=
  AUTOLOAD:=
  DEPENDS:=@TARGET_rtd1295
endef


$(eval $(call KernelPackage,rtl8169soc))

define KernelPackage/rtl8168/description
  This package contains the Realtek R8168 PCI-E Gigibit Ethernet driver
endef

define KernelPackage/rtl8168
  SUBMENU:=$(NETWORK_DEVICES_MENU)
  TITLE:=Realtek R8168 PCI-E Gigabit Ethernet driver
  DEPENDS:=@TARGET_rtd1295 @PCI_SUPPORT +kmod-mii
  KCONFIG:=CONFIG_R8168
  FILES:=$(LINUX_DIR)/drivers/net/ethernet/realtek/r8168/r8168.ko
  AUTOLOAD:=$(call AutoProbe,r8168)
endef

$(eval $(call KernelPackage,rtl8168))

define KernelPackage/rtl8169soc/description
  This package contains the Realtek Gigibit Ethernet driver
endef

define KernelPackage/rtd1295hwnat
  SUBMENU:=$(NETWORK_DEVICES_MENU)
  TITLE:=Realtek 1295 HWNAT driver
  KCONFIG:= \
	CONFIG_RTD_1295_HWNAT=y \
	CONFIG_BRIDGE_IGMP_SNOOPING=y \
	CONFIG_RTD_1295_MAC0_SGMII_LINK_MON=y \
	CONFIG_RTL_HARDWARE_NAT=y \
	CONFIG_RTL_819X=y \
	CONFIG_RTL_HW_NAPT=y \
	CONFIG_RTL_LAYERED_ASIC_DRIVER=y \
	CONFIG_RTL_LAYERED_ASIC_DRIVER_L3=y \
	CONFIG_RTL_LAYERED_ASIC_DRIVER_L4=y \
	CONFIG_RTL_LAYERED_DRIVER_ACL=y \
	CONFIG_RTL_LAYERED_DRIVER_L2=y \
	CONFIG_RTL_LAYERED_DRIVER_L3=y \
	CONFIG_RTL_LAYERED_DRIVER_L4=y \
	CONFIG_RTL_LINKCHG_PROCESS=y \
	CONFIG_RTL_NETIF_MAPPING=y \
	CONFIG_RTL_PROC_DEBUG=y \
	CONFIG_RTL_FASTPATH_HWNAT_SUPPORT_KERNEL_3_X=y \
	CONFIG_RTL_JUMBO_FRAME=n \
	CONFIG_RTL_LOG_DEBUG=n \
	CONFIG_RTL865X_ROMEPERF=n \
	CONFIG_RTK_VLAN_SUPPORT=n \
	CONFIG_RTL_EEE_DISABLED=n \
	CONFIG_RTL_SOCK_DEBUG=n \
	CONFIG_RTL_EXCHANGE_PORTMASK=n \
	CONFIG_RTL_INBAND_CTL_ACL=n \
	CONFIG_RTL_ETH_802DOT1X_SUPPORT=n \
	CONFIG_RTL_TSO=n \
	CONFIG_RTL_MULTI_LAN_DEV=y \
	CONFIG_AUTO_DHCP_CHECK=n \
	CONFIG_RTL_HW_MULTICAST_ONLY=n \
	CONFIG_RTL_HW_L2_ONLY=n \
	CONFIG_RTL_MULTIPLE_WAN=n \
	CONFIG_RTL865X_LANPORT_RESTRICTION=n \
	CONFIG_RTL_IVL_SUPPORT=y \
	CONFIG_RTL_LOCAL_PUBLIC=n \
	CONFIG_RTL_HW_DSLITE_SUPPORT=n \
	CONFIG_RTL_HW_6RD_SUPPORT=n \
	CONFIG_RTL_IPTABLES_RULE_2_ACL=n \
	CONFIG_RTL_FAST_FILTER=n \
	CONFIG_RTL_ETH_PRIV_SKB=n \
	CONFIG_RTL_HW_QOS_SUPPORT=n \
	CONFIG_RTL_EXT_PORT_SUPPORT=n \
	CONFIG_RTL_HARDWARE_IPV6_SUPPORT=n \
	CONFIG_RTL_ROMEPERF_24K=n \
	CONFIG_RTL_VLAN_PASSTHROUGH_SUPPORT=n \
	CONFIG_RTL_8211F_SUPPORT=n \
	CONFIG_RTL_8367R_SUPPORT=n
  FILES:=
  AUTOLOAD:=
  DEPENDS:=@TARGET_rtd1295 
endef

define KernelPackage/rtd1295hwnat/description
  This package contains the Realtek HW NAT Driver
endef

$(eval $(call KernelPackage,rtd1295hwnat))

define KernelPackage/rtd1295xen
  SUBMENU:=Virtualization Support
  TITLE:=Realtek 1295 XEN support
  KCONFIG:= \
	CONFIG_HVC_DRIVER=y \
	CONFIG_HVC_IRQ=y \
	CONFIG_HVC_XEN=y \
	CONFIG_HVC_XEN_FRONTEND=y \
	CONFIG_RTK_XEN_SUPPORT=y \
	CONFIG_RTK_DOMU_DMA=y \
	CONFIG_RTK_XEN_HYPERCALL=y \
	CONFIG_RTK_XEN_GPIO=y \
	CONFIG_SWIOTLB_XEN=y \
	CONFIG_SYS_HYPERVISOR=y \
	CONFIG_XEN=y \
	CONFIG_XENFS=y \
	CONFIG_XEN_AUTO_XLATE=y \
	CONFIG_XEN_BACKEND=y \
	CONFIG_XEN_BALLOON=y \
	CONFIG_XEN_BLKDEV_BACKEND=y \
	CONFIG_XEN_BLKDEV_FRONTEND=y \
	CONFIG_XEN_COMPAT_XENFS=y \
	CONFIG_XEN_DEV_EVTCHN=y \
	CONFIG_XEN_DOM0=y \
	CONFIG_XEN_GNTDEV=y \
	CONFIG_XEN_GRANT_DEV_ALLOC=y \
	CONFIG_XEN_NETDEV_BACKEND=y \
	CONFIG_XEN_NETDEV_FRONTEND=y \
	CONFIG_XEN_PRIVCMD=y \
	CONFIG_XEN_SCRUB_PAGES=y \
	CONFIG_XEN_SYS_HYPERVISOR=y \
	CONFIG_XEN_XENBUS_FRONTEND=y \
	CONFIG_XEN_USB_BACKEND=y \
	CONFIG_XEN_USB_FRONTEND=y \
	CONFIG_XEN_USB_SS_PVUSB=y \
	CONFIG_ARM_REALTEK_CPUFREQ_HYSTERESIS=y \
	CONFIG_RTK_ACPU_RELOAD=y \
	CONFIG_RTK_IPCSHM_RESET=y \
	CONFIG_RTK_XEN_DOMU_RSVMEM=n \
	CONFIG_INPUT_EVDEV=y \
	CONFIG_CPUFREQ_OD_HELPER=n \
	CONFIG_CPUFREQ_XEN_DOM0=y \
	CONFIG_CPUFREQ_RTK_XEN_DOM0=y \
	CONFIG_COMMON_CLK_RTK129X=y \
	CONFIG_RTK_SW_LOCK_API=y \
	CONFIG_RTK_XEN_HWLOCK=y \
	CONFIG_XEN_REGDEV_BACKEND=y \
	CONFIG_FB_RTK=y \
	CONFIG_FB_RTK_FPGA=n \
	CONFIG_ANDROID=y \
	CONFIG_ION=y \
	CONFIG_ION_RTK_PHOENIX=y \
	CONFIG_INPUT_MOUSEDEV=y \
	CONFIG_INPUT_MOUSEDEV_PSAUX=y \
	CONFIG_INPUT_MOUSEDEV_SCREEN_X=1024 \
	CONFIG_INPUT_MOUSEDEV_SCREEN_Y=768 \
	CONFIG_THERMAL=y \
	CONFIG_CPU_THERMAL=y \
	CONFIG_RTK_THERMAL=y \
	CONFIG_UIO=y \
	CONFIG_UIO_ASSIGN_MINOR=y \
	CONFIG_IP_NF_IPTABLES=y \
	CONFIG_PPP=y \
	CONFIG_RTL_IPTABLES_FAST_PATH=y \
	CONFIG_RTL_FAST_PPPOE=y \
	CONFIG_UIO_RTK_RBUS=y
  FILES:=
  AUTOLOAD:=
  DEPENDS:=@TARGET_rtd1295 @AUDIO_ADDR_LEGACY +libfdt +yajl +bash +perl
endef

define KernelPackage/rtd1295xen/description
  This package contains the Realtek XEN
endef

$(eval $(call KernelPackage,rtd1295xen))

define KernelPackage/rtktranscoding
  SUBMENU:=$(VIDEO_MENU)
  TITLE:=Realtek NAS Transcoding
  KCONFIG:= \
	CONFIG_ION=y \
	CONFIG_ION_RTK_PHOENIX=y \
	CONFIG_RTK_CODEC=y \
	CONFIG_RTK_RESERVE_MEMORY=y \
	CONFIG_VE1_CODEC=y \
	CONFIG_VE3_CODEC=y \
	CONFIG_IMAGE_CODEC=y \
	CONFIG_ANDROID=y \
	CONFIG_ASHMEM=y \
	CONFIG_ANDROID_LOW_MEMORY_KILLER=y \
	CONFIG_ANDROID_LOW_MEMORY_KILLER_AUTODETECT_OOM_ADJ_VALUES=y \
	CONFIG_SYNC=y \
	CONFIG_SW_SYNC=y \
	CONFIG_SW_SYNC_USER=y \
	CONFIG_ZSMALLOC=y \
	CONFIG_REGMAP=y \
	CONFIG_REGMAP_I2C=y \
	CONFIG_JUMP_LABEL=y \
	CONFIG_KSM=y \
	CONFIG_UIO=y \
	CONFIG_UIO_ASSIGN_MINOR=y \
	CONFIG_UIO_RTK_RBUS=y \
	CONFIG_UIO_RTK_REFCLK=y \
	CONFIG_UIO_RTK_SE=y \
	CONFIG_UIO_RTK_MD=y \
	CONFIG_ARCH_MMAP_RND_BITS_MAX=24 \
	CONFIG_ARCH_MMAP_RND_BITS_MIN=18 \
	CONFIG_ARCH_MMAP_RND_COMPAT_BITS_MIN=11 \
	CONFIG_ARM64_4K_PAGES=y \
	CONFIG_ARM64_64K_PAGES=n \
	CONFIG_ARM64_VA_BITS=39 \
	CONFIG_ARM64_VA_BITS_39=y \
	CONFIG_ARM64_VA_BITS_42=n \
	CONFIG_PGTABLE_LEVELS=3 \
	CONFIG_RTK_NAS_TRANSCODING=y
  FILES:=
  AUTOLOAD:=
  DEPENDS:=@TARGET_rtd1295 \
	+gstreamer1-libs +gstreamer1 \
	+gstreamer1-utils \
	+gstreamer1-plugins-base +gst1-plugins-base \
	+gstreamer1-plugins-good +gst1-plugins-good \
	+gstreamer1-plugins-bad +gst1-plugins-bad \
	+gstreamer1-plugins-ugly +gst1-plugins-ugly \
	+gst1-libav \
	+gst1-omx \
	+linuxmedia
endef

define KernelPackage/rtktranscoding/description
  This package provides transcoding feature
endef

$(eval $(call KernelPackage,rtktranscoding))

define KernelPackage/rtd1295cma
  SUBMENU:=$(OTHER_MENU)
  TITLE:=Realtek CMA improvement
  KCONFIG:= \
	CONFIG_CMA_IMPROVE=y
  FILES:=
  AUTOLOAD:=
  DEPENDS:=@TARGET_rtd1295
endef

define KernelPackage/rtd1295cma/description
  This package contains the Realtek CMA tweak
endef

$(eval $(call KernelPackage,rtd1295cma))
