#! /bin/bash

echo " \n ################################"
echo " ### Compiling LIBCDS library ###"
echo " ################################\n"
cd ../libcds/
make

echo " \n ###########################"
echo " ### Compiling SLP index ###"
echo " ###########################\n"
cd ../slp/
make

echo " \n ############################"
echo " ### Compiling WSLP index ###"
echo " ############################\n"
cd ../wslp/
make

echo " \n ###############################################"
echo " ### SLP-based indexes are ready to be used! ###"
echo " ###############################################\n"

echo " \n ------------------------------------"
echo " --- Populating evaluation folder ---"
echo " ------------------------------------\n"
cd .. && mkdir evaluation && cd evaluation
ln -s ../../../data/texts/wiki_src2gb.txt text
ln -s ../../../data/texts/wiki_src2gb.txt.DOCBOUNDARIES text.DOCBOUNDARIES
mkdir patterns && mkdir intervals && mkdir indexes && mkdir results
cd patterns
ln -s ../../../../data/patterns/wiki2g* .
cd ../intervals
ln -s ../../../../data/intervals/wiki2g* .
cd ../../



echo " \n -----------------------"
echo " --- SLP indexing... ---"
echo " -----------------------\n"

rm -f evaluation/results/Bslp*-LOG.dat*
python ../../utils-py/starttime.py  evaluation/results/Bslp-LOG.dat "Build-starts!"   ##LOGS-ELAPSED-TIMES

cd slp
./SLPIndex -i ../evaluation/text ../evaluation/indexes/index.slp 4
cd ..

python ../../utils-py/endtime.py    evaluation/results/Bslp-LOG.dat                  ##LOGS-ELAPSED-TIMES


echo " \n ------------------------"
echo " --- WSLP indexing... ---"
echo " ------------------------\n"

rm -f evaluation/results/Bwslp*-LOG.dat*
python ../../utils-py/starttime.py  evaluation/results/Bwslp-LOG.dat "Build-starts!"   ##LOGS-ELAPSED-TIMES

cd wslp
./wSLPIndex -i ../evaluation/text ../evaluation/indexes/index.wslp
cd ..

python ../../utils-py/endtime.py    evaluation/results/Bwslp-LOG.dat                  ##LOGS-ELAPSED-TIMES

echo " \n =========================================================="
echo " === The collection is indexed and ready to be evaluated ==="
echo "  ==========================================================\n"

cd ./scripts
