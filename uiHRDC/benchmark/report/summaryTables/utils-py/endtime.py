
from sys import argv
from mytiming import *

#----------main -----------------------------------------------
def tests2():
    start = datetime.datetime.now()

    # some code
    for i in xrange(1,10000000):
        d = i%55

    end = datetime.datetime.now()
    elapsed = end - start


    print(start)
    print(end)
    print(elapsed)
    # or
    #print(elapsed.seconds,":",elapsed.microseconds) 

    #-----------
    strstart = datetime2str(start)
    strend = datetime2str(end)
    elapsedstr = timedelta2str(str2datetime(strend) - str2datetime(strstart))
    print "elapsedstr", elapsedstr
    #-----------

    readdate=str2datetime("2018-04-22 15:38:35.835000")
    print readdate

    readdate=str2datetime("2018-11-27 00:00:00.0")
    print readdate

    elapsed = end - readdate
    #print(timedelta2strOLD(elapsed))
    print(timedelta2str(elapsed))


    val= read_str_with_datetime("B.dat")
    if val == None:
        print "text-->"+"start-time-not-valid"+"<--"
    else:
        print "text-->"+datetime2str(val)+"<--"
 
 
##--main --------------------------------------------------------------##

if len(argv) !=2:
    print "Incorrect syntax! You must provide a file to save timings"
    print "   use: python %s <filename>  " % argv[0]
    exit(0)

logFile = argv[1]
logFileE = argv[1]+".elapsed"

# the logFile must exist!
if not existsFile(logFile):
    print "the log-file you provided does not exist!"
    exit(0);

deleteFileIfExists(logFileE)  ## file for elapsed times only

# Tries to parse the starting datetime, from logFile
val= read_str_with_datetime(logFile)
if val == None:
	print "File "+ logFile+" does not contain a well-formatted starting time!"
	exit(0);
else:
	start = val
    #print "text-->"+datetime2str(val)+"<--"


## Gets current (end)date, calculates elapsed-times and appends 
## both ending time and elapsed time to logFile.
## elapsed time is also set into logFile.elapsed

end = datetime.datetime.now()

#-----------
strstart = datetime2str(start)
strend   = datetime2str(end)
elapsedstr = timedelta2str(end - start)
#print "elapsedstr", elapsedstr
#-----------

addDataToLog(logFile, "end-time    : "+strend)
addDataToLog(logFile, "elapsed-time: "+elapsedstr)

addDataToLog(logFileE, elapsedstr)




