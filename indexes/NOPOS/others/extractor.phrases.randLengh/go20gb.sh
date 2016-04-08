#Use:  ./EXTRACTPHRASES  <in file>  <out file> <numberOfPhrases> <min phrase len>< max phrase Lenght> <1/0 onlywords ?> [<stopwords file>]
#                  text                 outfile      n   min max onlyWords  stopwords_file

#./EXTRACTPHRASES ../texts/text20gb.txt  ../wiki20g_1_1 1000 1 1 0 src/english_stopwords.txt
./EXTRACTPHRASES ../texts/text20gb.txt  ../wiki20g_2_2 1000 2 2 0 src/english_stopwords.txt
./EXTRACTPHRASES ../texts/text20gb.txt  ../wiki20g_5_5 1000 5 5 0 src/english_stopwords.txt



