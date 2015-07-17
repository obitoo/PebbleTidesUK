#!/usr/bin/python
#
# My timeline wrapper - uses pypebbleapi
#

from pypebbleapi import Timeline, Pin
from datetime import datetime

import urllib3.contrib.pyopenssl
urllib3.contrib.pyopenssl.inject_into_urllib3()

class myTimeline(object):

   def __init__ (self, my_api_key):
      self.timeline = Timeline(my_api_key, "https://timeline-api.getpebble.com")
   
   
   # TODO - actions
   def send_user_pin (self, user_key, hi_lo_string, date_time, height, portname):
       # add the T eg '2015-07-15 21:17:53.217456Z' to '2015-07-15T21:17:53.217456Z'
       date_time=date_time.replace (" ","T",1)

       layout={
           "type": "weatherPin"
          ,"title": hi_lo_string
          ,"subtitle": height
          ,"tinyIcon": "system://images/TIDE_IS_HIGH"
          ,"largeIcon": "system://images/TIDE_IS_HIGH"
          ,"locationName": portname
          ,"lastUpdated": datetime.utcnow().isoformat()+'Z'
         }
#       actions =  '[ { "title": "Open Cuntface!", "type": "openWatchApp" }]'

       pin_id = self.next_pin_id()
       my_pin = Pin(id=pin_id, time = date_time, layout = layout)#, actions = actions)

       result=self.timeline.send_user_pin(user_key, my_pin)

       return result



   # TODO - actions
   def send_shared_pin (self, port_id,  hi_lo_string, date_time, height, portname, utc_offset):

       # add the T eg '2015-07-15 21:17:53.217456Z' to '2015-07-15T21:17:53.217456Z'
       date_time=date_time.replace (" ","T",1)


       if (hi_lo_string == "hi"):
          high_low = "High Tide"
       if (hi_lo_string == "lo"):
          high_low = "Low Tide"

       layout={
           "type": "weatherPin"
          ,"title": high_low
          ,"subtitle": height
          ,"tinyIcon": "system://images/TIDE_IS_HIGH"
          ,"largeIcon": "system://images/TIDE_IS_HIGH"
          ,"locationName": portname
          ,"lastUpdated": datetime.utcnow().isoformat()+'Z'  # aha - isformat is the T
         }
#       actions =  '[ { "title": "Open Watchface!", "type": "openWatchApp" }]'

       pin_id = self.next_pin_id()
       my_pin = Pin(id=pin_id, time = date_time, layout = layout)#, actions = actions)

       # topic: hi_0110_-60
       topic = hi_lo_string + "_" + port_id + "_" + str(utc_offset)

       # make request
       result=self.timeline.send_shared_pin([topic], my_pin)
       print "Sent to topic: ",topic

       return result


   # TODO - lame, do in sql
   def next_pin_id (self):
      filename="./pin_id.txt"
      with open(filename, "r+") as f:
         data = f.read()
         data = int(data) + 1
         f.seek(0)
         f.write(str(data))
      return str (data -1)

       

          
          
          
