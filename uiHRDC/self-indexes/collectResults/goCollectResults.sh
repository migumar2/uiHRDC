echo "---------------------------------------------------------------------"
echo "GENERATING gnuplot .dat files for: SLP, WSLP, LZ77, LZEND, and RLCSA "
echo "from files in "
echo "    ../SLP/evaluation/results/ "
echo "    ../SLP/evaluation/results/ "
echo "    ../LZ/evaluation/results/  "
echo "    ../LZ/evaluation/results/  "
echo "    ../RLCSAM/scripts/         "
echo "and retrieving files (already in gnuplot-type files) for WCSA from"
echo "    ../WCSA/scripts.swcsa3.is/ "
echo "---------------------------------------------------------------------"
rm -rf slp wslp lz77 lzend rlcsa wcsa
mkdir slp wslp lz77 lzend rlcsa wcsa

#python collectResults.py <source dir > <prefix>
python collectResults.py     ../SLP/evaluation/results/ slp
python collectResultsWSLP.py ../SLP/evaluation/results/ wslp   
python collectResults.py     ../LZ/evaluation/results/  lz77
python collectResults.py     ../LZ/evaluation/results/  lzend

#python collectResults.py <source dir > <basename=rlcsa>
python collectResultsRLCSA.py ../RLCSAM/scripts rlcsa

# copy files for WCSA.
sh collectResultsWCSA.sh

echo "---------------------------------------------------------------------"
printf 'Result files in gnuplot-data format were collected\n'
printf 'See directories: slp, wslp, lz77, lzend, rlcsa, and wcsa\n'
echo "---------------------------------------------------------------------"

