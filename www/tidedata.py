#!/usr/bin/python
# 
# 
# Owen Bullock - UK Tides - UKHO Easytide webscrape.
#
# 16 Mar 15 - Created this file
#

import urllib2
import sys
import pprint
import json
from bs4 import BeautifulSoup





class public():

   def __init__ (self,port):
     self.port = port
     self.error = None


   def scrape(self):
     
     url='http://www.ukho.gov.uk/easytide/EasyTide/ShowPrediction.aspx?PortID='+self.port+'&PredictionLength=7'
     html_doc = urllib2.urlopen(url)#.read()
     
     soup = BeautifulSoup(html_doc)
     
     #
     # parse,and build json
     #
     
       #
       # Port details/name - TODO
       #
     
     
     #
     # tide times
     #  
     array=[]
     
     imax=0
     
     for first in  soup.find_all('table', {'class':'HWLWTable'} ):  # also picks up 'HWLWTable first' it seems?
       for tr in first.find_all('tr'):
         i = imax
     
         # either th or td
         for th in tr.findAll('th', {'class':'HWLWTableHWLWCell'}):
            #
            # ---- hi/lo indicators
            #
            line={}
            if th.string == 'LW':
               line['state']='lo'
            else:
               line['state']='hi'
            array.append(line)
            i = i + 1
     
         for td in tr.findAll('td', {'class':'HWLWTableCell'}):
            #
            # ---- height
            #
            if "m" in td.string:  

               # example:   <td class="HWLWTableCell">0.5 m</td>
               hm=td.string[:-2].encode("ascii")

               # remove .
               # zero pad front to 3 chars
               array[i]['height']=hm.replace(".","").zfill(3)

     
            #
            # ---- times 
            #
            if ":" in td.string: 
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
     
     # Convert time zone
     utc = utc.replace(tzinfo=from_zone)
     self.london = utc.astimezone(to_zone)
     minsnow=hhmm_to_mins(self.london.strftime("%H:%M"))
     
     #
     # Modify all tidetimes to local 
     #
     for element in array:
        utc_str= element["time"]
        result = utc.replace(hour=int(utc_str[:2]), minute=int(utc_str[3:]))
        result=result.astimezone(to_zone)
        result_str=result.strftime("%H:%M")
        element["time"]=result_str
     
     
     #
     # Exit and warn if nothing scraped. 
     #
     if len (array) == 0: 
        self.error="No tide data scraped for port ("+self.port+")"
        return
 
     
     #
     #  Website gives all tides from midnight, discard any in the past
     #
     prevmins=0
     for i in range(0,4):
        t=array[0]["time"]
        tidemins=hhmm_to_mins(t)
        if (tidemins < minsnow ) and (tidemins > prevmins):  # if its suddenly less we;ve hit midnight
           del array[0]
           prevmins=tidemins
     
     # 
     # save
     # 
     self.tides_array = array
  





     # Output file - max 5 entries
   def dump_to_file(self, outdir):
     outfile=outdir+"tides"+self.port+".json"
     f=open(outfile, 'w')
    
     f.write( "{"               )
     f.write( '"tides":['         )
     first=True
     for i in range(0, 6):
        if not first:
           f.write(",")
        f.write(json.dumps(self.tides_array[i]))
        first=False
    
     f.write( "]"              )
     f.write( "}"              )
     f.close()
     print "created ",outfile
    

   def dump(self):
     print ( "{"               ),
     print ( '"tides":['       )

     first=True
     for i in range(0, 6):
        if not first:
           print(",")
        print json.dumps(self.tides_array[i]),
        first=False
    
     print  ( "]"              ),
     print  ( "}"              )
    
    


