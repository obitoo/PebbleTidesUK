#!/usr/bin/python
# 
# 
# Owen Bullock - UK Tides - UKHO Easytide webscrape.
#
# 16 Mar 15 - Created this file
# 17Mar      - DST, ish. Add date to array
# 12May      - return port name
#

import urllib2
import sys
import pprint
import json
from bs4 import BeautifulSoup
from datetime import datetime, timedelta
from dateutil import tz


Max_Portname_Length=30



class public():

   def __init__ (self,port):
     self.port = port
     self.error = None

     # Validate instance vars
     if (port == "null" or port == ""):
        self.error = "Invalid null port"



   #
   # pull from easytide, parse and store
   #
   def scrape(self):
     
     url='http://www.ukho.gov.uk/easytide/EasyTide/ShowPrediction.aspx?PortID='+self.port+'&PredictionLength=7'
     html_doc = urllib2.urlopen(url)#.read()
     
     #soup = BeautifulSoup(html_doc, "lxml")
     soup = BeautifulSoup(html_doc)
     
     
     #
     # Port details/name 
     #
     rows =  soup.find_all('span', {'class':'PortName'})
     self.portname = rows[0].get_text()[:Max_Portname_Length]
        
     
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

            #testing
            #if int(day)==19:
            #   day="31"
            #if int(day)==20:
            #   day="1"
        

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
     
     


   #
   #  util
   #
   def _hhmm_to_mins(self, timestr):
       return int(timestr[:2])*60+int(timestr[3:])



   #
   #  User defined time offset - used for non UK DST or manual port adjustment
   #
   def adjust_offset(self, offset):
      for element in self.tides_array:
         #print element
         date_object = datetime.strptime('Jan '+element["day"]+' 2001 ' +element["time"], '%b %d %Y %H:%M')
         date_object = date_object + timedelta(minutes = offset)
         
         element["time"] = date_object.strftime("%H:%M")
         element["day"]  = date_object.strftime("%d")
         #print element["time"]



   
   #
   #  Get current UK time - use tz to get BST/GMT depending
   #
   def adjust_bst(self):
     
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
   #  Website gives all tides from midnight, discard any in the past. 
   #
   def delete_in_past(self, time_hhmm):
     prevmins=0
     minsnow=self._hhmm_to_mins(time_hhmm)
     for i in range(0,4):
        t=self.tides_array[0]["time"]   # always zero :) 
        tidemins=self._hhmm_to_mins(t)
        if (tidemins < minsnow ) and (tidemins > prevmins):  # if its suddenly less we;ve hit midnight
           del self.tides_array[0]
           prevmins=tidemins

   #
   # similarly for date. TODO: 1st of month
   #
   def delete_in_past_day(self, day):
     for i in range(0,4):
        if int(self.tides_array[0]["day"]) < day:
           del self.tides_array[0]
        if day == 1 and int(self.tides_array[0]["day"]) >= 27:
           del self.tides_array[0]






   #
   # file - max 5 entries
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
     print ( ',"portname":"'+self.portname+'"')
     print  ( "}"              )
    

   #
   # format dict for timeline
   #  TODO month end
   #
   def dictionary(self):
     array=[]
     for i in range(0,6):
       row={}
       d=datetime.utcnow()
       #print self.tides_array[i]["time"]
       d=d.replace (day = int(self.tides_array[i]["day"]))
       d=d.replace (hour = int(self.tides_array[i]["time"][:2]))
       d=d.replace (minute = int(self.tides_array[i]["time"][3:]))
#       d=d.replace (second = 0)
#       d=d.replace (microsecond = 0)
#       d=d.replace (tzinfo = 'Z')

       row["state"]=self.tides_array[i]["state"]
       row["datetime"]=d
       row["height"]=float(self.tides_array[i]["height"])/10
       row["portname"]=self.portname

       array.append(row)

     return array

   # 
   # Topic subs - TODO - sql. 
   #              TODO - this design can't work out when a topic no longer has any subscribers. 
   # 
   def store_timeline_subscription( self, sub_hi, sub_lo, port_no, utc_offset):
      if (sub_hi == "on"):
         topic =  "hi_" + port_no + "_" + str(utc_offset)
         self.store_topic (topic)

      if (sub_lo == "on"):
         topic =  "lo_" + port_no + "_" + str(utc_offset)
         self.store_topic (topic)


   def store_topic (self, topic):
      filename="/var/www/tides/topics.txt"

      # check if already exists. 
      with open(filename, "r") as f:
         line = f.read()
         if (line == topic):
            return

      # if not, append
      with open(filename, "a") as f:
         f.write(topic+'\n')

