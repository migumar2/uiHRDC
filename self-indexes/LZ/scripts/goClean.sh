#! /bin/bash

echo " \n ################################"
echo " ### Cleaning LZ-bases indexes ###"
echo " ################################\n"
cd ..
make cleanall

rm -rf evaluation
echo " \n ============================================="
echo " === The evaluation folder is now cleaned! ==="
echo " =============================================\n"
