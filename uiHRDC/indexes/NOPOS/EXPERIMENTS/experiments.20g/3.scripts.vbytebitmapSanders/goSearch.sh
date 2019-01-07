rm -f S-LOG.dat*
rm -f SB-LOG.dat*


rm -f E.dat
rm -f N.dat

python ../../../../../utils-py/starttime.py  S-LOG.dat "Search-starts!"   ##LOGS-ELAPSED-TIMES

	./SEARCH indexes/wiki20gb_bitmap1_k16  E 4000 "int2=lookup;intn=lookup" < ../../patterns/wiki20g.words.f1_1000
	#./SEARCH indexes/wiki20gb_bitmap1_k64  E 400 "int2=lookup;intn=lookup" < ../../patterns/wiki20g.words.f1_1000
	./SEARCH indexes/wiki20gb_bitmap1_k128 E 4000 "int2=lookup;intn=lookup" < ../../patterns/wiki20g.words.f1_1000
	mv E.dat E_Wa.3.vbyte.sanders.dat

	./SEARCH indexes/wiki20gb_bitmap1_k16  E 4000 "int2=lookup;intn=lookup" < ../../patterns/wiki20g.words.f1001_100k
	#./SEARCH indexes/wiki20gb_bitmap1_k64  E 400 "int2=lookup;intn=lookup" < ../../patterns/wiki20g.words.f1001_100k
	./SEARCH indexes/wiki20gb_bitmap1_k128 E 4000 "int2=lookup;intn=lookup" < ../../patterns/wiki20g.words.f1001_100k
	mv E.dat E_Wb.3.vbyte.sanders.dat

	./SEARCH indexes/wiki20gb_bitmap1_k16  N   100 "int2=lookup;intn=lookup" < ../../patterns/wiki20g_2_2
	#./SEARCH indexes/wiki20gb_bitmap1_k64  N   100 "int2=lookup;intn=lookup" < ../../patterns/wiki20g_2_2
	./SEARCH indexes/wiki20gb_bitmap1_k128 N   100 "int2=lookup;intn=lookup" < ../../patterns/wiki20g_2_2
	mv N.dat N_p2.3.vbyte.sanders.dat

	./SEARCH indexes/wiki20gb_bitmap1_k16  N   100 "int2=lookup;intn=lookup" < ../../patterns/wiki20g_5_5
	#./SEARCH indexes/wiki20gb_bitmap1_k64  N   100 "int2=lookup;intn=lookup" < ../../patterns/wiki20g_5_5
	./SEARCH indexes/wiki20gb_bitmap1_k128 N   100 "int2=lookup;intn=lookup" < ../../patterns/wiki20g_5_5
	mv N.dat N_p5.3.vbyte.sanders.dat


python ../../../../../utils-py/endtime.py    S-LOG.dat                  ##LOGS-ELAPSED-TIMES



rm -f E.dat
rm -f N.dat

python ../../../../../utils-py/starttime.py  SB-LOG.dat "Search-starts!"   ##LOGS-ELAPSED-TIMES

	./SEARCH indexes/wiki20gb_bitmap8_k16  E 4000 "int2=lookup;intn=lookup" < ../../patterns/wiki20g.words.f1_1000
	#./SEARCH indexes/wiki20gb_bitmap1_k64  E 400 "int2=lookup;intn=lookup" < ../../patterns/wiki20g.words.f1_1000
	./SEARCH indexes/wiki20gb_bitmap8_k128 E 4000 "int2=lookup;intn=lookup" < ../../patterns/wiki20g.words.f1_1000
	mv E.dat  E_Wa.3.vbyte.sanders.bitmap8.dat

	./SEARCH indexes/wiki20gb_bitmap8_k16  E 4000 "int2=lookup;intn=lookup" < ../../patterns/wiki20g.words.f1001_100k
	#./SEARCH indexes/wiki20gb_bitmap1_k64  E 400 "int2=lookup;intn=lookup" < ../../patterns/wiki20g.words.f1001_100k
	./SEARCH indexes/wiki20gb_bitmap8_k128 E 4000 "int2=lookup;intn=lookup" < ../../patterns/wiki20g.words.f1001_100k
	mv E.dat E_Wb.3.vbyte.sanders.bitmap8.dat

	./SEARCH indexes/wiki20gb_bitmap8_k16  N   100 "int2=lookup;intn=lookup" < ../../patterns/wiki20g_2_2
	#./SEARCH indexes/wiki20gb_bitmap1_k64  N   100 "int2=lookup;intn=lookup" < ../../patterns/wiki20g_2_2
	./SEARCH indexes/wiki20gb_bitmap8_k128 N   100 "int2=lookup;intn=lookup" < ../../patterns/wiki20g_2_2
	mv N.dat N_p2.3.vbyte.sanders.bitmap8.dat

	./SEARCH indexes/wiki20gb_bitmap8_k16  N   100 "int2=lookup;intn=lookup" < ../../patterns/wiki20g_5_5
	#./SEARCH indexes/wiki20gb_bitmap1_k64  N   100 "int2=lookup;intn=lookup" < ../../patterns/wiki20g_5_5
	./SEARCH indexes/wiki20gb_bitmap8_k128 N   100 "int2=lookup;intn=lookup" < ../../patterns/wiki20g_5_5
	mv N.dat N_p5.3.vbyte.sanders.bitmap8.dat

python ../../../../../utils-py/endtime.py    SB-LOG.dat                  ##LOGS-ELAPSED-TIMES


#echo "################################################################" 
#./SEARCH indexes/torsen200mb I 1  "int2=lookup;intn=lookup" < ../patterns/torsen200mb_1_1
#
#./SEARCH indexes/torsen2000mb_bitmap8_k16 E 1 "int2=lookup;intn=lookup" < ../patterns/torsen2000mb_1_1_10k
#./SEARCH indexes/torsen2000mb_bitmap8_k16 I 1 "int2=lookup;intn=lookup" < ../patterns/torsen2000mb_2_2_10k
#./SEARCH indexes/torsen2000mb_bitmap8_k16 N 1 "int2=lookup;intn=lookup" < ../patterns/torsen2000mb_2_5_10k
#
#./SEARCH indexes/torsen2000mb_bitmap8_k64 E 1 "int2=lookup;intn=lookup" < ../patterns/torsen2000mb_1_1_10k
#./SEARCH indexes/torsen2000mb_bitmap8_k64 I 1 "int2=lookup;intn=lookup" < ../patterns/torsen2000mb_2_2_10k
#./SEARCH indexes/torsen2000mb_bitmap8_k64 N 1 "int2=lookup;intn=lookup" < ../patterns/torsen2000mb_2_5_10k
#
#./SEARCH indexes/torsen2000mb_bitmap1_k16 E 1 "int2=lookup;intn=lookup" < ../patterns/torsen2000mb_1_1_10k
#./SEARCH indexes/torsen2000mb_bitmap1_k16 I 1 "int2=lookup;intn=lookup" < ../patterns/torsen2000mb_2_2_10k
#./SEARCH indexes/torsen2000mb_bitmap1_k16 N 1 "int2=lookup;intn=lookup" < ../patterns/torsen2000mb_2_5_10k
#
#./SEARCH indexes/torsen2000mb_bitmap1_k64 E 1 "int2=lookup;intn=lookup" < ../patterns/torsen2000mb_1_1_10k
#./SEARCH indexes/torsen2000mb_bitmap1_k64 I 1 "int2=lookup;intn=lookup" < ../patterns/torsen2000mb_2_2_10k
#./SEARCH indexes/torsen2000mb_bitmap1_k64 N 1 "int2=lookup;intn=lookup" < ../patterns/torsen2000mb_2_5_10k
#
#echo "################################################################"
#
#cp E.dat E.3.vbyte.sanders.dat
#cp I.dat I.3.vbyte.sanders.dat
#cp N.dat N.3.vbyte.sanders.dat


