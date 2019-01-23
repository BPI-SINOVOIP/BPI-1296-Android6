#!/bin/bash
SCRIPTDIR=$PWD
source $SCRIPTDIR/build_prepare.sh
ANDROIDDIR=$SCRIPTDIR/android
KERNELDIR=$SCRIPTDIR/linux-kernel
MALIDIR=$SCRIPTDIR/mali
PHOENIXDIR=$SCRIPTDIR/phoenix
UBOOTDIR=$SCRIPTDIR/bootcode
TOOLCHAINDIR=$PHOENIXDIR/toolchain

ERR=0
VERBOSE=
NCPU=`grep processor /proc/cpuinfo | wc -l`
MULTI=`expr $NCPU + 2`


# set umask here to prevent incorrect file permission
umask 0022

config_get CUSTOMER
config_get GIT_SERVER_URL
config_get SDK_BRANCH
config_get USE_RTK_REPO
config_get BUILDTYPE_ANDROID
config_get USE_RTK1295_EMMC_SWAP
export BUILDTYPE_ANDROID=$BUILDTYPE_ANDROID
echo -e "export \033[0;33mBUILDTYPE_ANDROID=$BUILDTYPE_ANDROID\033[0m"


init_android()
{
	[ ! -d "$ANDROIDDIR" ] && mkdir $ANDROIDDIR
	pushd $ANDROIDDIR > /dev/null
		if [ "$USE_RTK_REPO" == true ]; then
			repo init -u ssh://$GIT_SERVER_URL:29418/$CUSTOMER/manifests -b $SDK_BRANCH -m android.xml --repo-url=ssh://$GIT_SERVER_URL:29418/git-repo
		else
			repo init -u ssh://$GIT_SERVER_URL:29418/$CUSTOMER/manifests -b $SDK_BRANCH -m android.xml
		fi
	popd > /dev/null
	return 0
}

sync_android()
{
    ret=1
    [ ! -d "$ANDROIDDIR" ] && return 0
    pushd $ANDROIDDIR > /dev/null
        repo sync --force-sync
        ret=$?
        [ "$ret" = "0" ] && > .repo_ready
    popd > /dev/null
    return $ret
}

checkout_android()
{
    [ -e "$ANDROIDDIR/.repo_ready" ] && return 0
    init_android
    sync_android
    return $?
}

build_android()
{
	[ ! -e "$ANDROIDDIR/.repo_ready" ] && build_cmd checkout_android

    pushd $ANDROIDDIR > /dev/null
        source ./env.sh
        lunch rtk_$BUILDTYPE_ANDROID-eng
        make -j $MULTI $VERBOSE
        ERR=$?
    popd > /dev/null
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

case_for_USE_EMMC_SWAP()
{
if [ "$USE_RTK1295_EMMC_SWAP" = "true" ]; then
	pushd $ANDROIDDIR/out/target/product/$BUILDTYPE_ANDROID/ > /dev/null
        # -----------------------------------------------------------#
        # set vm.min_free_kbytes to 32768 if IMAGE_ADJ_VM is true    #
        # -----------------------------------------------------------#
        sed -i '/write \/proc\/sys\/vm\/min_free_kbytes 32768/d' root/init.rc
        sed -i 's/on late-init/on late-init\n    write \/proc\/sys\/vm\/min_free_kbytes 32768/' root/init.rc

        # ---------------------------------------------------------- #
        # set vm.extra_free_kbytes to 20480 if IMAGE_ADJ_VM is true  #
        # -----------------------------------------------------------#
        sed -i 's/#on property:sys.sysctl.extra_free_kbytes=*/on property:sys.sysctl.extra_free_kbytes=*/' root/init.rc
        sed -i 's/write \/proc\/sys\/vm\/extra_free_kbytes 20480/write \/proc\/sys\/vm\/extra_free_kbytes ${sys.sysctl.extra_free_kbytes}/' root/init.rc
        sed -i 's/write \/proc\/sys\/vm\/extra_free_kbytes ${sys.sysctl.extra_free_kbytes}/write \/proc\/sys\/vm\/extra_free_kbytes 20480/' root/init.rc
        # ---------------------------------------------------------- #
        # enable eMMC swap if IMAGE_LAYOUT_USE_EMMC_SWAP is true     #
        # -----------------------------------------------------------#
        sed -i 's/#\/dev\/block\/mmcblk0p11/\/dev\/block\/mmcblk0p11/' root/fstab.kylin
        sed -i 's/#start mk_emmc_swap/start mk_emmc_swap/' root/init.fschk.rc
        # -----------------------------------------------------------#
	popd > /dev/null
else
	echo "CONFIG_USE_RTK1295_EMMC_SWAP=$USE_RTK1295_EMMC_SWAP, do nothing"
fi
}

link_lib()
{
echo android link_lib ++++++++++++++++++++++++++++++++++++
PRODUCT_DEVICE_PATH=$ANDROIDDIR/out/target/product/$BUILDTYPE_ANDROID

rsync -avP --exclude .git $ANDROIDDIR/ext_bin/ $PRODUCT_DEVICE_PATH/system/bin/

pushd $PRODUCT_DEVICE_PATH/system/lib/ > /dev/null
ln -s libOMX.realtek.video.dec.so libOMX.realtek.video.dec.3gpp.so
ln -s libOMX.realtek.video.dec.so libOMX.realtek.video.dec.avc.so
ln -s libOMX.realtek.video.dec.so libOMX.realtek.video.dec.avs.so
ln -s libOMX.realtek.video.dec.so libOMX.realtek.video.dec.divx3.so
ln -s libOMX.realtek.video.dec.so libOMX.realtek.video.dec.flv.so
ln -s libOMX.realtek.video.dec.so libOMX.realtek.video.dec.hevc.so
ln -s libOMX.realtek.video.dec.so libOMX.realtek.video.dec.mjpg.so
ln -s libOMX.realtek.video.dec.so libOMX.realtek.video.dec.mp43.so
ln -s libOMX.realtek.video.dec.so libOMX.realtek.video.dec.mpeg2.so
ln -s libOMX.realtek.video.dec.so libOMX.realtek.video.dec.mpeg4.so
ln -s libOMX.realtek.video.dec.so libOMX.realtek.video.dec.raw.so
ln -s libOMX.realtek.video.dec.so libOMX.realtek.video.dec.rv30.so
ln -s libOMX.realtek.video.dec.so libOMX.realtek.video.dec.rv40.so
ln -s libOMX.realtek.video.dec.so libOMX.realtek.video.dec.rv.so
ln -s libOMX.realtek.video.dec.so libOMX.realtek.video.dec.vc1.so
ln -s libOMX.realtek.video.dec.so libOMX.realtek.video.dec.vp8.so
ln -s libOMX.realtek.video.dec.so libOMX.realtek.video.dec.wmv3.so
ln -s libOMX.realtek.video.dec.so libOMX.realtek.video.dec.wmv.so
ln -s libOMX.realtek.video.dec.so libOMX.realtek.video.dec.wvc1.so
ln -s libOMX.realtek.video.enc.so libOMX.realtek.video.enc.avc.so
ln -s libOMX.realtek.video.enc.so libOMX.realtek.video.enc.mpeg4.so
ln -s libOMX.realtek.audio.dec.so libOMX.realtek.audio.dec.raw.so
ln -s libOMX.realtek.video.dec.so libOMX.realtek.video.dec.hevc.secure.so
ln -s libOMX.realtek.video.dec.vp9.so libOMX.realtek.video.dec.vp9.secure.so


popd > /dev/null
exit $ERR
}

sync_log()
{
DATE=`cat $SCRIPTDIR/.build_config |grep SYNC_DATE: | awk '{print $2}'`
LOG_FILE=$SCRIPTDIR/change_log_android_`date +%Y-%m-%d`.txt
pushd $ANDROIDDIR > /dev/null
	repo list |awk 'BEGIN{system("rm /tmp/change_log.txt "); cnt=1}{printf "\n==== "cnt" === "$1" "$2" "$3" ===========\n" >> "/tmp/change_log.txt" ; cnt++; system("pushd "$1";  git log --stat --since='$DATE' >> /tmp/change_log.txt ; popd > /dev/null")} END{system("mv /tmp/change_log.txt '$LOG_FILE'")}'
popd > /dev/null
}


if [ "$1" = "" ]; then
    echo "$0 commands are:"
    echo "    checkout    "
    echo "    sync        "
    echo "    build       "
else
    while [ "$1" != "" ]
    do
        case "$1" in
            build)
                build_cmd build_android
				build_cmd case_for_USE_EMMC_SWAP
				build_cmd link_lib
                ;;
			checkout)
				build_cmd checkout_android
				;;
            sync)
                build_cmd sync_android
				sync_log
                ;;
            *)
                echo -e "$0 \033[47;31mUnknown CMD: $1\033[0m"
                exit 1
                ;;
        esac
        shift 1
    done
fi
