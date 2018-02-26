set xlabel; set ylabel; set title
#set terminal postscript monochrome "Bitstream-Vera-Sans" 16
set terminal postscript enhanced color "Bitstream-Vera-Sans" 16
set output "nonpos-Wa.eps"
set title "Location of words (low frequency)" font "Bitstream-Vera-Sans,24"
set xlabel "space (% of the collection)"
set ylabel "time (microsec/occurrence)"
#set key bottom left
set key top left
#set key at 0.49, 0.09
#set nokey
set pointsize 1.5
set logscale x
set logscale y
#set  yrange [0.001:0.1]
#set xr [0.1:10]
#set xtics ("0.1" 0.1, "0.2" 0.2, "0.3" 0.3, "0.4" 0.4,"" 0.5,"0.6" 0.6,"" 0.7,"0.8" 0.8,""0.9,"1"1,"2"2,"3"3,"4"4,"5"5,"6"6,"7"7,"8"8,"9"9,"10"10)
plot\
  "E_Wa.1.4.rice.dat" using (100*$1/26597454863):3 title "Rice" 				with linespoints ls 13	pt 12 lc rgb 'black',\
  "E_Wa.2.1.vbyte.dat" using (100*$1/26597454863):3 title "Vbyte" 				with linespoints ls 3 lc rgb 'red',\
  "E_Wa.2.vbyte.moffat.dat" using (100*$1/26597454863):3 title 	"Vbyte-CM" 			with linespoints ls 4 lc rgb 'red',\
  "E_Wa.3.vbyte.sanders.dat" using (100*$1/26597454863):3 title "Vbyte-ST" 			with linespoints ls 5 lc rgb 'red',\
  "E_Wa.1.4.rice.bitmap8.dat" using (100*$1/26597454863):3 title "RiceB" 			with linespoints ls 1 lc rgb 'black',\
  "E_Wa.2.1.vbyte.bitmap8.dat" using (100*$1/26597454863):3 title "VbyteB" 			with linespoints ls 2 lc rgb 'red',\
  "E_Wa.2.vbyte.moffat.bitmap8.dat" using (100*$1/26597454863):3 title "Vbyte-CMB" 	with linespoints ls 8,\
  "E_Wa.3.vbyte.sanders.bitmap8.dat" using (100*$1/26597454863):3 title "Vbyte-STB" with linespoints ls 10,\
  "E_Wa.7.simple9.dat" using (100*$1/26597454863):3 title "Simple9" 				with linespoints ls 2 lc rgb 'blue',\
  "E_Wa.8.pfordelta.dat" using (100*$1/26597454863):3 title "Pfordelta" 			with linespoints ls 16,\
  "E_Wa.9.qmx.dat" using (100*$1/26597454863):3 title "QMX-simd" 					with linespoints ls 18,\
  "E_Wa.10.eliasfanopart.dat" using (100*$1/26597454863):3 title "EF-opt*" 			with linespoints ls 19,\
  "E_Wa.11.optpfd.dat" using (100*$1/26597454863):3 title "OPT-PFD*" 			with linespoints ls 20,\
  "E_Wa.12.interpolative.dat" using (100*$1/26597454863):3 title "Interpolative*" 			with linespoints ls 21,\
  "E_Wa.13.varint.dat" using (100*$1/26597454863):3 title "Varint-G8IU*" 			with linespoints ls 22
#  ,\
# "E_Wa.1.5.riceRLE.dat" using (100*$1/26597454863):3 title "Rice RLE" 				with linespoints ls 17	pt 13,\
# "E_Wa.6.vbyte.lzma.dat" using (100*$1/26597454863):3 title "Vbyte-LZMA" 			with linespoints ls 7,\
# "E_Wa.5.lzend.dat" using (100*$1/26597454863):3 title "Vbyte-lzend" 				with linespoints ls 3 pt 14,\
# "E_Wa.4.2.repairG.dat" using (100*$1/26597454863):3 title "RePair" 				with linespoints lt 3 pt 11,\
# "E_Wa.4.3.repairG.skipping.dat" using (100*$1/26597454863):3 title "RePair-Skip" 	with linespoints lt 3 pt 9, \
# "E_Wa.4.3.1repairG.skipping.moffat.dat" using (100*$1/26597454863):3 title "RePair-Skip-CM" 	with linespoints lt 3 pt 8,\
# "E_Wa.4.3.2repairG.skipping.sanders.dat" using (100*$1/26597454863):3 title "RePair-Skip-ST" 	with linespoints lt 3 pt 10



##  "E_Wa.4.2.repairG.dat" using (100*$1/20971541599):3 title "RePair" 				with linespoints lt 3 pt 11,\
##  "E_Wa.4.3.repairG.skipping.dat" using (100*$1/20971541599):3 title "RePair-Skip" 	with linespoints lt 3 pt 9
##
###  "E_Wa.4.2.repair.dat" using (100*$1/20971541599):3 title "RePairAprox" 			with linespoints ls 10,\
###  "E_Wa.4.3.repair.skipping.dat" using (100*$1/20971541599):3 title "RePairAprox-Skip" with linespoints ls 8,\

