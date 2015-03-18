#!/usr/bin/python
# 
# 
# Owen Bullock - UK Tides - UKHO Easytide webscrape.
#
# 16 Mar 15 - Created this file
# 17Mar      - DST, ish. Add date to array
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


   #
   # pull from easytide, parse and store
   #
   def scrape(self):
     
     url='http://www.ukho.gov.uk/easytide/EasyTide/ShowPrediction.aspx?PortID='+self.port+'&PredictionLength=7'
     html_doc = urllib2.urlopen(url)#.read()
     
     soup = BeautifulSoup(html_doc)
     
     
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

         for th in tr.findAll('th', {'class':'HWLWTableHeaderCell'}):
            # ---- dates  ----------
            # assume: day is chars 4-5. TODO - test on 1 Apr
            day=th.string[4:][:-4]

         # either th or td
         for th in tr.findAll('th', {'class':'HWLWTableHWLWCell'}):

            #
            # ---- hi/lo indicators
            #
            line={'day': day }
            if th.string == 'LW':
               line['state']='lo'
            else:
               line['state']='hi'
            array.append(line)
     
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
     
     
     # Exit and warn if nothing scraped. 
     if len (array) == 0: 
        self.error="No tide data scraped for port ("+self.port+")"
        return

     # save
     self.tides_array = array
     
     


   def _hhmm_to_mins(self, timestr):
       return int(timestr[:2])*60+int(timestr[3:])

   #
   #  Get current UK time - use tz to get BST/GMT depending
   #
   def adjust_bst(self):
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
     minsnow=self._hhmm_to_mins(self.london.strftime("%H:%M"))
     

     # Modify all tidetimes to local 
     for element in self.tides_array:
        utc_str= element["time"]
        result = utc.replace(hour=int(utc_str[:2]), minute=int(utc_str[3:]))
        result=result.astimezone(to_zone)
        result_str=result.strftime("%H:%M")
        element["time"]=result_str
     
 
     
     
  

   #
   #  Website gives all tides from midnight, discard any in the past
   #
   def delete_in_past(self, time_hhmm):
     prevmins=0
     minsnow=self._hhmm_to_mins(time_hhmm)
     for i in range(0,4):
        t=self.tides_array[0]["time"]
        tidemins=self._hhmm_to_mins(t)
        if (tidemins < minsnow ) and (tidemins > prevmins):  # if its suddenly less we;ve hit midnight
           del self.tides_array[0]
           prevmins=tidemins


   #
   # Output file - max 5 entries
   #
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
    
    


