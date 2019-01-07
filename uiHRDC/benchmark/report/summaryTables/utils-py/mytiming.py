import datetime
import os
from os.path import isfile, join


#----------------------------------------------------------------
#returns a datetime object from str = "2018-04-22 15:38:35.835000"
#returns None if conversion is not possible
def str2datetime(str):
    try:
        pos=0
        idx=str.find("-",pos,len(str))
        yyyy=str[pos:idx]
        pos=idx+1

        idx=str.find("-",pos,len(str))
        mt=str[pos:idx]
        pos=idx+1

        idx=str.find(" ",pos,len(str))
        dd=str[pos:idx]
        pos=idx+1

        idx=str.find(":",pos,len(str))
        hh=str[pos:idx]
        pos=idx+1

        idx=str.find(":",pos,len(str))
        mm=str[pos:idx]
        pos=idx+1

        idx=str.find(".",pos,len(str))
        ss=str[pos:idx]
        pos=idx+1

        idx = len(str)
        micros = str[pos:idx]

        #print "%s-%s-%s %s:%s:%s.%s" % (yyyy,mt,dd,hh,mm,ss,micros)
        dt = datetime.datetime(int(yyyy),int(mt),int(dd),int(hh),int(mm),int(ss),int(micros))
        return dt
    except:
        return None 


#returns a string from a datetime object
def datetime2str(dt):
   x=""+ dt.strftime("%Y-%m-%d %H:%M:%S.%f")
   return x

#returns a string with format "D day[s], ][H]H:MM:SS[.UUUUUU]" for a timedelta object
def timedelta2str(delta):
    dd=0
    hh=0
    mm=0
    ss=0
    us=0

    if hasattr(delta, 'seconds'):
        ss = delta.seconds %60

    if hasattr(delta, 'microseconds'):
        us = delta.microseconds

    if hasattr(delta, 'days'):
        dd = delta.days

    mm = (delta.seconds/60)%60

    hh = (delta.seconds/3600)%24
        
    st = ""
    if (dd>0):
        st = st + str(dd) + " days, "
    #st = st + str(hh).zfill(2)+"h"+str(mm).zfill(2)+"m"+str(ss).zfill(2)+"s"+"."+str(us)
    st = st + str(hh).zfill(2)+"h"+str(mm).zfill(2)+"m"
    st = st + str(ss).zfill(2)+"."+str(us/10000).zfill(2)+"s"

    return st


#--------------------------------------------------------------

##deletes a file if it exists
def deleteFileIfExists(filename):
    if os.path.exists(filename):
        os.remove(filename)

##returns true if file exists
def existsFile(filename):
    return os.path.exists(filename)

##Appends a line to the end of an existing/new file
def addDataToLog(logFile,line):
    data=line+'\n'
    f=open(logFile,"a")
    f.write(data)
    f.close()        
    print line
    
def repString(s,leng):
    return (s*leng)

##Reads file contents an returns a string
def readFile(filename):
    content=""
    if not isfile(filename): print 'File does not exist('+filename+')'
    else:
        # Open the file as f.
        f =open(filename,"r")
        content = f.read()
        f.close()
    return content  

##Looks for a "start-time  :" token in filename, and returns the
##corresponding datetime Object. Returns None if not found or
##an error occurred.
def read_str_with_datetime(filename):
    txt=""
    content = readFile(filename)
    pos =0
    idx=content.find("start-time  : ",pos,len(content))
    #print idx
    if (idx!= -1):
        pos = idx+len("start-time  :")
        idx=content.find("\n",pos,len(content))
        if (idx != -1):
            txt = content[pos:idx]
        else:
            txt = content[pos:len(content)]
    return str2datetime(txt)
 #   return txt  




##Looks for a "elapsed-time  :" token in filename, and returns the
##corresponding datetime Object. Returns None if not found or
##an error occurred.
def read_str_val_for_KEY(filename,key="elapsed-time:"):
    txt=""
    content = readFile(filename)
    pos =0
    idx=content.find(key,pos,len(content))
    #print idx
    if (idx!= -1):
        pos = idx+len(key)
        idx=content.find("\n",pos,len(content))
        if (idx != -1):
            txt = content[pos:idx]
        else:
            txt = ""
    return txt  


#----------tests -----------------------------------------------
def teststiming():
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
     
    key="start-time  :"
    val= read_str_val_for_KEY("B.dat",key)
    print "key="+ key+ " val ="+val

    key="end-time    :"
    val= read_str_val_for_KEY("B.dat",key)
    print "key="+ key+ " val ="+val

    key="elapsed-time:"
    val= read_str_val_for_KEY("B.dat",key)
    print "key="+ key+ " val ="+val

    key="elapsed-time:"
    val= read_str_val_for_KEY("B.dat")   ########################
    print "key="+ key+ " val ="+val

#----------main -----------------------------------------------
#teststiming()

