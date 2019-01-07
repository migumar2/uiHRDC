# -*- coding: cp850 -*-
# cp1252, cp850, utf-8

import subprocess
import os
import os.path
from os import listdir
from os.path import isfile, join
from sys import argv

def getFilesInDir(ruta):
    files = [f for f in listdir(ruta) if isfile(join(ruta, f))]
    return files

def getFullPath(dir, file):    
    return dir+"\\"+file

def getFullPaths(dir,datasets):
    files=[]
    for f in datasets:
        files.append(getFullPath(dir,f))
    return files
        

def addDataToLog(logFile,data):
    data=data+"\n"
    f=open(logFile,"a")
    f.write(data)
    f.close()        
    #print data
    
def repString(s,leng):
    return (s*leng)


def readFile(filename):
    # Define a filename. filename = "p13.dat"
    if not isfile(filename): print filename + 'File does not exist.'
    else:
        # Open the file as f.
        f =open(filename,"r")
        content = f.read()
    #    content.splitlines()        
    # Open the file as f. f =open(filename) content = f.read() content.splitlines()
    # Show the file contents line by line.
    # We added the comma to print single newlines and not double newlines.
    # This is because the lines contain the newline character '\n'. for           ntent:
    #for line in content:
    #    print line
    f.close()
    return content  

#Tiempo en micros/occ # dividir 602.89/47340415
#Espacio (bytes: sumar 66991202+599048)

# RLCSA test
# Base name: ./indexes/wiki2gb.txt
# Sample: 1024
# Sequences:       1
# Original size:   1991.05 MB
# Block size:      32 bytes
# Sample rate:     1024
# 
# RLCSA:           41.4879 MB
#   BWT only:      32.741 MB
# SA samples:      15.6272 MB
# Total size:      57.1151 MB
# 
# time/occ:0.0333
# 
# Patterns:    1000 (25.6095 / sec)
# Total size:  8033 bytes (205.721 / sec)
# Matches:     1171124 (29991.9 / sec)
# Time:        39.048 seconds


def parseIndexSize(content):
    idxsize=0
    pos=0
    pos= content.find("Total size:",pos)
    pos = pos + len("Total size:") +1
    pos1= content.find("MB",pos)
    #print content[pos:pos1]
    idxsize = float(content[pos:pos1])
    return long(idxsize*1024*1024)

def parseMatches(content):
    matches=0
    pos=0
    pos= content.find("Matches:",pos)
    pos = pos + len("Matches:") +1
    pos1= content.find("(",pos)
    #print content[pos:pos1]
    matches = long(content[pos:pos1])
    return matches


#################
def parseTimesL(content):
    times=[]
    totaltime=0
    pos=0
    pos= content.find("Time:",pos)
    pos = pos + len("Time:") +1
    pos1= content.find("seconds",pos)    
    
    while (pos !=-1):
        #print "-----------------"+content[pos:pos1]
        time = float(content[pos:pos1])
        times.append(time)
        totaltime = totaltime + time

        pos= content.find("Time:",pos)
        if (pos==-1):
            break;
        pos = pos + len("Time:") +1
        pos1= content.find("seconds",pos)
    #print "--------tt", len(times)
    #print "--------tt",times
    #print "--------tt", totaltime/len(times)
    return totaltime/len(times)


def parseResultsLocate(content):    
    idxsize= parseIndexSize(content)   ##size in bytes
    occs = parseMatches(content)       ##number of occurrences
    timetotal = parseTimesL(content)   ##search time in seconds
    
    return idxsize, timetotal/occs*1000000, timetotal, occs


#################################
# Sequences:       1
# Original size:   1991.05 MB
# Block size:      32 bytes
# Sample rate:     64
# 
# RLCSA:           41.4879 MB
#   BWT only:      32.741 MB
# SA samples:      251.046 MB
# Total size:      292.534 MB
# 
# mchars/s:2.3229
# Total num chars extracted = 8000000
# Extract time = 3.44 secs
# Extract_time/Num_chars_extracted = 0.0004 msecs/chars
# Num_chars_extracted/extract_time = 2322880.3717 chars/sec
# (Load_time+Extract_time)/Num_chars_extracted = 0.0006 msecs/chars
# Num_chars_extracted/(Load_time+Extract_time) = 1784121.3202 chars/sec
# 

#################

def parseChars(content):
    matches=0
    pos=0
    pos= content.find("Total num chars extracted =",pos)
    pos = pos + len("Total num chars extracted =") +1
    pos1= content.find("\n",pos)
    #print content[pos:pos1]
    matches = long(content[pos:pos1])
    return matches


def parseTimesE(content):    
    times=[]
    totaltime=0
    pos=0
    pos= content.find("Extract time =",pos)
    pos = pos + len("Extract time =") +1
    pos1= content.find("secs",pos)    
    
    while (pos !=-1):
        #print "-----------------"+content[pos:pos1]
        time = float(content[pos:pos1])
        times.append(time)
        totaltime = totaltime + time

        pos= content.find("Extract time =",pos)
        if (pos==-1):
            break;
        pos = pos + len("Extract time =") +1
        pos1= content.find("secs",pos) 
    #print "--------tt", len(times)
    #print "--------tt",times
    #print "--------tt", totaltime/len(times)
    return totaltime/len(times)

def parseResultsExtract(content):    
    idxsize= parseIndexSize(content)   ##size in bytes
    occs = parseChars(content)         ##number of chars extracted
    timetotal = parseTimesE(content)   ##search time in seconds
    
    return idxsize, timetotal/occs*1000000, timetotal, occs



##--principal -------------------------------------------------------------------##




if len(argv) !=3:
    print "sintaxe incorrecta: debe invocar script"
    print "asi: %s <src-directory> <prefix>" % argv[0]
    exit(0)

srcdir = argv[1]
basename = argv[2]   ##rlcsa       ##prefix_log_basename_suffix   Wa_log_rlcsa_1024.txt

#test ---------------
#inputfile = srcdir+"/"+"Wa_log_rlcsa_1024.txt"
#content = readFile(inputfile)
#(spaceL,time,timetotal, occs)=parseResultsLocate(content)
#print spaceL,time
#
#inputfile = srcdir + "/" + "e80_log_rlcsa_E1024.txt"
#content = readFile(inputfile)
#(spaceL,time,timetotal, occs)=parseResultsExtract(content)
#print spaceL,time
##test ends ------------


samples = [32,64,128,256,512,1024,2048]
prefixesL = ["Wa", "Wb", "P2", "P5"]
#prefixesL = ["Wa", "Wb"]
#prefixesE = ["e80"]
prefixesE = ["e80", "e13000"]

header = "#"+"totalSize(byte)".rjust(18," ")+"\t"+"usec/occ".rjust(10," ")+"\t"+ "#"+"indexbasename"
size=0
for pref in prefixesL:
    outputFile = basename+"/"+pref+"_"+basename
    if os.path.exists(outputFile):
        os.remove(outputFile)
    addDataToLog(outputFile,header)

    for s in samples:
        inputfile= srcdir + "/" + pref+"_log_"+basename+"_"+str(s)+".txt"
        content = readFile(inputfile)
        (spaceL,exetime,timetotal, occs)=parseResultsLocate(content)
        #print inputfile, spaceL, exetime,timetotal, occs
        size=spaceL
        
        outputline= ""+str(size).rjust(18," ")+"\t" + str(exetime).rjust(10," ")+"\t"+"#" +basename+"_"+str(s)+".txt"
        addDataToLog(outputFile,outputline)
        if s==samples[0]:
            print " "*4,outputFile.ljust(30," "),str(size).rjust(10," ")+" " + str(exetime).rjust(15," ")+ " samplerate="+str(s)
        else:
            print " "*4," ".ljust(30," "),str(size).rjust(10," ")+" " + str(exetime).rjust(15," ")+ " samplerate="+str(s)


#------------------------------------------

size=0
header = "#"+"totalSize(byte)".rjust(18," ")+"\t"+"usec/char".rjust(10," ")+"\t"+ "#"+"indexbasename"
for pref in prefixesE:
    outputFile = basename+"/"+pref+"_"+basename
    if os.path.exists(outputFile):
        os.remove(outputFile)
    addDataToLog(outputFile,header)
    
    for s in samples:
        inputfile= srcdir + "/" + pref+"_log_"+basename+"_E"+str(s)+".txt"
        content = readFile(inputfile)
        (spaceL,exetime,timetotal, occs)=parseResultsExtract(content)
        #print inputfile, spaceL, exetime,timetotal, occs
        size=spaceL
        
        outputline= ""+str(size).rjust(18," ")+"\t" + str(exetime).rjust(10," ")+"\t"+"#" +basename+"_"+str(s)+".txt"
        addDataToLog(outputFile,outputline)
        if s==samples[0]:
            print " "*4,outputFile.ljust(30," "),str(size).rjust(10," ")+" " + str(exetime).rjust(15," ")+ " samplerate="+str(s)
        else:
            print " "*4," ".ljust(30," "),str(size).rjust(10," ")+" " + str(exetime).rjust(15," ")+ " samplerate="+str(s)



#------------------------------------------


