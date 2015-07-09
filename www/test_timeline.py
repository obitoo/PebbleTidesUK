#!/usr/bin/python
#
# Sandbox key:
#
from pypebbleapi import Timeline, Pin
import datetime



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
my_pin = Pin(id='123', time = '2015-07-09T23:00:00.000Z', layout = layout, createNotification = notify)
#my_pin = Pin(id='123', time = datetime.date.today().isoformat()+'Z')


#import pdb; pdb.set_trace()
#result=timeline.send_shared_pin(['a_topic', 'another_topic'], my_pin)
result=timeline.send_user_pin(user_key, my_pin)
print result

