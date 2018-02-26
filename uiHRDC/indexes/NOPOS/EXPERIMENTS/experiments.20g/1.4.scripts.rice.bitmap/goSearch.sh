rm -f E.dat
rm -f N.dat

echo "################################################################"

#./SEARCH indexes/wiki20gb_bitmap1 E 100 "int2=merge;intn=svs;fsearch=seqM" < ../../patterns/einstein.words
#./SEARCH indexes/wiki20gb_bitmap1 N 100 "int2=merge;intn=svs;fsearch=seqM" < ../../patterns/einstein_2_5

./SEARCH indexes/wiki20gb_bitmap1 E 200 "int2=merge;intn=svs;fsearch=seqM" < ../../patterns/wiki20g.words.f1_1000
mv E.dat E_Wa.1.4.rice.dat

./SEARCH indexes/wiki20gb_bitmap1 E 200 "int2=merge;intn=svs;fsearch=seqM" < ../../patterns/wiki20g.words.f1001_100k
mv E.dat E_Wb.1.4.rice.dat

./SEARCH indexes/wiki20gb_bitmap1 N 2 "int2=merge;intn=svs;fsearch=seqM" < ../../patterns/wiki20g_2_2
mv N.dat N_p2.1.4.rice.dat

./SEARCH indexes/wiki20gb_bitmap1 N 2 "int2=merge;intn=svs;fsearch=seqM" < ../../patterns/wiki20g_5_5
mv N.dat N_p5.1.4.rice.dat

echo "################################################################"

#cp E.dat E.1.4.rice.dat
#cp N.dat N.1.4.rice.dat

rm -f E.dat
rm -f N.dat

echo "################################################################"


./SEARCH indexes/wiki20gb_bitmap8 E 200 "int2=merge;intn=svs;fsearch=seqM" < ../../patterns/wiki20g.words.f1_1000
mv E.dat E_Wa.1.4.rice.bitmap8.dat

./SEARCH indexes/wiki20gb_bitmap8 E 200 "int2=merge;intn=svs;fsearch=seqM" < ../../patterns/wiki20g.words.f1001_100k
mv E.dat E_Wb.1.4.rice.bitmap8.dat

./SEARCH indexes/wiki20gb_bitmap8 N 2 "int2=merge;intn=svs;fsearch=seqM" < ../../patterns/wiki20g_2_2
mv N.dat N_p2.1.4.rice.bitmap8.dat

./SEARCH indexes/wiki20gb_bitmap8 N 2 "int2=merge;intn=svs;fsearch=seqM" < ../../patterns/wiki20g_5_5
mv N.dat N_p5.1.4.rice.bitmap8.dat

echo "################################################################"


