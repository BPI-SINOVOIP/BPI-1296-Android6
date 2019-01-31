#!/sbin/busybox sh
while read DEV MOUNT; do
    /sbin/resize2fs $DEV >> /ext4resize.log 2>&1
done < /sys.part.rc
/sbin/busybox touch /ext4resize.done
