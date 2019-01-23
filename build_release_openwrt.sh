#!/bin/bash
SCRIPTDIR=$PWD
source $SCRIPTDIR/build_prepare.sh
ERR=0

# set umask here to prevent incorrect file permission
umask 0022

config_get CUSTOMER
config_get GIT_SERVER_URL
config_get SDK_BRANCH
config_get USE_RTK_REPO
config_get BUILDTYPE_ANDROID
config_get IMAGE_DRAM_SIZE
config_get OPENWRT_CONFIG
config_get USE_RTK1295_EMMC_SWAP
config_get IMAGE_TARGET_BOARD 

export BUILDTYPE_ANDROID=$BUILDTYPE_ANDROID
echo -e "export \033[0;33mBUILDTYPE_ANDROID=$BUILDTYPE_ANDROID\033[0m"

MODULE_PATH=$ANDROIDDIR/out/target/product/$BUILDTYPE_ANDROID/system/vendor/modules


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

init_openwrt()
{
	[ ! -d "$OPENWRTDIR" ] && mkdir $OPENWRTDIR
	pushd $OPENWRTDIR > /dev/null
	menifest=openwrt_a01.xml

	if [ "$USE_RTK_REPO" == true ]; then
		repo init -u ssh://$GIT_SERVER_URL:29418/$CUSTOMER/manifests -b $SDK_BRANCH -m $menifest --repo-url=ssh://$GIT_SERVER_URL:29418/git-repo
	else
		repo init -u ssh://$GIT_SERVER_URL:29418/$CUSTOMER/manifests -b $SDK_BRANCH -m $menifest
	fi
	popd
	return 0
}

sync_openwrt()
{
    ret=1
    [ ! -d "$OPENWRTDIR" ] && return 0
    pushd $OPENWRTDIR > /dev/null
        repo sync --force-sync
        ret=$?
        [ "$ret" = "0" ] && > .repo_ready
    popd > /dev/null
    return $ret
}

checkout_openwrt()
{
    [ -e "$OPENWRTDIR/.repo_ready" ] && return 0
    init_openwrt
    sync_openwrt
    return $?
}

build_openwrt()
{
	[ ! -e "$OPENWRTDIR/.repo_ready" ] && build_cmd checkout_openwrt
#	[ -d "$MODULE_PATH" ] && rm $MODULE_PATH/*

	pushd $OPENWRTDIR > /dev/null
		./netconfig.sh $OPENWRT_CONFIG
		if [ "$USE_RTK1295_EMMC_SWAP" = "true" ]; then
			sed -i 's/# CONFIG_PACKAGE_kmod-rtd1295cma is not set/CONFIG_PACKAGE_kmod-rtd1295cma=y/' .config
			sed -i 's/# CONFIG_TARGET_NAS_IMG_EMMC_SWAP is not set/CONFIG_TARGET_NAS_IMG_EMMC_SWAP=y/' .config
		else
			#echo "CONFIG_USE_RTK1295_EMMC_SWAP=$USE_RTK1295_EMMC_SWAP, do nothing"
			sed -i 's/CONFIG_PACKAGE_kmod-rtd1295cma=y/# CONFIG_PACKAGE_kmod-rtd1295cma is not set/' .config
			sed -i 's/CONFIG_TARGET_NAS_IMG_EMMC_SWAP=y/# CONFIG_TARGET_NAS_IMG_EMMC_SWAP is not set/' .config
		fi

		if [ "$IMAGE_TARGET_BOARD" = "saola" ]; then
			sed -i 's/CONFIG_TARGET_rtd1295_mnas_emmc_giraffe-2GB=y/CONFIG_TARGET_rtd1295_mnas_emmc_saola-2GB=y/' .config
			sed -i 's/TARGET_rtd1295_mnas_emmc_giraffe-2GB/TARGET_rtd1295_mnas_emmc_saola-2GB/' Config-build.in
			echo "CONFIG_RTK_DPTX=y" >> target/linux/rtd1295/mnas_emmc/config-4.1
		elif [ "$IMAGE_TARGET_BOARD" = "giraffe" ]; then
			sed -i 's/CONFIG_TARGET_rtd1295_mnas_emmc_saola-2GB=y/CONFIG_TARGET_rtd1295_mnas_emmc_giraffe-2GB=y/' .config
			sed -i 's/TARGET_rtd1295_mnas_emmc_saola-2GB/TARGET_rtd1295_mnas_emmc_giraffe-2GB/' Config-build.in
			git checkout target/linux/rtd1295/mnas_emmc/config-4.1
		else
			echo -e "$0 \033[47;31mplease check config IMAGE_TARGET_BOARD $IMAGE_TARGET_BOARD\033[0m"
	        exit 1
		fi

		make clean V=99 -j4
		make modules V=99 -j4
		make image PROFILE=$IMAGE_TARGET_BOARD-$IMAGE_DRAM_SIZE PACKAGES=ALL
		ERR=$?
	popd  > /dev/null
	return $ERR
}

clean_openwrt()
{
	pushd $OPENWRTDIR > /dev/null
		make clean V=99 -j4;rm .config.old;git stash;git stash clear
	popd  > /dev/null

	pushd $OPENWRTKERNELDIR > /dev/null
		make clean;rm -rf include/config/;git stash;git stash clear
	popd > /dev/null

	return $ERR

}

sync_log()
{
DATE=`cat $SCRIPTDIR/.build_config |grep SYNC_DATE: | awk '{print $2}'`
LOG_FILE=$SCRIPTDIR/change_log_openwrt_`date +%Y-%m-%d`.txt
pushd $OPENWRTDIR > /dev/null
repo list |awk 'BEGIN{system("rm /tmp/change_log.txt "); cnt=1}{printf "\n==== "cnt" === "$1" "$2" "$3" ===========\n" >> "/tmp/change_log.txt" ; cnt++; system("pushd "$1";  git log --stat --since='$DATE' >> /tmp/change_log.txt ; popd > /dev/null")} END{system("mv /tmp/change_log.txt '$LOG_FILE'")}'
popd > /dev/null
}



if [ "$1" = "" ]; then
    echo "$0 commands are:"
	echo "    checkout    "
	echo "    build       "
    echo "    clean       "
	echo "	  sync		  "
else
    while [ "$1" != "" ]
	 do
	   case "$1" in
		   checkout)
				build_cmd checkout_openwrt
				;;
		   clean)
				build_cmd clean_openwrt
           		;;
		   build)
				build_cmd build_openwrt
				;;
			sync)
				build_cmd clean_openwrt
				build_cmd sync_openwrt
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



