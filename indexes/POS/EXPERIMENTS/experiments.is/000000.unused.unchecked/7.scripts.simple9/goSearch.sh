
echo "################################################################" 
## elegir "merge" (extract+merge) o "mergeS9" (extract+intersección durante extracción)

#./SEARCH indexes/wiki2gb E 100 "int2=merge;intn=merge" < ../../patterns/einstein.words
#./SEARCH indexes/wiki2gb N 100 "int2=merge;intn=merge" < ../../patterns/einstein_2_5

./SEARCH indexes/wiki2gb E 100 "int2=mergeS9;intn=mergeS9" < ../../patterns/einstein.words
./SEARCH indexes/wiki2gb N 100 "int2=mergeS9;intn=mergeS9" < ../../patterns/einstein_2_5

echo "################################################################"

cp E.dat E.7.simple9.dat
cp N.dat N.7.simple9.dat


