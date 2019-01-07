set xlabel; set ylabel; set title
#set terminal postscript monochrome "Bitstream-Vera-Sans" 16
set terminal postscript enhanced color "Bitstream-Vera-Sans" 17
set output "locate-5_5.eps"

#set multiplot

######################################################
# Bigger plot options

set title "Phrase queries (5 words)" font "Bitstream-Vera-Sans,24"
set xlabel "space (% of the collection)"
set ylabel "time (microsec/occurrence)"
#set key bottom left
set key top right
#set nokey
set pointsize 1.5
set logscale x
set logscale y
#set yr [0.1:15000]
#set xr [1:41]
set xr [1:51]
set xtics ("1" 1, "2" 2, "3" 3, "4" 4,"5" 5,"6" 6,"7" 7,"8" 8,"9" 9, "10" 10,"20" 20, "30" 30,"40" 40,"" 50)


plot\
  "N_p5.7.1.simple9e.dat" using (100*$1/2087767849):3 title "Simple9" with linespoints ls 2 lc rgb 'blue',\
  "N_p5.10.eliasfanopart.dat" using (100*$1/2087767849):3 title "EF-opt*" 			with linespoints ls 19,\
  "N_p5.11.optpfd.dat" using (100*$1/2087767849):3 title "OPT-PFD*" 			with linespoints ls 20,\
  "N_p5.13.varint.dat" using (100*$1/2087767849):3 title "Varint-G8IU*" 			with linespoints ls 22,\
  "N_p5.6.vbyte.lzma.dat" using (100*$1/2087767849):3 title "Vbyte-LZMA" with linespoints ls 7,\
  "N_p5.4.2.repairG.dat"          using (100*$1/2087767849):3 title "RePair" with linespoints lt 3 pt 11,\
  "N_p5.4.3.repairG.skipping.dat" using (100*$1/2087767849):3 title "RePair-Skip" with linespoints lt 3 pt 9,\
  "N_p5.4.3.1repairG.skipping.moffat.dat" using (100*$1/2087767849):3 title "RePair-Skip-CM" 	with linespoints lt 3 pt 8,\
  "N_p5.4.3.2repairG.skipping.sanders.dat" using (100*$1/2087767849):3 title "RePair-Skip-ST" 	with linespoints lt 3 pt 10,\
  "lz77.dat" using (100*$1/2087767849):($2) title "LZ77-Index" with linespoints ls 24 lc rgb 'blue' ,\
  "lzend.dat" using (100*$1/2087767849):($2) title "LZEnd-Index" with linespoints ls 25,\
  "rlcsa.dat" using (100*$1/2087767849):2 title "RLCSA" with linespoints ls 16,\
  "slp.dat" using (100*$1/2087767849):2 title "SLP" with linespoints ls 4 lc rgb 'blue',\
  "wslp.dat" using (100*$1/2087767849):2 title "WSLP" with linespoints ls 5 lc rgb 'blue',\
  "N.P5_swcsa.dat" using (100*$1/2087767849):($3) title "WCSA" with linespoints ls 14 lc rgb 'black'

#  "N_p5.4.2.repair.dat" using (100*$1/2087767849):3 title "RePair" with linespoints lt 3 pt 11,\
#  "N_p5.4.3.repair.skipping.dat" using (100*$1/2087767849):3 title "RePair-Skip" with linespoints lt 3 pt 9,\





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
# set xtics ("21" 21,"22" 22, "23"23)
# set ytics ("0.90" .90,"0.95" .95,"1.00" 1.00,"1.05" 1.05,"1.10" 1.10)
# ######################################################
# # And finally let's plot the same set of data, but in the smaller plot
# 
# plot [21:23] [0.9:1.1]\
#   "N_p5.7.1.simple9e.dat" using (100*$1/2087767849):3 title "Simple9" with linespoints ls 2 lc rgb 'blue',\
#   "N_p5.10.eliasfanopart.dat" using (100*$1/2087767849):3 title "EF-opt*" 			with linespoints ls 19,\
#   "N_p5.11.optpfd.dat" using (100*$1/2087767849):3 title "OPT-PFD*" 			with linespoints ls 20,\
#   "N_p5.13.varint.dat" using (100*$1/2087767849):3 title "Varint-G8IU*" 			with linespoints ls 22,\
#   "N_p5.6.vbyte.lzma.dat" using (100*$1/2087767849):3 title "Vbyte-LZMA" with linespoints ls 7,\
#   "N_p5.4.2.repairG.dat"          using (100*$1/2087767849):3 title "RePair" with linespoints lt 3 pt 11,\
#   "N_p5.4.3.repairG.skipping.dat" using (100*$1/2087767849):3 title "RePair-Skip" with linespoints lt 3 pt 9,\
#   "N_p5.4.3.1repairG.skipping.moffat.dat" using (100*$1/2087767849):3 title "RePair-Skip-CM" 	with linespoints lt 3 pt 8,\
#   "N_p5.4.3.2repairG.skipping.sanders.dat" using (100*$1/2087767849):3 title "RePair-Skip-ST" 	with linespoints lt 3 pt 10,\
#   "rlcsa.dat" using (100*$1/2087767849):2 title "RLCSA" with linespoints ls 16,\
#   "slp.dat" using (100*$1/2087767849):2 title "SLP" with linespoints ls 4 lc rgb 'blue',\
#   "wslp.dat" using (100*$1/2087767849):2 title "WSLP" with linespoints ls 5 lc rgb 'blue',\
#   "N.P5_swcsa.dat" using (100*$1/2087767849):($3) title "WCSA" with linespoints ls 14 lc rgb 'black' ,\
#   "lz77.dat" using (100*$1/2087767849):($2*1000) title "LZ77-Index" with linespoints ls 24 lc rgb 'blue' ,\
#   "lzend.dat" using (100*$1/2087767849):($2*1000) title "LZEnd-Index" with linespoints ls 25
# 
# #  "N_p5.4.2.repair.dat" using (100*$1/2087767849):3 title "RePair" with linespoints lt 3 pt 11,\
# #  "N_p5.4.3.repair.skipping.dat" using (100*$1/2087767849):3 title "RePair-Skip" with linespoints lt 3 pt 9,\
# 
# 
# # It's important to close the multiplot environment!!!
# unset multiplot
# 
