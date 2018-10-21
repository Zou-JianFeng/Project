#!/bin/bash

NowTime=`date +"%Y-%m-%d__%H:%M:%S"`

Name=`hostname`

Verson=(` cat /etc/issue.net | tr -s " " "_" `)

CoreVerson=(` uname -r `)

UpTime=` uptime | tr -s " " "_" `

Loadavg=` cat /proc/loadavg | awk '{print $1,$2,$3 }'` 

echo "$NowTime $Name $Verson $CoreVerson $UpTime  " 
