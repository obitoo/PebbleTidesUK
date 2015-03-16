#!/bin/bash
logfile=/var/log/apache2/access.log
logerrors=/var/log/apache2/error.log
tmpfile=/tmp/analyse_apache.tmp
configfile=/var/www/tides/config.html


if [ "$EUID" -ne 0 ]
  then echo "Please run as root"
  exit
fi

first_date=$(head -1  $logfile | cut -d'[' -f2 | sed s'/].*//')


#
#  Old style - v1 client
#

count_ip=$(grep tides $logfile | grep json  | cut -d' ' -f1  |sort -u |  wc -l)

grep tides /var/log/apache2/access.log | grep json | sed 's/json.*/json/' | sed 's/.*GET//' | sort -u > $tmpfile
count_tides=$(cat $tmpfile | wc -l)

grep tides /var/log/apache2/error.log | sed "s/.*client//" | sort -u > $tmpfile.404
count_404=$(cat $tmpfile.404 | wc -l)


lookup_ports()
{
   for line in `cat $1`
   do
      portno=$(echo $line | cut -c13-16)
      portconfig=$(grep $portno $configfile )
      portname=$(echo $portconfig  | cut -d'>' -f2 | sed 's/<.*//')
      echo $line $portname
   done
}
lookup_ports_v2()
{
   for portno in `cat $1`
   do
      portconfig=$(grep $portno $configfile )
      portname=$(echo $portconfig  | cut -d'>' -f2 | sed 's/<.*//')
      echo $portno $portname
   done
}



echo "----------------"
echo "Access Report for : $first_date"
echo "logfile: $logfile"
echo "---------404----"
cat $tmpfile.404
echo "Distinct 404 client requests: $count_404"

echo "---------errors.log----"
grep "tide data" $logerrors
echo
echo "-------Access Summary-----"
echo "---------Ports v1.0-------"
lookup_ports $tmpfile
echo "----------------"
echo "Distinct IP addresses requesting tide info: $count_ip"
echo "Distinct tides requested: $count_tides"
echo "----------------"



#
#  New style - v2 client
#
count_ip=$(grep get_tide $logfile | cut -d' ' -f1  |sort -u |  wc -l)
grep get_tide /var/log/apache2/access.log | sed 's/\&vsn=.*//' | sed 's/.*port=//' | sed 's/ HTTP.*//' | sort -u > $tmpfile
count_tides=$(cat $tmpfile | wc -l)

echo 
echo "---------Ports v2.0-------"
lookup_ports_v2 $tmpfile
echo "----------------"
echo "Distinct IP addresses requesting tide info: $count_ip"
echo "Distinct tides requested: $count_tides"
echo "----------------"



