import subprocess
from sys import argv
from mytiming  import *
from mycheckfiles import *


def ejecuta(argslist):
    #lineacomandos = []
    #lineacomandos.append(programa)
    #lineacomandos.append(fparametros)
    #lineacomandos.append(fentrada)
    #lineacomandos.append(fsalida)
    #print lineacomandos
    process = subprocess.Popen(argslist, stdout=subprocess.PIPE, stderr=subprocess.PIPE) #, shell=True)
    #Launch the shell command:
    output = process.communicate()
    return output[0]

def latexscape(str):
    res=""
    for c in str:
        t=c
        if c=="#":
            t=r"\#"
        elif c=="$":
            t=r"\$"
        elif c=="%":
            t=r"\%"
        elif c=="&":
            t=r"\&"
        elif c=="~":
            t=r"\~{}"
        elif c=="_":
            t=r"\_"
        elif c=="^":
            t=r"\^{}"
        elif c=="{":
            t=r"\{$"
        elif c=="}":
            t=r"\}$"
        res = res + t
    return res

#----------------------- main ------------------------------------------


if len(argv) !=2:
    print "Incorrect syntax! You must provide an output .tex filename"
    print "   use: python %s <filename.tex>" % argv[0]
    exit(0)

#salida = ejecuta(exe, rutadatasets[i], rutaparametros[j], outpath)

filename= argv[1]
deleteFileIfExists(filename)

#CPU-MODEL
cpu=read_str_val_for_KEY("/proc/cpuinfo", "model name	:")
cpu=cpu.strip()
texts=cpu.split()
cpu=' '.join(texts)

#NCORES
nsiblings = ejecuta(["nproc"])
nsiblings=nsiblings.strip()
textsnsiblings=nsiblings.split()
nsiblings=' '.join(textsnsiblings)

#INSTALLED MEMORY (APROX)
        
mem=read_str_val_for_KEY("/proc/meminfo", "MemTotal:")
mem=mem.strip()
texts=mem.split()
mem=' '.join(texts)

#TOTAL-SWAP (APROX)
        
swapmem=read_str_val_for_KEY("/proc/meminfo", "SwapTotal:")
swapmem=swapmem.strip()
texts=swapmem.split()
swapmem=' '.join(texts)

#mem= texts[0]

# OS-VERSION

osfull = readFile("/proc/version");
osfull=osfull.strip()
texts=osfull.split()
osfull=' '.join(texts)

osversion = ejecuta(["lsb_release", "-a"])
deleteFileIfExists("temp-lsb-release.tmp")
addDataToLog("temp-lsb-release.tmp",osversion)
os1=read_str_val_for_KEY("temp-lsb-release.tmp", "Description:")
os1=os1.strip()
os2=read_str_val_for_KEY("temp-lsb-release.tmp", "Codename:")
os2=os2.strip()
osversion = os1+ " ("+os2+")"

deleteFileIfExists("temp-lsb-release.tmp")


#EXPERIMENTS-START-DATE, EXPERIMENTS-END-DATE, EXPERIMENTS-ELAPSED-TIME

elapsed=getElapsedTime("../../../ALL-LOG.dat")
elapsed=elapsed.strip()

inidate=getStartDate("../../../ALL-LOG.dat")
inidate=inidate.strip()

enddate=getEndDate("../../../ALL-LOG.dat")
enddate=enddate.strip()

print osfull


cpu       = latexscape(cpu      )
nsiblings = latexscape(nsiblings)
mem       = latexscape(mem      )
swapmem   = latexscape(swapmem  )
elapsed   = latexscape(elapsed  )
inidate   = latexscape(inidate  )
enddate   = latexscape(enddate  )
osversion = latexscape(osversion)
osfull    = latexscape(osfull   )

print cpu
print nsiblings
print mem
print swapmem
print elapsed
print inidate
print enddate
print osversion
print osfull

addDataToLog(filename,r"\newcommand{\cpu}{"+cpu+r"}")
addDataToLog(filename,r"\newcommand{\nsiblings}{"+nsiblings+r"}")
addDataToLog(filename,r"\newcommand{\mem}{"+mem+r"}")  #in kb
addDataToLog(filename,r"\newcommand{\swapmem}{"+swapmem+r"}")  #in kb
addDataToLog(filename,r"\newcommand{\elapsed}{"+elapsed+r"}")
addDataToLog(filename,r"\newcommand{\runinidate}{"+inidate+r"}")
addDataToLog(filename,r"\newcommand{\runenddate}{"+enddate+r"}")
addDataToLog(filename,r"\newcommand{\osversion}{"+osversion+r"}")
addDataToLog(filename,r"\newcommand{\osfull}{"+osfull+r"}")




#print strresult
#addDataToLog(filename, strresult)

str=getElapsedTime("../../../indexes/POS/POS-LOG.dat")
str=str.rjust(13)+" "














 
 
 
