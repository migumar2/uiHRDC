#! /bin/bash

echo " \n ###############################################"
echo " ### Cleaning LZ-DATA-INDEXES... RETAIN RESULTS ###"
echo " ##################################################\n"
cd ../src
make cleanall

cd ..
cd evaluation
rm -f *.*
rm -f text
rm -rf indexes
rm -rf intervals
rm -rf patterns
echo ""
echo " ==============================================================="
echo " === The evaluation folder is now cleaned!                   ==="
echo " === Yet, searching results are still at evaluation/results! ==="
echo " ===============================================================\n"
cd ..
cd scripts
