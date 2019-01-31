### defined in rtk_kylin32.mk and rtk_kylin64.mk
# PRODUCT_AAPT_CONFIG := normal large tvdpi mdpi hdpi
# PRODUCT_AAPT_PREF_CONFIG := hdpi
###

### defined in rtk_kylin32.mk and rtk_kylin32_tv.mk
# PRODUCT_CHARACTERISTICS := tablet
###

## A all realtek devices share overlay_common
DEVICE_PACKAGE_OVERLAYS := device/realtek/kylin/overlay_common

#PRODUCT_COPY_FILES += $(call find-copy-subdir-files,*,device/realtek/kylin/prebuilt/system,system)
#PRODUCT_COPY_FILES += $(call find-copy-subdir-files,*,device/realtek/kylin/prebuilt/root,root)

PRODUCT_COPY_FILES += system/core/rootdir/init.rc:root/init.rc
PRODUCT_COPY_FILES += device/realtek/kylin/init.kylin.rc:root/init.kylin.rc
PRODUCT_COPY_FILES += device/realtek/kylin/init.kylin.usb.rc:root/init.kylin.usb.rc
PRODUCT_COPY_FILES += device/realtek/kylin/ueventd.kylin.rc:root/ueventd.kylin.rc
PRODUCT_COPY_FILES += device/realtek/kylin/audio_policy.conf:system/etc/audio_policy.conf
PRODUCT_COPY_FILES += device/realtek/kylin/fstab.kylin:root/fstab.kylin
PRODUCT_COPY_FILES += device/realtek/kylin/sys.part.rc:root/sys.part.rc
PRODUCT_COPY_FILES += device/realtek/kylin/init.fschk.rc:root/init.fschk.rc

PRODUCT_COPY_FILES += frameworks/native/data/etc/android.hardware.location.xml:system/etc/permissions/android.hardware.location.xml
PRODUCT_COPY_FILES += frameworks/native/data/etc/android.hardware.wifi.xml:system/etc/permissions/android.hardware.wifi.xml
PRODUCT_COPY_FILES += frameworks/native/data/etc/android.hardware.wifi.direct.xml:system/etc/permissions/android.hardware.wifi.direct.xml
PRODUCT_COPY_FILES += frameworks/native/data/etc/android.hardware.usb.host.xml:system/etc/permissions/android.hardware.usb.host.xml
PRODUCT_COPY_FILES += frameworks/native/data/etc/android.hardware.usb.accessory.xml:system/etc/permissions/android.hardware.usb.accessory.xml
PRODUCT_COPY_FILES += frameworks/native/data/etc/android.hardware.camera.xml:system/etc/permissions/android.hardware.camera.xml
PRODUCT_COPY_FILES += frameworks/native/data/etc/android.hardware.camera.autofocus.xml:system/etc/permissions/android.hardware.camera.autofocus.xml
PRODUCT_COPY_FILES += frameworks/native/data/etc/android.hardware.camera.flash-autofocus.xml:system/etc/permissions/android.hardware.camera.flash-autofocus.xml
PRODUCT_COPY_FILES += frameworks/native/data/etc/android.hardware.ethernet.xml:system/etc/permissions/android.hardware.ethernet.xml
PRODUCT_COPY_FILES += frameworks/native/data/etc/android.hardware.pppoe.xml:system/etc/permissions/android.hardware.pppoe.xml
PRODUCT_COPY_FILES += frameworks/native/data/etc/android.hardware.opengles.aep.xml:system/etc/permissions/android.hardware.opengles.aep.xml
PRODUCT_TAGS += dalvik.gc.type-precise

# setup dalvik vm configs.
PRODUCT_PROPERTY_OVERRIDES += dalvik.vm.heapstartsize=8m
PRODUCT_PROPERTY_OVERRIDES += dalvik.vm.heaptargetutilization=0.75
PRODUCT_PROPERTY_OVERRIDES += dalvik.vm.heapminfree=2m
PRODUCT_PROPERTY_OVERRIDES += dalvik.vm.heapmaxfree=8m

# setup dex2oat compiler options: everything|speed|balanced|space|interpret-only|verify-none
PRODUCT_PROPERTY_OVERRIDES += dalvik.vm.dex2oat-filter=speed
PRODUCT_PROPERTY_OVERRIDES += dalvik.vm.image-dex2oat-filter=speed

# pre-optimization options
PRODUCT_DEX_PREOPT_BOOT_FLAGS := --compiler-filter=speed
PRODUCT_DEX_PREOPT_DEFAULT_FLAGS := --compiler-filter=speed
#$(call add-product-dex-preopt-module-config,services,--compiler-filter=space)

PRODUCT_PROPERTY_OVERRIDES += persist.sys.timezone=Asia/Taipei
PRODUCT_PROPERTY_OVERRIDES += wifi.interface=wlan0
PRODUCT_PROPERTY_OVERRIDES += wifi.supplicant_scan_interval=120
PRODUCT_PROPERTY_OVERRIDES += ro.opengles.version=196610
PRODUCT_PROPERTY_OVERRIDES += ro.zygote.disable_gl_preload=true
PRODUCT_PROPERTY_OVERRIDES += ro.kernel.android.checkjni=false
PRODUCT_PROPERTY_OVERRIDES += keyguard.no_require_sim=true
PRODUCT_PROPERTY_OVERRIDES += persist.sys.strictmode.visual=0
PRODUCT_PROPERTY_OVERRIDES += persist.sys.strictmode.disable=1
PRODUCT_PROPERTY_OVERRIDES += hwui.render_dirty_regions=false
PRODUCT_PROPERTY_OVERRIDES += net.dns1=168.95.192.1
PRODUCT_PROPERTY_OVERRIDES += net.dns2=8.8.8.8
PRODUCT_PROPERTY_OVERRIDES += persist.pppoe.coldboot.conn=true
PRODUCT_PROPERTY_OVERRIDES += persist.rtk.audio.in=3
NUM_FRAMEBUFFER_SURFACE_BUFFERS := 3
##USE_LAYOUT_UI_1080 := true
#ifeq ($(IMAGE_DRAM_SIZE), 1GB)
#USE_LAYOUT_MEM_LOW := true
#endif

USE_AFBC := true

# Set this to false will cause compilation error
USE_RT_MEDIA_PLAYER := true

ENABLE_LIGHTTPD := true
ENABLE_RTK_HDMI_SERVICE := true
ENABLE_RTK_CEC_SERVICE := false
USE_SETTINGS_SERVICE := true
ENABLE_MEDIA_QUERY := true
ENABLE_RTK_HDMIRX_SERVICE := true
ENABLE_RTK_DPTX_SERVICE := true
USE_RTK_GPIO := false
# For DRM code flow
ENABLE_DRM_PLUGIN_SERVICE := true
# For TEE OS, video used physical Address
ENABLE_TEE_DRM_FLOW	:= false

USE_RTK_HDMI_CONTROL := NO
USE_RTK_MHL_CONTROL := NO
USE_RTK_DPTX_CONTROL := NO
USE_RTK_HDCP1x_CONTROL := NO
USE_RTK_HDCP22_CONTROL := NO
USE_RTK_CEC_CONTROL := NO
USE_RTK_VOUT_UTIL := YES
USE_RTK_AOUT_UTIL := YES
USE_RTK_SETUP_CLASS := NO

USE_RT_ION := true
USE_RTK_EXTRACTOR := false

USE_RTK_MEDIA_PROVIDER := YES
USE_DTCPIP_SIK := NO
USE_TV_MANAGER := YES
USE_TOGO_MANAGER := YES
USE_AIRFUN_MANAGER := NO

USE_RTK_UART_BT := YES
USE_JPU := NO
USE_RTK_PPPOE := YES
USE_RTK_OPENWRT := YES
ENABLE_RT_WIFIDISPLAY := true
# for DTV
ENABLE_1195_DTV_FLOW := YES

#ENABLE_FFMPEG := true

# used for recovery mode - apply for factory update
# example: RECOVERY_FACTORY_CMD0 := rm factory_test.txt
RECOVERY_FACTORY_CMD0 := cd ../../;cp -rf tmp/install_factory/* tmp/factory/;
RECOVERY_FACTORY_CMD1 :=

RECOVERY_SECUREBOOT := 0
LAYOUT := nand

# Used for NAS OTA package
NAS_ENABLE := NO

ifeq ($(USE_SETTINGS_SERVICE), true)
USE_RTK_SETUP_CLASS := YES
PRODUCT_PROPERTY_OVERRIDES += ro.config.use_settingsservice=true
else
PRODUCT_PROPERTY_OVERRIDES += ro.config.use_settingsservice=false
endif

ifeq ($(ENABLE_MEDIA_QUERY), true)
PRODUCT_PROPERTY_OVERRIDES += ro.config.enablemediaquery=true
else
PRODUCT_PROPERTY_OVERRIDES += ro.config.enablemediaquery=false
endif

ifeq ($(USE_RTK_VOUT_UTIL), YES)
USE_RTK_HDMI_CONTROL := YES
USE_RTK_SETUP_CLASS := YES
PRODUCT_PROPERTY_OVERRIDES += ro.config.enablevoututilservice=true
else
PRODUCT_PROPERTY_OVERRIDES += ro.config.enablevoututilservice=false
endif

ifeq ($(USE_RTK_AOUT_UTIL), YES)
PRODUCT_PROPERTY_OVERRIDES += ro.config.enableaoututilservice=true
else
PRODUCT_PROPERTY_OVERRIDES += ro.config.enableaoututilservice=false
endif

ifeq ($(ENABLE_RTK_HDMIRX_SERVICE), true)
PRODUCT_PROPERTY_OVERRIDES += ro.config.enablehdmirxservice=true
else
PRODUCT_PROPERTY_OVERRIDES += ro.config.enablehdmirxservice=false
endif

ifeq ($(ENABLE_RTK_DPTX_SERVICE), true)
USE_RTK_DPTX_CONTROL := YES
PRODUCT_PROPERTY_OVERRIDES += ro.config.enabledptxservice=true
else
PRODUCT_PROPERTY_OVERRIDES += ro.config.enabledptxservice=false
endif

ifeq ($(ENABLE_RTK_HDMI_SERVICE), true)
USE_RTK_HDMI_CONTROL := YES
# Enable the following USE_RTK_MHL_CONTROL := YES when MHL is enabled.
USE_RTK_MHL_CONTROL := YES
# Enable the following USE_RTK_HDCP1x_CONTROL := YES when HDCP is enabled.
# USE_RTK_HDCP1x_CONTROL := YES
# USE_RTK_HDCP22_CONTROL := YES
USE_RTK_VOUT_UTIL := YES
USE_RTK_SETUP_CLASS := YES
PRODUCT_PROPERTY_OVERRIDES += ro.config.enablehdmiservice=true
else
PRODUCT_PROPERTY_OVERRIDES += ro.config.enablehdmiservice=false
endif

ifeq ($(USE_RTK_MHL_CONTROL), YES)
PRODUCT_PROPERTY_OVERRIDES += ro.config.enablemhlserivce=false #FIXME
else
PRODUCT_PROPERTY_OVERRIDES += ro.config.enablemhlserivce=false
endif

ifeq ($(USE_RTK_HDCP1x_CONTROL), YES)
PRODUCT_PROPERTY_OVERRIDES += ro.config.enablehdcp=true
else
PRODUCT_PROPERTY_OVERRIDES += ro.config.enablehdcp=false
endif

ifeq ($(ENABLE_RTK_CEC_SERVICE), true)
PRODUCT_PROPERTY_OVERRIDES += ro.config.enablececservice=true
USE_RTK_CEC_CONTROL := YES
USE_RTK_SETUP_CLASS := YES
else
PRODUCT_PROPERTY_OVERRIDES += ro.config.enablececservice=false
PRODUCT_COPY_FILES += frameworks/native/data/etc/android.hardware.hdmi.cec.xml:system/etc/permissions/android.hardware.hdmi.cec.xml
endif

ifeq ($(USE_RT_MEDIA_PLAYER), true)
PRODUCT_PROPERTY_OVERRIDES += persist.media.USE_RTMediaPlayer=false
PRODUCT_PROPERTY_OVERRIDES += persist.media.USE_RtkExtractor=false
PRODUCT_PROPERTY_OVERRIDES += persist.media.RTSP_USE_RTPLAYER=false
PRODUCT_PROPERTY_OVERRIDES += persist.media.MMS_USE_RTPLAYER=false
else
PRODUCT_PROPERTY_OVERRIDES += persist.media.USE_RTMediaPlayer=false
PRODUCT_PROPERTY_OVERRIDES += persist.media.USE_RtkExtractor=false
PRODUCT_PROPERTY_OVERRIDES += persist.media.RTSP_USE_RTPLAYER=false
PRODUCT_PROPERTY_OVERRIDES += persist.media.MMS_USE_RTPLAYER=false
endif
PRODUCT_PROPERTY_OVERRIDES += persist.media.DisableInband=false

ifeq ($(USE_RTK_MEDIA_PROVIDER), YES)
PRODUCT_PROPERTY_OVERRIDES += persist.media.RTKMediaProvider=true
else
PRODUCT_PROPERTY_OVERRIDES += persist.media.RTKMediaProvider=false
endif
ifeq ($(ENABLE_DRM_PLUGIN_SERVICE), true)
PRODUCT_PROPERTY_OVERRIDES += drm.service.enabled=true
PRODUCT_PACKAGES += libOMX.realtek.video.dec.avc.secure
else
PRODUCT_PROPERTY_OVERRIDES += drm.service.enabled=false
endif

ifeq ($(USE_AIRFUN_MANAGER), YES)
PRODUCT_PROPERTY_OVERRIDES += ro.config.with_airfun=true
else
PRODUCT_PROPERTY_OVERRIDES += ro.config.with_airfun=false
endif

PRODUCT_PROPERTY_OVERRIDES += persist.sys.RunRTKIniSettings=false
PRODUCT_PROPERTY_OVERRIDES += ro.config.eventdelayms=33
PRODUCT_PROPERTY_OVERRIDES += persist.storage.resizefs=0
PRODUCT_PROPERTY_OVERRIDES += persist.rtk.forceOrientation=0
PRODUCT_PROPERTY_OVERRIDES += persist.rtk.mb.dlna.disable=false
PRODUCT_PROPERTY_OVERRIDES += persist.rtk.mb.samba.disable=false
PRODUCT_PROPERTY_OVERRIDES += persist.rtk.abstract_socket=true
PRODUCT_PROPERTY_OVERRIDES += ro.hdmi.device_type=4
PRODUCT_PROPERTY_OVERRIDES += ro.hdmi.cec_vendor_id=32640
PRODUCT_PROPERTY_OVERRIDES += ro.hdmi.cec_osd_name=DMP
PRODUCT_PROPERTY_OVERRIDES += rtk.screen.HideNavigationBar=1
PRODUCT_PROPERTY_OVERRIDES += persist.sys.hdmi.addr.playback=4
PRODUCT_PROPERTY_OVERRIDES += persist.sys.hdmi.resolution=Max
PRODUCT_PROPERTY_OVERRIDES += rtk.omx.hevc_performance_mode=0
PRODUCT_PROPERTY_OVERRIDES += persist.monitor.enable=1
PRODUCT_PROPERTY_OVERRIDES += persist.camera.no_deinterlace=0

user_variant := $(filter user userdebug,$(TARGET_BUILD_VARIANT))
ifneq (,$(user_variant))
# ro.adb.secure is required for passing CTS
PRODUCT_PROPERTY_OVERRIDES += ro.adb.secure=1
else
# for eng build, no auth is required
PRODUCT_PROPERTY_OVERRIDES += ro.adb.secure=0
endif

# wifi switching profile:
#   native: native Android behavior, STA/P2P & SoftAP switches are independent
#   toggle: WiFi is toggled between STA/P2P & SoftAP mode
PRODUCT_PROPERTY_OVERRIDES += persist.rtk.wifi.switchProfile=native
# wifi initial mode: sta / softap
PRODUCT_PROPERTY_OVERRIDES += ro.rtk.wifi.initial_mode=sta
# wifi mac as default device name : true or false
PRODUCT_PROPERTY_OVERRIDES += ro.rtk.wifi.mac.as.devicename=false
PRODUCT_PROPERTY_OVERRIDES += persist.rtk.wifi.fisrt.boot=true
PRODUCT_PROPERTY_OVERRIDES += ro.boot.wificountrycode=US

PRODUCT_PROPERTY_OVERRIDES += persist.rtk.ctstest=0
PRODUCT_PROPERTY_OVERRIDES += persist.rtk.video.maxAcqCnt=3
PRODUCT_PROPERTY_OVERRIDES += persist.rtk.cameraMode=1
PRODUCT_PROPERTY_OVERRIDES += persist.rtk.deepcolor=off
PRODUCT_PROPERTY_OVERRIDES += persist.rtk.hdmirx.raw=0
PRODUCT_PROPERTY_OVERRIDES += persist.rtk.vold.trymount=1
PRODUCT_PROPERTY_OVERRIDES += persist.rtk.vold.checkvolume=0
PRODUCT_PROPERTY_OVERRIDES += persist.rtk.vold.formatonerror=0
PRODUCT_PROPERTY_OVERRIDES += persist.rtk.hdmi.audio=on
#RTK USB Type C contorller
PRODUCT_PROPERTY_OVERRIDES += sys.usb.controller="98020000.dwc3_drd"

# GPU AFBC
ifeq ($(USE_AFBC), true)
PRODUCT_PROPERTY_OVERRIDES += persist.gralloc.afbc.fb=1
PRODUCT_PROPERTY_OVERRIDES += persist.gralloc.afbc.texture=1
else
PRODUCT_PROPERTY_OVERRIDES += persist.gralloc.afbc.fb=0
PRODUCT_PROPERTY_OVERRIDES += persist.gralloc.afbc.texture=0
endif
###################################################
# built-in libraries
###################################################
PRODUCT_PACKAGES += audio.usb.default
PRODUCT_PACKAGES += audio.a2dp.default
PRODUCT_PACKAGES += audio.r_submix.default
PRODUCT_PACKAGES += audio.primary.kylin
PRODUCT_PACKAGES += camera.kylin
PRODUCT_PACKAGES += dptx.kylin
PRODUCT_PACKAGES += hdmi.kylin
PRODUCT_PACKAGES += mhl.kylin
#PRODUCT_PACKAGES += rtk_cec.kylin
PRODUCT_PACKAGES += hdmi_cec.kylin
PRODUCT_PACKAGES += tv_input.kylin
PRODUCT_PACKAGES += hdcp.kylin
PRODUCT_PACKAGES += libhwse.kylin
PRODUCT_PACKAGES += hwcomposer.kylin

PRODUCT_PACKAGES += libsrec_jni
PRODUCT_PACKAGES += libaudioroute
PRODUCT_PACKAGES += libstagefrighthw
PRODUCT_PACKAGES += libRTK_lib
#PRODUCT_PACKAGES += libRTK_OSAL_memory
#PRODUCT_PACKAGES += libRTKOMX_OSAL
PRODUCT_PACKAGES += libRTKOMX_OSAL_RTK
PRODUCT_PACKAGES += libOMX_Core
PRODUCT_PACKAGES += libjpu
PRODUCT_PACKAGES += libvpu
PRODUCT_PACKAGES += libve3
PRODUCT_PACKAGES += libOMX.realtek.video.dec
PRODUCT_PACKAGES += libOMX.realtek.video.enc
PRODUCT_PACKAGES += libOMX.realtek.video.dec.vp9
PRODUCT_PACKAGES += libOMX.realtek.image.dec
PRODUCT_PACKAGES += libOMX.realtek.image.enc
PRODUCT_PACKAGES += libOMX.realtek.audio.dec
PRODUCT_PACKAGES += libRTKHDMIControl
PRODUCT_PACKAGES += libRTKVoutUtil
PRODUCT_PACKAGES += libRTKAoutUtil
PRODUCT_PACKAGES += libRTKSetupClass
PRODUCT_PACKAGES += libRT_IPC
PRODUCT_PACKAGES += libRtkTranscodePlayer
PRODUCT_PACKAGES += libRTMediaPlayer
PRODUCT_PACKAGES += libRTDtvMediaPlayer
PRODUCT_PACKAGES += lib_driver_cmd_rtl
PRODUCT_PACKAGES += librvsd
PRODUCT_PACKAGES += libsettingsclient
PRODUCT_PACKAGES += libRtkRpcClientServer
PRODUCT_PACKAGES += librealtek_runtime
PRODUCT_PACKAGES += libaudioParser
PRODUCT_PACKAGES += libRtkAudioRedirect
PRODUCT_PACKAGES += libiconv
PRODUCT_PACKAGES += librtksmb
PRODUCT_PACKAGES += libDLNADMPClass
PRODUCT_PACKAGES += libMCPControl
PRODUCT_PACKAGES += libRTKASFExtractor
PRODUCT_PACKAGES += libRTKAIFFExtractor
PRODUCT_PACKAGES += libRTKAllocator
PRODUCT_PACKAGES += libRTKFLVExtractor
PRODUCT_PACKAGES += libRTKAPEExtractor
PRODUCT_PACKAGES += libRTKHEIFExtractor

ifeq ($(USE_RTK_EXTRACTOR), true)
PRODUCT_PACKAGES += libRTKExtractor
endif

ifeq ($(ENABLE_TEE_DRM_FLOW),true)
PRODUCT_PACKAGES += libTEEapi
PRODUCT_PROPERTY_OVERRIDES += ro.rtk.drm.addrMode=2
else
PRODUCT_PROPERTY_OVERRIDES += ro.rtk.drm.addrMode=3
endif

ifeq ($(USE_RTK_GPIO), true)
PRODUCT_PACKAGES += rtk_gpio.kylin
PRODUCT_PACKAGES += libRTKGPIO
PRODUCT_PACKAGES += gpioproxy
PRODUCT_PROPERTY_OVERRIDES += ro.config.enablegpioservice=true
else
PRODUCT_PROPERTY_OVERRIDES += ro.config.enablegpioservice=false
endif

PRODUCT_PACKAGES += libwpa_client
PRODUCT_PACKAGES += wpa_cli
PRODUCT_PACKAGES += wpa_supplicant
PRODUCT_PACKAGES += wpa_supplicant.conf
PRODUCT_PACKAGES += rtwpriv
PRODUCT_PACKAGES += rtwpriv_org

PRODUCT_PACKAGES += libsideband
PRODUCT_PACKAGES += sideband

PRODUCT_PACKAGES += libRTKHWMBinderapi
PRODUCT_PACKAGES += RtkHWMBinder
#PRODUCT_PACKAGES += audio_firmware
#PRODUCT_PACKAGES += libfw_socket_client

PRODUCT_PACKAGES += libwfdisplay
PRODUCT_PACKAGES += rtk_wfd

###################################################
# built-in binaries
###################################################
PRODUCT_PACKAGES += coda980_dec_test
PRODUCT_PACKAGES += coda980_enc_test
PRODUCT_PACKAGES += w4_dec_test
PRODUCT_PACKAGES += vp9dec
PRODUCT_PACKAGES += vpuinit
PRODUCT_PACKAGES += jpurun

PRODUCT_PACKAGES += factory
PRODUCT_PACKAGES += multilan
PRODUCT_PACKAGES += settingsproxy

ifeq ($(TARGET_BUILD_VARIANT), eng)
PRODUCT_PACKAGES += gatord
endif

## add for Android L
#ifeq ($(BOARD_WIFI_VENDOR), realtek)
PRODUCT_PACKAGES += rtw_fwloader
PRODUCT_PACKAGES += hostapd
PRODUCT_PACKAGES += hostapd_cli
#endif

ifeq ($(USE_RTK_PPPOE), YES)
PRODUCT_PACKAGES += pppoe
endif
ifeq ($(USE_RTK_OPENWRT), YES)
PRODUCT_PACKAGES += com.rtk.net.ubusrpc
endif
###################################################
# built-in files
###################################################
PRODUCT_PACKAGES += dhcpcd.conf

###################################################
# built-in applications
###################################################
PRODUCT_PACKAGES += RtkLauncher2
PRODUCT_PACKAGES += TvProvider
PRODUCT_PACKAGES += RTKSourceIn
PRODUCT_PACKAGES += DMR
PRODUCT_PACKAGES += RealtekGallery2
#PRODUCT_PACKAGES += RealtekInitialSettings
PRODUCT_PACKAGES += RealtekSoftwareUpdater
#PRODUCT_PACKAGES += RealtekSettings
PRODUCT_PACKAGES += Settings
PRODUCT_PACKAGES += Music
PRODUCT_PACKAGES += MediaBrowser
PRODUCT_PACKAGES += Browser2
PRODUCT_PACKAGES += RealtekQuickSearchBox
PRODUCT_PACKAGES += RTKControlPanel
#PRODUCT_PACKAGES += ApplicationsProvider
PRODUCT_PACKAGES += ToGo
PRODUCT_PACKAGES += RTKMiracastSink
PRODUCT_PACKAGES += RealtekSystemUI
#PRODUCT_PACKAGES += SystemUI
PRODUCT_PACKAGES += RealtekSuspendHandler
PRODUCT_PACKAGES += RealtekPIP
PRODUCT_PACKAGES += RealtekPIPRecord
PRODUCT_PACKAGES += RealtekMediaRecoderDemo
PRODUCT_PACKAGES += RealtekFloatingWindowDemo
PRODUCT_PACKAGES += RealtekEncoder

#ifeq ($(TARGET_BUILD_VARIANT), eng)
PRODUCT_LOCALES := en_US zh_TW zh_CN
#else # user build
#PRODUCT_LOCALES := en_US zh_TW zh_CN
#endif

PRODUCT_COPY_FILES += device/realtek/kylin/venus_IR_input.kl:system/usr/keylayout/venus_IR_input.kl
PRODUCT_COPY_FILES += device/realtek/kylin/venus_IR_input.kcm:system/usr/keychars/venus_IR_input.kcm

#Wifi Module Mapper
#PRODUCT_COPY_FILES += device/realtek/kylin/wifi_module_list.cfg:system/etc/wifi/wifi_module_list.cfg
PRODUCT_COPY_FILES += device/realtek/kylin/wifi_module_mapper.json:system/etc/wifi/wifi_module_mapper.json

ifeq ($(ENABLE_LIGHTTPD), true)
PRODUCT_COPY_FILES += $(call find-copy-subdir-files,*.html,device/realtek/www/rtCGI/html,system/data/www/htdocs)
PRODUCT_COPY_FILES += $(call find-copy-subdir-files,*,device/realtek/www/rtCGI/images,system/data/www/htdocs/images)
PRODUCT_COPY_FILES += $(call find-copy-subdir-files,*,device/realtek/www/rtCGI/css,system/data/www/htdocs/css)
PRODUCT_COPY_FILES += $(call find-copy-subdir-files,*,device/realtek/www/rtCGI/javascript,system/data/www/htdocs/javascript)
PRODUCT_COPY_FILES += $(call find-copy-subdir-files,*,device/realtek/www/rtCGI/jslib,system/data/www/htdocs/jslib)
endif

#ifeq ($(USE_LAYOUT_MEM_LOW), true)
#  PRODUCT_PROPERTY_OVERRIDES += dalvik.vm.heapsize=192m
#  PRODUCT_PROPERTY_OVERRIDES += dalvik.vm.heapgrowthlimit=128m
#
#  PRODUCT_PROPERTY_OVERRIDES += ro.config.low_ram=true
#  PRODUCT_PROPERTY_OVERRIDES += dalvik.vm.jit.codecachesize=0
#else
#  PRODUCT_PROPERTY_OVERRIDES += dalvik.vm.heapsize=384m
#  PRODUCT_PROPERTY_OVERRIDES += dalvik.vm.heapgrowthlimit=128m
#endif

#gms
$(call inherit-product-if-exists, vendor/google/products/gms.mk)
PRODUCT_PROPERTY_OVERRIDES += ro.com.google.clientidbase=android-acme
PRODUCT_PROPERTY_OVERRIDES += ro.product.first_api_level=23

# Include deviceCommon.mk at end of file for purpose
# If DvdPlayer exists in both common & kylin directoy,
# only DvdPlayer in kylin will be copied to /system/bin
include device/realtek/kylin/common/deviceCommon.mk

