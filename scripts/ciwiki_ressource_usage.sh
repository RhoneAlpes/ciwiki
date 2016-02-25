#!/bin/bash
# CPU and Memory usage
# JP Redonnet Febuary 19, 2016 - inphilly@gmail.com
# Rev 1.0
#
watch -n 1 'ps -C ciwiki' -o %cpu,%mem,trs,drs,rss,pid,cmd
