set xlabel; set ylabel; set title
#set terminal postscript monochrome "Bitstream-Vera-Sans" 16
set terminal postscript enhanced color "Bitstream-Vera-Sans" 17
set output "extract13000.eps"
set title "Extract of Snippets (13,000 chars)" font "Bitstream-Vera-Sans,24"
set xlabel "space (% of the collection)"
set ylabel "time (microsec/char)"
#set key bottom left
#set key top right
#set nokey
set pointsize 1.5
set logscale x
set logscale y

#set yr [0.01:100]
set xr [1:51]
set xtics ("1" 1, "2" 2, "3" 3, "4" 4,"5" 5,"6" 6,"7" 7,"8" 8,""9, "10" 10,"20" 20, "30" 30,"40"40, "50" 50,"60"60,""70, "80"80, "30" 30,""90, "100"100)

plot\
  "m.iip.repairT.dat" using (100*$7/2087767849):($4) title "RePair" with linespoints lt 3 pt 11 lc rgb 'red',\
  "lz77.dat" using (100*$1/2087767849):($2) title "LZ77-Index" with linespoints ls 24 lc rgb 'blue',\
  "lzend.dat" using (100*$1/2087767849):($2) title "LZEnd-Index" with linespoints ls 25,\
  "rlcsa.dat" using (100*$1/2087767849):2 title "RLCSA" with linespoints ls 16,\
  "slp.dat" using (100*$1/2087767849):2 title "SLP" with linespoints ls 4 lc rgb 'blue',\
  "wslp.dat" using (100*$1/2087767849):2 title "WSLP" with linespoints ls 5 lc rgb 'blue',\
  "e.Words3000_swcsa.dat" using (100*$1/2087767849):($4) title "WCSA" with linespoints ls 14 lc rgb 'black'
