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
   
   
   def send_user_pin (self, user_key, tz_offset_mins,  hi_lo_string, date_time, height, portname):

       layout={
           "type": "weatherPin"
          ,"title": hi_lo_string
          ,"subtitle": height
          ,"tinyIcon": "system://images/TIDE_IS_HIGH"
          ,"largeIcon": "system://images/TIDE_IS_HIGH"
          ,"locationName": portname
          ,"lastUpdated": datetime.utcnow().isoformat()+'Z'
         }
       actions =  '[ { "title": "Open Cuntface!", "type": "openWatchApp" }]'
       
       my_pin = Pin(id='124', time = '2015-07-11T23:26:00.000Z', layout = layout, actions = actions)

       result=self.timeline.send_user_pin(user_key, my_pin)

       return result

       
#TODO
# pin id 
# tz offset
# datetime passed in right format

          
          
          
