#! /bin/bash

echo " \n ###############################################"
echo " ### Cleaning slp/wslp/libcds DATA...           ###"
echo " ##################################################\n"

echo " \n ###############################"
echo " ### Cleaning LIBCDS library    ###"
echo " ##################################\n"
cd ../libcds/
make clean

echo " \n ##########################"
echo " ### Cleaning SLP index    ###"
echo " #############################\n"
cd ../slp/
make clean

echo " \n ###########################"
echo " ### Cleaning WSLP index    ###"
echo " ##############################\n"
cd ../wslp/
make clean
cd ..

echo " \n ##########################################"
echo " ### Cleaning evaluation dir (keep results)###"
echo " #############################################\n"
cd evaluation
rm -f *.*
rm -f text
rm -rf indexes
rm -rf intervals
rm -rf patterns
echo " \n ============================================================"
echo " === The evaluation folder is now cleaned!                   ==="
echo " === Yet, searching results are still at evaluation/results! ==="
echo " ===============================================================\n"
cd ..
cd scripts
