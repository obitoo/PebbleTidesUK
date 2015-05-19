#!/usr/bin/python
# 
# 
# Owen Bullock - UK Tides - UKHO Easytide webscrape. 
#                         - cgi script
#                           Params:     port=<4 char string>
#                                       time=hh:mm
#                                       vsn=<client version no>
# TODO:  partial forecast:   
# http://192.168.7.175//cgi-bin/tides/get_tide.py?port=3422&time=20:47
#
#
# Southend:
# http://192.168.7.175//cgi-bin/tides/get_tide.py?port=0110&time=20:47
# Baltimore:
# http://192.168.7.175//cgi-bin/tides/get_tide.py?port=2716&time=20:47
#                           
# 16Mar15 - Created
# 17Mar15 - use time param
# 18Mar15 - strip country from port
# 19Mar15 - date logic
# 16Apr   - default time for v2 client
# 19May   - offsets. Handle undefined param
#

import json
import tidedata
import cgi, cgitb, os, sys
import optparse

#
#  Init 
#
cgitb.enable(); # formats errors in HTML
form = cgi.FieldStorage()
port   = 0
time   = 0
version= 3.1
offset = 0

#
# test cmdline options:   -p <port no> : testrun for one port
#
parser = optparse.OptionParser("usage: %prog [options] ")
parser.add_option("-p",  dest="port", type="string", 
                  help = "port number, run for single ")
parser.add_option("-t",  dest="time", type="string", 
                  help = "current (local) time in HH:MM")
parser.add_option("-d",  dest="date", type="int", 
                  help = "Day of month  ")
parser.add_option("-o",  dest="offset", type="int", 
                  help = "Offset (minutes)")
(options, args) = parser.parse_args()

if options.port:
   port = options.port
if options.time:
   time = options.time
if options.offset:
   offset = options.offset



#
# Parse url params. Ignore version for now, we can see it in the apache logs
#
for field in form.keys():
    if field == "port" :
       port = form[field].value
    if field == "time"   and form[field].value != 'undefined':
       time = form[field].value
    if field == "vsn"    and form[field].value != 'undefined':
       version = form[field].value
    if field == "offset" and form[field].value != 'undefined':
       offset = int(form[field].value)

if time == 0:
   time="13:37"
   version="2.2"
   #sys.stderr.write("No time param passed \n")  #// apache errors.log
   #print "Status: 406 Not Acceptable\r\n"
   #print "Content-Type: text/html\r\n\r\n"
   #print "<h1>406 Missing param: time</h1>"
   #sys.exit(1)


#
# Port can have an optional country code, its only for the config screen, so strip off
#
if ":" in port:
   port = port.split(":")[1]


#
# Create a tidesdata request object 
#
tides = tidedata.public(port)


#
# Scrape data
#
tides.scrape()
if tides.error: 
   sys.stderr.write(tides.error+"\n")  #// apache errors.log
   print "Status: 404 Not Found\r\n"
   print "Content-Type: text/html\r\n\r\n"
   print "<h1>404 File not found!</h1>"
   sys.exit(1)

#
# Adjust UK ports for BST. The others will just haveto have a DST flag
#
if (int(port) < 801):
   tides.adjust_bst()

#
# ..which we handle here
#
if (offset != 0):
   tides.adjust_offset(offset)

#
# Use passed localtime to only show tides > now
# - only on v3 client
if (int(float(str(version))) >= 3):

   #sys.stderr.write ("version 3")
      # Use passed localtime to only show tides > now
   tides.delete_in_past(time)
      # In addition to the above, for tz's ahead of UTC easytide will still return yesterdays data. 
   if options.date:
      tides.delete_in_past_day(options.date)
else:
   from datetime import datetime
   from dateutil import tz
   #sys.stderr.write ("version 2")
   from_zone = tz.gettz('UTC')
   to_zone = tz.gettz('Europe/London')
   utc = datetime.utcnow()
   utc = utc.replace(tzinfo=from_zone)
   london = utc.astimezone(to_zone)
   tides.delete_in_past(london.strftime("%H:%M"))

#
# Return json 
#
print "Content-type: application/json"
print
tides.dump()



# EOF
