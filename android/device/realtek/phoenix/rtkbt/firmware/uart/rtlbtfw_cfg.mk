PRODUCT_COPY_FILES += \
                $(LOCAL_PATH)/rtl8723a_fw:system/etc/firmware/rtl8723as_fw \
                $(LOCAL_PATH)/rtl8723a_config:system/etc/firmware/rtl8723as_config \
                $(LOCAL_PATH)/rtl8723b_fw:system/etc/firmware/rtl8723bs_fw \
                $(LOCAL_PATH)/rtl8723b_config:system/etc/firmware/rtl8723bs_config \
                $(LOCAL_PATH)/rtl8761a_fw:system/etc/firmware/rtl8761as_fw \
                $(LOCAL_PATH)/rtl8821a_fw:system/etc/firmware/rtl8821as_fw   \
                $(LOCAL_PATH)/rtl8821a_config:system/etc/firmware/rtl8821as_config \
                $(LOCAL_PATH)/rtl8723a_fw:system/etc/firmware/mp_rtl8723a_fw \
                $(LOCAL_PATH)/rtl8723b_fw:system/etc/firmware/mp_rtl8723b_fw \
                $(LOCAL_PATH)/rtl8761a_fw:system/etc/firmware/mp_rtl8761a_fw \
                $(LOCAL_PATH)/rtl8821a_fw:system/etc/firmware/mp_rtl8821a_fw

ifeq ($(BOARD_HAVE_BLUETOOTH_RTK_COEX), true)
PRODUCT_COPY_FILES += \
                $(LOCAL_PATH)/rtl8761a_config.coex:system/etc/firmware/rtl8761as_config
else
PRODUCT_COPY_FILES += \
                $(LOCAL_PATH)/rtl8761a_config:system/etc/firmware/rtl8761as_config
endif
