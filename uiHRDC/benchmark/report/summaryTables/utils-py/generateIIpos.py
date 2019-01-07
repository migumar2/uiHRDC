
from sys import argv
from mytiming  import *
from mycheckfiles import *




##--main --------------------------------------------------------------##


variants=[]
variants.append( ["RICE"              ,"../../../indexes/POS/EXPERIMENTS/experiments.is/1.4.scripts.rice.bitmap"                ,"B-LOG.dat"               , "S-LOG.dat"               , "E_Wa.1.4.rice.dat"                     ,"E_Wb.1.4.rice.dat"                     ,"N_p2.1.4.rice.dat"                     ,"N_p5.1.4.rice.dat"                     , "",""] )
variants.append( ["VBYTE"             ,"../../../indexes/POS/EXPERIMENTS/experiments.is/2.1.scripts.vbyte.TestExtract"          ,"B-LOG.dat"               , "S-E-LOG.dat"             , "E_Wa.2.1.vbyte.dat"                    ,"E_Wb.2.1.vbyte.dat"                    ,"N_p2.2.1.vbyte.dat"                    ,"N_p5.2.1.vbyte.dat"                    , "m.80chars_ii_repairT.dat","m.13000chars_ii_repairT.dat"] )
variants.append( ["VBYTE-CM"          ,"../../../indexes/POS/EXPERIMENTS/experiments.is/2.scripts.vbytebitmapmoffat"            ,"B-LOG.dat"               , "S-LOG.dat"               , "E_Wa.2.vbyte.moffat.dat"               ,"E_Wb.2.vbyte.moffat.dat"               ,"N_p2.2.vbyte.moffat.dat"               ,"N_p5.2.vbyte.moffat.dat"               , "",""] )
variants.append( ["VBYTE-ST"          ,"../../../indexes/POS/EXPERIMENTS/experiments.is/3.scripts.vbytebitmapSanders"           ,"B-LOG.dat"               , "S-LOG.dat"               , "E_Wa.vbyte.sanders.dat"                ,"E_Wb.vbyte.sanders.dat"                ,"N_p2.vbyte.sanders.dat"                ,"N_p5.vbyte.sanders.dat"                , "",""] )
variants.append( ["SIMPLE-9e"          ,"../../../indexes/POS/EXPERIMENTS/experiments.is/7.1.scripts.simple9e"                  ,"B-LOG.dat"               , "S-LOG.dat"               , "E_Wa.7.1.simple9e.dat"                 ,"E_Wb.7.1.simple9e.dat"                 ,"N_p2.7.1.simple9e.dat"                 ,"N_p5.7.1.simple9e.dat"                 , "",""] )
variants.append( ["QMX-SIMD"          ,"../../../indexes/POS/EXPERIMENTS/experiments.is/9.scripts.qmx.bis"                      ,"B-LOG.dat"               , "S-LOG.dat"               , "E_Wa.9.qmx.dat"                        ,"E_Wb.9.qmx.dat"                        ,"N_p2.9.qmx.dat"                        ,"N_p5.9.qmx.dat"                        , "",""] )
variants.append( ["*ELIAS-FANO-OPT"   ,"../../../indexes/POS/EliasFano.OV14/partitioned_elias_fano/partitioned.EF.pos"          ,"B-OPT-LOG.dat"           , "S-OPT-LOG.dat"           , "E_Wa.10.eliasfanopart.dat"             ,"E_Wb.10.eliasfanopart.dat"             ,"N_p2.10.eliasfanopart.dat"             ,"N_p5.10.eliasfanopart.dat"             , "",""] )
variants.append( ["*OPT-PFD"          ,"../../../indexes/POS/EliasFano.OV14/partitioned_elias_fano/partitioned.EF.pos"          ,"B-OPTPFOR-LOG.dat"       , "S-OPTPFOR-LOG.dat"       , "E_Wa.11.optpfd.dat"                    ,"E_Wb.11.optpfd.dat"                    ,"N_p2.11.optpfd.dat"                    ,"N_p5.11.optpfd.dat"                    , "",""] )
variants.append( ["*INTERPOLATIVE"    ,"../../../indexes/POS/EliasFano.OV14/partitioned_elias_fano/partitioned.EF.pos"          ,"B-INTERPOLATIVE-LOG.dat" , "S-INTERPOLATIVE-LOG.dat" , "E_Wa.12.interpolative.dat"             ,"E_Wb.12.interpolative.dat"             ,"N_p2.12.interpolative.dat"             ,"N_p5.12.interpolative.dat"             , "",""] )
variants.append( ["*VARINT-G8IU"      ,"../../../indexes/POS/EliasFano.OV14/partitioned_elias_fano/partitioned.EF.pos"          ,"B-VARINT-LOG.dat"        , "S-VARINT-LOG.dat"        , "E_Wa.13.varint.dat"                    ,"E_Wb.13.varint.dat"                    ,"N_p2.13.varint.dat"                    ,"N_p5.13.varint.dat"                    , "",""] )
variants.append( ["VBYTE-LZMA"        , "../../../indexes/POS/EXPERIMENTS/experiments.is/6.scripts.vbytelzma"                   ,"B-LOG.dat"               , "S-LOG.dat"               , "E_Wa.6.vbyte.lzma.dat"                 ,"E_Wb.6.vbyte.lzma.dat"                 ,"N_p2.6.vbyte.lzma.dat"                 ,"N_p5.6.vbyte.lzma.dat"                 , "",""] )
variants.append( ["REPAIR"            , "../../../indexes/POS/EXPERIMENTS/experiments.is/4.22.scripts.RepairGonzalo"            ,"B-LOG.dat"               , "S-LOG.dat"               , "E_Wa.4.2.repairG.dat"                  ,"E_Wb.4.2.repairG.dat"                  ,"N_p2.4.2.repairG.dat"                  ,"N_p5.4.2.repairG.dat"                  , "",""] )
variants.append( ["REPAIR-SKIPPING"   , "../../../indexes/POS/EXPERIMENTS/experiments.is/4.33.scripts.Repair.skipping.Gonzalo"  ,"B-LOG.dat"               , "S-LOG.dat"               , "E_Wa.4.3.repairG.skipping.dat"         ,"E_Wb.4.3.repairG.skipping.dat"         ,"N_p2.4.3.repairG.skipping.dat"         ,"N_p5.4.3.repairG.skipping.dat"         , "",""] )  
variants.append( ["REPAIR-SKIPPING-CM", "../../../indexes/POS/EXPERIMENTS/experiments.is/4.33.1.scripts.RepairSkipping.Moffat"  ,"B-LOG.dat"               , "S-LOG.dat"               , "E_Wa.4.3.1repairG.skipping.moffat.dat" ,"E_Wb.4.3.1repairG.skipping.moffat.dat" ,"N_p2.4.3.1repairG.skipping.moffat.dat" ,"N_p5.4.3.1repairG.skipping.moffat.dat" , "",""] )  
variants.append( ["REPAIR-SKIPPING-ST", "../../../indexes/POS/EXPERIMENTS/experiments.is/4.33.2.scripts.RepairSkipping.Sanders" ,"B-LOG.dat"               , "S-LOG.dat"               , "E_Wa.4.3.2repairG.skipping.lookup.dat" ,"E_Wb.4.3.2repairG.skipping.lookup.dat" ,"N_p2.4.3.2repairG.skipping.lookup.dat" ,"N_p5.4.3.2repairG.skipping.lookup.dat" , "",""] )
  
#src=vbytePos[0:10] #src is a COPY of the list (which remains un-modified)
#src=rice[0:8]       #src is a COPY of the list (which remains un-modified)
#src=riceB[0:8]      #src is a COPY of the list (which remains un-modified)



header= r"""
%%%%   STATIC HEADER %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
\begin{table}[htbp]
  \scriptsize
  \centering
    \begin{tabular}{|l|r|r|c|c|c|c|c|c|}
    \cline{2-9}    \multicolumn{1}{r|}{} & \multicolumn{2}{c|}{Overall Time} &                 \multicolumn{4}{c|}{Locate}                &\multicolumn{2}{c|}{Extract} \\
    \cline{2-9}    \multicolumn{1}{r|}{} & \multicolumn{2}{c|}{            } & \multicolumn{2}{c|}{Words}  & \multicolumn{2}{c|}{Phrases} &     80   & 13,000 \\   
    \cline{2-9}    \multicolumn{1}{r|}{} &       Building & Querying         &    {Low freq} & {High freq} &    {2-words} & {5-words}     &  chars   & chars \\   
    \hline
    \hline
    %%%% CONTENTS GENERATED BY SCRIPT                            %%%%%%%%%%%%%%%%%%%%%%%%%%"""


footer= r"""%%%%   STATIC FOOTER %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%    
   \cline{1-3}    
   \end{tabular}%
  \caption{Summary and state of the experiments run on the test machine: positional inverted indexes. Note that Repair extraction times are identical for all the indexes (actually they do not depend on the type of index). Therefore, we only measured them once for \texttt{VBYTE} index.}
  \label{ap1:pos}%
\end{table}%

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
"""


if len(argv) !=2:
    print "Incorrect syntax! You must provide an output .tex filename"
    print "   use: python %s <filename.tex>" % argv[0]
    exit(0)

filename= argv[1]
deleteFileIfExists(filename)


#print header
addDataToLog(filename, header)

#processes all the techniques in "variants"
for t in variants:
    src=t[0:len(t)]
    
    if len(src)==8:
        src.append("none")  #no extract80-file
        src.append("none")  #no extract13000-file

    strresult= getStrIIResultIdx(src)
    
    #print strresult
    addDataToLog(filename, strresult)

    str=getElapsedTime("../../../indexes/POS/POS-LOG.dat")
    str=str.rjust(13)+" "


overall=r"""   \hline
   \textbf{OVERALL TIME }    &     \multicolumn{2}{|c|}{""" +   str + r""" }   &\multicolumn{4}{|r}{}  \\"""

#print overall
addDataToLog(filename, overall)


#print footer
addDataToLog(filename, footer)

















 
 
 
