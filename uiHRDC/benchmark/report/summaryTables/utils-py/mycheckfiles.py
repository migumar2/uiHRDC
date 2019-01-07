
from sys import argv
from mytiming import *

KO='\ko'
OK='\ok'
ENDROW = '\\\\ \hline'

########################################################################
########################################################################
## gets the elapsed-time from a filename
def getElapsedTime(filename):
    val=read_str_val_for_KEY(filename)
    if val=="":
        return KO
    return val

## gets the starting-date from a filename
def getStartDate(filename):
    val=read_str_val_for_KEY(filename,"start-time  : ")
    if val=="":
        return KO
    return val

## gets the ending-date from a filename
def getEndDate(filename):
    val=read_str_val_for_KEY(filename,"end-time    :")
    if val=="":
        return KO
    return val

########################################################################
########################################################################

# Returs OK|KO depending on that the second line of the file contains a number.
def existsNumberInLineBeginning(str):
    try:
        str=str.strip()
        if len(str)==0: 
            return KO
               
        values = str.split()         #tries to convert the first string in the line to float
        number = float(values[0])    # raises exception if a float cannot be converted. Succeeds otherwise 
        return OK
        
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
        return KO


# Checks if a gnuplot-data file contains at least two lines, and there is a line starting with a number.
#    We cover the case in which the data-file has only the header "#....."
def checkDataFileOk(filename):
    if existsFile(filename):
        content = readFile(filename)
        lines = content.splitlines(True)
        
        if len(lines)<=1: 
            return KO
        else:
            for line in lines[1:len(lines)]:
                if (existsNumberInLineBeginning(line) == OK):
                    return OK
            return KO
    else: return KO


def checkIIResultIdx(nameidx, basedir,blog,slog,wa,wb,p2,p5, e80="none", e13000="none"):
    res=[]
    
    str=nameidx.ljust(20)    
    res.append(str)
    
    str=getElapsedTime(basedir+"/"+blog)
    str=str.rjust(13)+" "
    res.append(str)

    str=getElapsedTime(basedir+"/"+slog)
    str=str.rjust(13)+" "
    res.append(str)    

    str=checkDataFileOk(basedir+"/"+wa)
    str=str.rjust(5)+" "
    res.append(str)    

    str=checkDataFileOk(basedir+"/"+wb)
    str=str.rjust(5)+" "
    res.append(str)    

    str=checkDataFileOk(basedir+"/"+p2)
    str=str.rjust(5)+" "
    res.append(str)    

    str=checkDataFileOk(basedir+"/"+p5)
    str=str.rjust(5)+" "
    res.append(str)    

    if e80!="none":                  ## "none" to skip generating this column
        if e80!="":                  ## "" (blank) to generate an empty column, "filename" to checkDatafileOK(filename)
            str=checkDataFileOk(basedir+"/"+e80)
        else: 
            str=" ~ "
        str=str.rjust(5)+" "
        res.append(str)    

    if e13000!="none":               ## "none" to skip generating this column
        if e13000!="":               ## "" (blank) to generate an empty column, "filename" to checkDatafileOK(filename)
            str=checkDataFileOk(basedir+"/"+e13000)
        else:
            str=" ~ "
        str=str.rjust(5)+" "
        res.append(str)      
    return res


###################################################################################################
###################################################################################################
# Returns a latex-table-shaped row, with date corresponding to the results of a given technique:
# The parameter has the shape:
# rice= ["RICE", "../../../indexes/NOPOS/EXPERIMENTS/experiments.20g/1.4.scripts.rice.bitmap", "B-LOG.dat","S-LOG.dat", "E_Wa.1.4.rice.dat", "E_Wb.1.4.rice.dat", "N_p2.1.4.rice.dat", "N_p5.1.4.rice.dat"]

def getStrIIResultIdx(src): 
    result = checkIIResultIdx(src[0],src[1],src[2],src[3],src[4],src[5],src[6],src[7],src[8],src[9])
    #print result
    #for str in result:
    #    print "   --"+str+"--"

    strresult = '     '+"&".join(result)+ ENDROW
    return strresult
###################################################################################################
###################################################################################################

 
##--main --------------------------------------------------------------##
#
#rice= ["RICE", "../../../indexes/NOPOS/EXPERIMENTS/experiments.20g/1.4.scripts.rice.bitmap", "B-LOG.dat","S-LOG.dat", "E_Wa.1.4.rice.dat", "E_Wb.1.4.rice.dat", "N_p2.1.4.rice.dat", "N_p5.1.4.rice.dat"]
#
#vbytePos= [ "VBYTE", "../../../indexes/POS/EXPERIMENTS/experiments.is/2.1.scripts.vbyte.TestExtract","B-LOG.dat","S-LOG.dat", "E_Wa.2.1.vbyte.dat", "E_Wb.2.1.vbyte.dat","N_p2.2.1.vbyte.dat","N_p5.2.1.vbyte.dat", "m.80chars_ii_repairT.dat", "m.13000chars_ii_repairT.dat" ]
#
##srcX=vbytePos
#srcX=rice
#
#
##src=rice[0:8]
#
#src=vbytePos[0:10] #src is a COPY of the list (which remains un-modified)
#src=rice[0:8]      #src is a COPY of the list (which remains un-modified)
#
#if len(src)==8:
#    src.append("none")  #no extract80-file
#    src.append("none")  #no extract13000-file
#
#strresult= getStrIIResultIdx(src)
#
#print strresult
