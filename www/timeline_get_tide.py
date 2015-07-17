#!/usr/bin/python
# 
# 
# Owen Bullock - UK Tides - UKHO Easytide webscrape. 
#                         - cgi script
#                           Params:     port=<4 char string> + offset
#                               or 
#                                     topic
#
# 
#

import json
import tidedata
import os, sys
import optparse
from timeline import myTimeline

app_api_key = "SBna3itxdgseq99nbw87kur33gi1q4yi"  # This is the sandbox one

#
#  Init 
#
port       = 0
utc_offset = 0
hi_lo      = ""
topic      = ""

#
# test cmdline options:   -p <port no> : testrun for one port
#
parser = optparse.OptionParser("usage: %prog [options] ")
parser.add_option("-p",  dest="port", type="string", 
                  help = "port number, run for single ")
parser.add_option("-o",  dest="utc_offset", type="int", 
                  help = "utc offset, eg -60 ")
parser.add_option("-u",  dest="user_token", type="string",   
                  help = "long user token string ")
parser.add_option("-t",  dest="topic", type="string",   
                  help = "long user token string ")
(options, args) = parser.parse_args()

if options.port:
   port = options.port
if options.utc_offset:
   utc_offset = options.utc_offset
if options.user_token:
   user_token = options.user_token
if options.topic:
   topic = options.topic

#
#  Validate the topic, if supplied
#
if topic:
   try:
      (hi_lo, port, utc_offset) = topic.split("_")
      utc_offset=int(utc_offset)
   except:
      print "Error, cannot parse topic:",topic
      sys.exit(1)

   print "Parsed topic ",topic
   print "got ",hi_lo, port, utc_offset


#
# Create a tidesdata request object 
#
tides = tidedata.public(port)
if tides.error: 
   sys.stderr.write(tides.error+"\n")  
   sys.exit(1)


#
# Scrape data
#
tides.scrape()
if tides.error: 
   sys.stderr.write(tides.error+"\n")  
   sys.exit(1)

#
# Adjust UK ports for BST. The others will have a DST flag, which TODO I need to cater for
#
if (int(port) < 801):
   tides.adjust_bst()



#
#  adjust to UTC using offset passed from the user's phone javascript
#
if (utc_offset != 0):
   tides.adjust_offset(utc_offset)



#
# Loop round tides and Send timeline user pins
#
t=myTimeline( app_api_key )

array=tides.dictionary()

for i in range (0,6):  # prod: 0,6
   print "-----",i, array[i]["state"] , array[i]["datetime"],  array[i]["height"]

   if (hi_lo) and (hi_lo != array[i]["state"]): 
        continue 

   if (1):
      result = t.send_shared_pin( port_id        = port
                                 ,hi_lo_string   = array[i]["state"]
                                 ,date_time      = str(array[i]["datetime"])+"Z"
                                 ,height         = str(array[i]["height"])
                                 ,portname       = "help"#array[i]["portname"]
                                 ,utc_offset     = utc_offset
                                )

      if result.status_code != 200:
         print "Error:",result.status_code
      else:
         print "200 - success"




# EOF
