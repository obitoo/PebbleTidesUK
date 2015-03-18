#!/usr/bin/python
# 
# 
# Owen Bullock - UK Tides - UKHO Easytide webscrape. 
#                           
#
# >> more portlists/5.txt 
# 5379,Buleleng 
# 5380,Teluk Padang 
# 5381,Sanur 
# 5382,Benoa 
#
#

import json
import tidedata
import cgi, cgitb, os, sys
import optparse

#
#  Init 
#
cgitb.enable(); # formats errors in HTML
form = cgi.FieldStorage()
country = 0

#
# test cmdline options:   -p <port no> : testrun for one port
#
parser = optparse.OptionParser("usage: %prog [options] ")
parser.add_option("-c",  dest="country", type="string",
                  help = "country number")
(options, args) = parser.parse_args()

if options.country:
   country = options.country


#
# Parse url params. Ignore version for now, we can see it in the apache logs
#
for field in form.keys():
    if field == "country":
       country = form[field].value

if country == 0:
   sys.stderr.write("No country param passed \n")  #// apache errors.log
   print "Status: 406 Missing param\r\n"
   print "Content-Type: text/html\r\n\r\n"
   print "<h1>406 Missing param: country</h1>"
   sys.exit(1)



#
#  Find the country file 
#
portsfile="portlists/"+country+".txt"
try:
  fo = open(portsfile, "r")
except:
  sys.stderr.write("FAIL to open "+portsfile+"\n")
  print "Status: 406 File not found\r\n"
  print "Content-Type: text/html\r\n\r\n"
  print "<h1>406 Missing file: "+portsfile+"</h1>"
  sys.exit(1)


#
#  Print the file 
#
print "Content-Type: text/html\n"

for line in fo:
   if line.strip():                     # ignore blank lines
     port=line.split(',')[0]
     name=line.split(',')[1].rstrip()   # chomp

     if port == "optgroup":
        print  ('<optgroup label="%s">'% ( name))
     else:
        print  ('<option value="%s">%s</option>'% ( port, name))




# EOF
