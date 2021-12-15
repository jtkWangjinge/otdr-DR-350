#!/bin/sh

SDCARDPATH="/mnt/sdcard"

TARGETPATH=$SDCARDPATH

mkdir "$TARGETPATH"
touch "$TARGETPATH"
mkdir /tmp/sdcard
touch /tmp/sdcard

/bin/mount -t vfat -o shortname=mixed -o utf8=yes /dev/$1 $TARGETPATH

Sync
