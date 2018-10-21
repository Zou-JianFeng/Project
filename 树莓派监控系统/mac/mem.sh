#!/bin/bash
nowtime=`date +%Y-%m-%d__%H:%M:%S`
allSize=(`df -hl | awk -v sum=0 '{if($1 ~ /\/dev\//) {sum=$2}} END{print sum}'`)
allAva=(`df -hl | awk -v sum=0 '{if($1 ~ /\/dev\//) {sum=$4}} END{print sum}'`)
Used=(`df -hl | awk -v sum=0 '{if($1 ~ /\/dev\//) {sum=sum+$5}} END{print sum}'`)
echo $nowtime 0 $allSize $allAva ${Used}%
#df -hl | awk -v d=${NowTime} '{if($1 ~ /\/dev\//){print "'$nowtime' "1, $2, $4, $5}}'