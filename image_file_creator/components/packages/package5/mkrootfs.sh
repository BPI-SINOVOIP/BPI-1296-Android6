#!/bin/bash

CUR_PWD=`pwd`
PACKAGE_BASE_DIR=$CUR_PWD/packages/package5
echo "PACKAGE_BASE_DIR=".$PACKAGE_BASE_DIR
ROOTFS_DIR=$PACKAGE_BASE_DIR/root
LAYOUT=$1

IMG_SZ=4  #2MB  // rootfs android

PKG_NAME="android" # "init"

$CUR_PWD/bin/mkbootfs $ROOTFS_DIR | $CUR_PWD/bin/minigzip > $PACKAGE_BASE_DIR/rootfs_$PKG_NAME.cpio.gz

dd if=/dev/zero of=$PACKAGE_BASE_DIR/pad.img bs=1M count=$IMG_SZ
cat $PACKAGE_BASE_DIR/rootfs_$PKG_NAME.cpio.gz $PACKAGE_BASE_DIR/pad.img > $PACKAGE_BASE_DIR/temp.img

#dd if=$PACKAGE_BASE_DIR/temp.img of=$PACKAGE_BASE_DIR/rootfs_$PKG_NAME.cpio.gz_pad.img bs=1M count=$IMG_SZ
dd if=$PACKAGE_BASE_DIR/temp.img of=$PACKAGE_BASE_DIR/$PKG_NAME.root.$LAYOUT.cpio.gz_pad.img bs=1M count=$IMG_SZ

#rm -f temp.img pad.img rootfs_$PKG_NAME.cpio.gz rootfs/rootfs_$PKG_NAME.cpio
rm -f $PACKAGE_BASE_DIR/temp.img $PACKAGE_BASE_DIR/pad.img $PACKAGE_BASE_DIR/rootfs/rootfs_$PKG_NAME.cpio
