#
# Copyright 2013 The Android Open-Source Project
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#
#USE_LEANBACK_UI := true

$(call inherit-product, $(SRC_TARGET_DIR)/product/aosp_base.mk)
$(call inherit-product, device/realtek/phoenix/device.mk)
#rtkbt
$(call inherit-product, device/realtek/phoenix/rtkbt/rtkbt.mk)

PRODUCT_BRAND  := realtek
PRODUCT_NAME   := rtk_phoenix
PRODUCT_DEVICE := phoenix
PRODUCT_MODEL  := phoenix
PRODUCT_MANUFACTURER := realtek

NUM_FRAMEBUFFER_SURFACE_BUFFERS := 3
##USE_LAYOUT_UI_1080 := true
##USE_LAYOUT_MEM_LOW := true

USE_RT_MEDIA_PLAYER := true
ENABLE_LIGHTTPD := true
ENABLE_RTK_HDMI_SERVICE := true
ENABLE_RTK_CEC_SERVICE := false
USE_SETTINGS_SERVICE := true
ENABLE_MEDIA_QUERY := true
ENABLE_RTK_HDMIRX_SERVICE := true
# For DRM code flow
ENABLE_DRM_PLUGIN_SERVICE := true
# For TEE OS, video used physical Address
ENABLE_TEE_DRM_FLOW	:= false

USE_RTK_HDMI_CONTROL := NO
USE_RTK_MHL_CONTROL := NO
USE_RTK_HDCP1x_CONTROL := NO
USE_RTK_HDCP22_CONTROL := NO
USE_RTK_CEC_CONTROL := NO
USE_RTK_VOUT_UTIL := YES
USE_RTK_AOUT_UTIL := YES
USE_RTK_SETUP_CLASS := NO

USE_RT_ION := true
USE_RTK_EXTRACTOR := true

USE_RTK_MEDIA_PROVIDER := YES
USE_DTCPIP_SIK := NO
USE_TV_MANAGER := YES
USE_TOGO_MANAGER := YES
USE_AIRFUN_MANAGER := YES

USE_RTK_UART_BT := YES
USE_JPU := NO
USE_RTK_PPPOE := YES
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
PRODUCT_PROPERTY_OVERRIDES += ro.config.enablemhlserivce=true
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
PRODUCT_PROPERTY_OVERRIDES += persist.media.USE_RTMediaPlayer=true
PRODUCT_PROPERTY_OVERRIDES += persist.media.USE_RtkExtractor=false
PRODUCT_PROPERTY_OVERRIDES += persist.media.RTSP_USE_RTPLAYER=true
PRODUCT_PROPERTY_OVERRIDES += persist.media.MMS_USE_RTPLAYER=true
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
PRODUCT_PACKAGES += libOMX.realtek.video.dec.secure
else
PRODUCT_PROPERTY_OVERRIDES += drm.service.enabled=false
endif

PRODUCT_PROPERTY_OVERRIDES += persist.sys.RunRTKIniSettings=false
PRODUCT_PROPERTY_OVERRIDES += ro.config.eventdelayms=33
PRODUCT_PROPERTY_OVERRIDES += persist.storage.resizefs=0
PRODUCT_PROPERTY_OVERRIDES += rtk.screen.forceOrientation=0
PRODUCT_PROPERTY_OVERRIDES += persist.rtk.mb.dlna.disable=false
PRODUCT_PROPERTY_OVERRIDES += persist.rtk.mb.samba.disable=false
PRODUCT_PROPERTY_OVERRIDES += persist.rtk.abstract_socket=false
PRODUCT_PROPERTY_OVERRIDES += ro.hdmi.device_type=4
PRODUCT_PROPERTY_OVERRIDES += ro.hdmi.cec_vendor_id=32640
PRODUCT_PROPERTY_OVERRIDES += ro.hdmi.cec_osd_name=DMP
PRODUCT_PROPERTY_OVERRIDES += rtk.screen.HideNavigationBar=1 
PRODUCT_PROPERTY_OVERRIDES += persist.sys.hdmi.addr.playback=4
PRODUCT_PROPERTY_OVERRIDES += persist.sys.hdmi.resolution=Max

# wifi switching profile:
#   native: native Android behavior, STA/P2P & SoftAP switches are independent
#   toggle: WiFi is toggled between STA/P2P & SoftAP mode
PRODUCT_PROPERTY_OVERRIDES += persist.rtk.wifi.switchProfile=native
# wifi initial mode: sta / softap
PRODUCT_PROPERTY_OVERRIDES += ro.rtk.wifi.initial_mode=sta
# wifi mac as default device name : true or false
PRODUCT_PROPERTY_OVERRIDES += ro.rtk.wifi.mac.as.devicename=false
PRODUCT_PROPERTY_OVERRIDES += persist.rtk.wifi.fisrt.boot=true

PRODUCT_PROPERTY_OVERRIDES += persist.rtk.ctstest=0
PRODUCT_PROPERTY_OVERRIDES += persist.rtk.cameraMode=1

PRODUCT_PACKAGES += RtkLauncher2
PRODUCT_PACKAGES += TvProvider
#PRODUCT_PACKAGES += TvSettings
#PRODUCT_PACKAGES += TvQuickSettings
PRODUCT_PACKAGES += RealtekSettings
# Disable RealtekTvSettings, add LEANBACK_SETTINGS in RealtekSettings to let leanback launcher able to call RealtekSettings
# Enable RealtekTvSettings for Initial Setup wizard
# only build RealtekTvSettings if USE_LEANBACK_UI is true

ifeq ($(USE_LEANBACK_UI), true)
PRODUCT_PACKAGES += RealtekTvSettings
#PRODUCT_PACKAGES += RealtekTvQuickSettings
PRODUCT_PACKAGES += RealtekAppDrawer
endif

PRODUCT_PACKAGES += RTKSourceIn
PRODUCT_PACKAGES += wpa_supplicant.conf
PRODUCT_PACKAGES += libsrec_jni
PRODUCT_PACKAGES += libaudioroute
PRODUCT_PACKAGES += camera.$(PRODUCT_DEVICE)
PRODUCT_PACKAGES += audio.primary.$(PRODUCT_DEVICE)
PRODUCT_PACKAGES += audio.a2dp.default
#PRODUCT_PACKAGES += audio.usb.default
PRODUCT_PACKAGES += audio.r_submix.default
PRODUCT_PACKAGES += hdmi.$(PRODUCT_DEVICE)
PRODUCT_PACKAGES += mhl.$(PRODUCT_DEVICE)
# Realtek HDMI_CEC_HAL
PRODUCT_PACKAGES += rtk_cec.$(PRODUCT_DEVICE)
# Android L HDMI_CEC_Hal
PRODUCT_PACKAGES += hdmi_cec.$(PRODUCT_DEVICE)
# Android L TV Input HAL
PRODUCT_PACKAGES += tv_input.$(PRODUCT_DEVICE)
PRODUCT_PACKAGES += hdcp.$(PRODUCT_DEVICE)
PRODUCT_PACKAGES += rtk_gpio.$(PRODUCT_DEVICE)
PRODUCT_PACKAGES += libRTKGPIO
PRODUCT_PACKAGES += gpioproxy
PRODUCT_PACKAGES += libRTKHWMBinderapi
PRODUCT_PACKAGES += RtkHWMBinder

PRODUCT_PACKAGES += libstagefrighthw
PRODUCT_PACKAGES += libhwse.$(PRODUCT_DEVICE)
PRODUCT_PACKAGES += libRTK_lib
PRODUCT_PACKAGES += libRTK_OSAL_memory
PRODUCT_PACKAGES += libRTKOMX_OSAL
PRODUCT_PACKAGES += libOMX_Core
PRODUCT_PACKAGES += libjpu
PRODUCT_PACKAGES += libvpu
PRODUCT_PACKAGES += vpurun
PRODUCT_PACKAGES += vpuinit
PRODUCT_PACKAGES += jpurun
PRODUCT_PACKAGES += jpuinit
PRODUCT_PACKAGES += libOMX.realtek.video.dec
PRODUCT_PACKAGES += libOMX.realtek.video.enc
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
PRODUCT_PACKAGES += libwpa_client
PRODUCT_PACKAGES += wpa_cli
PRODUCT_PACKAGES += wpa_supplicant
PRODUCT_PACKAGES += dhcpcd.conf
PRODUCT_PACKAGES += lib_driver_cmd_rtl
PRODUCT_PACKAGES += rtwpriv
PRODUCT_PACKAGES += rtwpriv_org
PRODUCT_PACKAGES += librvsd
PRODUCT_PACKAGES += settingsproxy
PRODUCT_PACKAGES += libsettingsclient
PRODUCT_PACKAGES += hwcomposer.phoenix
PRODUCT_PACKAGES += libRtkRpcClientServer
PRODUCT_PACKAGES += librealtek_runtime
PRODUCT_PACKAGES += libaudioParser
PRODUCT_PACKAGES += libRtkAudioRedirect
PRODUCT_PACKAGES += libiconv
PRODUCT_PACKAGES += factory
PRODUCT_PACKAGES += multilan

ifeq ($(USE_RTK_EXTRACTOR), true)
PRODUCT_PACKAGES += libRTKExtractor
endif

ifeq ($(TARGET_BUILD_VARIANT), eng)
PRODUCT_PACKAGES += gatord
endif

ifeq ($(ENABLE_TEE_DRM_FLOW),true)
PRODUCT_PACKAGES += libTEEapi
PRODUCT_PROPERTY_OVERRIDES += ro.rtk.drm.addrMode=2
else
PRODUCT_PROPERTY_OVERRIDES += ro.rtk.drm.addrMode=3
endif

#ifeq ($(TARGET_BUILD_VARIANT), eng)
PRODUCT_LOCALES := en_US zh_TW zh_CN
#else # user build
#PRODUCT_LOCALES := en_US zh_TW zh_CN
#endif

PRODUCT_PACKAGES += DMR

PRODUCT_PACKAGES += RealtekGallery2

#PRODUCT_PACKAGES += RealtekInitialSettings
PRODUCT_PACKAGES += RealtekSoftwareUpdater
### ### PRODUCT_PACKAGES += RealtekSettings
PRODUCT_PACKAGES += Settings

PRODUCT_PACKAGES += MediaBrowser
PRODUCT_PACKAGES += Browser2
PRODUCT_PACKAGES += RealtekQuickSearchBox
PRODUCT_PACKAGES += RTKControlPanel
PRODUCT_PACKAGES += librtksmb
### ### PRODUCT_PACKAGES += ApplicationsProvider
PRODUCT_PACKAGES += ToGo
PRODUCT_PACKAGES += libwfdisplay
PRODUCT_PACKAGES += rtk_wfd
PRODUCT_PACKAGES += libMCPControl
PRODUCT_PACKAGES += RTKMiracastSink
#PRODUCT_PACKAGES += RealtekSystemUI
PRODUCT_PACKAGES += SystemUI

PRODUCT_PACKAGES += RealtekSuspendHandler

## add for Android L
#ifeq ($(BOARD_WIFI_VENDOR), realtek)
PRODUCT_PACKAGES += rtw_fwloader
PRODUCT_PACKAGES += hostapd
PRODUCT_PACKAGES += hostapd_cli
#endif

ifeq ($(USE_RTK_PPPOE), YES)
PRODUCT_PACKAGES += pppoe
endif

PRODUCT_COPY_FILES += device/realtek/phoenix/venus_IR_input.kl:system/usr/keylayout/venus_IR_input.kl
PRODUCT_COPY_FILES += device/realtek/phoenix/venus_IR_input.kcm:system/usr/keychars/venus_IR_input.kcm

#Wifi Module Mapper
#PRODUCT_COPY_FILES += device/realtek/phoenix/wifi_module_list.cfg:system/etc/wifi/wifi_module_list.cfg
PRODUCT_COPY_FILES += device/realtek/phoenix/wifi_module_mapper.json:system/etc/wifi/wifi_module_mapper.json

ifeq ($(USE_LEANBACK_UI), true)
ifeq ($(USE_LAYOUT_UI_1080), true)
PRODUCT_PROPERTY_OVERRIDES += ro.sf.lcd_density=320
else
PRODUCT_PROPERTY_OVERRIDES += ro.sf.lcd_density=200
endif
else
ifeq ($(USE_LAYOUT_UI_1080), true)
PRODUCT_PROPERTY_OVERRIDES += ro.sf.lcd_density=240
else
PRODUCT_PROPERTY_OVERRIDES += ro.sf.lcd_density=160
endif
endif

ifeq ($(ENABLE_LIGHTTPD), true)
PRODUCT_COPY_FILES += $(call find-copy-subdir-files,*.html,device/realtek/www/rtCGI/html,system/data/www/htdocs)
PRODUCT_COPY_FILES += $(call find-copy-subdir-files,*,device/realtek/www/rtCGI/images,system/data/www/htdocs/images)
PRODUCT_COPY_FILES += $(call find-copy-subdir-files,*,device/realtek/www/rtCGI/css,system/data/www/htdocs/css)
PRODUCT_COPY_FILES += $(call find-copy-subdir-files,*,device/realtek/www/rtCGI/javascript,system/data/www/htdocs/javascript)
PRODUCT_COPY_FILES += $(call find-copy-subdir-files,*,device/realtek/www/rtCGI/jslib,system/data/www/htdocs/jslib)
endif

ifeq ($(USE_LAYOUT_MEM_LOW), true)
  PRODUCT_PROPERTY_OVERRIDES += \
        dalvik.vm.heapsize=192m \
        dalvik.vm.heapgrowthlimit=192m

  PRODUCT_PROPERTY_OVERRIDES += ro.config.low_ram=true
  PRODUCT_PROPERTY_OVERRIDES += dalvik.vm.jit.codecachesize=0
else
## default ##
  PRODUCT_PROPERTY_OVERRIDES += \
        dalvik.vm.heapsize=384m \
        dalvik.vm.heapgrowthlimit=128m
endif


