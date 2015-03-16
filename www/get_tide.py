#!/usr/bin/python
# 
# 
# Owen Bullock - UK Tides - UKHO Easytide webscrape. 
#                         - cgi script
#                           Params:     port=<4 char string>
#                                       time=hh:mm
#                                       vsn=<client version no>
#                           
#                           
#                           
# 16Mar15 - Created
#

import urllib2
import sys
import pprint
import json
from bs4 import BeautifulSoup

import tidedata




def scrape_and_create_json_file (port):
     if port == "0":
          return

     tides = tidedata.public(port)
     tides.scrape()
     tides.dump_to_file(g_outdir)

     
     


#
# MAIN # # # # # # # # # # # # # # ##
#
print "----------- starting"

g_outdir="/var/www/tides/"

import cgi, cgitb, os, sys
cgitb.enable(); # formats errors in HTML


form = cgi.FieldStorage()

for field in form.keys():
    print "<tr><td>%s<td>%s" % (field, form[field].value)



#   scrape_and_create_json_file(options.port)

