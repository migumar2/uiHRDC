rm -f S-LOG.dat*
python ../../../../../utils-py/starttime.py  S-LOG.dat "Search-starts!"   ##LOGS-ELAPSED-TIMES

rm -f E.dat
rm -f N.dat

echo "################################################################" 
## elegir "merge" (extract+merge) o "mergeS9" (extract+intersección durante extracción)
## en simple9 scape, sólo disponible "merge" !!!

./SEARCH indexes/wiki2gb E 50 "int2=merge;intn=merge" < ../../patterns/wiki2g.words.f1_1000
mv E.dat E_Wa.7.1.simple9e.dat

./SEARCH indexes/wiki2gb E 50 "int2=merge;intn=merge" < ../../patterns/wiki2g.words.f1001_100k
mv E.dat E_Wb.7.1.simple9e.dat

./SEARCH indexes/wiki2gb N  5 "int2=merge;intn=merge" < ../../patterns/wiki2g_2_2
mv N.dat N_p2.7.1.simple9e.dat

./SEARCH indexes/wiki2gb N  5 "int2=merge;intn=merge" < ../../patterns/wiki2g_5_5
mv N.dat N_p5.7.1.simple9e.dat


#./SEARCH indexes/wiki2gb E 100 "int2=merge;intn=merge" < ../../patterns/einstein.words
#./SEARCH indexes/wiki2gb N 100 "int2=merge;intn=merge" < ../../patterns/einstein_2_5
#./SEARCH indexes/wiki2gb E 100 "int2=mergeS9;intn=mergeS9" < ../../patterns/einstein.words
#./SEARCH indexes/wiki2gb N 100 "int2=mergeS9;intn=mergeS9" < ../../patterns/einstein_2_5

echo "################################################################"

#cp E.dat E.7.1.simple9e.dat
#cp N.dat N.7.1.simple9e.dat



python ../../../../../utils-py/endtime.py    S-LOG.dat                  ##LOGS-ELAPSED-TIMES
