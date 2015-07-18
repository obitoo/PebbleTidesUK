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
    #"locationName": "Southend-on-Sea",
    "locationName": "Margate-Sea",
#    "lastUpdated": "2015-07-11T23:24:00.000Z"
    "lastUpdated": datetime.utcnow().isoformat()+'Z'
  }
my_pin = Pin(id='125', time = '2015-07-15T23:27:00.000Z', layout = layout)

#actions =  '[ { "title": "Open Cuntface!", "type": "openWatchApp" }]'
#my_pin = Pin(id='124', time = '2015-07-15T23:26:00.000Z', layout = layout, actions = actions)

# Example: works in cloudpebble
#{ "actions": [ { "title": "Open Cuntface!", "type": "openWatchApp" }],
# "id": "124",
# "layout": {"largeIcon": "system://images/TIDE_IS_HIGH",
#            "lastUpdated": "2015-07-11T21:59:35.764097Z",
#            "locationName": "Majorca",
#            "subtitle": "-3.0",
#            "tinyIcon": "system://images/TIDE_IS_HIGH",
#            "title": "High Tide",
#            "type": "weatherPin"},
# "time": "2015-07-15T23:26:00.000Z"}


#result=timeline.send_shared_pin(['0110'], my_pin)
#result=timeline.send_user_pin(user_key, my_pin)
#result=timeline.delete_user_pin(user_key, my_pin)

# trying to make subs request - didnt qork
#import requests
#url = "https://timeline-api.getpebble.com/v1/user/subscriptions/"
#result = requests.post(
#            url,
#            headers={
#                'X-User-Token': user_key,
#            },
#)

# unsubscribe
result = timeline.unsubscribe(user_key, "hi_0108_-60")


print result

