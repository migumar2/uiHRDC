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

#Tiempo en micros/occ # dividir 0.78*1000000/99228
#Espacio (bytes:  60465820)

#Index size;Occs;#1;#2;#3;#4;#5;#6;#7;#8;#9;#10;;Avg.Time
#60465820;;99228;;0.81;0.77;0.77;0.78;0.77;0.80;0.77;0.77;0.77;0.76;;0.78

def parseResultsLocate(content):    
    idxsize=0
    docsize=0
    pos=0
    pos= content.find("Avg.Time",pos)
    pos = pos + len("Avg.Time") +1
    pos1= content.find(";;",pos)
    #print content[pos:pos1]
    idxsize = long(content[pos:pos1])
    totalsize=idxsize 
    #print idxsize
    
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
##print spaceL,time
#
#inputfile = srcdir + "/" + prefix + "." + "snippets80"
#content = readFile(inputfile)
#(space,time)=parseResultsExtract(content)
##print spaceL,time
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


