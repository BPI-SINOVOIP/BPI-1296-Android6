#!/bin/bash

NCPU=`grep processor /proc/cpuinfo | wc -l`
MULTI=`expr $NCPU + 2`

SCRIPTDIR=$PWD
KERNELDIR=$SCRIPTDIR/linux-kernel
PHOENIXDIR=$SCRIPTDIR/phoenix
TOOLCHAINDIR=$PHOENIXDIR/toolchain
ERR=0

#export CCACHE=ccache
export ARCH=arm64
export CROSS_COMPILE="ccache asdk64-linux-"
export KERNEL_IMAGE=Image
#export AS=${CROSS_COMPILE}as
#export LD=${CROSS_COMPILE}ld
#export CC=${CCACHE}${CROSS_COMPILE}gcc
#export AR=${CROSS_COMPILE}ar
#export NM=${CROSS_COMPILE}nm
#export STRIP=${CROSS_COMPILE}strip
#export OBJCOPY=${CROSS_COMPILE}objcopy
#export OBJDUMP=${CROSS_COMPILE}objdump

export PATH=$TOOLCHAINDIR/asdk64-4.9.4-a53-EL-3.10-g2.19-a64nt-160307/bin:$PATH

KERNELTOOLCHAIN=$TOOLCHAINDIR/asdk64-4.9.4-a53-EL-3.10-g2.19-a64nt-160307/bin

PARAGONDIR=$PHOENIXDIR/system/src/external/paragon
EXT_DRIVERS=$PHOENIXDIR/system/src/drivers

KERNEL_TARGET_CHIP_LIST="phoenix kylin"

build_kernel()
{
    BUILD_PARAMETERS=$*
    pushd $KERNELDIR > /dev/null
        make -j $MULTI $BUILD_PARAMETERS DTC_FLAGS="-p 8192"
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

build_external_modules()
{
    export KERNELDIR
    export KERNELTOOLCHAIN
if [ "$KERNEL_TARGET_CHIP" = "phoenix" ]; then
    if [ -d "$KERNELDIR/modules/mali" ]; then
    pushd $KERNELDIR
        pushd modules
            if [ -d "mali" ]; then
                make -C mali -j $MULTI TARGET_KDIR=$KERNELDIR install
                ERR=$?
            fi
        popd
    popd
    fi
    [ "$ERR" = "0" ] || return $ERR;
    if [ -d "$OPTEE_DRIVERS" ]; then
        pushd $KERNELDIR
            make -j $MULTI M=$OPTEE_DRIVERS modules
            ERR=$?
        popd
    fi
    [ "$ERR" = "0" ] || return $ERR;
fi
    if [ -d "$PARAGONDIR" ]; then
        pushd $PARAGONDIR > /dev/null
            make clean && make -j $MULTI
            ERR=$?
        popd > /dev/null
    fi
    if [ -d "$EXT_DRIVERS" ]; then
        pushd $EXT_DRIVERS > /dev/null
            make clean && make -j $MULTI ANDROID_VERSION=$RTK_ANDROID_VERSION && make install
            ERR=$?
        popd > /dev/null
    fi

    return $ERR;
}

clean_kernel()
{
    pushd $KERNELDIR
        build_cmd make clean
    popd
    return $ERR;
}


if [ "$1" = "" ]; then
    echo "$0 commands are:"
    echo "    clean       "
    echo "    build       "
else
    while [ "$1" != "" ]
    do
        case "$1" in
            clean)
                build_cmd clean_kernel
                ;;
            build)
                build_cmd build_kernel $KERNEL_IMAGE modules dtbs
                build_cmd build_external_modules
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
