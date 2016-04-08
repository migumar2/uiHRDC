echo "################################################################" 
rm -f N.dat

./SEARCH indexes/wiki100gb  N   1 "int2=merge;intn=merge" < ../patterns/suel_patterns

mv N.dat N_p5.6.vbyte.lzma.dat

echo "################################################################" 




