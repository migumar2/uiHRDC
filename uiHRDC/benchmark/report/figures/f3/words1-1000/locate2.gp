set xlabel; set ylabel; set title
#set terminal postscript monochrome "Bitstream-Vera-Sans" 16
set terminal postscript enhanced color "Bitstream-Vera-Sans" 17
set output "locate-words1-1000.eps"
set title "Word queries (low frequency)" font "Bitstream-Vera-Sans,24"
set xlabel "space (% of the collection)"
set ylabel "time (microsec/occurrence)"
#set key bottom left
set key top right
#set nokey
set pointsize 1.5
set logscale x
set logscale y
#set yr [0:5]
#set xr [20:151]
set xr [10:151]
#set xtics ("" 1,"" 10,20,30,40,"" 50,60,"" 70,80,"" 90,100,"" 110,120,"" 130,140,"" 150)


plot\
  "E_Wa.1.4.rice.dat" using (100*$1/2087767849):3 title "Rice" with linespoints ls 13	pt 12 lc rgb 'black',\
  "E_Wa.2.1.vbyte.dat" using (100*$1/2087767849):3 title "Vbyte" with linespoints ls 3 lc rgb 'red',\
  "E_Wa.7.1.simple9e.dat" using (100*$1/2087767849):3 title "Simple9" with linespoints ls 2 lc rgb 'blue',\
  "E_Wa.9.qmx.dat" using (100*$1/2087767849):3 title "QMX-simd" 		with linespoints ls 18,\
  "E_Wa.10.eliasfanopart.dat" using (100*$1/2087767849):3 title "EF-opt*" 			with linespoints ls 19,\
  "E_Wa.11.optpfd.dat" using (100*$1/2087767849):3 title "OPT-PFD*" 			with linespoints ls 20,\
  "E_Wa.12.interpolative.dat" using (100*$1/2087767849):3 title "Interpolative*" 			with linespoints ls 21,\
  "E_Wa.13.varint.dat" using (100*$1/2087767849):3 title "Varint-G8IU*" 			with linespoints ls 22

