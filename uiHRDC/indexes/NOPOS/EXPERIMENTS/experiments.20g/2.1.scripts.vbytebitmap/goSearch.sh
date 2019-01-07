rm -f S-LOG.dat*
rm -f SB-LOG.dat*

python ../../../../../utils-py/starttime.py  S-LOG.dat "Search-starts!"   ##LOGS-ELAPSED-TIMES

echo "################################################################" 
rm -f E.dat
rm -f N.dat

./SEARCH indexes/wiki20gb_bitmap1 E 4000 "int2=merge;intn=svs;fsearch=seqM" < ../../patterns/wiki20g.words.f1_1000
mv E.dat E_Wa.2.1.vbyte.dat

./SEARCH indexes/wiki20gb_bitmap1 E 4000 "int2=merge;intn=svs;fsearch=seqM" < ../../patterns/wiki20g.words.f1001_100k
mv E.dat E_Wb.2.1.vbyte.dat

./SEARCH indexes/wiki20gb_bitmap1 N 100 "int2=merge;intn=svs;fsearch=seqM" < ../../patterns/wiki20g_2_2
mv N.dat N_p2.2.1.vbyte.dat

./SEARCH indexes/wiki20gb_bitmap1 N 100 "int2=merge;intn=svs;fsearch=seqM" < ../../patterns/wiki20g_5_5
mv N.dat N_p5.2.1.vbyte.dat


echo "################################################################"
python ../../../../../utils-py/endtime.py    S-LOG.dat                  ##LOGS-ELAPSED-TIMES


python ../../../../../utils-py/starttime.py  SB-LOG.dat "Search-starts!"   ##LOGS-ELAPSED-TIMES

echo "################################################################" 
rm -f E.dat
rm -f N.dat

./SEARCH indexes/wiki20gb_bitmap8 E 4000 "int2=merge;intn=svs;fsearch=seqM" < ../../patterns/wiki20g.words.f1_1000
mv E.dat E_Wa.2.1.vbyte.bitmap8.dat

./SEARCH indexes/wiki20gb_bitmap8 E 4000 "int2=merge;intn=svs;fsearch=seqM" < ../../patterns/wiki20g.words.f1001_100k
mv E.dat E_Wb.2.1.vbyte.bitmap8.dat

./SEARCH indexes/wiki20gb_bitmap8 N 100 "int2=merge;intn=svs;fsearch=seqM" < ../../patterns/wiki20g_2_2
mv N.dat N_p2.2.1.vbyte.bitmap8.dat

./SEARCH indexes/wiki20gb_bitmap8 N 100 "int2=merge;intn=svs;fsearch=seqM" < ../../patterns/wiki20g_5_5
mv N.dat N_p5.2.1.vbyte.bitmap8.dat


echo "################################################################"


python ../../../../../utils-py/endtime.py    SB-LOG.dat                  ##LOGS-ELAPSED-TIMES


