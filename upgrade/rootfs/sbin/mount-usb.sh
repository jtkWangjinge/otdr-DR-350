#!/bin/sh

mkdir /mnt/usb/
touch /mnt/usb/

mkdir /tmp/usb/
touch /tmp/usb/

/bin/mount -t vfat -o shortname=mixed -o utf8=yes /dev/$1 /mnt/usb
Sync
