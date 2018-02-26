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
#set yr [0.01:15000]

#set xr [1:41]
#set xtics ("1" 1, "2" 2, "3" 3, "4" 4,"5" 5,"6" 6,"7" 7,"8" 8,"9" 9, "10" 10,"20" 20, "30" 30,""40)

plot\
  "E_Wa.7.1.simple9e.dat" using (100*$1/2087767849):3 title "Simple9"             with linespoints ls 2 lc rgb 'blue',\
  "E_Wa.10.eliasfanopart.dat" using (100*$1/2087767849):3 title "EF-opt*" 	      with linespoints ls 19,\
  "E_Wa.11.optpfd.dat" using (100*$1/2087767849):3 title "OPT-PFD*" 		      with linespoints ls 20,\
  "E_Wa.13.varint.dat" using (100*$1/2087767849):3 title "Varint-G8IU*" 	      with linespoints ls 22,\
  "E_Wa.6.vbyte.lzma.dat" using (100*$1/2087767849):3 title "Vbyte-LZMA"          with linespoints ls 7,\
  "E_Wa.4.2.repairG.dat"          using (100*$1/2087767849):3 title "RePair"      with linespoints lt 3 pt 11,\
  "E_Wa.4.3.repairG.skipping.dat" using (100*$1/2087767849):3 title "RePair-Skip" with linespoints lt 3 pt 9,\
  "lz77.dat" using (100*$1/2087767849):($2) title "LZ77-Index"                    with linespoints ls 24 lc rgb 'blue' ,\
  "lzend.dat" using (100*$1/2087767849):($2) title "LZEnd-Index"                  with linespoints ls 25 ,\
  "rlcsa.dat" using (100*$1/2087767849):2 title "RLCSA"                           with linespoints ls 16,\
  "slp.dat" using (100*$1/2087767849):2 title "SLP"                               with linespoints ls 4 lc rgb 'blue',\
  "wslp.dat" using (100*$1/2087767849):2 title "WSLP"                             with linespoints ls 5 lc rgb 'blue',\
  "N.Wa_swcsa.dat" using (100*$1/2087767849):($3) title "WCSA"                    with linespoints ls 14 lc rgb 'black'


#  "E_Wa.2.vbyte.moffat.dat" using (100*$1/2087767849):3 title "Vbyte-CM" with linespoints,\
#  "E_Wa.vbyte.sanders.dat" using (100*$1/2087767849):3 title "Vbyte-ST" with linespoints,\
#  "E_Wa.4.2.repair.dat" using (100*$1/2087767849):3 title "RePair" with linespoints lt 3 pt 11,\
#  "E_Wa.4.3.repair.skipping.dat" using (100*$1/2087767849):3 title "RePair-Skip" with linespoints lt 3 pt 9,\





