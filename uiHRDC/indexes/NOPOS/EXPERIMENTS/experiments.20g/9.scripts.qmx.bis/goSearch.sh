rm -f S-LOG.dat*
python ../../../../../utils-py/starttime.py  S-LOG.dat "Search-starts!"   ##LOGS-ELAPSED-TIMES

rm -f E.dat
rm -f N.dat
echo "################################################################" 


./SEARCH indexes/wiki20gb E  25000 "int2=merge;intn=merge" < ../../patterns/wiki20g.words.f1_1000
mv E.dat E_Wa.9.qmx.dat

./SEARCH indexes/wiki20gb E  25000 "int2=merge;intn=merge" < ../../patterns/wiki20g.words.f1001_100k
mv E.dat E_Wb.9.qmx.dat

./SEARCH indexes/wiki20gb N   200 "int2=merge;intn=merge" < ../../patterns/wiki20g_2_2
mv N.dat N_p2.9.qmx.dat

./SEARCH indexes/wiki20gb N   200 "int2=merge;intn=merge" < ../../patterns/wiki20g_5_5
mv N.dat N_p5.9.qmx.dat

echo "################################################################"


#cp E.dat E.8.qmx.dat
#cp I.dat I.8.qmx.dat
#cp N.dat N.8.qmx.dat


python ../../../../../utils-py/endtime.py    S-LOG.dat                  ##LOGS-ELAPSED-TIMES
