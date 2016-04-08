ln -s ../../II_docs_large/SEARCH_REPAIR_NOSKIP_NOTEXT SEARCH
ln -s ../../II_docs_large/lib lib



echo "################################################################" 
rm -f L.dat

./SEARCH indexes/wiki100gb L   1 "int2=merge;intn=merge" < ../patterns/suel_patterns

mv L.dat L_p.4.2.repairG.dat

echo "################################################################" 

