#!/bin/sh

while [ 0 -eq 0 ]
do
	trap "./app/fct/wndadjust" 62
	trap "./app/wndcalibration" 61
	sleep 2
done
