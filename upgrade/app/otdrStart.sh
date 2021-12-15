#!/bin/sh

export LD_LIBRARY_PATH=/app/lib:$LD_LIBRARY_PATH
#export PATH=/app/bin:$PATH

#insmod /app/drivers/drv_fpga.ko
#insmod /app/drivers/drv_keypad.ko
insmod /app/drivers/drv_key_interupt.ko #key
insmod /app/drivers/drv_optic.ko        #fpga
#insmod /app/drivers/inno_f252.ko
#insmod /app/drivers/8189es.ko
insmod /app/drivers/drv_powermeter.ko   #opm
insmod /app/drivers/lcd_backlight.ko    #lcd
insmod /app/drivers/drv_netwire.ko      #rj45
chown root /etc/vsftpd.conf
/app/bin/vsftpd &

/sbin/server &

/app/lcd_unblank &
/app/wndproc &

/sbin/listener.sh

#start ftp
#tcpsvd -vE 0.0.0.0 21 ftpd /mnt & 

#klogd
#syslogd -O /mnt/sdcard/message.log

