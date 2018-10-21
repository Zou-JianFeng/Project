#!/bin/bash

NowTime=`date +"%Y-%m-%d__%H:%M:%S"`

Alldisk=(` df -m | awk -v sum=0 '{if($1 ~ /\/dev\//) {sum=sum+$2}} END {print sum}' `)

AllAva=(` df -m | awk -v sum=0 '{if($1 ~ /\/dev\//) {sum=sum+$4}} END {print sum}' `)

Used=$[100 - $AllAva *100 /$Alldisk ]

DiskNote=(`	echo $Used | awk '{if($1>90) printf"warning"; else if($1>80) printf"note"; else printf"normal"}' `)

echo "$NowTime 0 $Alldisk $AllAva ${Used}%"

df -m | awk -v d=${NowTime} '{if($1 ~ /\/dev\//){print "'$NowTime'","1",$2,$4,$5}}'

