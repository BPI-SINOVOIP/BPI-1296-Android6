#!/bin/bash

SCRIPTDIR=$PWD
KERNELDIR=$SCRIPTDIR/linux-kernel
ANDROIDDIR=$SCRIPTDIR/android
IMAGEDIR=$SCRIPTDIR/image_file_creator
TARGET_PACKAGE=package5
PACKAGE_DIR=$IMAGEDIR/components/packages/$TARGET_PACKAGE
DAILY_BUILD_DIR=$IMAGEDIR/dailybuild/CustBranch-QA160627-b/CustBranch-QA160627-nuplayer-2016-11-17
ERR=0

KERNEL_BOOT_DIR=$KERNELDIR/arch/arm64/boot
KUIMAGE=$KERNEL_BOOT_DIR/Image
RESCUE_DTB=$KERNEL_BOOT_DIR/dts/realtek/rtd-1295-qa-rescue.dtb
AUDIOFW=$SCRIPTDIR/bluecore.audio.nofile
AUDIOFW_ZIP=$DAILY_BUILD_DIR/$TARGET_PACKAGE/bluecore.audio.release.SQA.zip
AUDIOFW_MAP=$DAILY_BUILD_DIR/$TARGET_PACKAGE/System.map.release.SQA.audio
DVDPLAYER_BIN=$DAILY_BUILD_DIR/$TARGET_PACKAGE/system/bin/DvdPlayer
ALSADAEMON_BIN=$DAILY_BUILD_DIR/$TARGET_PACKAGE/system/bin/ALSADaemon
BUILDTYPE_ANDROID=`grep -s "lunch" build_release_android.sh | awk '{print $2}'`

cd image_file_creator/components/packages/package5
unlink root
unlink system

function build_cmd()
{
    $@
    ERR=$?
    printf "$* "
    if [ "$ERR" != "0" ]; then
        echo -e "\033[47;31m [ERROR] $ERR \033[0m"
        exit 1
    else
        echo "[OK]"
    fi
}

# Set default build :
[ "$IMAGE_DRAM_SIZE"        = "" ] && IMAGE_DRAM_SIZE=2GB
[ "$LAYOUT_TYPE"            = "" ] && LAYOUT_TYPE=emmc
[ "$LAYOUT_SIZE"            = "" ] && LAYOUT_SIZE=8gb
[ "$INSTALL_DTB"            = "" ] && INSTALL_DTB=1
[ "$INSTALL_AVFILE_COUNT"   = "" ] && INSTALL_AVFILE_COUNT=1
[ "$INSTALL_FACTORY"  	    = "" ] && INSTALL_FACTORY=0
[ "$TARGET_BOARD"           = "" ] && TARGET_BOARD=giraffe

# LAYOUT_TYPE=emmc
# LAYOUT_SIZE=4gb
# INSTALL_DTB=0
# INSTALL_AVFILE_COUNT=1
# TARGET_BOARD=giraffe

if [ "$INSTALL_DTB" = "1" ]; then
    if [ "$TARGET_BOARD" = "qa" ]; then
        ANDROID_DTB=$KERNEL_BOOT_DIR/dts/realtek/rtd-1295-qa-rescue.dtb
    elif [ "$TARGET_BOARD" = "giraffe" ]; then
        ANDROID_DTB=$KERNEL_BOOT_DIR/dts/realtek/rtd-1295-giraffe-$IMAGE_DRAM_SIZE.dtb
    fi
fi


prepare_image()
{

    ret=0
    ret=$?
    PRODUCT_DEVICE=`echo $BUILDTYPE_ANDROID|sed 's/-eng\|-user\|-userdebug//g'|sed 's/rtk_//g'`
    ANDROID_OUT=$ANDROIDDIR/out/target/product/${PRODUCT_DEVICE}
    ANDROID_ROOT=$ANDROID_OUT/root
    ANDROID_SYSTEM=$ANDROID_OUT/system

    if [ "$ret" != "0" ] || [ ! -d "$ANDROID_ROOT" ] || [ ! -d "$ANDROID_SYSTEM" ]; then
        echo ERROR!
        echo "PRODUCT_DEVICE : $PRODUCT_DEVICE"
        echo "ANDROID_ROOT   : $ANDROID_ROOT"
        echo "ANDROID_SYSTEM : $ANDROID_SYSTEM"
        return 2
    fi

    pushd $IMAGEDIR/components/packages/package5 > /dev/null
	[ -e root       ] && rm -r root
        ln -s $ANDROID_ROOT root

        [ -e system     ] && rm -r system
        ln -s $ANDROID_SYSTEM system	

        [ ! -e data     ] && mkdir data
        [ ! -e cache    ] && mkdir cache
        if [ "$LAYOUT_TYPE" = "emmc" ]; then
            > data/null_file
            > cache/null_file
        fi
        if [ -e $KUIMAGE ]; then
            echo copy Image:$KUIMAGE
            cp $KUIMAGE $LAYOUT_TYPE.uImage
            dd if=/dev/zero of=$LAYOUT_TYPE.uImage conv=notrunc oflag=append bs=1k count=512 > /dev/null 2>&1

            # fix output size : 6MB
            #SEEK_BYTE=$(stat -c%s $LAYOUT_TYPE.uImage)
            #APPEND_BYTE=`expr 6 \* 1024 \* 1024  - $SEEK_BYTE`
            #dd if=/dev/zero of=$LAYOUT_TYPE.uImage conv=notrunc oflag=append bs=$APPEND_BYTE count=1 > /dev/null 2>&1
        fi

        if [ -e $AUDIOFW_ZIP ]; then
            echo copy audio fw \(zip\) :$AUDIOFW_ZIP
            cp $AUDIOFW_ZIP bluecore.audio.zip
        fi

        if [ -e $AUDIOFW_MAP ]; then
            echo copy audio fw \(map\) :$AUDIOFW_MAP
            cp $AUDIOFW_MAP System.map.audio
        fi

        if [ -e $AUDIOFW ]; then
            echo copy audio fw:$AUDIOFW
            cp $AUDIOFW bluecore.audio
            #dd if=/dev/zero of=bluecore.audio conv=notrunc oflag=append bs=1k count=512 > /dev/null 2>&1
            zip bluecore.audio.zip bluecore.audio
        fi

        if [ -e $ANDROID_DTB ]; then
            echo copy android dtb:$ANDROID_DTB
            cp $ANDROID_DTB android.$LAYOUT_TYPE.dtb
        fi

        if [ -e $RESCUE_DTB ]; then
            echo copy rescue dtb:$RESCUE_DTODUCT_DEVICE
            cp $RESCUE_DTB rescue.$LAYOUT_TYPE.dtb
        fi

        PACKAGE_SYSTEM=$PACKAGE_DIR/system

        echo copy kernel module
        ANDROID_MODULES_DIR=$PACKAGE_SYSTEM/vendor/modules
	PHOENIXDIR=$SCRIPTDIR/phoenix
	PHOENIX_DRIVERS_DIR=$PHOENIXDIR/system/src/drivers/
	PHOENIX_EXTERNAL_DIR=$PHOENIXDIR/system/src/external/
        PARAGONDIR=$PHOENIXDIR/system/src/external/paragon
	#DRIVERDIR=$PHOENIXDIR/system/src/bin
        MALIKODIR=$KERNELDIR/modules/mali

        [ -d $PHOENIX_DRIVERS_DIR ] && ( find $PHOENIX_DRIVERS_DIR  -name *.ko |xargs -i cp {} $ANDROID_MODULES_DIR/ )
        [ -d $MALIKODIR   ] && ( find $MALIKODIR/       -name *.ko  |xargs -i cp {} $ANDROID_MODULES_DIR/ )
        [ -d $PARAGONDIR  ] && ( find $PARAGONDIR/      -name *.ko  |xargs -i cp {} $ANDROID_MODULES_DIR/ )
	[ -d $KERNELDIR   ] && ( find $KERNELDIR/       -name *.ko  |xargs -i cp {} $ANDROID_MODULES_DIR/ )
	[ -d $PHOENIX_EXTERNAL_DIR   ] && ( find $PHOENIX_EXTERNAL_DIR/       -name *.ko  |xargs -i cp {} $ANDROID_MODULES_DIR/ )

        chmod 644 $ANDROID_MODULES_DIR/*

        if [ -e $DVDPLAYER_BIN ]; then
            echo copy DvdPlayer to /system/bin/ : $DVDPLAYER_BIN
            cp $DVDPLAYER_BIN $PACKAGE_SYSTEM/bin/
        else
            echo -e "\033[47;31m [WARNING] DvdPlayer not find!! ($DVDPLAYER_BIN) \033[0m"
        fi

        if [ -e $ALSADAEMON_BIN ]; then
            echo copy ALSADaemon to /system/bin/ : $ALSADAEMON_BIN
            cp $ALSADAEMON_BIN $PACKAGE_SYSTEM/bin/
        else
            echo -e "\033[47;31m [WARNING] ALSADaemon not find!! ($ALSADAEMON_BIN) \033[0m"
        fi

    popd > /dev/null
    return $ERR;
}

build_image()
{
    echo IMAGE_DRAM_SIZE=$IMAGE_DRAM_SIZE
    echo LAYOUT_TYPE=$LAYOUT_TYPE
    echo LAYOUT_SIZE=$LAYOUT_SIZE
    echo INSTALL_DTB=$INSTALL_DTB
    echo INSTALL_FACTORY=$INSTALL_FACTORY
    echo INSTALL_AVFILE_COUNT=$INSTALL_AVFILE_COUNT
    [ "$INSTALL_DTB" = "1" ] && echo TARGET_BOARD=$TARGET_BOARD
    build_cmd prepare_image
    pushd $IMAGEDIR > /dev/null
        make image                                      \
            PACKAGES=$TARGET_PACKAGE                    \
            install_dtb=$INSTALL_DTB                    \
            layout_type=$LAYOUT_TYPE                    \
            layout_size=$LAYOUT_SIZE                    \
            install_avfile_count=$INSTALL_AVFILE_COUNT  \
	    install_factory=$INSTALL_FACTORY            \
            > /dev/null

        ERR=$?
    popd > /dev/null
    ls -l $IMAGEDIR/install.img
    return $ERR;
}


clean_all()
{
  echo "delete install.img"
  rm -rf $IMAGEDIR/install.img
  echo "delete tmp"
  rm -rf $IMAGEDIR/components/tmp
  echo "delete $PACKAGE_DIR/system"
  rm -rf $PACKAGE_DIR/system
  echo "delete $PACKAGE_DIR/root"
  rm -rf $PACKAGE_DIR/root
}

if [ "$1" = "" ]; then
    echo "$0 commands are:"
    echo "    build       "
else
    while [ "$1" != "" ]
    do
        case "$1" in
            build)
                build_cmd build_image
                ;;
	    clean)
                build_cmd clean_all
                ;;
            *)
                echo -e "$0 \033[47;31mUnknown CMD: $1\033[0m"
                exit 1
                ;;
        esac
        shift 1
    done
fi

exit $ERR
