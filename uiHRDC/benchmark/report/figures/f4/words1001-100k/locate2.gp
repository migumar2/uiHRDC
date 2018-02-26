set xlabel; set ylabel; set title
#set terminal postscript monochrome "Bitstream-Vera-Sans" 16
set terminal postscript enhanced color "Bitstream-Vera-Sans" 17
set output "locate-words1001-100k.eps"
#set multiplot

######################################################
# Bigger plot options

set title "Word queries (high frequency)" font "Bitstream-Vera-Sans,24"
set xlabel "space (% of the collection)"
set ylabel "time (microsec/occurrence)"
#set key bottom left
set key top right
#set nokey
set pointsize 1.5
set logscale x
set logscale y
#set yr [0.01:15000]
#set xr [1:41]
#set xtics ("1" 1, "2" 2, "3" 3, "4" 4,"5" 5,"6" 6,"7" 7,"8" 8,"9" 9, "10" 10,"20" 20, "30" 30,""40)

plot\
  "E_Wb.7.1.simple9e.dat" using (100*$1/2087767849):3 title "Simple9" with linespoints ls 2 lc rgb 'blue',\
  "E_Wb.10.eliasfanopart.dat" using (100*$1/2087767849):3 title "EF-opt*" 			with linespoints ls 19,\
  "E_Wb.11.optpfd.dat" using (100*$1/2087767849):3 title "OPT-PFD*" 			with linespoints ls 20,\
  "E_Wb.13.varint.dat" using (100*$1/2087767849):3 title "Varint-G8IU*" 			with linespoints ls 22,\
  "E_Wb.6.vbyte.lzma.dat" using (100*$1/2087767849):3 title "Vbyte-LZMA" with linespoints ls 7,\
  "E_Wb.4.2.repairG.dat"          using (100*$1/2087767849):3 title "RePair" with linespoints lt 3 pt 11,\
  "E_Wb.4.3.repairG.skipping.dat" using (100*$1/2087767849):3 title "RePair-Skip" with linespoints lt 3 pt 9,\
  "lz77.dat" using (100*$1/2087767849):($2) title "LZ77-Index" with linespoints ls 24 lc rgb 'blue' ,\
  "lzend.dat" using (100*$1/2087767849):($2) title "LZEnd-Index" with linespoints ls 25 ,\
  "rlcsa.dat" using (100*$1/2087767849):2 title "RLCSA" with linespoints ls 16,\
  "slp.dat" using (100*$1/2087767849):2 title "SLP" with linespoints ls 4 lc rgb 'blue',\
  "wslp.dat" using (100*$1/2087767849):2 title "WSLP" with linespoints ls 5 lc rgb 'blue',\
  "N.Wb_swcsa.dat" using (100*$1/2087767849):($3) title "WCSA" with linespoints ls 14 lc rgb 'black'

#  "E_Wb.2.vbyte.moffat.dat" using (100*$1/2087767849):3 title "Vbyte-CM" with linespoints,\
#  "E_Wb.vbyte.sanders.dat" using (100*$1/2087767849):3 title "Vbyte-ST" with linespoints,\
#  "E_Wb.4.2.repair.dat"          using (100*$1/2087767849):3 title "RePair" with linespoints lt 3 pt 11,\
#  "E_Wb.4.3.repair.skipping.dat" using (100*$1/2087767849):3 title "RePair-Skip" with linespoints ls 6  lc rgb 'blue' ,\





# ######################################################
# # Now we set the options for the smaller plot
# set title 'Zoom'
# #set notitle
# set xlabel ""
# set ylabel ""
# set size 0.3,0.3
# set nokey
# set origin 0.15,.1   # porcentaje inicio respecto al 0,0 que es izq/abajo
# #set xtics 10,.1,100
# #set ytics 1,10,30
# #set xtics (30,40,50,60,70)
# 
# unset logscale x
# unset logscale y
# 
# #     set xtics ("low" 0, "medium" 50, "high" 100)
# #     set xtics (1,2,4,8,16,32,64,128,256,512,1024)
# #set xrange [30:65]
# #set yrange [1:100]
# 
# set xtics ("30" 30,"32" 32, "34" 34, "36" 36, "38" 38)
# set ytics ("0.010" .01,"0.015" .015,"0.020" .02,"0.025" .025)
# ######################################################
# # And finally let's plot the same set of data, but in the smaller plot
# 
# plot [29.5:39] [0.008:0.025]\
#   "E_Wb.1.4.rice.dat" using (100*$1/2087767849):3 title "Rice" with linespoints ls 13	pt 12 lc rgb 'black',\
#   "E_Wb.7.1.simple9e.dat" using (100*$1/2087767849):3 title "Simple9" with linespoints ls 2 lc rgb 'blue',\
#   "E_Wb.2.1.vbyte.dat" using (100*$1/2087767849):3 title "Vbyte" with linespoints ls 3 lc rgb 'red',\
#   "E_Wb.9.qmx.dat" using (100*$1/2087767849):3 title "QMX-simd" 		with linespoints ls 18,\
#   "E_Wb.10.eliasfanopart.dat" using (100*$1/2087767849):3 title "EF-opt*" 			with linespoints ls 19,\
#   "E_Wb.11.optpfd.dat" using (100*$1/2087767849):3 title "OPT-PFD*" 			with linespoints ls 20,\
#   "E_Wb.12.interpolative.dat" using (100*$1/2087767849):3 title "Interpolative*" 			with linespoints ls 21,\
#   "E_Wb.13.varint.dat" using (100*$1/2087767849):3 title "Varint-G8IU*" 			with linespoints ls 22,\
#   "E_Wb.6.vbyte.lzma.dat" using (100*$1/2087767849):3 title "Vbyte-LZMA" with linespoints ls 7,\
#   "E_Wb.4.2.repairG.dat"          using (100*$1/2087767849):3 title "RePair" with linespoints lt 3 pt 11,\
#   "E_Wb.4.3.repairG.skipping.dat" using (100*$1/2087767849):3 title "RePair-Skip" with linespoints lt 3 pt 9,\
#   "rlcsa.dat" using (100*$1/2087767849):2 title "RLCSA" with linespoints ls 16,\
#   "slp.dat" using (100*$1/2087767849):2 title "SLP" with linespoints ls 4 lc rgb 'blue',\
#   "wslp.dat" using (100*$1/2087767849):2 title "WSLP" with linespoints ls 5 lc rgb 'blue',\
#   "N.Wb_swcsa.dat" using (100*$1/2087767849):($3) title "WCSA" with linespoints ls 14 lc rgb 'black' ,\
#   "lz77.dat" using (100*$1/2087767849):($2*1000) title "LZ77-Index" with linespoints ls 24 lc rgb 'blue' ,\
#   "lzend.dat" using (100*$1/2087767849):($2*1000) title "LZEnd-Index" with linespoints ls 25
# 
# 
# # It's important to close the multiplot environment!!!
# unset multiplot
# 
