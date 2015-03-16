#!/usr/bin/python
# 
# 
# Owen Bullock - UK Tides - UKHO Easytide webscrape. 
#                         - cgi script
#                           Params:     port=<4 char string>
#                                       time=hh:mm
#                                       vsn=<client version no>
#  TODO
#    -error handling         
#                           
# 16Mar15 - Created
#
#

import json
import tidedata
import cgi, cgitb, os, sys
import optparse


#
# test cmdline options:   -p <port no> : testrun for one port
#
parser = optparse.OptionParser("usage: %prog [options] ")
parser.add_option("-p",  dest="port", type="string", 
                  help = "port number, run for single ")
(options, args) = parser.parse_args()

if options.port:
   port = options.port



#
#  Init 
#
cgitb.enable(); # formats errors in HTML
form = cgi.FieldStorage()



#
# Parse params. Ignore version for now, we can see it in the logs
#
for field in form.keys():
    if field == "port":
       port = form[field].value
    if field == "time":
       time = form[field].value

#
# Create a tidesdata request object 
#
tides = tidedata.public(port)


#
# Scrape data
#
tides.scrape()
if tides.error: 
   sys.stderr.write(tides.error+"\n")
   print "Status: 404 Not Found\r\n"
   print "Content-Type: text/html\r\n\r\n"
   print "<h1>404 File not found!</h1>"

   sys.exit(1)


#
# Return json 
#
print "Content-type: application/json"
print
tides.dump()



# EOF
