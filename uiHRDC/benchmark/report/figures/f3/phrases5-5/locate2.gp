set xlabel; set ylabel; set title
#set terminal postscript monochrome "Bitstream-Vera-Sans" 16
set terminal postscript enhanced color "Bitstream-Vera-Sans" 17
set output "locate-5_5.eps"
set title "Phrase queries (5 words)" font "Bitstream-Vera-Sans,24"
set xlabel "space (% of the collection)"
set ylabel "time (microsec/occurrence)"
#set key bottom left
set key top right
#set nokey
set pointsize 1.5
set logscale x
set logscale y

#set yr [0.1:11]
set xr [20:151]
#set xr [10:151]
set xtics ("" 1,"" 10,20,30,40,"" 50,60,"" 70,80,"" 90,100,"" 110,120,"" 130,140,"" 150)

plot\
  "N_p5.1.4.rice.dat" using (100*$1/2087767849):3 title "Rice" with linespoints ls 13	pt 12 lc rgb 'black',\
  "N_p5.2.1.vbyte.dat" using (100*$1/2087767849):3 title "Vbyte" with linespoints ls 3 lc rgb 'red',\
  "N_p5.2.vbyte.moffat.dat" using (100*$1/2087767849):3 title "Vbyte-CM" with linespoints ls 4 lc rgb 'red',\
  "N_p5.vbyte.sanders.dat" using (100*$1/2087767849):3 title "Vbyte-ST" with linespoints ls 5 lc rgb 'red',\
  "N_p5.7.1.simple9e.dat" using (100*$1/2087767849):3 title "Simple9" with linespoints ls 2 lc rgb 'blue',\
  "N_p5.9.qmx.dat" using (100*$1/2087767849):3 title "QMX-simd" 		with linespoints ls 18,\
  "N_p5.10.eliasfanopart.dat" using (100*$1/2087767849):3 title "EF-opt*" 			with linespoints ls 19,\
  "N_p5.11.optpfd.dat" using (100*$1/2087767849):3 title "OPT-PFD*" 			with linespoints ls 20,\
  "N_p5.12.interpolative.dat" using (100*$1/2087767849):3 title "Interpolative*" 			with linespoints ls 21,\
  "N_p5.13.varint.dat" using (100*$1/2087767849):3 title "Varint-G8IU*" 			with linespoints ls 22
#  ,\
#  "N_p5.6.vbyte.lzma.dat" using (100*$1/2087767849):3 title "Vbyte-LZMA" with linespoints ls 7,\
#  "N_p5.4.2.repairG.dat"          using (100*$1/2087767849):3 title "RePair" with linespoints lt 3 pt 11,\
#  "N_p5.4.3.repairG.skipping.dat" using (100*$1/2087767849):3 title "RePair-Skip" with linespoints lt 3 pt 9,\
#  "N_p5.4.3.1repairG.skipping.moffat.dat" using (100*$1/2087767849):3 title "RePair-Skip-CM" 	with linespoints lt 3 pt 8,\
#  "N_p5.4.3.2repairG.skipping.sanders.dat" using (100*$1/2087767849):3 title "RePair-Skip-ST" 	with linespoints lt 3 pt 10,\
#  "rlcsa.dat" using (100*$1/2087767849):2 title "RLCSA" with linespoints ls 16,\
#  "slp.dat" using (100*$1/2087767849):2 title "SLP" with linespoints ls 4 lc rgb 'blue',\
#  "wslp.dat" using (100*$1/2087767849):2 title "WSLP" with linespoints ls 5 lc rgb 'blue',\
#  "N.P5_swcsa.dat" using (100*$1/2087767849):($3) title "WCSA" with linespoints ls 14 lc rgb 'black' ,\
#  "lz77.dat" using (100*$1/2087767849):($2*1000) title "LZ77-Index" with linespoints ls 24 lc rgb 'blue' ,\
#  "lzend.dat" using (100*$1/2087767849):($2*1000) title "LZEnd-Index" with linespoints ls 25

#  "N_p5.4.2.repair.dat" using (100*$1/2087767849):3 title "RePair" with linespoints lt 3 pt 11,\
#  "N_p5.4.3.repair.skipping.dat" using (100*$1/2087767849):3 title "RePair-Skip" with linespoints lt 3 pt 9,\
