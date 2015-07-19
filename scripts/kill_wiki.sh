#!/bin/sh
echo "Useful for dev only. kill ciwiki daemon. ctr-c to quit"
read a
kill -15 `pidof ciwiki`
