#!/bin/bash
nowtime=` date +%Y-%m-%d__%H:%M:%S `
Name=`hostname`
CoreVerson=(` uname -r `)
Uptime=` uptime | tr -s " " "_" `
Used=(`df -hl | awk -v sum=0 '{if($1 ~ /\/dev\//) {sum=sum+$5}} END{print sum}'`)
DiskNote=(`	echo $Used | awk '{if($1>90) printf"warning"; else if($1>80) printf"note"; else printf"normal"}' `)
MemNote=(`	echo ${Mem[1]} | awk '{if($1>90) printf"warning"; else if($1>80) printf"note"; else printf"normal"}' `)
echo $nowtime $Name $CoreVerson $Uptime $Used% $DiskNote 