Pebble Tides Watchface

UK Only - tide info scraped from Easytide

Pebble SDK 2

Python 2.6 serverside - server cron job does the heavy lifting, small json served to Pebble upon request.  
Assumes Pebble is running local UK time, server adjusts tide times before creating json.  

Python pre:
pip install beautifulsoup4;
pip install python-dateutil



