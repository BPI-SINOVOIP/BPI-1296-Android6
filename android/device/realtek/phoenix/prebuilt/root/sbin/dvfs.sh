#!/sbin/busybox sh

dvfs_set_mode()
{
    [ "$2" != " " ] && sleep $2
    dvfs_chmod 660
    echo write $1 to scaling_available_governors
    echo $1 > /sys/devices/system/cpu/cpu0/cpufreq/scaling_governor
    echo $1 > /sys/devices/system/cpu/cpu1/cpufreq/scaling_governor
}

dvfs_get_freq()
{
    echo cpu0: cpuinfo_cur_freq = `cat /sys/devices/system/cpu/cpu0/cpufreq/cpuinfo_cur_freq`
    echo cpu1: cpuinfo_cur_freq = `cat /sys/devices/system/cpu/cpu1/cpufreq/cpuinfo_cur_freq`
}

dvfs_freq_force_to()
{
    echo $1 > /sys/devices/system/cpu/cpu0/cpufreq/scaling_max_freq
    echo $1 > /sys/devices/system/cpu/cpu1/cpufreq/scaling_max_freq
}

dvfs_chmod()
{
    chmod $1 /sys/devices/system/cpu/cpu0/cpufreq/scaling_governor
    chmod $1 /sys/devices/system/cpu/cpu1/cpufreq/scaling_governor
    chmod $1 /sys/devices/system/cpu/cpu0/cpufreq/scaling_max_freq
    chmod $1 /sys/devices/system/cpu/cpu1/cpufreq/scaling_max_freq
}

dvfs_random_test()
{
    delay=$1
    maxFreq=$2
    minFreq=$3
    freqRange=`busybox expr ${maxFreq} - ${minFreq}`

    dvfs_chmod 777
    dvfs_set_mode performance

    until [ ]
    do
        freq=`busybox expr ${RANDOM} % ${freqRange}`
        freq=`busybox expr ${freq} + ${minFreq}`
        dvfs_freq_force_to ${freq}000
        sleep ${delay}
    done
}

if [ "$1" = "" ]; then
    echo "scaling_available_governors: `cat /sys/devices/system/cpu/cpu0/cpufreq/scaling_available_governors`"
    echo "$0 commands are:"
    echo "  force {freq-MHz}"
    echo "      ex: $0 force 900"
    echo "  {scaling_available_governors} {setupDelaySec}"
    echo "      ex: $0 performance 0 ondemand 60"
    echo "  test changeDelaySec maxFreq minFreq"
    echo "      ex: $0 test 0.5 1300 300"
else
    while [ "$1" != "" ]
    do
        case "$1" in
            interactive | conservative | ondemand | powersave | performance)
                dvfs_set_mode $1 $2
                shift 2
                ;;
            force)
                dvfs_set_mode performance 0
                dvfs_freq_force_to ${2}000
                shift 2
                ;;
            test)
                dvfs_random_test $2 $3 $4
                shift 3
                ;;
            *)
                echo Unknown CMD $1
                exit 1
                ;;
        esac
    done
fi

