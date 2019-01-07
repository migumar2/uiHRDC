
from sys import argv
from mytiming import *


 
 
##--main --------------------------------------------------------------##

if len(argv) !=3:
    print "Incorrect syntax! You must provide ..."
    print "...  a file to save timings and a title-message"
    print "   use: python %s <filename> <title> " % argv[0]
    exit(0)

logFile = argv[1]
msg     = argv[2]

# we could not include "start-time:" in the file, since we will search
#for "start-time:" within the file later (so it would occur only once!)
if msg.find("start-time  :",0,len(msg)) != -1:
    msg = msg.upper()

## Gets current date, and appends it to logFile.
deleteFileIfExists(logFile)
start = datetime.datetime.now()
txt = datetime2str(start)
addDataToLog(logFile, msg)
addDataToLog(logFile, "start-time  : "+txt)




