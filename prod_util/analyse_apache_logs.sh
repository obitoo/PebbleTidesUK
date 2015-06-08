#!/bin/bash
logfile=/var/log/apache2/access.log
logerrors=/var/log/apache2/error.log
tmpfile=/tmp/analyse_apache.tmp
configfile=/var/www/tides/config.html
configfilev3=/usr/lib/cgi-bin/tides/portlists/*


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


lookup_ports_v2()
{
   for portno in `cat $1`
   do
      portconfig=$(grep $portno $configfile )
      portname=$(echo $portconfig  | cut -d'>' -f2 | sed 's/<.*//')
      echo $portno $portname
   done
}
lookup_ports_v3()
{
   for portno in `cat $1`
   do
      portno=$(echo $portno | cut -d':' -f2)
      portconfig=$(grep $portno $configfilev3 )
      portname=$(echo $portconfig  | cut -d',' -f2 | sed 's/<.*//')
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



#
#  New style - v2 client
#
count_ip=$(grep get_tide $logfile |grep "vsn=2"  | cut -d' ' -f1  |sort -u |  wc -l)
grep get_tide /var/log/apache2/access.log | grep "vsn=2" | sed 's/\&vsn=.*//' | sed 's/.*port=//' | sed 's/ HTTP.*//' | sort -u > $tmpfile
count_tides=$(cat $tmpfile | wc -l)

echo "---------Ports v2.0-------"
lookup_ports_v2 $tmpfile
echo "----------------"
echo "Distinct IP addresses requesting tide info: $count_ip"
echo "Distinct tides requested: $count_tides"
echo "----------------"


#
#  New style - v3 client
#
count_ip=$(grep get_tide $logfile | grep "vsn=3"  | cut -d' ' -f1  |sort -u |  wc -l)
grep get_tide /var/log/apache2/access.log | grep "vsn=3" | sed 's/\&vsn=.*//' | sed 's/.*port=//' | sed 's/ HTTP.*//' | sort -u > $tmpfile
count_tides=$(cat $tmpfile | wc -l)

echo
echo "---------Ports v3.0-------"
lookup_ports_v3 $tmpfile
echo "----------------"
echo "Distinct IP addresses requesting tide info: $count_ip"
echo "Distinct tides requested: $count_tides"
echo "----------------"



