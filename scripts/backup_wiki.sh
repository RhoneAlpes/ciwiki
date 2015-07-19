#!/bin/sh
# Simple backup - JP Redonnet - inphilly@gmail.com
homedir=~
wikidir=.ciwiki
bckupdir=~/wikibkup
echo "Run in Console - tar.gz all files in $homedir/$wikidir/"
cd $homedir
if [ -d $bckupdir ]
  then echo "backup will be in $bckupdir"
else
  echo "$bckupdir is created"
  mkdir $bckupdir
fi
bkdate=$(date +%Y%m%d_%H%M)
tar -czvf $bckupdir/wiki_$bkdate.tar.gz $wikidir/
