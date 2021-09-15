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
from urllib2 import HTTPError

import sys
import pprint
import json
from bs4 import BeautifulSoup
from datetime import datetime, timedelta
from dateutil import tz
from time import strftime
import subprocess


Max_Portname_Length=30



class public():

   def __init__ (self,port):
     self.port = port
     self.error = None
     self.tides_array = []

     # Validate instance vars
     if (port == "null" or port == ""):
        self.error = "Invalid null port"


   def push_tide_to_array(self,day,state,height,time):
     line={'day': day }
     line['state']  =state
     line['height'] =height
     line['time']   =str(time)[0:2]+':'+str(time)[2:4]
     self.tides_array.append(line)


   def set_error(self, errormsg):
      # todays day
     utc = datetime.utcnow()
     day = str(utc.day)

     self.push_tide_to_array(day,'hi','153','0100')
     self.push_tide_to_array(day,'lo','053','0400')
     self.push_tide_to_array(day,'hi','153','0600')
     self.push_tide_to_array(day,'lo','053','0800')
     self.push_tide_to_array(day,'hi','153','1200')
     self.push_tide_to_array(day,'lo','053','1400')
     self.push_tide_to_array(day,'hi','153','1700')
     self.push_tide_to_array(day,'lo','053','1900')
     self.push_tide_to_array(day,'hi','153','2100')
     self.push_tide_to_array(day,'lo','053','2300')
     self.portname = errormsg
#     self.portname = ">> FIX IN PROGRESS.. <<"

   # 
   # pull data from UKHO api
   #
   # TODO - deal with non uk ports that return nothing - DONE
   # TODO - abstract the two urlib calls
   def get_tides(self):

     #
     # read api key
     #
     try:
       file = open("./api_key.txt")
       apikey = file.read().replace("\n", " ")
       file.close()
     except IOError:
       return self.set_error("No API Key")


     ## make Station info request to get port name from numeric id
     url='https://admiraltyapi.azure-api.net/uktidalapi/api/V1/Stations/'+self.port
     req = urllib2.Request(url)
     req.add_header('Ocp-Apim-Subscription-Key', apikey)
     try:
       resp = urllib2.urlopen(req)
     except HTTPError as err:
        #debug print err
        if err.code == 404 :
           return self.set_error("UKHO Brexit failure")
        elif err.code == 401 :
           return self.set_error("Bad API Key 401")
        else :
           return self.set_error("ERROR:"+str(err.code))


     stationcontent = resp.read()
     #print  "DEBUG:"+stationcontent

     json_array = json.loads(stationcontent)

     ## this is the output of this section 
     self.portname = json_array['properties']['Name']



     ## make second request, for tide times
     url='https://admiraltyapi.azure-api.net/uktidalapi/api/V1/Stations/'+self.port+'/TidalEvents'
     #print "DEBUG:"+url

     req = urllib2.Request(url)
     req.add_header('Ocp-Apim-Subscription-Key', apikey)
     try:
       resp = urllib2.urlopen(req)
     except HTTPError as err: 
        if err.code == 404 :
           return self.set_error("UKHO Brexit failure")
        else :
           return self.set_error("ERROR:"+str(err.code))


     tidescontent = resp.read()

     ## parse returned json array
     json_array = json.loads(tidescontent)

     #print  "DEBUG RAW:"+tidescontent
     for i in range(len(json_array)):
       #print "DEBUG"+json_array[i]['EventType'] + ':' +  json_array[i]['DateTime'] +'/'+str(json_array[i]['Height'])

         #returned format is  2021-09-16T07:20:00
         # or sometimes its 2021-09-16T07:20:00.333  ffs, so 0:19 strips that
       datestr=json_array[i]['DateTime'][0:19]
       tidedatetime=datetime.strptime(json_array[i]['DateTime'][0:19], '%Y-%m-%dT%H:%M:%S')

         ## day     00-31
       day=str(tidedatetime.day)
         ## state   hi|lo
       if json_array[i]['EventType'] == 'LowWater':
          state='lo'
       else:
          state='hi'
         ## height  000-999 (00.0m - 99.9m)
       height = '0'+str(json_array[i]['Height']).replace(".","").zfill(3)
       height=height[0:3]


         ## time    hhmm
       time=tidedatetime.strftime('%H%M')

       self.push_tide_to_array(day,state,height,time)





   #
   # pull from easytide, parse and store DEPRECATED
   #
   def scrape(self):
     return self.get_tides()
     #return self.temp_hack()



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
     for i in range(0,len(self.tides_array)):
        t=self.tides_array[0]["time"]   # always zero :) 
        tidemins=self._hhmm_to_mins(t)
        if (i< len(self.tides_array)):
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
   # quick workaround for double hi tides
   #
   def delete_double_hi (self):
      for i in range (0,len(self.tides_array)-2): 
         if (i< len(self.tides_array) -1):
            if  (self.tides_array[i]["state"] == self.tides_array[i+1]["state"]):
               del self.tides_array[i+1]




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
     #for i in range(0, 6):
     for i in range(len(self.tides_array)):
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
         
         # new topic? Then publish
         if (self.store_topic (topic)):
             self.publish(topic, "publish.hi.log")

      if (sub_lo == "on"):
         topic =  "lo_" + port_no + "_" + str(utc_offset)

         # new topic? Then publish
         if (self.store_topic (topic)):
             self.publish(topic,"publish.lo.log")



   # If its a new topic, return 1
   def store_topic (self, topic):
      filename="/var/www/tides/topics.txt"

      with open(filename, "r") as f:
         for line in f:
           if (line.rstrip() == topic):
              return 0

      # if not, append
      with open(filename, "a") as f:
         f.write(topic+'\n')
      return 1

   #
   # Call the script we use from cron. Takes a while so don't wait
   #

   def publish (self, topic, logfilename):

      path = '/home/owen/tides/prod'
      f = open (path + '/' + logfilename , "a+")
      path = '/usr/lib/cgi-bin/tides/'
      subprocess.Popen( [path + '/timeline_get_tide.py', '-t', topic], stdout=f, stderr=f)
      # thats it. Close file? 

