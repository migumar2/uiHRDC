rm -f E.dat
rm -f N.dat

./SEARCH indexes/wiki2gb_bitmap1_k16  E 200 "int2=lookup;intn=lookup" < ../../patterns/wiki2g.words.f1_1000
./SEARCH indexes/wiki2gb_bitmap1_k64  E 200 "int2=lookup;intn=lookup" < ../../patterns/wiki2g.words.f1_1000
./SEARCH indexes/wiki2gb_bitmap1_k128 E 200 "int2=lookup;intn=lookup" < ../../patterns/wiki2g.words.f1_1000
mv E.dat E_Wa.vbyte.sanders.dat

./SEARCH indexes/wiki2gb_bitmap1_k16  E 100 "int2=lookup;intn=lookup" < ../../patterns/wiki2g.words.f1001_100k
./SEARCH indexes/wiki2gb_bitmap1_k64  E 100 "int2=lookup;intn=lookup" < ../../patterns/wiki2g.words.f1001_100k
./SEARCH indexes/wiki2gb_bitmap1_k128 E 100 "int2=lookup;intn=lookup" < ../../patterns/wiki2g.words.f1001_100k
mv E.dat E_Wb.vbyte.sanders.dat

./SEARCH indexes/wiki2gb_bitmap1_k16  N 10 "int2=lookup;intn=lookup" < ../../patterns/wiki2g_2_2
./SEARCH indexes/wiki2gb_bitmap1_k64  N 10 "int2=lookup;intn=lookup" < ../../patterns/wiki2g_2_2
./SEARCH indexes/wiki2gb_bitmap1_k128 N 10 "int2=lookup;intn=lookup" < ../../patterns/wiki2g_2_2
mv N.dat N_p2.vbyte.sanders.dat

./SEARCH indexes/wiki2gb_bitmap1_k16  N 10 "int2=lookup;intn=lookup" < ../../patterns/wiki2g_5_5
./SEARCH indexes/wiki2gb_bitmap1_k64  N 10 "int2=lookup;intn=lookup" <  wiki2g_5_5_990patrones
./SEARCH indexes/wiki2gb_bitmap1_k128 N 10 "int2=lookup;intn=lookup" < wiki2g_5_5_990patrones
mv N.dat N_p5.vbyte.sanders.dat



