
echo "################################################################" 
rm -f E.dat
rm -f N.dat

./SEARCH indexes/wiki20gb_bitmap1_k4  E 400 "int2=svs;intn=svs;fsearch=expM" < ../../patterns/wiki20g.words.f1_1000
./SEARCH indexes/wiki20gb_bitmap1_k32 E 400 "int2=svs;intn=svs;fsearch=expM" < ../../patterns/wiki20g.words.f1_1000
mv E.dat E_Wa.2.vbyte.moffat.dat

./SEARCH indexes/wiki20gb_bitmap1_k4  E 400 "int2=svs;intn=svs;fsearch=expM" < ../../patterns/wiki20g.words.f1001_100k
./SEARCH indexes/wiki20gb_bitmap1_k32 E 400 "int2=svs;intn=svs;fsearch=expM" < ../../patterns/wiki20g.words.f1001_100k
mv E.dat E_Wb.2.vbyte.moffat.dat

./SEARCH indexes/wiki20gb_bitmap1_k4  N 20 "int2=svs;intn=svs;fsearch=expM" < ../../patterns/wiki20g_2_2
./SEARCH indexes/wiki20gb_bitmap1_k32 N 20 "int2=svs;intn=svs;fsearch=expM" < ../../patterns/wiki20g_2_2
mv N.dat N_p2.2.vbyte.moffat.dat

./SEARCH indexes/wiki20gb_bitmap1_k4  N 20 "int2=svs;intn=svs;fsearch=expM" < ../../patterns/wiki20g_5_5
./SEARCH indexes/wiki20gb_bitmap1_k32 N 20 "int2=svs;intn=svs;fsearch=expM" < ../../patterns/wiki20g_5_5
mv N.dat N_p5.2.vbyte.moffat.dat
echo "################################################################" 


echo "################################################################" 
rm -f E.dat
rm -f N.dat

./SEARCH indexes/wiki20gb_bitmap8_k4  E 400 "int2=svs;intn=svs;fsearch=expM" < ../../patterns/wiki20g.words.f1_1000
./SEARCH indexes/wiki20gb_bitmap8_k32 E 400 "int2=svs;intn=svs;fsearch=expM" < ../../patterns/wiki20g.words.f1_1000
mv E.dat E_Wa.2.vbyte.moffat.bitmap8.dat

./SEARCH indexes/wiki20gb_bitmap8_k4  E 400 "int2=svs;intn=svs;fsearch=expM" < ../../patterns/wiki20g.words.f1001_100k
./SEARCH indexes/wiki20gb_bitmap8_k32 E 400 "int2=svs;intn=svs;fsearch=expM" < ../../patterns/wiki20g.words.f1001_100k
mv E.dat E_Wb.2.vbyte.moffat.bitmap8.dat

./SEARCH indexes/wiki20gb_bitmap8_k4  N 20 "int2=svs;intn=svs;fsearch=expM" < ../../patterns/wiki20g_2_2
./SEARCH indexes/wiki20gb_bitmap8_k32 N 20 "int2=svs;intn=svs;fsearch=expM" < ../../patterns/wiki20g_2_2
mv N.dat N_p2.2.vbyte.moffat.bitmap8.dat

./SEARCH indexes/wiki20gb_bitmap8_k4  N 20 "int2=svs;intn=svs;fsearch=expM" < ../../patterns/wiki20g_5_5
./SEARCH indexes/wiki20gb_bitmap8_k32 N 20 "int2=svs;intn=svs;fsearch=expM" < ../../patterns/wiki20g_5_5
mv N.dat N_p5.2.vbyte.moffat.bitmap8.dat
echo "################################################################" 











#echo "################################################################" 
#./SEARCH indexes/torsen200mb I 1  "int2=svs;intn=svs;fsearch=expM" < ../patterns/torsen200mb_1_1

#./SEARCH indexes/torsen2000mb_bitmap8_k4  E 1 "int2=svs;intn=svs;fsearch=expM" < ../patterns/torsen2000mb_1_1_10k
#./SEARCH indexes/torsen2000mb_bitmap8_k4  I 1 "int2=svs;intn=svs;fsearch=expM" < ../patterns/torsen2000mb_2_2_10k
#./SEARCH indexes/torsen2000mb_bitmap8_k4  N 1 "int2=svs;intn=svs;fsearch=expM" < ../patterns/torsen2000mb_2_5_10k

#./SEARCH indexes/torsen2000mb_bitmap8_k32 E 1 "int2=svs;intn=svs;fsearch=expM" < ../patterns/torsen2000mb_1_1_10k
#./SEARCH indexes/torsen2000mb_bitmap8_k32 I 1 "int2=svs;intn=svs;fsearch=expM" < ../patterns/torsen2000mb_2_2_10k
#./SEARCH indexes/torsen2000mb_bitmap8_k32 N 1 "int2=svs;intn=svs;fsearch=expM" < ../patterns/torsen2000mb_2_5_10k

#./SEARCH indexes/torsen2000mb_bitmap1_k4  E 1 "int2=svs;intn=svs;fsearch=expM" < ../patterns/torsen2000mb_1_1_10k
#./SEARCH indexes/torsen2000mb_bitmap1_k4  I 1 "int2=svs;intn=svs;fsearch=expM" < ../patterns/torsen2000mb_2_2_10k
#./SEARCH indexes/torsen2000mb_bitmap1_k4  N 1 "int2=svs;intn=svs;fsearch=expM" < ../patterns/torsen2000mb_2_5_10k

#./SEARCH indexes/torsen2000mb_bitmap1_k32 E 1 "int2=svs;intn=svs;fsearch=expM" < ../patterns/torsen2000mb_1_1_10k
#./SEARCH indexes/torsen2000mb_bitmap1_k32 I 1 "int2=svs;intn=svs;fsearch=expM" < ../patterns/torsen2000mb_2_2_10k
#./SEARCH indexes/torsen2000mb_bitmap1_k32 N 1 "int2=svs;intn=svs;fsearch=expM" < ../patterns/torsen2000mb_2_5_10k
#
#echo "################################################################"
#
#cp E.dat E.2.vbyte.moffat.dat
#cp I.dat I.2.vbyte.moffat.dat
#cp N.dat N.2.vbyte.moffat.dat
