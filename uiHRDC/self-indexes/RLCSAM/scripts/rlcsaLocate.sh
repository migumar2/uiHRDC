#!/bin/bash

rm -f Wa_log_rlcsa* Wb_log_rlcsa* P2_log_rlcsa* P5_log_rlcsa*

echo "#### LOCATE: Wa: ../data/patterns/wiki2g.words.f1_1000 ######" 
for i in `seq 1 10`;
do
printf 'run %s of 10 [samples 32,64,128,256]\r' "$i"
./rlcsa_locate ./indexes/wiki2gb.txt ./indexes/wiki2gb.txt.DOCBOUNDARIES  32  < ../data/patterns/wiki2g.words.f1_1000 >> Wa_log_rlcsa_32.txt 2>> Wa_log_rlcsa_32.txt 
sleep 1 && touch ./indexes/wiki2gb.txt.rlcsa.array
./rlcsa_locate ./indexes/wiki2gb.txt ./indexes/wiki2gb.txt.DOCBOUNDARIES  64  < ../data/patterns/wiki2g.words.f1_1000 >> Wa_log_rlcsa_64.txt 2>> Wa_log_rlcsa_64.txt 
sleep 1 && touch ./indexes/wiki2gb.txt.rlcsa.array
./rlcsa_locate ./indexes/wiki2gb.txt ./indexes/wiki2gb.txt.DOCBOUNDARIES 128  < ../data/patterns/wiki2g.words.f1_1000 >> Wa_log_rlcsa_128.txt 2>> Wa_log_rlcsa_128.txt 
sleep 1 && touch ./indexes/wiki2gb.txt.rlcsa.array
./rlcsa_locate ./indexes/wiki2gb.txt ./indexes/wiki2gb.txt.DOCBOUNDARIES 256  < ../data/patterns/wiki2g.words.f1_1000 >> Wa_log_rlcsa_256.txt 2>> Wa_log_rlcsa_256.txt 
sleep 1 && touch ./indexes/wiki2gb.txt.rlcsa.array
done
echo ""

for i in `seq 1 2`;
do
printf 'run %s of 2 [samples 512,1024,2048]\r' "$i"
./rlcsa_locate ./indexes/wiki2gb.txt ./indexes/wiki2gb.txt.DOCBOUNDARIES 512  < ../data/patterns/wiki2g.words.f1_1000 >> Wa_log_rlcsa_512.txt 2>> Wa_log_rlcsa_512.txt 
sleep 1 && touch ./indexes/wiki2gb.txt.rlcsa.array
./rlcsa_locate ./indexes/wiki2gb.txt ./indexes/wiki2gb.txt.DOCBOUNDARIES 1024  < ../data/patterns/wiki2g.words.f1_1000 >> Wa_log_rlcsa_1024.txt 2>> Wa_log_rlcsa_1024.txt 
sleep 1 && touch ./indexes/wiki2gb.txt.rlcsa.array
./rlcsa_locate ./indexes/wiki2gb.txt ./indexes/wiki2gb.txt.DOCBOUNDARIES 2048  < ../data/patterns/wiki2g.words.f1_1000 >> Wa_log_rlcsa_2048.txt 2>> Wa_log_rlcsa_2048.txt 
sleep 1 && touch ./indexes/wiki2gb.txt.rlcsa.array
done
echo ""

echo "#################################" >> Wa_log_rlcsa_32.txt 
echo "#################################" >> Wa_log_rlcsa_64.txt 
echo "#################################" >> Wa_log_rlcsa_128.txt 
echo "#################################" >> Wa_log_rlcsa_256.txt 
echo "#################################" >> Wa_log_rlcsa_512.txt 
echo "#################################" >> Wa_log_rlcsa_1024.txt 
echo "#################################" >> Wa_log_rlcsa_2048.txt 





echo "#### LOCATE: Wb: ../data/patterns/wiki2g.words.f1001_100k ######" 

for i in `seq 1 10`;
do
printf 'run %s of 10 [samples 32,64,128,256]\r' "$i"
./rlcsa_locate ./indexes/wiki2gb.txt ./indexes/wiki2gb.txt.DOCBOUNDARIES  32  < ../data/patterns/wiki2g.words.f1001_100k >> Wb_log_rlcsa_32.txt 2>> Wb_log_rlcsa_32.txt 
sleep 1 && touch ./indexes/wiki2gb.txt.rlcsa.array
./rlcsa_locate ./indexes/wiki2gb.txt ./indexes/wiki2gb.txt.DOCBOUNDARIES  64  < ../data/patterns/wiki2g.words.f1001_100k >> Wb_log_rlcsa_64.txt 2>> Wb_log_rlcsa_64.txt 
sleep 1 && touch ./indexes/wiki2gb.txt.rlcsa.array
./rlcsa_locate ./indexes/wiki2gb.txt ./indexes/wiki2gb.txt.DOCBOUNDARIES 128  < ../data/patterns/wiki2g.words.f1001_100k >> Wb_log_rlcsa_128.txt 2>> Wb_log_rlcsa_128.txt 
sleep 1 && touch ./indexes/wiki2gb.txt.rlcsa.array
./rlcsa_locate ./indexes/wiki2gb.txt ./indexes/wiki2gb.txt.DOCBOUNDARIES 256  < ../data/patterns/wiki2g.words.f1001_100k >> Wb_log_rlcsa_256.txt 2>> Wb_log_rlcsa_256.txt 
sleep 1 && touch ./indexes/wiki2gb.txt.rlcsa.array
done
echo ""

for i in `seq 1 2`;
do
printf 'run %s of 2 [samples 512,1024,2048]\r' "$i"
./rlcsa_locate ./indexes/wiki2gb.txt ./indexes/wiki2gb.txt.DOCBOUNDARIES 512  < ../data/patterns/wiki2g.words.f1001_100k >> Wb_log_rlcsa_512.txt 2>> Wb_log_rlcsa_512.txt 
sleep 1 && touch ./indexes/wiki2gb.txt.rlcsa.array
./rlcsa_locate ./indexes/wiki2gb.txt ./indexes/wiki2gb.txt.DOCBOUNDARIES 1024  < ../data/patterns/wiki2g.words.f1001_100k >> Wb_log_rlcsa_1024.txt 2>> Wb_log_rlcsa_1024.txt 
sleep 1 && touch ./indexes/wiki2gb.txt.rlcsa.array
./rlcsa_locate ./indexes/wiki2gb.txt ./indexes/wiki2gb.txt.DOCBOUNDARIES 2048  < ../data/patterns/wiki2g.words.f1001_100k >> Wb_log_rlcsa_2048.txt 2>> Wb_log_rlcsa_2048.txt 
sleep 1 && touch ./indexes/wiki2gb.txt.rlcsa.array
done
echo ""

echo "#################################" >> Wb_log_rlcsa_32.txt 
echo "#################################" >> Wb_log_rlcsa_64.txt 
echo "#################################" >> Wb_log_rlcsa_128.txt 
echo "#################################" >> Wb_log_rlcsa_256.txt 
echo "#################################" >> Wb_log_rlcsa_512.txt
echo "#################################" >> Wb_log_rlcsa_1024.txt 
echo "#################################" >> Wb_log_rlcsa_2048.txt  




echo "#### LOCATE: P2: ../data/patterns/wiki2g_2_2 ######" 

for i in `seq 1 10`;
do
printf 'run %s of 10 [samples 32,64,128,256]\r' "$i"
./rlcsa_locate ./indexes/wiki2gb.txt ./indexes/wiki2gb.txt.DOCBOUNDARIES  32  < ../data/patterns/wiki2g_2_2 >> P2_log_rlcsa_32.txt 2>> P2_log_rlcsa_32.txt 
sleep 1 && touch ./indexes/wiki2gb.txt.rlcsa.array
./rlcsa_locate ./indexes/wiki2gb.txt ./indexes/wiki2gb.txt.DOCBOUNDARIES  64  < ../data/patterns/wiki2g_2_2 >> P2_log_rlcsa_64.txt 2>> P2_log_rlcsa_64.txt 
sleep 1 && touch ./indexes/wiki2gb.txt.rlcsa.array
./rlcsa_locate ./indexes/wiki2gb.txt ./indexes/wiki2gb.txt.DOCBOUNDARIES 128  < ../data/patterns/wiki2g_2_2 >> P2_log_rlcsa_128.txt 2>> P2_log_rlcsa_128.txt 
sleep 1 && touch ./indexes/wiki2gb.txt.rlcsa.array
./rlcsa_locate ./indexes/wiki2gb.txt ./indexes/wiki2gb.txt.DOCBOUNDARIES 256  < ../data/patterns/wiki2g_2_2 >> P2_log_rlcsa_256.txt 2>> P2_log_rlcsa_256.txt 
sleep 1 && touch ./indexes/wiki2gb.txt.rlcsa.array
done
echo ""

for i in `seq 1 2`;
do
printf 'run %s of 2 [samples 512,1024,2048]\r' "$i"
./rlcsa_locate ./indexes/wiki2gb.txt ./indexes/wiki2gb.txt.DOCBOUNDARIES 512  < ../data/patterns/wiki2g_2_2 >> P2_log_rlcsa_512.txt 2>> P2_log_rlcsa_512.txt 
sleep 1 && touch ./indexes/wiki2gb.txt.rlcsa.array
./rlcsa_locate ./indexes/wiki2gb.txt ./indexes/wiki2gb.txt.DOCBOUNDARIES 1024  < ../data/patterns/wiki2g_2_2 >> P2_log_rlcsa_1024.txt 2>> P2_log_rlcsa_1024.txt 
sleep 1 && touch ./indexes/wiki2gb.txt.rlcsa.array
./rlcsa_locate ./indexes/wiki2gb.txt ./indexes/wiki2gb.txt.DOCBOUNDARIES 2048  < ../data/patterns/wiki2g_2_2 >> P2_log_rlcsa_2048.txt 2>> P2_log_rlcsa_2048.txt 
sleep 1 && touch ./indexes/wiki2gb.txt.rlcsa.array
done
echo ""

echo "#################################" >> P2_log_rlcsa_32.txt 
echo "#################################" >> P2_log_rlcsa_64.txt 
echo "#################################" >> P2_log_rlcsa_128.txt 
echo "#################################" >> P2_log_rlcsa_256.txt 
echo "#################################" >> P2_log_rlcsa_512.txt 
echo "#################################" >> P2_log_rlcsa_1024.txt 
echo "#################################" >> P2_log_rlcsa_2048.txt 



echo "#### LOCATE: P5: ../data/patterns/wiki2g_5_5 ######" 

for i in `seq 1 10`;
do
printf 'run %s of 10 [samples 32,64,128,256]\r' "$i"
./rlcsa_locate ./indexes/wiki2gb.txt ./indexes/wiki2gb.txt.DOCBOUNDARIES  32  < ../data/patterns/wiki2g_5_5 >> P5_log_rlcsa_32.txt 2>> P5_log_rlcsa_32.txt 
sleep 1 && touch ./indexes/wiki2gb.txt.rlcsa.array
./rlcsa_locate ./indexes/wiki2gb.txt ./indexes/wiki2gb.txt.DOCBOUNDARIES  64  < ../data/patterns/wiki2g_5_5 >> P5_log_rlcsa_64.txt 2>> P5_log_rlcsa_64.txt 
sleep 1 && touch ./indexes/wiki2gb.txt.rlcsa.array
./rlcsa_locate ./indexes/wiki2gb.txt ./indexes/wiki2gb.txt.DOCBOUNDARIES 128  < ../data/patterns/wiki2g_5_5 >> P5_log_rlcsa_128.txt 2>> P5_log_rlcsa_128.txt 
sleep 1 && touch ./indexes/wiki2gb.txt.rlcsa.array
./rlcsa_locate ./indexes/wiki2gb.txt ./indexes/wiki2gb.txt.DOCBOUNDARIES 256  < ../data/patterns/wiki2g_5_5 >> P5_log_rlcsa_256.txt 2>> P5_log_rlcsa_256.txt 
sleep 1 && touch ./indexes/wiki2gb.txt.rlcsa.array
done
echo ""

for i in `seq 1 2`;
do
printf 'run %s of 2 [samples 512,1024,2048]\r' "$i"
./rlcsa_locate ./indexes/wiki2gb.txt ./indexes/wiki2gb.txt.DOCBOUNDARIES 512  < ../data/patterns/wiki2g_5_5 >> P5_log_rlcsa_512.txt 2>> P5_log_rlcsa_512.txt 
sleep 1 && touch ./indexes/wiki2gb.txt.rlcsa.array
./rlcsa_locate ./indexes/wiki2gb.txt ./indexes/wiki2gb.txt.DOCBOUNDARIES 1024  < ../data/patterns/wiki2g_5_5 >> P5_log_rlcsa_1024.txt 2>> P5_log_rlcsa_1024.txt 
sleep 1 && touch ./indexes/wiki2gb.txt.rlcsa.array
./rlcsa_locate ./indexes/wiki2gb.txt ./indexes/wiki2gb.txt.DOCBOUNDARIES 2048  < ../data/patterns/wiki2g_5_5 >> P5_log_rlcsa_2048.txt 2>> P5_log_rlcsa_2048.txt 
sleep 1 && touch ./indexes/wiki2gb.txt.rlcsa.array
done
echo ""

echo "#### LOCATE: TESTS COMPLETED: see log-files: ??_log_rlcsa* #####" 
