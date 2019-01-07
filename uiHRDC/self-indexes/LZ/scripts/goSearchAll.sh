#! /bin/bash

echo " \n #############################"
echo " ### Evaluating LZ77 index ###"
echo " #############################\n"
cd ..

rm -f evaluation/results/Slz77-LOG.dat*
python ../../utils-py/starttime.py  evaluation/results/Slz77-LOG.dat "Search-starts!"   ##LOGS-ELAPSED-TIMES

echo " --- Locating low frequency words..."
echo "Index size;Doc index size;Occs:#1;#2;#3;#4;#5;#6;#7;#8;#9;#10;;Avg.Time" >> evaluation/results/lz77.f1_1000 
./src/bin/locate evaluation/indexes/index.lz77 evaluation/text.DOCBOUNDARIES < evaluation/patterns/wiki2g.words.f1_1000 2>> evaluation/results/lz77.f1_1000 

echo " --- Locating high frequency words..."
echo "Index size;Doc index size;Occs:#1;#2;#3;#4;#5;#6;#7;#8;#9;#10;;Avg.Time" >> evaluation/results/lz77.f1001_100k
./src/bin/locate evaluation/indexes/index.lz77 evaluation/text.DOCBOUNDARIES < evaluation/patterns/wiki2g.words.f1001_100k 2>> evaluation/results/lz77.f1001_100k

echo " --- Locating 2-word phrases..."
echo "Index size;Doc index size;Occs:#1;#2;#3;#4;#5;#6;#7;#8;#9;#10;;Avg.Time" >> evaluation/results/lz77.2_2
./src/bin/locate evaluation/indexes/index.lz77 evaluation/text.DOCBOUNDARIES < evaluation/patterns/wiki2g_2_2 2>> evaluation/results/lz77.2_2

echo " --- Locating 5-word phrases..."
echo "Index size;Doc index size;Occs:#1;#2;#3;#4;#5;#6;#7;#8;#9;#10;;Avg.Time" >> evaluation/results/lz77.5_5
./src/bin/locate evaluation/indexes/index.lz77 evaluation/text.DOCBOUNDARIES < evaluation/patterns/wiki2g_5_5 2>> evaluation/results/lz77.5_5

echo " --- Extracting small snippets..."
echo "Index size;Extracted chars;#1;#2;#3;#4;#5;#6;#7;#8;#9;#10;;Avg.Time" >> evaluation/results/lz77.snippets80
./src/bin/extract evaluation/indexes/index.lz77 1000 < evaluation/intervals/wiki2g.intervals.80chars.txt 2>> evaluation/results/lz77.snippets80

echo " --- Extracting large snippets..."
echo "Index size;Extracted chars;#1;#2;#3;#4;#5;#6;#7;#8;#9;#10;;Avg.Time" >> evaluation/results/lz77.snippets13000
./src/bin/extract evaluation/indexes/index.lz77 1000 < evaluation/intervals/wiki2g.intervals.13000chars.txt 2>> evaluation/results/lz77.snippets13000

python ../../utils-py/endtime.py    evaluation/results/Slz77-LOG.dat                  ##LOGS-ELAPSED-TIMES


echo " \n ##############################"
echo " ### Evaluating LZend index ###"
echo " ##############################\n"

rm -f evaluation/results/Slzend-LOG.dat*
python ../../utils-py/starttime.py  evaluation/results/Slzend-LOG.dat "Search-starts!"   ##LOGS-ELAPSED-TIMES

echo " --- Locating low frequency words..."
echo "Index size;Doc index size;#1;#2;#3;#4;#5;#6;#7;#8;#9;#10;;Avg.Time" >> evaluation/results/lzend.f1_1000 
./src/bin/locate evaluation/indexes/index.lzend evaluation/text.DOCBOUNDARIES < evaluation/patterns/wiki2g.words.f1_1000 2>> evaluation/results/lzend.f1_1000 

echo " --- Locating high frequency words..."
echo "Index size;Doc index size;#1;#2;#3;#4;#5;#6;#7;#8;#9;#10;;Avg.Time" >> evaluation/results/lzend.f1001_100k
./src/bin/locate evaluation/indexes/index.lzend evaluation/text.DOCBOUNDARIES < evaluation/patterns/wiki2g.words.f1001_100k 2>> evaluation/results/lzend.f1001_100k

echo " --- Locating 2-word phrases..."
echo "Index size;Doc index size;#1;#2;#3;#4;#5;#6;#7;#8;#9;#10;;Avg.Time" >> evaluation/results/lzend.2_2
./src/bin/locate evaluation/indexes/index.lzend evaluation/text.DOCBOUNDARIES < evaluation/patterns/wiki2g_2_2 2>> evaluation/results/lzend.2_2

echo " --- Locating 5-word phrases..."
echo "Index size;Doc index size;#1;#2;#3;#4;#5;#6;#7;#8;#9;#10;;Avg.Time" >> evaluation/results/lzend.5_5
./src/bin/locate evaluation/indexes/index.lzend evaluation/text.DOCBOUNDARIES < evaluation/patterns/wiki2g_5_5 2>> evaluation/results/lzend.5_5

echo " --- Extracting small snippets..."
echo "Index size;Extracted chars;#1;#2;#3;#4;#5;#6;#7;#8;#9;#10;;Avg.Time" >> evaluation/results/lzend.snippets80
./src/bin/extract evaluation/indexes/index.lzend 1000 < evaluation/intervals/wiki2g.intervals.80chars.txt 2>> evaluation/results/lzend.snippets80

echo " --- Extracting large snippets..."
echo "Index size;Extracted chars;#1;#2;#3;#4;#5;#6;#7;#8;#9;#10;;Avg.Time" >> evaluation/results/lzend.snippets13000
./src/bin/extract evaluation/indexes/index.lzend 1000 < evaluation/intervals/wiki2g.intervals.13000chars.txt 2>> evaluation/results/lzend.snippets13000

python ../../utils-py/endtime.py    evaluation/results/Slzend-LOG.dat                  ##LOGS-ELAPSED-TIMES


cd ./scripts

