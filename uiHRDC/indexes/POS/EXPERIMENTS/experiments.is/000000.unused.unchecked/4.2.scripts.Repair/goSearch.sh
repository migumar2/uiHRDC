rm -f E.dat
rm -f N.dat

echo "################################################################" 

#./SEARCH indexes/wiki2gb E 20 "int2=merge;intn=merge" < ../../patterns/einstein.words
#./SEARCH indexes/wiki2gb N 20 "int2=merge;intn=merge" < ../../patterns/einstein_2_5

./SEARCH indexes/wiki2gb E 20 "int2=merge;intn=merge" < ../../patterns/wiki2g.words.f1_1000
mv E.dat E_Wa.4.2.repair.dat

./SEARCH indexes/wiki2gb E 20 "int2=merge;intn=merge" < ../../patterns/wiki2g.words.f1001_100k
mv E.dat E_Wb.4.2.repair.dat

./SEARCH indexes/wiki2gb N  5 "int2=merge;intn=merge" < ../../patterns/wiki2g_2_2
mv N.dat N_p2.4.2.repair.dat

./SEARCH indexes/wiki2gb N  5 "int2=merge;intn=merge" < ../../patterns/wiki2g_5_5
mv N.dat N_p5.4.2.repair.dat

echo "################################################################"

#cp E.dat E.4.2.repair.dat
#cp N.dat N.4.2.repair.dat



