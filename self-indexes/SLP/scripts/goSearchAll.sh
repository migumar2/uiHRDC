#! /bin/bash

echo " \n ###########################"
echo " ### Evaluating SLP index ###"
echo " ###########################\n"
cd ../slp

echo " --- Locating low frequency words..."
echo "Index size;Doc index size;Occs;#1;#2;#3;#4;#5;#6;#7;#8;#9;#10;;Avg.Time" >> ../evaluation/results/slp.f1_1000 
./SLPIndex -l ../evaluation/indexes/index.slp ../evaluation/text.DOCBOUNDARIES 1000 < ../evaluation/patterns/wiki2g.words.f1_1000 2>> ../evaluation/results/slp.f1_1000 

echo " --- Locating high frequency words..."
echo "Index size;Doc index size;Occs;#1;#2;#3;#4;#5;#6;#7;#8;#9;#10;;Avg.Time" >> ../evaluation/results/slp.f1001_100k
./SLPIndex -l ../evaluation/indexes/index.slp ../evaluation/text.DOCBOUNDARIES 1000 < ../evaluation/patterns/wiki2g.words.f1001_100k 2>> ../evaluation/results/slp.f1001_100k

echo " --- Locating 2-word phrases..."
echo "Index size;Doc index size;Occs;#1;#2;#3;#4;#5;#6;#7;#8;#9;#10;;Avg.Time" >> ../evaluation/results/slp.2_2
./SLPIndex -l ../evaluation/indexes/index.slp ../evaluation/text.DOCBOUNDARIES 1000 < ../evaluation/patterns/wiki2g_2_2 2>> ../evaluation/results/slp.2_2

echo " --- Locating 5-word phrases..."
echo "Index size;Doc index size;Occs;#1;#2;#3;#4;#5;#6;#7;#8;#9;#10;;Avg.Time" >> ../evaluation/results/slp.5_5
./SLPIndex -l ../evaluation/indexes/index.slp ../evaluation/text.DOCBOUNDARIES 1000 < ../evaluation/patterns/wiki2g_5_5 2>> ../evaluation/results/slp.5_5

echo " --- Extracting small snippets..."
echo "Index size;Extracted chars;#1;#2;#3;#4;#5;#6;#7;#8;#9;#10;;Avg.Time" >> ../evaluation/results/slp.snippets80
./SLPIndex -e ../evaluation/indexes/index.slp 1000 < ../evaluation/intervals/wiki2g.intervals.80chars.txt 2>> ../evaluation/results/slp.snippets80

echo " --- Extracting large snippets..."
echo "Index size;Extracted chars;#1;#2;#3;#4;#5;#6;#7;#8;#9;#10;;Avg.Time" >> ../evaluation/results/slp.snippets13000
./SLPIndex -e ../evaluation/indexes/index.slp 1000 < ../evaluation/intervals/wiki2g.intervals.13000chars.txt 2>> ../evaluation/results/slp.snippets13000

echo " \n ###########################"
echo " ### Evaluating wSLP index ###"
echo " ###########################\n"
cd ../wslp

echo " --- Locating low frequency words..."
echo "Index size;Occs;#1;#2;#3;#4;#5;#6;#7;#8;#9;#10;;Avg.Time" >> ../evaluation/results/wslp.f1_1000 
./wSLPIndex -l ../evaluation/indexes/index.wslp 1000 < ../evaluation/patterns/wiki2g.words.f1_1000 2>> ../evaluation/results/wslp.f1_1000 

echo " --- Locating high frequency words..."
echo "Index size;Occs;#1;#2;#3;#4;#5;#6;#7;#8;#9;#10;;Avg.Time" >> ../evaluation/results/wslp.f1001_100k
./wSLPIndex -l ../evaluation/indexes/index.wslp 1000 < ../evaluation/patterns/wiki2g.words.f1001_100k 2>> ../evaluation/results/wslp.f1001_100k

echo " --- Locating 2-word phrases..."
echo "Index size;Occs;#1;#2;#3;#4;#5;#6;#7;#8;#9;#10;;Avg.Time" >> ../evaluation/results/wslp.2_2
./wSLPIndex -l ../evaluation/indexes/index.wslp 1000 < ../evaluation/patterns/wiki2g_2_2 2>> ../evaluation/results/wslp.2_2

echo " --- Locating 5-word phrases..."
echo "Index size;Occs;#1;#2;#3;#4;#5;#6;#7;#8;#9;#10;;Avg.Time" >> ../evaluation/results/wslp.5_5
./wSLPIndex -l ../evaluation/indexes/index.wslp 1000 < ../evaluation/patterns/wiki2g_5_5 2>> ../evaluation/results/wslp.5_5

echo " --- Extracting small snippets..."
echo "Index size;Extracted chars;#1;#2;#3;#4;#5;#6;#7;#8;#9;#10;;Avg.Time" >> ../evaluation/results/wslp.snippets80
./wSLPIndex -e ../evaluation/indexes/index.wslp 1000 < ../evaluation/intervals/wiki2g.intervals.80chars.txt 2>> ../evaluation/results/wslp.snippets80

echo " --- Extracting large snippets..."
echo "Index size;Extracted chars;#1;#2;#3;#4;#5;#6;#7;#8;#9;#10;;Avg.Time" >> ../evaluation/results/wslp.snippets13000
./wSLPIndex -e ../evaluation/indexes/index.wslp 1000 < ../evaluation/intervals/wiki2g.intervals.13000chars.txt 2>> ../evaluation/results/wslp.snippets13000

