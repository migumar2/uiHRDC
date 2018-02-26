
echo "################################################################" 

./SEARCH indexes/einstein450mb_bitmap8_k16 E 100 "int2=lookup;intn=lookup" < ../../patterns/einstein.words
./SEARCH indexes/einstein450mb_bitmap8_k16 N 100 "int2=lookup;intn=lookup" < ../../patterns/einstein_2_5

./SEARCH indexes/einstein450mb_bitmap8_k64 E 100 "int2=lookup;intn=lookup" < ../../patterns/einstein.words
./SEARCH indexes/einstein450mb_bitmap8_k64 N 100 "int2=lookup;intn=lookup" < ../../patterns/einstein_2_5

./SEARCH indexes/einstein450mb_bitmap1_k16 E 100 "int2=lookup;intn=lookup" < ../../patterns/einstein.words
./SEARCH indexes/einstein450mb_bitmap1_k16 N 100 "int2=lookup;intn=lookup" < ../../patterns/einstein_2_5

./SEARCH indexes/einstein450mb_bitmap1_k64 E 100 "int2=lookup;intn=lookup" < ../../patterns/einstein.words
./SEARCH indexes/einstein450mb_bitmap1_k64 N 100 "int2=lookup;intn=lookup" < ../../patterns/einstein_2_5

echo "################################################################"

cp E.dat E.1.3.rice.sanders.dat
cp N.dat N.1.3.rice.sanders.dat

