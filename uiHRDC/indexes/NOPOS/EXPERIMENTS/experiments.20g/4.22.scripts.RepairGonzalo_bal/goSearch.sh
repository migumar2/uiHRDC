rm -f S-LOG.dat*
python ../../../../../utils-py/starttime.py  S-LOG.dat "Search-starts!"   ##LOGS-ELAPSED-TIMES

rm -f E.dat
rm -f N.dat
echo "################################################################" 

#./SEARCH indexes/wiki2gb E 100 "int2=merge;intn=merge" < ../../patterns/einstein.words
#./SEARCH indexes/wiki2gb N 100 "int2=merge;intn=merge" < ../../patterns/einstein_2_5

./SEARCH indexes/wiki20gb E  500 "int2=merge;intn=merge" < ../../patterns/wiki20g.words.f1_1000
mv E.dat E_Wa.4.2.repairG.dat

./SEARCH indexes/wiki20gb E  500 "int2=merge;intn=merge" < ../../patterns/wiki20g.words.f1001_100k
mv E.dat E_Wb.4.2.repairG.dat

./SEARCH indexes/wiki20gb N   2 "int2=merge;intn=merge" < ../../patterns/wiki20g_2_2
mv N.dat N_p2.4.2.repairG.dat

./SEARCH indexes/wiki20gb N   2 "int2=merge;intn=merge" < ../../patterns/wiki20g_5_5
mv N.dat N_p5.4.2.repairG.dat

echo "################################################################"


#echo "################################################################" 
#./SEARCH indexes/torsen200mb I 1  "int2=merge;intn=merge" < ../patterns/torsen200mb_1_1
#./SEARCH indexes/torsen2000mb E 1 "int2=merge;intn=merge" < ../patterns/torsen2000mb_1_1_10k
#./SEARCH indexes/torsen2000mb I 1 "int2=merge;intn=merge" < ../patterns/torsen2000mb_2_2_10k
#./SEARCH indexes/torsen2000mb N 1 "int2=merge;intn=merge" < ../patterns/torsen2000mb_2_5_10k
#echo "################################################################"
#cp E.dat E.4.2.repair.dat
#cp I.dat I.4.2.repair.dat
#cp N.dat N.4.2.repair.dat


python ../../../../../utils-py/endtime.py    S-LOG.dat                  ##LOGS-ELAPSED-TIMES
