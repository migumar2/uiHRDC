
echo "################################################################" 

./SEARCH indexes/einstein450mb E 100 "int2=merge;intn=merge" < ../../patterns/einstein.words
./SEARCH indexes/einstein450mb N 100 "int2=merge;intn=merge" < ../../patterns/einstein_2_5

echo "################################################################"

cp E.dat E.5.lzend.dat
cp N.dat N.5.lzend.dat


