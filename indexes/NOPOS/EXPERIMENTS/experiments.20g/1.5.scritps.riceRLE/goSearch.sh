
echo "################################################################" 
rm -f E.dat
rm -f N.dat

./SEARCH indexes/wiki20gb E 200 "int2=merge;intn=merge" < ../../patterns/wiki20g.words.f1_1000
mv E.dat E_Wa.1.5.riceRLE.dat

./SEARCH indexes/wiki20gb E 200 "int2=merge;intn=merge" < ../../patterns/wiki20g.words.f1001_100k
mv E.dat E_Wb.1.5.riceRLE.dat

./SEARCH indexes/wiki20gb N   2 "int2=merge;intn=merge" < ../../patterns/wiki20g_2_2
mv N.dat N_p2.1.5.riceRLE.dat

./SEARCH indexes/wiki20gb N   2 "int2=merge;intn=merge" < ../../patterns/wiki20g_5_5
mv N.dat N_p5.1.5.riceRLE.dat



#echo "################################################################" 
##./SEARCH indexes/torsen200mb I 1  "int2=merge;intn=merge" < ../patterns/torsen200mb_1_1
#
#./SEARCH indexes/torsen2000mb E 1 "int2=merge;intn=merge" < ../patterns/torsen2000mb_1_1_10k
#./SEARCH indexes/torsen2000mb I 1 "int2=merge;intn=merge" < ../patterns/torsen2000mb_2_2_10k
#./SEARCH indexes/torsen2000mb N 1 "int2=merge;intn=merge" < ../patterns/torsen2000mb_2_5_10k
#
#echo "################################################################"
#
#cp E.dat E.1.5.riceRLE.dat
#cp I.dat I.1.5.riceRLE.dat
#cp N.dat N.1.5.riceRLE.dat


