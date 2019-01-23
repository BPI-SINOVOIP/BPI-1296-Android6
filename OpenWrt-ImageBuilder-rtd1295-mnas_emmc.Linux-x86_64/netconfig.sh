#!/bin/bash

TARGETS="router ott"
if [ "$1" = "" ]; then
    echo "Please select config target:"
    select TARGET in $TARGETS;
    do
        echo "$TARGET selected!"
        break
    done
else
    TARGET=$1
fi

# OPTION ON/OFF
config(){
OPTION="CONFIG_$1"

    case "$2" in
        on|ON)
            sed -i -r "s/^#.?$OPTION is not set/$OPTION=y/g" ./.config
        ;;
        off|OFF)
            sed -i "s/^$OPTION=y/# $OPTION is not set/g" ./.config
        ;;
        *)
            echo "Unsupported state:$2 for config option:$2 is selected!"
            exit 1
        ;;
    esac
}

ROUTER_MOD="rtl8192cd mac80211 cfg80211 lib80211"
ROUTER_MOD+=" rtd1295hwnat"
OTT_MOD="ottrtl8188eu ottrtl8189es ottrtl8189fs ottrtl8192cu ottrtl8192eu ottrtl8723bs\
         ottrtl8723bu ottrtl8723ds ottrtl8811au ottrtl8812au ottrtl8812bu ottrtl8814au \
         ottrtl8821as ottrtl8821au ottrtl8821cs ottrtl8822be ottrtl8822bs ottrtl8822bu \
         rtl8169soc ottrtl8723du \
         ottbtusb"
OTT_MOD+=" ottrtl8192ee ottrtl8812ae ottrtl8822be"
NETWORK_PKG="netifd firewall luci-app-firewall dnsmasq"

case "$TARGET" in
    router)
        # Rename conflict ipks
        find ./packages -name "kmod-ott*.ipk" -execdir mv {} {}.bak \;

        # Update package configs
        for mod in $ROUTER_MOD; do
            config PACKAGE_kmod-$mod ON
        done
        for mod in $OTT_MOD; do
            config PACKAGE_kmod-$mod OFF
        done

        for pkg in $NETWORK_PKG; do
            config PACKAGE_$pkg ON
            find ./packages -name "${pkg}*.ipk.bak" -execdir sh -c 'mv {} $(basename {} .bak)' \;
        done

        cat ./rtl8192cd.conf >> .config
        yes 'n' | make oldconfig

        # Rename back needed ipks
        find ./packages -name "kmod-rtl8192cd*.ipk.bak" -execdir sh -c 'mv {} $(basename {} .bak)' \;
        find ./packages -name "kmod-*80211*.ipk.bak" -execdir sh -c 'mv {} $(basename {} .bak)' \;
    ;;
    ott)
        # Move conflict ipks
        find ./packages -name "kmod-rtl8192cd*.ipk" -execdir mv {} {}.bak \;
        find ./packages -name "kmod-*80211*.ipk" -execdir mv {} {}.bak \;

        # Update package configs
        for mod in $OTT_MOD; do
            config PACKAGE_kmod-$mod ON
        done
        for mod in $ROUTER_MOD; do
            config PACKAGE_kmod-$mod OFF
        done

        for pkg in $NETWORK_PKG; do
            config PACKAGE_$pkg OFF
            find ./packages -name "${pkg}*.ipk" -execdir mv {} {}.bak \;
        done

        make oldconfig

        # Move back needed ipks
        find ./packages -name "kmod-ott*.ipk.bak" -execdir sh -c 'mv {} $(basename {} .bak)' \;
    ;;
    *)
        echo "Unsupported target:$TARGET is selected!"
        exit 1
    ;;
esac
