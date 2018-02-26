#!/bin/bash

rm -f e80_log_rlcsa* e13000_log_rlcsa* 

echo "#### EXTRACT: short snippets (80chars): ../data/intervals/wiki2g.intervals.80chars.txt ######" 

for i in `seq 1 5`;
do
printf 'run %s of 5 [samples 32,64,128,512]\r' "$i"
./rlcsa_display ./indexes/wiki2gb.txt ./indexes/wiki2gb.txt.DOCBOUNDARIES  32  < ../data/intervals/wiki2g.intervals.80chars.txt >> e80_log_rlcsa_E32.txt 
sleep 1 && touch ./indexes/wiki2gb.txt.rlcsa.array
./rlcsa_display ./indexes/wiki2gb.txt ./indexes/wiki2gb.txt.DOCBOUNDARIES  64  < ../data/intervals/wiki2g.intervals.80chars.txt >> e80_log_rlcsa_E64.txt 
sleep 1 && touch ./indexes/wiki2gb.txt.rlcsa.array
./rlcsa_display ./indexes/wiki2gb.txt ./indexes/wiki2gb.txt.DOCBOUNDARIES  128  < ../data/intervals/wiki2g.intervals.80chars.txt >> e80_log_rlcsa_E128.txt 
sleep 1 && touch ./indexes/wiki2gb.txt.rlcsa.array
./rlcsa_display ./indexes/wiki2gb.txt ./indexes/wiki2gb.txt.DOCBOUNDARIES  256  < ../data/intervals/wiki2g.intervals.80chars.txt >> e80_log_rlcsa_E256.txt 
sleep 1 && touch ./indexes/wiki2gb.txt.rlcsa.array
./rlcsa_display ./indexes/wiki2gb.txt ./indexes/wiki2gb.txt.DOCBOUNDARIES  512  < ../data/intervals/wiki2g.intervals.80chars.txt >> e80_log_rlcsa_E512.txt 
sleep 1 && touch ./indexes/wiki2gb.txt.rlcsa.array
done
echo ""

for i in `seq 1 2`;
do
printf 'run %s of 2 [samples 1024,2048]\r' "$i"
./rlcsa_display ./indexes/wiki2gb.txt ./indexes/wiki2gb.txt.DOCBOUNDARIES  1024  < ../data/intervals/wiki2g.intervals.80chars.txt >> e80_log_rlcsa_E1024.txt 
sleep 1 && touch ./indexes/wiki2gb.txt.rlcsa.array
./rlcsa_display ./indexes/wiki2gb.txt ./indexes/wiki2gb.txt.DOCBOUNDARIES  2048  < ../data/intervals/wiki2g.intervals.80chars.txt >> e80_log_rlcsa_E2048.txt 
sleep 1 && touch ./indexes/wiki2gb.txt.rlcsa.array
done
echo ""


echo "#################################" >> e80_log_rlcsa_E32.txt 
echo "#################################" >> e80_log_rlcsa_E64.txt 
echo "#################################" >> e80_log_rlcsa_E128.txt 
echo "#################################" >> e80_log_rlcsa_E256.txt 
echo "#################################" >> e80_log_rlcsa_E512.txt 
echo "#################################" >> e80_log_rlcsa_E1024.txt 
echo "#################################" >> e80_log_rlcsa_E2048.txt 



echo "#### EXTRACT: short snippets (13000chars): ../data/intervals/wiki2g.intervals.80chars.txt ######" 

for i in `seq 1 5`;
do
printf 'run %s of 5 [samples 32,64,128,512]\r' "$i"
./rlcsa_display ./indexes/wiki2gb.txt ./indexes/wiki2gb.txt.DOCBOUNDARIES  32  < ../data/intervals/wiki2g.intervals.13000chars.txt >> e13000_log_rlcsa_E32.txt 
sleep 1 && touch ./indexes/wiki2gb.txt.rlcsa.array
./rlcsa_display ./indexes/wiki2gb.txt ./indexes/wiki2gb.txt.DOCBOUNDARIES  64  < ../data/intervals/wiki2g.intervals.13000chars.txt >> e13000_log_rlcsa_E64.txt 
sleep 1 && touch ./indexes/wiki2gb.txt.rlcsa.array
./rlcsa_display ./indexes/wiki2gb.txt ./indexes/wiki2gb.txt.DOCBOUNDARIES  128  < ../data/intervals/wiki2g.intervals.13000chars.txt >> e13000_log_rlcsa_E128.txt 
sleep 1 && touch ./indexes/wiki2gb.txt.rlcsa.array
./rlcsa_display ./indexes/wiki2gb.txt ./indexes/wiki2gb.txt.DOCBOUNDARIES  256  < ../data/intervals/wiki2g.intervals.13000chars.txt >> e13000_log_rlcsa_E256.txt 
sleep 1 && touch ./indexes/wiki2gb.txt.rlcsa.array
./rlcsa_display ./indexes/wiki2gb.txt ./indexes/wiki2gb.txt.DOCBOUNDARIES  512  < ../data/intervals/wiki2g.intervals.13000chars.txt >> e13000_log_rlcsa_E512.txt 
sleep 1 && touch ./indexes/wiki2gb.txt.rlcsa.array
done
echo ""

for i in `seq 1 2`;
do
printf 'run %s of 2 [samples 1024,2048]\r' "$i"
./rlcsa_display ./indexes/wiki2gb.txt ./indexes/wiki2gb.txt.DOCBOUNDARIES  1024  < ../data/intervals/wiki2g.intervals.13000chars.txt >> e13000_log_rlcsa_E1024.txt 
sleep 1 && touch ./indexes/wiki2gb.txt.rlcsa.array
./rlcsa_display ./indexes/wiki2gb.txt ./indexes/wiki2gb.txt.DOCBOUNDARIES  2048  < ../data/intervals/wiki2g.intervals.13000chars.txt >> e13000_log_rlcsa_E2048.txt 
sleep 1 && touch ./indexes/wiki2gb.txt.rlcsa.array
done
echo ""

echo "#################################" >> e13000_log_rlcsa_E32.txt 
echo "#################################" >> e13000_log_rlcsa_E64.txt 
echo "#################################" >> e13000_log_rlcsa_E128.txt 
echo "#################################" >> e13000_log_rlcsa_E256.txt 
echo "#################################" >> e13000_log_rlcsa_E512.txt
echo "#################################" >> e13000_log_rlcsa_E1024.txt 
echo "#################################" >> e13000_log_rlcsa_E2048.txt  


echo "#### EXTRACT: TESTS COMPLETED: see log-files: e*log_rlcsa* #####" 
