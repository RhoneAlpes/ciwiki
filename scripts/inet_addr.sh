#!/bin/sh
echo "Run in Console - Return your inet address"
str=`ifconfig | grep -Eo 'inet (addr:|adr:)?([0-9]*\.){3}[0-9]*' | grep -Eo '([0-9]*\.){3}[0-9]*' | grep -v '127.0.0.1'`
echo your inet address is ${str%% *}
echo "Enter to exit"
read a
