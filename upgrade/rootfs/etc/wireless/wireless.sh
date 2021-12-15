#!/bin/sh

wpa_supplicant -Dwext -iwlan0 -c/etc/wireless/wpa_supplicant.conf -B
#udhcpc -i eth1
