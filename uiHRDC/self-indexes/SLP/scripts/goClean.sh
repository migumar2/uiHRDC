#! /bin/bash

echo " \n ###############################"
echo " ### Cleaning LIBCDS library ###"
echo " ###############################\n"
cd ../libcds/
make clean

echo " \n ##########################"
echo " ### Cleaning SLP index ###"
echo " ##########################\n"
cd ../slp/
make clean

echo " \n ###########################"
echo " ### Cleaning WSLP index ###"
echo " ###########################\n"
cd ../wslp/
make clean

cd ..
rm -rf evaluation
echo " \n ============================================="
echo " === The evaluation folder is now cleaned! ==="
echo " =============================================\n"

cd ./scripts
