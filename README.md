Pebble Tides Watchface

UK Only - tide info scraped from Easytide

SDK 2.6
Python serverside - server cron job does the heavy lifting, small json served to Pebble upon request.  
Assumes Pebble is running local UK time, server adjusts tide times before creating json.  

Python pre:
pip install beautifulsoup4;
pip install python-dateutil



