#!/usr/bin/python
#
# Sandbox key:
#
from pypebbleapi import Timeline, Pin
import datetime




# Sandbox key: SBna3itxdgseq99nbw87kur33gi1q4yi
#
#  
my_api_key="SBna3itxdgseq99nbw87kur33gi1q4yi"

# Python debugger
#import pdb; pdb.set_trace()

timeline = Timeline(my_api_key, "https://timeline-api.getpebble.com")

my_pin = Pin(id='123', time = datetime.date.today().isoformat())


timeline.send_shared_pin(['a_topic', 'another_topic'], my_pin)

