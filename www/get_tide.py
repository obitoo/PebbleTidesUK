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


#
#  default 
#
port="0110"

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
# Create a tidesdata object 
#
tides = tidedata.public(port)


#
# Scrape data
#
tides.scrape()

#
# Return json 
#
print "Content-type: application/json"
print
tides.dump()



# EOF
