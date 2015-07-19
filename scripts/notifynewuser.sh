#!/bin/bash
# This simple bash script will send the validation code to the new users
# Require ssmtp (relay email to your smtp)
# if newwikiusers.txt exists, then it's renamed.
# Email, name,pwd and code are extracted and sent with ssmtp
# then the result file newusersnotified.txt is updated
# if newusermesage.txt exists, it will be read to prepare the email text.
# JP Redonnet May 2010 - inphilly@gmail.com
# Rev 1.5 - Febuary 22, 2015
# newwikiusers.txt should be in  /usr/local/libexec
file1=~/newwikiusers.txt
# temporary file
file2=~/newwikiusers.temp
# list of users notified
file3=~/newusersnotified.txt
# Email message can be customized
file4=~/newusermessage.txt

# ciwiki port - default port=80
port=80

# default subject and body
subject='Your CiWiki Access Code'
body='Hello\r\rTo validate your new account;
      Please click on the direct link below,
      or in the Wiki, click on login, choose new account,
      Reenter your username, password, and email,
      Enter your access code.'
thanks='Thank you!'
# default internet address
internetAddr=""
# default body and default thanks flag=0
flag1=0
flag2=0

#wiki address, we need the ip address and the port 
#we will create a direct link to validate a new registration
#get the port addr in the arguments
while getopts p:i arg
do
  case $arg in
    p) port=$OPTARG;;
    i) internet=1;;
  esac
done

#get the internet address, subject, body, thanks
if [ -f "$file4" ]
then
  while read line
    do
      if [[ "$line" =~ "I:" ]]
    then
      internetAddr=${line#*I:}
      internetAddr=${internetAddr%% *}
    fi
    
      if [[ "$line" =~ "S:" ]]
    then
      subject=${line#*S:}
    fi
    
      if [[ "$line" =~ "B:" ]]
    then
        if (($flag1 == 0))
      then
        body=${line#*B:}
        flag1=1
      else
        body=$body'\r'${line#*B:}
      fi
    fi
      if [[ "$line" =~ "T:" ]]
    then
        if (($flag2 == 0))
      then
        thanks=${line#*T:}
        flag2=1
      else
        thanks=$thanks'\r'${line#*T:}
      fi
    fi
  done < $file4
fi

#internet address available?
if [[ -n $internetAddr ]]
then
  #Create the  Internet wiki address
  wikiAddr=$internetAddr':'$port
else
  #extract the ip address of the server
  #some ifconfig version (old?) return adr: rather addr:
  str=`ifconfig | grep -Eo 'inet (addr:|adr:)?([0-9]*\.){3}[0-9]*' | grep -Eo '([0-9]*\.){3}[0-9]*' | grep -v '127.0.0.1'`
  #Create the  Intranet wiki addresse
  wikiAddr='http://'${str%% *}':'$port
fi

#extract the email, login name, password, access code 
# then send email
if [ -f "$file1" ]
then
  mv -f $file1 $file2
  while read line
  do
    email=${line#*M:}
    email=${email%% *}
    usr=${line#*U:}
    usr=${usr%% *}
    pwd=${line#*P:}
    pwd=${pwd%% *}
    code=${line#*C:}
    directLink=$wikiAddr'/Login?rac='$email,$code,$usr,$pwd

    date >> $file3
    echo "$line" >> $file3
    
    echo -e "Subject:$subject\r\r \
    $body\r\r \
    Your username:$usr\r \
    Your password:$pwd\r \
    Your access code:$code\r \
    Direct link : $directLink\r\r \
    $thanks\r"|sendmail $email >> $file3
        
  done < $file2
fi
exit 0
