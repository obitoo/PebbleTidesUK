#!/usr/bin/python
#
#
from datetime import datetime


# Sandbox key: SBna3itxdgseq99nbw87kur33gi1q4yi
#
#  
app_api_key="SBna3itxdgseq99nbw87kur33gi1q4yi"

# 
# Test user key (mine)
#
user_key="SBACTtnBhLD2Pig05GxWaT2FqvpwC0i2"



#
# My wrapper
#
from timeline import myTimeline
t=myTimeline( app_api_key )


#
# Send tide pin
#
result = t.send_user_pin( user_key = user_key
                         ,tz_offset_mins = 2                  # TODO
                         ,hi_lo_string   = "High Tide"
                         ,date_time      = 4                  # TODO
                         ,height         = "-3.1"
                         ,portname       = "Majorca")

if result.status_code != 200:
   print result.status_code
else:
   print "200 - ok"



