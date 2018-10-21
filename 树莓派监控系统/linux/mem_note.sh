#!/bin/bash
nowtime=`date +%Y-%m-%d__%H:%M:%S`
Mem=(` free -m | head -n 2 | tail -n 1 | awk '{print $2,($3/$2*100)}' `)

MemNote=(`	echo ${Mem[1]} | awk '{if($1>90) printf"warning"; else if($1>80) printf"note"; else printf"normal"}' `)
echo "$nowtime ${Mem[0]} ${Mem[1]}% $MemNote " 
