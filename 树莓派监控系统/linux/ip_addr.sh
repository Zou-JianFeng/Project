#!/bin/bash
ip_addr=(`ifconfig eth0 | grep "inet " | awk '{ print $2}'`)
echo $ip_addr