
echo "################################################################" 
#./SEARCH indexes/torsen200mb I 1  "int2=merge;intn=merge" < ../patterns/torsen200mb_1_1

./SEARCH indexes/torsen20gb_ds04 E 10 "int2=merge;intn=merge" < ../../patterns/wiki20g.words.f1_1000
./SEARCH indexes/torsen20gb_ds16 E 10 "int2=merge;intn=merge" < ../../patterns/wiki20g.words.f1_1000
./SEARCH indexes/torsen20gb_ds64 E 10 "int2=merge;intn=merge" < ../../patterns/wiki20g.words.f1_1000
./SEARCH indexes/torsen20gb_ds256 E 10 "int2=merge;intn=merge" < ../../patterns/wiki20g.words.f1_1000
mv E.dat E_Wa.5.lzend.dat

./SEARCH indexes/torsen20gb_ds04 E 10 "int2=merge;intn=merge" < ../../patterns/wiki20g.words.f1001_100k
./SEARCH indexes/torsen20gb_ds16 E 10 "int2=merge;intn=merge" < ../../patterns/wiki20g.words.f1001_100k
./SEARCH indexes/torsen20gb_ds64 E 10 "int2=merge;intn=merge" < ../../patterns/wiki20g.words.f1001_100k
./SEARCH indexes/torsen20gb_ds256 E 10 "int2=merge;intn=merge" < ../../patterns/wiki20g.words.f1001_100k
mv E.dat E_Wb.5.lzend.dat

./SEARCH indexes/torsen20gb_ds04 N 4 "int2=merge;intn=merge" < ../../patterns/wiki20g_2_2
./SEARCH indexes/torsen20gb_ds16 N 4 "int2=merge;intn=merge" < ../../patterns/wiki20g_2_2
./SEARCH indexes/torsen20gb_ds64 N 2 "int2=merge;intn=merge" < ../../patterns/wiki20g_2_2
./SEARCH indexes/torsen20gb_ds256 N 2 "int2=merge;intn=merge" < ../../patterns/wiki20g_2_2
mv N.dat N_p2.5.lzend.dat

./SEARCH indexes/torsen20gb_ds04 N 4 "int2=merge;intn=merge" < ../../patterns/wiki20g_5_5
./SEARCH indexes/torsen20gb_ds16 N 4 "int2=merge;intn=merge" < ../../patterns/wiki20g_5_5
./SEARCH indexes/torsen20gb_ds64 N 2 "int2=merge;intn=merge" < ../../patterns/wiki20g_5_5
./SEARCH indexes/torsen20gb_ds256 N 2 "int2=merge;intn=merge" < ../../patterns/wiki20g_5_5
mv N.dat N_p5.5.lzend.dat

echo "################################################################"



