#!/sbin/busybox sh
while read DEV MOUNT; do
    /sbin/e2fsck -y -f $DEV >> /e2fsck.log 2>&1
done < /sys.part.rc
/sbin/busybox touch /e2fsck.done
