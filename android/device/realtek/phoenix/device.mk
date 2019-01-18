PRODUCT_AAPT_CONFIG := normal large tvdpi mdpi hdpi
PRODUCT_AAPT_PREF_CONFIG := hdpi
PRODUCT_CHARACTERISTICS := tablet

DEVICE_PACKAGE_OVERLAYS := device/realtek/phoenix/overlay

PRODUCT_COPY_FILES += $(call find-copy-subdir-files,*,device/realtek/phoenix/prebuilt/system,system)
PRODUCT_COPY_FILES += $(call find-copy-subdir-files,*,device/realtek/phoenix/prebuilt/root,root)

PRODUCT_COPY_FILES += system/core/rootdir/init.rc:root/init.rc
PRODUCT_COPY_FILES += device/realtek/phoenix/init.phoenix.rc:root/init.phoenix.rc
PRODUCT_COPY_FILES += device/realtek/phoenix/init.phoenix.usb.rc:root/init.phoenix.usb.rc
PRODUCT_COPY_FILES += device/realtek/phoenix/ueventd.phoenix.rc:root/ueventd.phoenix.rc
PRODUCT_COPY_FILES += device/realtek/phoenix/audio_policy.conf:system/etc/audio_policy.conf
PRODUCT_COPY_FILES += device/realtek/phoenix/fstab.phoenix.emmc:root/fstab.phoenix.emmc
PRODUCT_COPY_FILES += device/realtek/phoenix/fstab.phoenix:root/fstab.phoenix
PRODUCT_COPY_FILES += device/realtek/phoenix/sys.part.rc:root/sys.part.rc
PRODUCT_COPY_FILES += device/realtek/phoenix/init.e2fsck.rc:root/init.e2fsck.rc
PRODUCT_COPY_FILES += device/realtek/phoenix/init.resize2fs.rc:root/init.resize2fs.rc

PRODUCT_COPY_FILES += device/realtek/phoenix/init.unicorn.rc:root/init.unicorn.rc
PRODUCT_COPY_FILES += device/realtek/phoenix/ueventd.unicorn.rc:root/ueventd.unicorn.rc

ifeq ($(USE_LEANBACK_UI), true)
PRODUCT_COPY_FILES += device/google/atv/permissions/tv_core_hardware.xml:system/etc/permissions/tv_core_hardware.xml
PRODUCT_COPY_FILES += frameworks/native/data/etc/android.hardware.bluetooth.xml:system/etc/permissions/android.hardware.bluetooth.xml
else
PRODUCT_COPY_FILES += frameworks/native/data/etc/tablet_core_hardware.xml:system/etc/permissions/tablet_core_hardware.xml
endif
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
PRODUCT_TAGS += dalvik.gc.type-precise

# setup dalvik vm configs.
PRODUCT_PROPERTY_OVERRIDES += \
    dalvik.vm.heapstartsize=8m \
    dalvik.vm.heaptargetutilization=0.75 \
    dalvik.vm.heapminfree=2m \
    dalvik.vm.heapmaxfree=8m

# setup dex2oat compiler options: everything|speed|balanced|space|interpret-only|verify-none
PRODUCT_PROPERTY_OVERRIDES += dalvik.vm.dex2oat-filter=speed
PRODUCT_PROPERTY_OVERRIDES += dalvik.vm.image-dex2oat-filter=speed

# pre-optimization options
PRODUCT_DEX_PREOPT_BOOT_FLAGS := --compiler-filter=speed
PRODUCT_DEX_PREOPT_DEFAULT_FLAGS := --compiler-filter=speed
#$(call add-product-dex-preopt-module-config,services,--compiler-filter=space)

PRODUCT_PROPERTY_OVERRIDES += \
    persist.sys.timezone=Asia/Taipei \
    persist.sys.language=zh \
    persist.sys.country=TW \
    wifi.interface=wlan0 \
    wifi.supplicant_scan_interval=120 \
    ro.opengles.version=131072 \
    ro.zygote.disable_gl_preload=true \
    ro.kernel.android.checkjni=false \
    keyguard.no_require_sim=true \
    persist.sys.strictmode.visual=0 \
    persist.sys.strictmode.disable=1 \
    hwui.render_dirty_regions=false \
    net.dns1 = 168.95.192.1 \
    net.dns2 = 8.8.8.8

# Include deviceCommon.mk at end of file for purpose
# If DvdPlayer exists in both common & phoenix directoy,
# only DvdPlayer in phoenix will be copied to /system/bin
include device/realtek/phoenix/common/deviceCommon.mk

