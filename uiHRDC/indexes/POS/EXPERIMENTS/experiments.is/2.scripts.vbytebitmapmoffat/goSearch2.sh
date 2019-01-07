rm -f S-LOG.dat*
python ../../../../../utils-py/starttime.py  S-LOG.dat "Search-starts!"   ##LOGS-ELAPSED-TIMES

rm -f E.dat
rm -f N.dat

./SEARCH indexes/wiki2gb_bitmap1_k4  E 1 "int2=svs;intn=svs;fsearch=expM" < ../../patterns/wiki2g.words.f1_1000
./SEARCH indexes/wiki2gb_bitmap1_k32 E 1 "int2=svs;intn=svs;fsearch=expM" < ../../patterns/wiki2g.words.f1_1000
mv E.dat E_Wa.2.vbyte.moffat.dat

./SEARCH indexes/wiki2gb_bitmap1_k4  E 1 "int2=svs;intn=svs;fsearch=expM" < ../../patterns/wiki2g.words.f1001_100k
./SEARCH indexes/wiki2gb_bitmap1_k32 E 1 "int2=svs;intn=svs;fsearch=expM" < ../../patterns/wiki2g.words.f1001_100k
mv E.dat E_Wb.2.vbyte.moffat.dat

./SEARCH indexes/wiki2gb_bitmap1_k4  N 1 "int2=svs;intn=svs;fsearch=expM" < ../../patterns/wiki2g_2_2
./SEARCH indexes/wiki2gb_bitmap1_k32 N 1 "int2=svs;intn=svs;fsearch=expM" < ../../patterns/wiki2g_2_2
mv N.dat N_p2.2.vbyte.moffat.dat

./SEARCH indexes/wiki2gb_bitmap1_k4  N 1 "int2=svs;intn=svs;fsearch=expM" < ../../patterns/wiki2g_5_5
./SEARCH indexes/wiki2gb_bitmap1_k32 N 1 "int2=svs;intn=svs;fsearch=expM" < ../../patterns/wiki2g_5_5
mv N.dat N_p5.2.vbyte.moffat.dat


python ../../../../../utils-py/endtime.py    S-LOG.dat                  ##LOGS-ELAPSED-TIMES
