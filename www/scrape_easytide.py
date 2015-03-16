#!/usr/bin/python
# 
# 
# Owen Bullock - UK Tides - UKHO Easytide webscrape. 
#                         - Parse config.html for list of ports and create a json file for each. 
#                           
# 25Sep2014 - Created
# TODO - timezones - done
# TODO - cope with only 3 tides a day - done
# remove . from height string, for little pebbles sake 
# 03Dec14 - multiple ports
# 15Mar15 - 3 digit heights
# 16Mar15 - tidedata obj
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

     
     

def run_scrape_for_all_ports():
   # parse config file for list of possible ports
   htmldoc= urllib2.urlopen(g_configfile).read()
   soup = BeautifulSoup(htmldoc)

   portlist1 = [str(x['value']) for x in soup.find(id=["port1"]).find_all('option')]
   portlist2 = [str(x['value']) for x in soup.find(id=["port2"]).find_all('option')]
   
   print portlist1
   print portlist2
   
   #loop for each primary port
   for port in portlist1:
        scrape_and_create_json_file(port)
   
   #loop for each secondary port
   for port in portlist2:
        scrape_and_create_json_file(port)




#
# MAIN # # # # # # # # # # # # # # ##
#
print "----------- starting"

g_outdir="/var/www/tides/"
g_configfile="file:///var/www/tides/config.html"


#
# options:   -p <port no> : testrun for one port
#
import optparse
parser = optparse.OptionParser("usage: %prog [options] ")
parser.add_option("-p",  dest="port", type="string", 
                  help = "port number, run for single ")
(options, args) = parser.parse_args()


#
#  Run for a single port, or look at config file to get list of all ports to scrape. 
#

if options.port:
   print "Single port TESTMODE:", options.port
   scrape_and_create_json_file(options.port)
else:
   run_scrape_for_all_ports() 

