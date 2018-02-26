echo ""
echo "GENERATING gnuplot .dat files for:"
echo "    OPT-OV, UNIF-OV, OPTPFOR, VARINT, INTERPOLATIVE"
echo "... from output.experiments.txt log file"
echo ""
grep "Documents" output.experiments.txt > sizes.txt
grep "Time per occ" output.experiments.txt > times.txt

python collectResults.py sizes.txt times.txt

rm -f *eliasfanoUNI.dat  #not needed (NON-optimized EF-index)
