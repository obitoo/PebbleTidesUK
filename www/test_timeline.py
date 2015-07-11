#!/usr/bin/python
#
# Sandbox key:
#
from pypebbleapi import Timeline, Pin
from datetime import datetime



import urllib3.contrib.pyopenssl
urllib3.contrib.pyopenssl.inject_into_urllib3()

# Sandbox key: SBna3itxdgseq99nbw87kur33gi1q4yi
#
#  
my_api_key="SBna3itxdgseq99nbw87kur33gi1q4yi"

user_key="SBACTtnBhLD2Pig05GxWaT2FqvpwC0i2"


# Python debugger
#import pdb; pdb.set_trace()

timeline = Timeline(my_api_key, "https://timeline-api.getpebble.com")

layout={'type' : 'genericPin', 
        'title': 'Example Pin',
        'tinyIcon': 'system://images/SCHEDULED_EVENT'}

notify={ 'layout': layout}

## no notifications?  NB update needs a time
#my_pin = Pin(id='123', time = '2015-07-11T23:24:00.000Z', layout = layout, updateNotification = notify)
# works
#my_pin = Pin(id='123', time = '2015-07-11T23:25:00.000Z', layout = layout, createNotification = notify)
#my_pin = Pin(id='123', time = datetime.utcnow().isoformat()+'Z', layout = layout, createNotification = notify)

# Tides
layout={
    "type": "weatherPin",
    "title": "Low Tide",
    "subtitle": "0.6",
    "tinyIcon": "system://images/TIDE_IS_HIGH",
    "largeIcon": "system://images/TIDE_IS_HIGH",
    "locationName": "Southend-on-Sea",
#    "lastUpdated": "2015-07-11T23:24:00.000Z"
    "lastUpdated": datetime.utcnow().isoformat()+'Z'
  }
#my_pin = Pin(id='124', time = '2015-07-11T23:26:00.000Z', layout = layout)
actions =  '[ { "title": "Open Cuntface!", "type": "openWatchApp" }]'
 
my_pin = Pin(id='124', time = '2015-07-11T23:26:00.000Z', layout = layout, actions = actions)





#result=timeline.send_shared_pin(['a_topic', 'another_topic'], my_pin)
result=timeline.send_user_pin(user_key, my_pin)
#result=timeline.delete_user_pin(user_key, my_pin)
print result

