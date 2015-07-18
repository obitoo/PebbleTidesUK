#!/bin/bash 

TOPICS=/var/www/tides/topics.txt
SCRIPT_DIR= /usr/lib/cgi-bin/tides/
LOG_DIR=/home/owen/tides/prod

SCRIPT=timeline_get_tide.py
LOG_FILE=cron.log





now=$(date)
echo "Starting : $now"   
echo "Starting : $now"   >> $LOG_DIR/$LOG_FILE


for topic in `cat $TOPICS`
do
   echo "Topic $topic ----------------------"
   
   $SCRIPT_DIR/$SCRIPT -t $topic >> $LOG_DIR/$LOG_FILE
done





now=$(date)
echo "Finished : $now"   
echo "Finished : $now"   >> $LOG_DIR/$LOG_FILE

