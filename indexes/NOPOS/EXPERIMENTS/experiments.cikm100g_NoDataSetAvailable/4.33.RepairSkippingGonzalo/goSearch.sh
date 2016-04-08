
ln -s ../../II_docs_large/SEARCH_REPAIR_SKIP_NOTEXT SEARCH
ln -s ../../II_docs_large/lib lib


echo "################################################################" 
rm -f N.dat

./SEARCH indexes/wiki100gb N   1 "int2=skip;intn=skip" < ../patterns/suel_patterns

mv N.dat N_p.4.3.repairG.skipping.dat

echo "################################################################" 

