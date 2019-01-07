rm -f S-LOG.dat*
python ../../../../../utils-py/starttime.py  S-LOG.dat "Search-starts!"   ##LOGS-ELAPSED-TIMES

rm -f E.dat
rm -f N.dat
#ln -s ../../II_docs/SEARCH_REPAIR_SKIP_REPAIR_T  SEARCH
#ln -s ../../II_docs/lib lib
echo "################################################################" 

#./SEARCH indexes/wiki2gb E 100 "int2=skip;intn=skip" < ../../patterns/einstein.words
#./SEARCH indexes/wiki2gb N 100 "int2=skip;intn=skip" < ../../patterns/einstein_2_5

./SEARCH indexes/wiki2gb E 20 "int2=skip;intn=skip" < ../../patterns/wiki2g.words.f1_1000
mv E.dat E_Wa.4.3.repairG.skipping.dat

./SEARCH indexes/wiki2gb E 20 "int2=skip;intn=skip" < ../../patterns/wiki2g.words.f1001_100k
mv E.dat E_Wb.4.3.repairG.skipping.dat

./SEARCH indexes/wiki2gb N  5 "int2=skip;intn=skip" < ../../patterns/wiki2g_2_2
mv N.dat N_p2.4.3.repairG.skipping.dat

./SEARCH indexes/wiki2gb N  5 "int2=skip;intn=skip" < ../../patterns/wiki2g_5_5
mv N.dat N_p5.4.3.repairG.skipping.dat

echo "################################################################"


python ../../../../../utils-py/endtime.py    S-LOG.dat                  ##LOGS-ELAPSED-TIMES
