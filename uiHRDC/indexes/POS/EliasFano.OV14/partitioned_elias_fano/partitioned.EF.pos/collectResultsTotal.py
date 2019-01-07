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
    if not isfile(filename): print 'File does not exist.'
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


def parseSizes(content):
    lista=[]
    pos =0
    while pos!=-1:
        pos= content.find("Documents:",pos)
        #print pos
        if pos==-1: break
        pos= pos+ len("Documents:")
        pos2=content.find("bytes",pos)
        sizeidx=content[pos:pos2]
        lista.append(long(sizeidx))
        pos=pos2
    return lista

def parseTimes(content):
    listaT=[]
    listaOccs=[]
    pos =0
    while pos!=-1:
        pos= content.find("Time per occ (micros):",pos)
        #print pos
        if pos==-1: break
        pos= pos+ len("Time per occ (micros):")
        pos2=content.find(" occs= ",pos)
        exetime=content[pos:pos2]
               
        pos2= pos2+ len(" occs= ")
        pos3=content.find("\n",pos2)
        occs=content[pos2:pos3]
        
        pos=pos2
        listaT.append(float(exetime))
        listaOccs.append (long(occs))

    return listaT, listaOccs

def existsNumberInLineBeginning(str):
    try:
        str=str.strip()
        if len(str)==0: 
            return KO
               
        values = str.split()         #tries to convert the first string in the line to float
        number0 = float(values[0])    # raises exception if a float cannot be converted. Succeeds otherwise 
        number1 = float(values[1])    # raises exception if a float cannot be converted. Succeeds otherwise 
        number2 = float(values[2])    # raises exception if a float cannot be converted. Succeeds otherwise 
        return number2
        
        #print "VALUE-----------"
        #print float(values[0])       
        #print "VALUE-----------"
                
        #newstr = ''.join((ch if ch in '0123456789.-' else ' ') for ch in str)
        #print "newstr", newstr
        #listOfNumbers = [float(i) for i in newstr.split()]
        #if len(listOfNumbers)==0: 
        #    #print "OKKKKKKKKKKKKKKK"
        #    return KO
        #else: 
		#    return OK
        
    except:
        #print "EXCEPTION RAISED"   # a float was not parsed at the beginning of the line
        return 0




def parsePartialTimePerOccs(content):
    lines = content.splitlines(True)
      
    if len(lines)<=1: 
        return 0
    else:
        #for line in lines[1:len(lines)]:
        #    if (existsNumberInLineBeginning(line) == OK):
        #        return OK
        #return KO
        return existsNumberInLineBeginning(lines[1])
##--principal -------------------------------------------------------------------##




if len(argv) !=7:
    print "sintaxe incorrecta: debe invocar script"
    print "asi: %s <sizes.txt> <times.txt> <partial-times-Dir>" % argv[0]
    exit(0)

sizesFile = argv[1]
timesFile = argv[2]
partialWa = argv[3]
partialWb = argv[4]
partialP2 = argv[5]
partialP5 = argv[6]

contentS = readFile(sizesFile)
contentT = readFile(timesFile)

contentWa = readFile(partialWa)
contentWb = readFile(partialWb)
contentP2 = readFile(partialP2)
contentP5 = readFile(partialP5)


sizes = parseSizes(contentS)
(times,occs) = parseTimes(contentT)

partialWa = parsePartialTimePerOccs(contentWa)
partialWb = parsePartialTimePerOccs(contentWb)
partialP2 = parsePartialTimePerOccs(contentP2)
partialP5 = parsePartialTimePerOccs(contentP5)

print sizes
print times
print occs
print partialWa, partialWb, partialP2, partialP5


techniques = ["10.eliasfanopart.dat", "10.eliasfanoUNI.dat","11.optpfd.dat","13.varint.dat","12.interpolative.dat"]
prefixes = ["E_Wa", "E_Wb", "N_p2", "N_p5"]
partialtimes = [partialWa, partialWb,partialP2, partialP5]  ##times (a) and (c) that must be addead to the intersection times measured by EF-varints.

# totaSize(byte)	       msec/pat	       usec/occ	           occs	        numpats	#indexBaseName
header = "#"+"totalSize(byte)".rjust(18," ")+"\t"+"msec/pat".rjust(10," ")+"\t"+"usec/occ".rjust(10," ")+"\t"+"occs".rjust(9," ")+"\t" +"numpats".rjust(7," ")+"\t"+ "#"+"indexbasename"

tech=0
run=0
for f in techniques:
    size= sizes[tech]
    pref=0
    for p in prefixes:  ##the four times measures for each technique: i.e.: Wa, Wb, P2, P5
        exetime=times[run] + partialtimes[pref]
        print "partial (",partialtimes[pref],"), intersection (",times[run],") --> sum = ",exetime
        noccs  =occs[run]
        npatts = 1000
        outputFile = p+"."+f
        if os.path.exists(outputFile):
            os.remove(outputFile)
        addDataToLog(outputFile,header)
        outputline= ""+str(size).rjust(18," ")+"\t"+"????????".rjust(10," ")+"\t" + str(exetime).rjust(10," ")+"\t"+ str(noccs).rjust(9," ")+"\t" +str(npatts).rjust(7," ")+"\t"+ "#"+ f
        addDataToLog(outputFile,outputline)
        pref=pref+1
        run =run +1
    tech=tech+1

print "\nThe following files have been created!\n"
for f in techniques:
    for p in prefixes:
        outputFile = p+"."+f
        print "\t"+outputFile



#
#
#
#datasets= getFilesInDir(dataDir)                    #filenames   f1, f2, ...
#rutadatasets = getFullPaths (dataDir,datasets)      #paths     datadir\f1, datadir\f2,...
#print rutadatasets
#
#fparametros = getFilesInDir(paramsDir)
#rutaparametros = getFullPaths(paramsDir, fparametros)
#
#i=0
#for dataset in datasets:   #non ruta completa, so nome ficheiro    
#    j=0
#
#    outgnupath = getFullPath (outgnudir, dataset)
#    addDataToLog(outgnupath,"#"+"iters".rjust(8," ")+"\t"+ "exetime")
#
#    for paramfile in fparametros:
#        if j>=7:
#            break
#        outpath = getFullPath (outDir, dataset + "-" + paramfile)
#        content = readFile(outpath)
#        pos= content.find("EXECUTION TIME:",0)
#        pos= pos+ len("EXECUTION TIME:")
#        pos2=content.find("ms",pos)
#        exetime=content[pos:pos2]
#
#        pos=content.find("Num iter.:",0)
#        pos=pos+len("Num iter.:")+1
#        pos2=pos
#        while content[pos2] in "0123456789":
#            pos2=pos2+1
#        iters = content[pos:pos2]
#
#        ### writes out iters and exetime to outgnupath file.
#        outline = iters.rjust(5," ") + "\t" + exetime.rjust(9," ")+ "\t" + outpath
#        addDataToLog(outgnupath, outline)
#
#        j=j+1
#
#    #next dataset
#    i=i+1

#------------------------------------------


