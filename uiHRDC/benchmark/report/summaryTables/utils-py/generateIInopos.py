
from sys import argv
from mytiming  import *
from mycheckfiles import *




##--main --------------------------------------------------------------##


variants=[]
variants.append( ["RICE"              , "../../../indexes/NOPOS/EXPERIMENTS/experiments.20g/1.4.scripts.rice.bitmap"               ,"B-LOG.dat"               , "S-LOG.dat"               , "E_Wa.1.4.rice.dat"                     ,"E_Wb.1.4.rice.dat"                     ,"N_p2.1.4.rice.dat"                     ,"N_p5.1.4.rice.dat"                     ] )
variants.append( ["RICE-B"            , "../../../indexes/NOPOS/EXPERIMENTS/experiments.20g/1.4.scripts.rice.bitmap"               ,"BB-LOG.dat"              , "SB-LOG.dat"              , "E_Wa.1.4.rice.bitmap8.dat"             ,"E_Wb.1.4.rice.bitmap8.dat"             ,"N_p2.1.4.rice.bitmap8.dat"             ,"N_p5.1.4.rice.bitmap8.dat"             ] )
variants.append( ["VBYTE"             ,"../../../indexes/NOPOS/EXPERIMENTS/experiments.20g/2.1.scripts.vbytebitmap"                ,"B-LOG.dat"               , "S-LOG.dat"               , "E_Wa.2.1.vbyte.dat"                    ,"E_Wb.2.1.vbyte.dat"                    ,"N_p2.2.1.vbyte.dat"                    ,"N_p5.2.1.vbyte.dat"                    ] )
variants.append( ["VBYTE-CM"          ,"../../../indexes/NOPOS/EXPERIMENTS/experiments.20g/2.scripts.vbytebitmapmoffat"            ,"B-LOG.dat"               , "S-LOG.dat"               , "E_Wa.2.vbyte.moffat.dat"               ,"E_Wb.2.vbyte.moffat.dat"               ,"N_p2.2.vbyte.moffat.dat"               ,"N_p5.2.vbyte.moffat.dat"               ] )
variants.append( ["VBYTE-ST"          ,"../../../indexes/NOPOS/EXPERIMENTS/experiments.20g/3.scripts.vbytebitmapSanders"           ,"B-LOG.dat"               , "S-LOG.dat"               , "E_Wa.3.vbyte.sanders.dat"              ,"E_Wb.3.vbyte.sanders.dat"              ,"N_p2.3.vbyte.sanders.dat"              ,"N_p5.3.vbyte.sanders.dat"              ] )
variants.append( ["VBYTE-B"           ,"../../../indexes/NOPOS/EXPERIMENTS/experiments.20g/2.1.scripts.vbytebitmap"                ,"BB-LOG.dat"              , "SB-LOG.dat"              , "E_Wa.2.1.vbyte.bitmap8.dat"            ,"E_Wb.2.1.vbyte.bitmap8.dat"            ,"N_p2.2.1.vbyte.bitmap8.dat"            ,"N_p5.2.1.vbyte.bitmap8.dat"            ] )
variants.append( ["VBYTE-CM-B"        ,"../../../indexes/NOPOS/EXPERIMENTS/experiments.20g/2.scripts.vbytebitmapmoffat"            ,"BB-LOG.dat"              , "SB-LOG.dat"              , "E_Wa.2.vbyte.moffat.bitmap8.dat"       ,"E_Wb.2.vbyte.moffat.bitmap8.dat"       ,"N_p2.2.vbyte.moffat.bitmap8.dat"       ,"N_p5.2.vbyte.moffat.bitmap8.dat"       ] )
variants.append( ["VBYTE-ST-B"        ,"../../../indexes/NOPOS/EXPERIMENTS/experiments.20g/3.scripts.vbytebitmapSanders"           ,"BB-LOG.dat"              , "SB-LOG.dat"              , "E_Wa.3.vbyte.sanders.bitmap8.dat"      ,"E_Wb.3.vbyte.sanders.bitmap8.dat"      ,"N_p2.3.vbyte.sanders.bitmap8.dat"      ,"N_p5.3.vbyte.sanders.bitmap8.dat"      ] )
variants.append( ["SIMPLE-9"          ,"../../../indexes/NOPOS/EXPERIMENTS/experiments.20g/7.scripts.simple9"                      ,"B-LOG.dat"               , "S-LOG.dat"               , "E_Wa.7.simple9.dat"                    ,"E_Wb.7.simple9.dat"                    ,"N_p2.7.simple9.dat"                    ,"N_p5.7.simple9.dat"                    ] )
variants.append( ["PFORDELTA"         ,"../../../indexes/NOPOS/EXPERIMENTS/experiments.20g/8.scripts.pfordelta"                    ,"B-LOG.dat"               , "S-LOG.dat"               , "E_Wa.8.pfordelta.dat"                  ,"E_Wb.8.pfordelta.dat"                  ,"N_p2.8.pfordelta.dat"                  ,"N_p5.8.pfordelta.dat"                  ] )
variants.append( ["QMX-SIMD"          ,"../../../indexes/NOPOS/EXPERIMENTS/experiments.20g/9.scripts.qmx.bis"                      ,"B-LOG.dat"               , "S-LOG.dat"               , "E_Wa.9.qmx.dat"                        ,"E_Wb.9.qmx.dat"                        ,"N_p2.9.qmx.dat"                        ,"N_p5.9.qmx.dat"                        ] )
variants.append( ["*ELIAS-FANO-OPT"   ,"../../../indexes/NOPOS/EliasFano.OV14/partitioned_elias_fano/partitioned_elias_fano.nopos" ,"B-OPT-LOG.dat"           , "S-OPT-LOG.dat"           , "E_Wa.10.eliasfanopart.dat"             ,"E_Wb.10.eliasfanopart.dat"             ,"N_p2.10.eliasfanopart.dat"             ,"N_p5.10.eliasfanopart.dat"             ] )
variants.append( ["*OPT-PFD"          ,"../../../indexes/NOPOS/EliasFano.OV14/partitioned_elias_fano/partitioned_elias_fano.nopos" ,"B-OPTPFOR-LOG.dat"       , "S-OPTPFOR-LOG.dat"       , "E_Wa.11.optpfd.dat"                    ,"E_Wb.11.optpfd.dat"                    ,"N_p2.11.optpfd.dat"                    ,"N_p5.11.optpfd.dat"                    ] )
variants.append( ["*INTERPOLATIVE"    ,"../../../indexes/NOPOS/EliasFano.OV14/partitioned_elias_fano/partitioned_elias_fano.nopos" ,"B-INTERPOLATIVE-LOG.dat" , "S-INTERPOLATIVE-LOG.dat" , "E_Wa.12.interpolative.dat"             ,"E_Wb.12.interpolative.dat"             ,"N_p2.12.interpolative.dat"             ,"N_p5.12.interpolative.dat"             ] )
variants.append( ["*VARINT-G8IU"      ,"../../../indexes/NOPOS/EliasFano.OV14/partitioned_elias_fano/partitioned_elias_fano.nopos" ,"B-VARINT-LOG.dat"        , "S-VARINT-LOG.dat"        , "E_Wa.13.varint.dat"                    ,"E_Wb.13.varint.dat"                    ,"N_p2.13.varint.dat"                    ,"N_p5.13.varint.dat"                    ] )
variants.append( ["RICE-RLE"          , "../../../indexes/NOPOS/EXPERIMENTS/experiments.20g/1.5.scritps.riceRLE"                   ,"B-LOG.dat"               , "S-LOG.dat"               , "E_Wa.1.5.riceRLE.dat"                  ,"E_Wb.1.5.riceRLE.dat"                  ,"N_p2.1.5.riceRLE.dat"                  ,"N_p5.1.5.riceRLE.dat"                  ] )
variants.append( ["VBYTE-LZMA"        , "../../../indexes/NOPOS/EXPERIMENTS/experiments.20g/6.scripts.vbytelzma"                   ,"B-LOG.dat"               , "S-LOG.dat"               , "E_Wa.6.vbyte.lzma.dat"                 ,"E_Wb.6.vbyte.lzma.dat"                 ,"N_p2.6.vbyte.lzma.dat"                 ,"N_p5.6.vbyte.lzma.dat"                 ] )
variants.append( ["VBYTE-LZEND"       , "../../../indexes/NOPOS/EXPERIMENTS/experiments.20g/555.scripts.lzend.delta_sample"        ,"B-LOG.dat"               , "S-LOG.dat"               , "E_Wa.5.lzend.dat"                      ,"E_Wb.5.lzend.dat"                      ,"N_p2.5.lzend.dat"                      ,"N_p5.5.lzend.dat"                      ] )
variants.append( ["REPAIR"            , "../../../indexes/NOPOS/EXPERIMENTS/experiments.20g/4.22.scripts.RepairGonzalo"            ,"B-LOG.dat"               , "S-LOG.dat"               , "E_Wa.4.2.repairG.dat"                  ,"E_Wb.4.2.repairG.dat"                  ,"N_p2.4.2.repairG.dat"                  ,"N_p5.4.2.repairG.dat"                  ] )
variants.append( ["REPAIR-SKIPPING"   , "../../../indexes/NOPOS/EXPERIMENTS/experiments.20g/4.33.RepairSkippingGonzalo"            ,"B-LOG.dat"               , "S-LOG.dat"               , "E_Wa.4.3.repairG.skipping.dat"         ,"E_Wb.4.3.repairG.skipping.dat"         ,"N_p2.4.3.repairG.skipping.dat"         ,"N_p5.4.3.repairG.skipping.dat"         ] )  
variants.append( ["REPAIR-SKIPPING-CM", "../../../indexes/NOPOS/EXPERIMENTS/experiments.20g/4.33.1.RepairSkippingMoffatGonzalo"    ,"B-LOG.dat"               , "S-LOG.dat"               , "E_Wa.4.3.1repairG.skipping.moffat.dat" ,"E_Wb.4.3.1repairG.skipping.moffat.dat" ,"N_p2.4.3.1repairG.skipping.moffat.dat" ,"N_p5.4.3.1repairG.skipping.moffat.dat" ] )  
variants.append( ["REPAIR-SKIPPING-ST", "../../../indexes/NOPOS/EXPERIMENTS/experiments.20g/4.33.2.RepairSkippingSandersGonzalo"   ,"B-LOG.dat"               , "S-LOG.dat"               , "E_Wa.4.3.2repairG.skipping.sanders.dat","E_Wb.4.3.2repairG.skipping.sanders.dat","N_p2.4.3.2repairG.skipping.sanders.dat","N_p5.4.3.2repairG.skipping.sanders.dat"] )
  
#src=vbytePos[0:10] #src is a COPY of the list (which remains un-modified)
#src=rice[0:8]       #src is a COPY of the list (which remains un-modified)
#src=riceB[0:8]      #src is a COPY of the list (which remains un-modified)


header= r"""
%%%%   STATIC HEADER %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
\begin{table}[htbp]
  \scriptsize
  \centering
    \begin{tabular}{|l|r|r|c|c|c|c|}
    \cline{2-7}    \multicolumn{1}{r|}{} & \multicolumn{2}{c|}{Overall Time} &                 \multicolumn{4}{c|}{Locate}               \\
    \cline{2-7}    \multicolumn{1}{r|}{} & \multicolumn{2}{c|}{            } & \multicolumn{2}{c|}{Words}  & \multicolumn{2}{c|}{Phrases} \\
    \cline{2-7}    \multicolumn{1}{r|}{} &       Building & Querying         &    {Low freq} & {High freq} &    {2-words} & {5-words} \\
    \hline
    \hline
    %%%% CONTENTS GENERATED BY SCRIPT                            %%%%%%%%%%%%%%%%%%%%%%%%%%"""


footer= r"""%%%%   STATIC FOOTER %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%    
   \cline{1-3}    
   \end{tabular}%
  \caption{Summary and state of the experiments run on the test machine: non-positional inverted indexes.}
  \label{ap1:nopos}%
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

    str=getElapsedTime("../../../indexes/NOPOS/NOPOS-LOG.dat")
    str=str.rjust(13)+" "


overall=r"""   \hline
   \textbf{OVERALL TIME }    &     \multicolumn{2}{|c|}{""" +   str + r""" }   &\multicolumn{4}{|r}{}  \\"""

#print overall
addDataToLog(filename, overall)


#print footer
addDataToLog(filename, footer)

















 
 
 
