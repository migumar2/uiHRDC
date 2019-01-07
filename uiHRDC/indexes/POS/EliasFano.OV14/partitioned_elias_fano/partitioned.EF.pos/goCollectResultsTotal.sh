echo ""
echo "GENERATING gnuplot .dat files for:"
echo "    OPT-OV, UNIF-OV, OPTPFOR, VARINT, INTERPOLATIVE"
echo "... from output.experiments.txt log file"
echo ""
grep "Documents" output.experiments.txt > sizes.txt
grep "Time per occ" output.experiments.txt > times.txt

python collectResultsTotal.py sizes.txt times.txt ../../2.1.aaa.EliasFano.PartialMeasurement/E_Wa.2.1.vbyte.dat ../../2.1.aaa.EliasFano.PartialMeasurement/E_Wb.2.1.vbyte.dat ../../2.1.aaa.EliasFano.PartialMeasurement/N_p2.2.1.vbyte.dat ../../2.1.aaa.EliasFano.PartialMeasurement/N_p5.2.1.vbyte.dat

rm -f *eliasfanoUNI.dat  #not needed (NON-optimized EF-index)
