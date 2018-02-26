rm -f E.dat
rm -f N.dat
echo "################################################################" 
#./SEARCH indexes/wiki20gb_bitmap1 E 100 "int2=mergeS9;intn=mergeS9" < ../../patterns/einstein.words
#./SEARCH indexes/wiki20gb_bitmap1 N 100 "int2=mergeS9;intn=mergeS9" < ../../patterns/einstein_2_5

./SEARCH indexes/wiki20gb E 200 "int2=mergeS9;intn=mergeS9" < ../../patterns/wiki20g.words.f1_1000
mv E.dat E_Wa.7.simple9.dat

./SEARCH indexes/wiki20gb E 200 "int2=mergeS9;intn=mergeS9" < ../../patterns/wiki20g.words.f1001_100k
mv E.dat E_Wb.7.simple9.dat

./SEARCH indexes/wiki20gb N 10 "int2=mergeS9;intn=mergeS9" < ../../patterns/wiki20g_2_2
mv N.dat N_p2.7.simple9.dat

./SEARCH indexes/wiki20gb N 10 "int2=mergeS9;intn=mergeS9" < ../../patterns/wiki20g_5_5
mv N.dat N_p5.7.simple9.dat

echo "################################################################"


#echo "################################################################" 
#./SEARCH indexes/torsen200mb I 1  "int2=merge;intn=merge" < ../patterns/torsen200mb_1_1
#./SEARCH indexes/torsen2000mb E 1 "int2=mergeS9;intn=mergeS9" < ../patterns/torsen2000mb_1_1_10k
#./SEARCH indexes/torsen2000mb I 1 "int2=mergeS9;intn=mergeS9" < ../patterns/torsen2000mb_2_2_10k
#./SEARCH indexes/torsen2000mb N 1 "int2=mergeS9;intn=mergeS9" < ../patterns/torsen2000mb_2_5_10k
#echo "################################################################"
#cp E.dat E.7.simple9.dat
#cp I.dat I.7.simple9.dat
#cp N.dat N.7.simple9.dat
