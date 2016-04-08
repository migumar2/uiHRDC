
echo "################################################################" 
rm -f L.dat

./SEARCH indexes/wiki100gb N   1 "int2=merge;intn=merge" < ../patterns/suel_patterns

mv L.dat L_p.1.5.riceRLE.dat

echo "################################################################" 

