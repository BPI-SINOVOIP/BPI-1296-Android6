BOARD_HAVE_BLUETOOTH := true
BOARD_HAVE_BLUETOOTH_RTK := true
BOARD_HAVE_BLUETOOTH_RTK_COEX := true

BOARD_BLUETOOTH_BDROID_BUILDCFG_INCLUDE_DIR := $(LOCAL_PATH)/bluetooth

PRODUCT_PACKAGES += \
    Bluetooth \
    audio.a2dp.default

PRODUCT_COPY_FILES += \
        frameworks/native/data/etc/android.hardware.bluetooth.xml:system/etc/permissions/android.hardware.bluetooth.xml \
        frameworks/native/data/etc/android.hardware.bluetooth_le.xml:system/etc/permissions/android.hardware.bluetooth_le.xml \


$(call inherit-product-if-exists, $(LOCAL_PATH)/firmware/usb/rtl8723a/device-rtl.mk)
$(call inherit-product-if-exists, $(LOCAL_PATH)/firmware/usb/rtl8723b/device-rtl.mk)
$(call inherit-product-if-exists, $(LOCAL_PATH)/firmware/usb/rtl8703b/device-rtl.mk)
$(call inherit-product-if-exists, $(LOCAL_PATH)/firmware/usb/rtl8761a/device-rtl.mk)
$(call inherit-product-if-exists, $(LOCAL_PATH)/firmware/usb/rtl8821a/device-rtl.mk)
$(call inherit-product-if-exists, $(LOCAL_PATH)/firmware/usb/rtl8822b/device-rtl.mk)

$(call inherit-product-if-exists, $(LOCAL_PATH)/firmware/uart/rtlbtfw_cfg.mk)

$(call inherit-product-if-exists, $(LOCAL_PATH)/conf/conf.mk)
$(call inherit-product-if-exists, $(LOCAL_PATH)/plugin/plugin.mk)

