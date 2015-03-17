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
#                           
# 16Mar15 - Created
# 17Mar15 - use time param
#
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
port = 0
time = 0

#
# test cmdline options:   -p <port no> : testrun for one port
#
parser = optparse.OptionParser("usage: %prog [options] ")
parser.add_option("-p",  dest="port", type="string", 
                  help = "port number, run for single ")
parser.add_option("-t",  dest="time", type="string", 
                  help = "current (local) time in HH:MM")
(options, args) = parser.parse_args()

if options.port:
   port = options.port
if options.time:
   time = options.time





#
# Parse url params. Ignore version for now, we can see it in the apache logs
#
for field in form.keys():
    if field == "port":
       port = form[field].value
    if field == "time":
       time = form[field].value

if time == 0:
   sys.stderr.write("No time param passed \n")  #// apache errors.log
   print "Status: 406 Not Acceptable\r\n"
   print "Content-Type: text/html\r\n\r\n"
   print "<h1>406 Missing param: time</h1>"
   sys.exit(1)


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
# Use passed localtime to only show tides > now
#
tides.delete_in_past(time)


#
# Return json 
#
print "Content-type: application/json"
print
tides.dump()



# EOF
