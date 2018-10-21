#!/bin/bash

time=`date +"%Y-%m-%d__%H:%M:%S"`
loadavg=`cut -d " " -f 1-3 /proc/loadavg`
Temp=` cat /sys/class/thermal/thermal_zone0/temp | awk '{print ($1/1000)}'` 
CpuNote=(`	echo $Temp \
	| awk '{if($1>70) printf"warning"; else if($1>50) printf"note"; else printf"normal"}' \
	`)
echo $time $loadavg ${Temp}°C $CpuNote