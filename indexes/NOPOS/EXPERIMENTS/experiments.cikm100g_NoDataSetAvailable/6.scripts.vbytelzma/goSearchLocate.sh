echo "################################################################" 
rm -f L.dat

./SEARCH indexes/wiki100gb  L   1 "int2=merge;intn=merge" < ../patterns/suel_patterns

mv L.dat L_p5.6.vbyte.lzma.dat

echo "################################################################" 




