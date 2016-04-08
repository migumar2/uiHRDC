rm -f E.dat
rm -f N.dat
echo "################################################################" 

#./SEARCH indexes/wiki2gb E 100 "int2=merge;intn=merge" < ../../patterns/einstein.words
#./SEARCH indexes/wiki2gb N 100 "int2=merge;intn=merge" < ../../patterns/einstein_2_5

./SEARCH indexes/wiki20gb E  30 "int2=merge;intn=merge" < ../../patterns/wiki20g.words.f1_1000
mv E.dat E_Wa.6.vbyte.lzma.dat

./SEARCH indexes/wiki20gb E  30 "int2=merge;intn=merge" < ../../patterns/wiki20g.words.f1001_100k
mv E.dat E_Wb.6.vbyte.lzma.dat

./SEARCH indexes/wiki20gb N   2 "int2=merge;intn=merge" < ../../patterns/wiki20g_2_2
mv N.dat N_p2.6.vbyte.lzma.dat

./SEARCH indexes/wiki20gb N   2 "int2=merge;intn=merge" < ../../patterns/wiki20g_5_5
mv N.dat N_p5.6.vbyte.lzma.dat

echo "################################################################"

#cp E.dat E.6.vbyte.lzma.dat
#cp N.dat N.6.vbyte.lzma.dat


