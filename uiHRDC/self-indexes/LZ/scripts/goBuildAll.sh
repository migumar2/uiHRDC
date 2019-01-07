#! /bin/bash

echo " \n ###########################"
echo " ### Compiling LZ index  ###"
echo " ###########################\n"
cd ../src
  make
  cd ..


echo " \n ##############################################"
echo " ### LZ-based indexes are ready to be used! ###"
echo " ##############################################\n"

echo " \n ------------------------------------"
echo " --- Populating evaluation folder ---"
echo " ------------------------------------\n"
mkdir -p evaluation 
cd evaluation
  ln -s ../../../data/texts/wiki_src2gb.txt text
  ln -s ../../../data/texts/wiki_src2gb.txt.DOCBOUNDARIES text.DOCBOUNDARIES
  mkdir patterns 
  mkdir intervals 
  mkdir indexes 
  mkdir results

  cd patterns
    ln -s ../../../../data/patterns/wiki2g* .
    cd .. 
  cd intervals
    ln -s ../../../../data/intervals/wiki2g* .
    cd ../../


rm -f evaluation/results/Blz*-LOG.dat*
echo " \n ------------------------"
echo " --- LZ77 indexing... ---"
echo " ------------------------\n"

python ../../utils-py/starttime.py  evaluation/results/Blz77-LOG.dat "Build-starts!"   ##LOGS-ELAPSED-TIMES
./src/bin/build_lz77 evaluation/text evaluation/indexes/index.lz77 bsst brev
python ../../utils-py/endtime.py    evaluation/results/Blz77-LOG.dat                  ##LOGS-ELAPSED-TIMES

echo " \n -------------------------"
echo " --- LZend indexing... ---"
echo " -------------------------\n"

python ../../utils-py/starttime.py  evaluation/results/Blzend-LOG.dat "Build-starts!"   ##LOGS-ELAPSED-TIMES
./src/bin/build_lzend evaluation/text evaluation/indexes/index.lzend bsst brev
python ../../utils-py/endtime.py    evaluation/results/Blzend-LOG.dat                  ##LOGS-ELAPSED-TIMES

rm evaluation/*alph*

echo " \n =========================================================="
echo " === The collection is indexed and ready to be evaluated ==="
echo "  ==========================================================\n"

cd ./scripts
