#!/bin/sh
# This simple bash script searchs user registrations in newwikiusers.txt
# JP Redonnet August 2010 - inphilly@gmail.com
# newwikiusers.txt should be in the home dir
file1=~/newwikiusers.txt
file2=~/newusersnotified.txt
echo "Search user registrations in newwikiusers.txt"
echo -n "Enter the user name (or all): "
read b
if [ ! $b ] || [ $b = "all" ]
then
    a=U:
else
    a=U:$b
fi

if [ -f "$file1" ]
then
    echo "List of users not notified yet:"
    grep -i $a $file1
    echo "------------------------------"
fi
if [ -f "$file2" ]
then
    echo "List of users alread notified:"
    grep -i $a $file2
    echo "------------------------------"
fi
echo "Press Enter"
read a
