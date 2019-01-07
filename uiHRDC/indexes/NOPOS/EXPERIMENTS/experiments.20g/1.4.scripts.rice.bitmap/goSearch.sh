rm -f S-LOG.dat*
rm -f SB-LOG.dat*

rm -f E.dat
rm -f N.dat

echo "################################################################"
python ../../../../../utils-py/starttime.py  S-LOG.dat "Search-starts!"   ##LOGS-ELAPSED-TIMES

	#./SEARCH indexes/wiki20gb_bitmap1 E 100 "int2=merge;intn=svs;fsearch=seqM" < ../../patterns/einstein.words
	#./SEARCH indexes/wiki20gb_bitmap1 N 100 "int2=merge;intn=svs;fsearch=seqM" < ../../patterns/einstein_2_5

	./SEARCH indexes/wiki20gb_bitmap1 E 2000 "int2=merge;intn=svs;fsearch=seqM" < ../../patterns/wiki20g.words.f1_1000
	mv E.dat E_Wa.1.4.rice.dat

	./SEARCH indexes/wiki20gb_bitmap1 E 2000 "int2=merge;intn=svs;fsearch=seqM" < ../../patterns/wiki20g.words.f1001_100k
	mv E.dat E_Wb.1.4.rice.dat

	./SEARCH indexes/wiki20gb_bitmap1 N 100 "int2=merge;intn=svs;fsearch=seqM" < ../../patterns/wiki20g_2_2
	mv N.dat N_p2.1.4.rice.dat

	./SEARCH indexes/wiki20gb_bitmap1 N 100 "int2=merge;intn=svs;fsearch=seqM" < ../../patterns/wiki20g_5_5
	mv N.dat N_p5.1.4.rice.dat

python ../../../../../utils-py/endtime.py    S-LOG.dat                  ##LOGS-ELAPSED-TIMES
echo "################################################################"

#cp E.dat E.1.4.rice.dat
#cp N.dat N.1.4.rice.dat

rm -f E.dat
rm -f N.dat

echo "################################################################"
python ../../../../../utils-py/starttime.py  SB-LOG.dat "Search-starts!"   ##LOGS-ELAPSED-TIMES

	./SEARCH indexes/wiki20gb_bitmap8 E 2000 "int2=merge;intn=svs;fsearch=seqM" < ../../patterns/wiki20g.words.f1_1000
	mv E.dat E_Wa.1.4.rice.bitmap8.dat

	./SEARCH indexes/wiki20gb_bitmap8 E 2000 "int2=merge;intn=svs;fsearch=seqM" < ../../patterns/wiki20g.words.f1001_100k
	mv E.dat E_Wb.1.4.rice.bitmap8.dat

	./SEARCH indexes/wiki20gb_bitmap8 N 100 "int2=merge;intn=svs;fsearch=seqM" < ../../patterns/wiki20g_2_2
	mv N.dat N_p2.1.4.rice.bitmap8.dat

	./SEARCH indexes/wiki20gb_bitmap8 N 100 "int2=merge;intn=svs;fsearch=seqM" < ../../patterns/wiki20g_5_5
	mv N.dat N_p5.1.4.rice.bitmap8.dat

python ../../../../../utils-py/endtime.py    SB-LOG.dat                  ##LOGS-ELAPSED-TIMES
echo "################################################################"


