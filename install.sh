#!/bin/bash
#
#

if [ "$EUID" -ne 0 ]
  then echo "Please run as root"
  exit
fi


install()
{
 src=$1
 dest=$2/$(basename $1)

 echo ""
 ls -l $src
 ls -l $dest

 echo "diff $src $dest "
 diff $src $dest > /dev/null
 if [ $? -eq 0 ] 
 then 
    echo "files are identical"
 else 
    echo "Warning - files differ"
    echo "install $1 to $2 ?"
    read
    cp $1 $2
    echo "ok"
 fi

}

##
##  Git local copy
##
gitdir=/home/owen/tides/PebbleTidesUK/www

##
## Straight html
##
for f in config.html  config3.html
do
   install $gitdir/$f /var/www/tides
done

##
## cgi scripts
##
for f in get_tide.py  scrape_easytide.py  tidedata.py ajax-getports.py
do
   install $gitdir/$f /usr/lib/cgi-bin/tides
done

## tide files
for f in $gitdir/portlists/*
do
   echo "copying $f"
   install $f /usr/lib/cgi-bin/tides/portlists/
done


##
## util scripts
##
gitdir=/home/owen/tides/PebbleTidesUK/prod_util
for f in analyse_apache_logs.sh
do
   install $gitdir/$f /home/owen/tides/prod
done





