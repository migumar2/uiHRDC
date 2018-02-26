set xlabel; set ylabel; set title
#set terminal postscript monochrome "Bitstream-Vera-Sans" 16
set terminal postscript enhanced color "Bitstream-Vera-Sans" 17
set output "nopos-2_2.eps"
set title "Location of phrases (2-words)" font "Bitstream-Vera-Sans,24"
set xlabel "space (% of the collection)"
set ylabel "time (microsec/occurrence)"
#set key bottom left
#set key top left
set key top right
#set nokey
set pointsize 1.5
set logscale x
set logscale y
#set  yrange [0.005:25]
#set yr [0:12]
#set xr [0.1:1]
#set xtics ("0.1" 0.1, "0.2" 0.2, "0.3" 0.3, "0.4" 0.4,"0.5" 0.5,"0.6" 0.6,"" 0.7,"0.8" 0.8,""0.9, "1" 1)
plot\
  "N_p2.7.simple9.dat" using (100*$1/26597454863):3 title "Simple9" 				with linespoints ls 2 lc rgb 'blue',\
  "N_p2.10.eliasfanopart.dat" using (100*$1/26597454863):3 title "EF-opt*" 			with linespoints ls 19,\
  "N_p2.11.optpfd.dat" using (100*$1/26597454863):3 title "OPT-PFD*" 			with linespoints ls 20,\
  "N_p2.1.5.riceRLE.dat" using (100*$1/26597454863):3 title "Rice-Runs" 			with linespoints ls 17	pt 13,\
  "N_p2.6.vbyte.lzma.dat" using (100*$1/26597454863):3 title "Vbyte-LZMA" 			with linespoints ls 7,\
  "N_p2.5.lzend.dat" using (100*$1/26597454863):3 title "Vbyte-lzend" 				with linespoints ls 3 pt 14,\
  "N_p2.4.2.repairG.dat" using (100*$1/26597454863):3 title "RePair" 				with linespoints lt 3 pt 11,\
  "N_p2.4.3.repairG.skipping.dat" using (100*$1/26597454863):3 title "RePair-Skip"	with linespoints lt 3 pt 9,\
  "N_p2.4.3.1repairG.skipping.moffat.dat" using (100*$1/26597454863):3 title "RePair-Skip-CM" 	with linespoints lt 3 pt 8,\
  "N_p2.4.3.2repairG.skipping.sanders.dat" using (100*$1/26597454863):3 title "RePair-Skip-ST" 	with linespoints lt 3 pt 10 


#plot\
#  "N_p2.1.4.rice.dat" using (100*$1/26597454863):3 title "Rice" 					with linespoints ls 13	pt 12 lc rgb 'black',\
#  "N_p2.2.1.vbyte.dat" using (100*$1/26597454863):3 title "Vbyte" 					with linespoints ls 3 lc rgb 'red',\
#  "N_p2.2.vbyte.moffat.bitmap1.dat" using (100*$1/26597454863):3 title "Vbyte-CM" 		with linespoints ls 4 lc rgb 'red',\
#  "N_p2.3.vbyte.sanders.dat" using (100*$1/26597454863):3 title "Vbyte-ST" 			with linespoints ls 5 lc rgb 'red',\
#  "N_p2.1.4.rice.bitmap8.dat" using (100*$1/26597454863):3 title "RiceB" 			with linespoints ls 1 lc rgb 'black',\
#  "N_p2.2.1.vbyte.bitmap8.dat" using (100*$1/26597454863):3 title "VbyteB" 			with linespoints ls 2 lc rgb 'red',\
#  "N_p2.2.vbyte.moffat.dat" using (100*$1/26597454863):3 title "Vbyte-CMB" 			with linespoints ls 8,\
#  "N_p2.3.vbyte.sanders.bitmap8.dat" using (100*$1/26597454863):3 title "Vbyte-STB" with linespoints ls 10,\
#  "N_p2.7.simple9.dat" using (100*$1/26597454863):3 title "Simple9" 				with linespoints ls 2 lc rgb 'blue',\
#  "N_p2.8.pfordelta.dat" using (100*$1/26597454863):3 title "PforDelta" 			with linespoints ls 16,\
#  "N_p2.9.qmx.dat" using (100*$1/26597454863):3 title "QMX-simd" 					with linespoints ls 18,\
#  "N_p2.10.eliasfanopart.dat" using (100*$1/26597454863):3 title "EF-opt*" 			with linespoints ls 19,\
#  "N_p2.11.optpfd.dat" using (100*$1/26597454863):3 title "OPT-PFD*" 			with linespoints ls 20,\
#  "N_p2.12.interpolative.dat" using (100*$1/26597454863):3 title "Interpolative*" 			with linespoints ls 21,\
#  "N_p2.13.varint.dat" using (100*$1/26597454863):3 title "Varint-G8IU*" 			with linespoints ls 22,\
#  "N_p2.1.5.riceRLE.dat" using (100*$1/26597454863):3 title "Rice-Runs" 			with linespoints ls 17	pt 13,\
#  "N_p2.6.vbyte.lzma.dat" using (100*$1/26597454863):3 title "Vbyte-LZMA" 			with linespoints ls 7,\
#  "N_p2.5.lzend.dat" using (100*$1/26597454863):3 title "Vbyte-lzend" 				with linespoints ls 3 pt 14,\
#  "N_p2.4.2.repairG.dat" using (100*$1/26597454863):3 title "RePair" 				with linespoints lt 3 pt 11,\
#  "N_p2.4.3.repairG.skipping.dat" using (100*$1/26597454863):3 title "RePair-Skip"	with linespoints lt 3 pt 9,\
#  "N_p2.4.3.1repairG.skipping.moffat.dat" using (100*$1/26597454863):3 title "RePair-Skip-CM" 	with linespoints lt 3 pt 8,\
#  "N_p2.4.3.2repairG.skipping.sanders.dat" using (100*$1/26597454863):3 title "RePair-Skip-ST" 	with linespoints lt 3 pt 10 
  
 
