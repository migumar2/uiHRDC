rm -rf intervals/ patterns/ text2gb/

mkdir -p intervals
mkdir -p text2gb
mkdir -p patterns

ln -s ../../../../data/intervals/einstein.intervals.13000chars.txt  intervals/wiki2g.intervals.13000chars.txt 
ln -s ../../../../data/intervals/einstein.intervals.20words.txt     intervals/wiki2g.intervals.20words.txt
ln -s ../../../../data/intervals/einstein.intervals.3000words.txt   intervals/wiki2g.intervals.3000words.txt
ln -s ../../../../data/intervals/einstein.intervals.80chars.txt     intervals/wiki2g.intervals.80chars.txt

ln -s ../../../../data/patterns/einstein_2_2                        patterns/wiki2g_2_2
ln -s ../../../../data/patterns/einstein_2_5                        patterns/wiki2g_5_5
ln -s ../../../../data/patterns/einstein.words_f101_50k             patterns/wiki2g.words.f1001_100k
ln -s ../../../../data/patterns/einstein.words_f1_100               patterns/wiki2g.words.f1_1000

ln -s ../../../../data/texts/einstein.en.txt                        text2gb/wiki_src2gb.txt
ln -s ../../../../data/texts/einstein.en.txt.DOCBOUNDARIES          text2gb/wiki_src2gb.txt.DOCBOUNDARIES
ln -s ../../../../data/texts/einstein.en.txt.len                    text2gb/wiki_src2gb.txt.lens



