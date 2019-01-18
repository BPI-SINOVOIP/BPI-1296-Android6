#!/bin/bash
SCRIPTDIR=$PWD
ANDROIDDIR=$SCRIPTDIR/android
KERNELDIR=$SCRIPTDIR/linux-kernel
MALIDIR=$SCRIPTDIR/mali
PHOENIXDIR=$SCRIPTDIR/phoenix
UBOOTDIR=$SCRIPTDIR/bootcode
TOOLCHAINDIR=$PHOENIXDIR/toolchain
PRODUCT_DEVICE_PATH=android/out/target/product/kylin32
#source $SCRIPTDIR/build_prepare.sh
ERR=0
VERBOSE=
NCPU=`grep processor /proc/cpuinfo | wc -l`
MULTI=`expr $NCPU + 2`


# set umask here to prevent incorrect file permission
umask 0022


build_android()
{
    pushd $ANDROIDDIR
        source ./env.sh
        lunch rtk_kylin32-eng
        make -j $MULTI $VERBOSE
        ERR=$?
    popd
    return $ERR
}

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


clean_firmware()
{
    pushd $ANDROIDDIR
        make clean
        ERR=$?
    popd
}


ln_libOMX_realtek()
{
echo "PRODUCT_DEVICE_PATH" $PRODUCT_DEVICE_PATH
cp android/ext_bin/* $PRODUCT_DEVICE_PATH/system/bin/.
cd $PRODUCT_DEVICE_PATH/system/lib/
	echo "Starting force link OMX libraries"
ln -sf libOMX.realtek.video.dec.so libOMX.realtek.video.dec.3gpp.so
ln -sf libOMX.realtek.video.dec.so libOMX.realtek.video.dec.avc.so
ln -sf libOMX.realtek.video.dec.so libOMX.realtek.video.dec.avs.so
ln -sf libOMX.realtek.video.dec.so libOMX.realtek.video.dec.divx3.so
ln -sf libOMX.realtek.video.dec.so libOMX.realtek.video.dec.flv.so
ln -sf libOMX.realtek.video.dec.so libOMX.realtek.video.dec.hevc.so
ln -sf libOMX.realtek.video.dec.so libOMX.realtek.video.dec.mjpg.so
ln -sf libOMX.realtek.video.dec.so libOMX.realtek.video.dec.mp43.so
ln -sf libOMX.realtek.video.dec.so libOMX.realtek.video.dec.mpeg2.so
ln -sf libOMX.realtek.video.dec.so libOMX.realtek.video.dec.mpeg4.so
ln -sf libOMX.realtek.video.dec.so libOMX.realtek.video.dec.raw.so
ln -sf libOMX.realtek.video.dec.so libOMX.realtek.video.dec.rv30.so
ln -sf libOMX.realtek.video.dec.so libOMX.realtek.video.dec.rv40.so
ln -sf libOMX.realtek.video.dec.so libOMX.realtek.video.dec.rv.so
ln -sf libOMX.realtek.video.dec.so libOMX.realtek.video.dec.vc1.so
ln -sf libOMX.realtek.video.dec.so libOMX.realtek.video.dec.vp8.so
ln -sf libOMX.realtek.video.dec.so libOMX.realtek.video.dec.wmv3.so
ln -sf libOMX.realtek.video.dec.so libOMX.realtek.video.dec.wmv.so
ln -sf libOMX.realtek.video.dec.so libOMX.realtek.video.dec.wvc1.so
ln -sf libOMX.realtek.video.enc.so libOMX.realtek.video.enc.avc.so
ln -sf libOMX.realtek.video.enc.so libOMX.realtek.video.enc.mpeg4.so
ln -sf libOMX.realtek.audio.dec.so libOMX.realtek.audio.dec.raw.so
ln -sf libOMX.realtek.video.dec.so libOMX.realtek.video.dec.hevc.secure.so
ln -sf libOMX.realtek.video.dec.vp9.so libOMX.realtek.video.dec.vp9.secure.so
ln -sf libOMX.realtek.audio.dec.so libOMX.realtek.audio.dec.secure.so
ln -sf libOMX.realtek.video.dec.so libOMX.realtek.video.dec.secure.so

} 



if [ "$1" = "" ]; then
    echo "$0 commands are:"
    echo "    build       "
    echo "    clean       "
else
    while [ "$1" != "" ]
    do
        case "$1" in
	    clean)
                build_cmd clean_firmware
                ;;
            build)
                build_cmd build_android
                build_cmd ln_libOMX_realtek
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
