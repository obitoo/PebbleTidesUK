#!/usr/bin/python
# 
# 
# Owen Bullock - UK Tides - UKHO Easytide webscrape. 
#                         - cgi script
#                           Params:     port=<4 char string>
#
#
# 
#
#

import json
import tidedata
import os, sys
import optparse

app_api_key = "SBna3itxdgseq99nbw87kur33gi1q4yi"  # This is the sandbox one

#
#  Init 
#
port       = 0
utc_offset = 0

#
# test cmdline options:   -p <port no> : testrun for one port
#
parser = optparse.OptionParser("usage: %prog [options] ")
parser.add_option("-p",  dest="port", type="string", 
                  help = "port number, run for single ")
parser.add_option("-o",  dest="utc_offset", type="int", 
                  help = "utc offset, eg -60 ")
(options, args) = parser.parse_args()

if options.port:
   port = options.port
if options.utc_offset:
   utc_offset = options.utc_offset


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
#  adjust to UTC using offset passed from the user's phone javascript
#
if (utc_offset != 0):
   tides.adjust_offset(utc_offset)


#
# Send timeline shared pins
#
from timeline import myTimeline
t=myTimeline( app_api_key )

array=tides.dictionary()
for i in range (0,6):
   print "-----",i, array[i]["state"] , array[i]["datetime"],  array[i]["height"]


   result = t.send_shared_pin( port_id        = port
                              ,hi_lo_string   = array[i]["state"]
                              ,date_time      = str(array[i]["datetime"])+"Z"
                              ,height         = str(array[i]["height"])
                              ,portname       = array[i]["portname"]
                             )

   if result.status_code != 200:
      print result.status_code
   else:
      print "200 - success"




# EOF
