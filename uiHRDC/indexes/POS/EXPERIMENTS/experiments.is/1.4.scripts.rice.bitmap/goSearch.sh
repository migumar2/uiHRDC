rm -f E.dat
rm -f N.dat

echo "################################################################" 

#./SEARCH indexes/wiki2gb_bitmap1 E 100 "int2=merge;intn=svs;fsearch=seqM" < ../../patterns/einstein.words
#./SEARCH indexes/wiki2gb_bitmap1 N 100 "int2=merge;intn=svs;fsearch=seqM" < ../../patterns/einstein_2_5

./SEARCH indexes/wiki2gb_bitmap1 E 200 "int2=merge;intn=svs;fsearch=seqM" < ../../patterns/wiki2g.words.f1_1000
mv E.dat E_Wa.1.4.rice.dat

./SEARCH indexes/wiki2gb_bitmap1 E 200 "int2=merge;intn=svs;fsearch=seqM" < ../../patterns/wiki2g.words.f1001_100k
mv E.dat E_Wb.1.4.rice.dat

./SEARCH indexes/wiki2gb_bitmap1 N 2 "int2=merge;intn=svs;fsearch=seqM" < ../../patterns/wiki2g_2_2
mv N.dat N_p2.1.4.rice.dat

./SEARCH indexes/wiki2gb_bitmap1 N 2 "int2=merge;intn=svs;fsearch=seqM" < ../../patterns/wiki2g_5_5
mv N.dat N_p5.1.4.rice.dat

echo "################################################################"

#cp E.dat E.1.4.rice.dat
#cp N.dat N.1.4.rice.dat


