#!/usr/bin/python
# 
# 
# Owen Bullock - UK Tides - UKHO Easytide webscrape. We pay taxes for this fucking data
# 25Sep2014 - Created
# TODO - timezones - done
# TODO - cope with only 3 tides a day - done
# remove . from height string, for little pebbles sake 
#

import urllib2
import sys
import pprint
import json
from bs4 import BeautifulSoup


#
outfile="/var/www/tides0110.json"

print "----------- starting"


#
# -d flag to dump
#
dump_file=False
if len(sys.argv) == 2 and sys.argv[1] == '-d':
   dump_file=True

#
# Download and parse tides data - location 0110 = southend, Essex
#
url='http://www.ukho.gov.uk/easytide/EasyTide/ShowPrediction.aspx?PortID=0110&PredictionLength=7'
print "opening ",url
html_doc = urllib2.urlopen(url)#.read()

soup = BeautifulSoup(html_doc)
if dump_file == True:
   print (soup.prettify())


#
# parse,and build json
#

  #
  # Port details
  #

  # todo?

  #
  # tide times
  #  todo - combinenext 2 blocks
array=[]

#for first in  soup.find_all('table', {'class':'HWLWTable first'} ):
#  for tr in first.find_all('tr'): 
#    i = 0
#
#    # either th or td
#    for th in tr.findAll('th', {'class':'HWLWTableHWLWCell'}):
#       line={}
#       i = i + 1
#       print "AA",i,th.string
#       if th.string == 'LW':
#          line['state']='lo'
#       else:
#          line['state']='hi'
#       array.append(line)
#     
#    for td in tr.findAll('td', {'class':'HWLWTableCell'}):
#       print "BB",i,td.string
#
#       if "m" in td.string:  # height
#          print i
#          array[i]['height']=td.string[:3].encode("ascii")
#
#       if ":" in td.string:  # time
#          array[i]['time']=td.string[1:].encode("ascii")
#       i = i + 1
#
imax=0

for first in  soup.find_all('table', {'class':'HWLWTable'} ):  # also picks up 'HWLWTable first' it seems?
  print "----------------------"
  for tr in first.find_all('tr'):
    i = imax

    # either th or td
    for th in tr.findAll('th', {'class':'HWLWTableHWLWCell'}):
       line={}
       print "A",i,th.string
       if th.string == 'LW':
          line['state']='lo'
       else:
          line['state']='hi'
       array.append(line)
       i = i + 1

    for td in tr.findAll('td', {'class':'HWLWTableCell'}):
       print "B",i,td.string

       if "m" in td.string:  # height
          print i
          hm=td.string[:3].encode("ascii")
          # remove .
          array[i]['height']=hm.replace(".","")

       if ":" in td.string:  # time
          array[i]['time']=td.string[1:].encode("ascii")
       i = i + 1
  imax=i



#
#  Get current UK time - use tz to get BST/GMT depending
#
def hhmm_to_mins(timestr):
  return int(timestr[:2])*60+int(timestr[3:])

from datetime import datetime
from dateutil import tz

# timezone - magic!
from_zone = tz.gettz('UTC')
to_zone = tz.gettz('Europe/London')

# get time
utc = datetime.utcnow()
print "utc time now = ",utc.strftime('%m/%d/%Y %H:%M:%S %Z')

# Convert time zone
utc = utc.replace(tzinfo=from_zone)
london = utc.astimezone(to_zone)
minsnow=hhmm_to_mins(london.strftime("%H:%M"))
print "london time now = ",london.strftime('%m/%d/%Y %H:%M:%S %Z')
print "london time now, mins= ",minsnow

#
# Modify all tidetimes to local 
#
print "Modify all tidetimes to local:"
for element in array:
   utc_str= element["time"]
   result = utc.replace(hour=int(utc_str[:2]), minute=int(utc_str[3:]))
   result=result.astimezone(to_zone)
   result_str=result.strftime("%H:%M")
   element["time"]=result_str
   ## print "%s -> %s",(utc_str,element["time"])



#
#  Website gives all tides from midnight, discard any in the past
#
print " discard todays entries that are < now"
prevmins=0
for i in range(0,4):
   t=array[0]["time"]
   print t
   tidemins=hhmm_to_mins(t)
   if (tidemins < minsnow ) and (tidemins > prevmins):  # if its suddenly less we;ve hit midnight
      print "discarding"
      del array[0]
      prevmins=tidemins
   else:
      print "keeping"





#
# Output file - max 5 entries
#
print "Opening ",outfile
f=open(outfile, 'w')

f.write( "{"               )
f.write( '"tides":['         )
first=True
for i in range(0, 6):
   if not first:
      f.write(",")
   f.write(json.dumps(array[i]))
   first=False

f.write( "]"              )
f.write( "}"              )
f.close()

print "Written ",outfile


#
# log to stdout
#
with open (outfile, "r") as f :
   for line in f:
      print line

print "finished @", london.strftime('%m/%d/%Y %H:%M:%S %Z')



