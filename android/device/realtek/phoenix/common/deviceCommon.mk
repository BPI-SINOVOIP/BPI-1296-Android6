PRODUCT_COPY_FILES += device/realtek/phoenix/common/init.e2fsck.rc:root/init.e2fsck.rc
PRODUCT_COPY_FILES += device/realtek/phoenix/common/init.nand.rc:root/init.nand.rc
PRODUCT_COPY_FILES += device/realtek/phoenix/common/init.emmc.rc:root/init.emmc.rc
PRODUCT_COPY_FILES += device/realtek/phoenix/common/dhcpcd.conf:system/etc/dhcpcd/dhcpcd.conf
PRODUCT_COPY_FILES += device/realtek/phoenix/common/media_profiles.xml:system/etc/media_profiles.xml
PRODUCT_COPY_FILES += device/realtek/phoenix/common/media_codecs.xml:system/etc/media_codecs.xml
PRODUCT_COPY_FILES += device/realtek/phoenix/common/media_codecs_rtk_video.xml:system/etc/media_codecs_rtk_video.xml
PRODUCT_COPY_FILES += device/realtek/phoenix/common/media_codecs_rtk_audio.xml:system/etc/media_codecs_rtk_audio.xml
PRODUCT_COPY_FILES += frameworks/av/media/libstagefright/data/media_codecs_google_audio.xml:system/etc/media_codecs_google_audio.xml
PRODUCT_COPY_FILES += frameworks/av/media/libstagefright/data/media_codecs_google_video.xml:system/etc/media_codecs_google_video.xml
PRODUCT_COPY_FILES += device/realtek/phoenix/common/mixer_paths.xml:system/etc/mixer_paths.xml
PRODUCT_COPY_FILES += device/realtek/phoenix/common/content-types.properties:system/lib/content-types.properties

# prebuilt binaries / modules / libraries
PRODUCT_COPY_FILES += $(call find-copy-subdir-files,*,device/realtek/phoenix/common/prebuilt/system,system)

