
echo "################################################################" 

./SEARCH indexes/einstein450mb_bitmap8_k4  E 100 "int2=svs;intn=svs;fsearch=expM" < ../../patterns/einstein.words
./SEARCH indexes/einstein450mb_bitmap8_k4  N 100 "int2=svs;intn=svs;fsearch=expM" < ../../patterns/einstein_2_5

./SEARCH indexes/einstein450mb_bitmap8_k32 E 100 "int2=svs;intn=svs;fsearch=expM" < ../../patterns/einstein.words
./SEARCH indexes/einstein450mb_bitmap8_k32 N 100 "int2=svs;intn=svs;fsearch=expM" < ../../patterns/einstein_2_5

./SEARCH indexes/einstein450mb_bitmap1_k4  E 100 "int2=svs;intn=svs;fsearch=expM" < ../../patterns/einstein.words
./SEARCH indexes/einstein450mb_bitmap1_k4  N 100 "int2=svs;intn=svs;fsearch=expM" < ../../patterns/einstein_2_5

./SEARCH indexes/einstein450mb_bitmap1_k32 E 100 "int2=svs;intn=svs;fsearch=expM" < ../../patterns/einstein.words
./SEARCH indexes/einstein450mb_bitmap1_k32 N 100 "int2=svs;intn=svs;fsearch=expM" < ../../patterns/einstein_2_5

echo "################################################################"

cp E.dat E.1.2.rice.moffat.dat
cp N.dat N.1.2.rice.moffat.dat
