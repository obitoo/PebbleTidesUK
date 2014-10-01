#!/usr/bin/python
# 
# 
# Owen Bullock - UK Tides - UKHO Easytide 
# 25Sep2014 - Created
#

#
# TODO - timezones

import urllib2
import sys
import pprint
import json


#
outfile="/var/www/tides0110.json"



#
# -d flag to dump
#
dump_file=False
if len(sys.argv) == 2 and sys.argv[1] == '-d':
   dump_file=True

#
# Download and parse weather data - location 352448 = Loughton, Essex
#
url='http://www.ukho.gov.uk/easytide/EasyTide/ShowPrediction.aspx?PortID=0110&PredictionLength=7'
print "opening ",url
html_doc = urllib2.urlopen(url)#.read()

from bs4 import BeautifulSoup
soup = BeautifulSoup(html_doc)
if dump_file == True:
   print (soup.prettify())


#
# parse,and build json
#

  #
  # Port details
  #

  #
  # tide times
  #
array=[]
for first in  soup.find_all('table', {'class':'HWLWTable first'} ):
  for tr in first.find_all('tr'): 
    i = 0

    # either th or td
    for th in tr.findAll('th', {'class':'HWLWTableHWLWCell'}):
       line={}
       i = i + 1
       print "A",i,th.string
       if th.string == 'LW':
          line['state']='lo'
       else:
          line['state']='hi'
       array.append(line)
     
    for td in tr.findAll('td', {'class':'HWLWTableCell'}):
       print "B",i,td.string

       if "m" in td.string:  # height
          print i
          array[i]['height']=td.string[:3].encode("ascii")

       if ":" in td.string:  # time
          array[i]['time']=td.string[1:].encode("ascii")
       i = i + 1

imax=i

print "----------------------"
for first in  soup.find_all('table', {'class':'HWLWTable'} ):
  for tr in first.find_all('tr'):
    i = imax

    # either th or td
    for th in tr.findAll('th', {'class':'HWLWTableHWLWCell'}):
       line={}
       i = i + 1
       print "A",i,th.string
       if th.string == 'LW':
          line['state']='lo'
       else:
          line['state']='hi'
       array.append(line)

    for td in tr.findAll('td', {'class':'HWLWTableCell'}):
       print "B",i,td.string

       if "m" in td.string:  # height
          print i
          array[i]['height']=td.string[:3].encode("ascii")

       if ":" in td.string:  # time
          array[i]['time']=td.string[1:].encode("ascii")
       i = i + 1
  imax=i



#
#  Get current GMT dat
#
from time import gmtime, strftime
def hhmm_to_mins(timestr):
  return int(timestr[:2])*60+int(timestr[3:])

minsnow=hhmm_to_mins(strftime("%H:%M",gmtime()))

print "time now gmt, ",strftime("%H:%M",gmtime())
print "time now, mins= ",hhmm_to_mins(strftime("%H:%M",gmtime()))
print " discard todays entries that are < now"
for i in range(0,4):
   t=array[0]["time"]
   print t
   if (hhmm_to_mins(t) < minsnow ): 
      print "discarding"
      del array[0]




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

print "finished"


