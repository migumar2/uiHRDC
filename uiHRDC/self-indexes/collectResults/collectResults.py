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
#Index size;Doc index size;Occs;#1;#2;#3;#4;#5;#6;#7;#8;#9;#10;;Avg.Time
#66991202;599048;;47340415;;587.34;588.37;589.46;590.32;588.01;571.16;563.58;572.80;733.82;644.04;;602.89 

#Index size;Doc index size;Occs;#1;#2;#3;#4;#5;#6;#7;#8;#9;#10;;Avg.Time
#66991202;599048;;1171109;;16.30;16.26;16.30;16.27;16.38;18.36;16.38;16.33;16.29;16.29;;16.51 


def parseResultsLocate(content):    
    idxsize=0
    docsize=0
    pos=0
    pos= content.find("Avg.Time",pos)
    pos = pos + len("Avg.Time") +1
    pos1= content.find(";",pos)
    #print content[pos:pos1]
    idxsize = long(content[pos:pos1])
    #print idxsize
    
    pos=pos1+1
    pos1= content.find(";",pos)
    #print content[pos:pos1]
    docidxsize = long(content[pos:pos1])
    #print docidxsize
    
    totalsize=idxsize + docidxsize
    
    pos=pos1+2
    pos1= content.find(";;",pos)
    #print content[pos:pos1]
    occs = long(content[pos:pos1])
    #print occs    
   
    pos = pos1+2
    pos= content.find(";;",pos)
    pos = pos+2 
    pos1= content.find("\n",pos)
    #print content[pos:pos1]
    timetotal= float(content[pos:pos1])
    
    return totalsize, timetotal/occs*1000000

def parseResultsExtract(content):    
    idxsize=0
    docsize=0
    pos=0
    pos= content.find("Avg.Time",pos)
    pos = pos + len("Avg.Time") +1
    pos1= content.find(";",pos)
    #print content[pos:pos1]
    idxsize = long(content[pos:pos1])
    #print idxsize
    
    
    totalsize=idxsize 
    
    pos=pos1+2   #skips ;;
    pos1= content.find(";;",pos)
    #print content[pos:pos1]
    occs = long(content[pos:pos1])
    #print occs    
   
    pos = pos1+2
    pos= content.find(";;",pos)
    pos = pos+2 
    pos1= content.find("\n",pos)
    #print content[pos:pos1]
    timetotal= float(content[pos:pos1])
    
    return totalsize, timetotal/occs*1000000


##--principal -------------------------------------------------------------------##




if len(argv) !=3:
    print "sintaxe incorrecta: debe invocar script"
    print "asi: %s <src-directory> <prefix>" % argv[0]
    exit(0)

srcdir = argv[1]
prefix = argv[2]

##test ---------------
#inputfile = srcdir + "/" + prefix + "." + "f1_1000"
#content = readFile(inputfile)
#(spaceL,time)=parseResultsLocate(content)
#print spaceL,time
#
#inputfile = srcdir + "/" + prefix + "." + "snippets80"
#content = readFile(inputfile)
#(space,time)=parseResultsExtract(content)
#print spaceL,time
##test ends ---------------

suffixesL = ["f1_1000", "f1001_100k", "2_2", "5_5"]
suffixesE= ["snippets13000", "snippets80"]


header = "#"+"totalSize(byte)".rjust(18," ")+"\t"+"usec/occ".rjust(10," ")+"\t"+ "#"+"indexbasename"
size=0
for f in suffixesL:

    inputfile = srcdir + "/" + prefix + "." + f
    content = readFile(inputfile)
    (spaceL,exetime)=parseResultsLocate(content)
    #print spaceL,exetime
    size=spaceL
	
    outputFile = prefix+"/"+prefix+"."+f
    #print " "*4, outputFile.ljust(32," "),spaceL,exetime
    print " "*4, outputFile.ljust(30," "),str(spaceL).rjust(10," ")+" " + str(exetime).rjust(15," ")


    if os.path.exists(outputFile):
        os.remove(outputFile)
    addDataToLog(outputFile,header)
    outputline= ""+str(size).rjust(18," ")+"\t" + str(exetime).rjust(10," ")+"\t"+"#"+ f
    addDataToLog(outputFile,outputline)

for f in suffixesE:

    inputfile = srcdir + "/" + prefix + "." + f
    content = readFile(inputfile)
    (spaceL,exetime)=parseResultsExtract(content)
    #print spaceL,exetime
    #size=spaceL   #reuses the size computed from Locate-files (it is complete!)
	
    outputFile = prefix+"/"+prefix+"."+f
    #print " "*4, outputFile.ljust(32," "),spaceL,exetime
    print " "*4, outputFile.ljust(30," "),str(spaceL).rjust(10," ")+" " + str(exetime).rjust(15," ")
    
    if os.path.exists(outputFile):
        os.remove(outputFile)
    addDataToLog(outputFile,header)
    outputline= ""+str(size).rjust(18," ")+"\t" + str(exetime).rjust(10," ")+"\t"+"#"+ f
    addDataToLog(outputFile,outputline)
    
#------------------------------------------


