#!/bin/bash

SCRIPTDIR=$PWD
BRANCH_QA_TARGET_SN=00  # should be changed each build
PATTERN_STR1=
PATTERN_STR2=
RTK_VERSION_FILE="./rtk_version.txt"  # realtek version information

ERR=0

KERNELDIR=$SCRIPTDIR/linux-kernel
PHOENIXDIR=$SCRIPTDIR/phoenix
IMAGEDIR=$SCRIPTDIR/image_file_creator

ANDROID_OUT=$PWD/android/out/target/product/phoenix/
ANDROID_SYSTEM=$ANDROID_OUT/system

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

put_version()
{
    echo -e "get_version()"
    # copy
    cp $SCRIPTDIR/$RTK_VERSION_FILE $ANDROID_SYSTEM/vendor/resource/
}


build_qa()
{
    pushd $SCRIPTDIR > /dev/null
    build_cmd ./build_release_android.sh build
    build_cmd ./build_release_linux_kernel.sh build
    popd > /dev/null
    return $ERR;
}

pack_prepare_qa()
{
    #rsync -avP $QA_SUPDIR/gapps-kk-20131119/system/* $IMAGEDIR/components/packages/package5/system/
    #rsync -avP $QA_SUPDIR/media_codecs.xml  $IMAGEDIR/components/packages/package5/system/etc/
    #rsync -avP $QA_SUPDIR/apk/OIFileManager.apk $IMAGEDIR/components/packages/package5/system/app/
    find $KERNELDIR/modules/mali/ -name *.ko |xargs -i cp {} $IMAGEDIR/components/packages/package5/system/vendor/modules/
    find $PHOENIXDIR/system/src/bin/ -name *.ko | xargs -i cp {} $IMAGEDIR/components/packages/package5/system/vendor/modules/
    find $PHOENIXDIR/system/src/drivers/ -name *.ko | xargs -i cp {} $IMAGEDIR/components/packages/package5/system/vendor/modules/
    find $PHOENIXDIR/system/src/external/ -name *.ko | xargs -i cp {} $IMAGEDIR/components/packages/package5/system/vendor/modules/
    chmod 644 $IMAGEDIR/components/packages/package5/system/vendor/modules/*
    return $ERR;
}

pack_qa()
{
    pushd $SCRIPTDIR > /dev/null
    build_cmd ./build_image.sh build
    popd > /dev/null
    return $ERR;
}

clean_all()
{
    pushd $SCRIPTDIR > /dev/null
    build_cmd ./build_release_android.sh clean
    build_cmd ./build_release_linux_kernel.sh clean
    build_cmd ./build_image.sh clean
    popd > /dev/null
    pushd $PHOENIXDIR/system/src/drivers > /dev/null
    make clean
    popd > /dev/null
    return $ERR;
}

if [ "$1" = "" ]; then
    echo "$0 commands are:"
    echo "    build       "
    echo "    package	"
    echo "    clean    "
else
    while [ "$1" != "" ]
    do
        case "$1" in
            build)
                build_cmd build_qa
                build_cmd pack_prepare_qa
                put_version
                build_cmd pack_qa
                ;;
	    package)
		build_cmd pack_prepare_qa
		put_version
		build_cmd pack_qa
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
