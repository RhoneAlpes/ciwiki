#!/bin/bash
# This little bash script gets your public IP address and checks it.
# If the address has changed you will receive an email
# you can choose ipecho.net, checkip.dyndns.org  and ifconfig.me
# Require ssmtp (relay email to your smtp)
# JP Redonnet Febuary 28, 2015 - inphilly@gmail.com
# Rev 1.0
#
email="inphilly@gmail.com"
file1=~/myPublicIPAddr.txt

#str=`wget -O - http://ipecho.net -q|grep -Eo 'Your IP is ?([0-9]*\.){3}[0-9]*' | grep -Eo '([0-9]*\.){3}[0-9]*'`
#str=`wget -O - http://checkip.dyndns.org -q|grep -Eo 'Current IP Address: ?([0-9]*\.){3}[0-9]*' | grep -Eo '([0-9]*\.){3}[0-9]*'`
str=`wget -O - http://ifconfig.me/ip -q`

str=${str%% *}

#Check if ip addr has changed
if [ -f "$file1" ]
then
  while read line
  do
    if [[ "$line" == "$str" ]]
    then
      #found!
      exit 0
    fi
  done < $file1
fi
#Address has changed!
#Save for the new comparison
echo $str>$file1
#Prepare and send the mail
echo -e "Subject:Your Public IP address has changed!\r\r\r \
Please update quickly your IP address (dynDSN...). \r\r \
Your new public IP address is http://$str \r\r
Thanks\r\r"|sendmail $email
exit 0
