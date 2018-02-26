ln -s ../../II_docs/BUILD_QMXCODINGSIMD_REPAIR_T  BUILD
ln -s ../../II_docs/SEARCH_QMXCODINGSIMD_REPAIR_T SEARCH
mkdir indexes

echo "################################################################" 

#./BUILD ../textos/torsen.text2000mb.txt indexes/torsen2000mb "pfdThreshold=100" 

#./BUILD ../../textos/text20gb.txt indexes/wiki20gb "pfdThreshold=100" 
./BUILD ../../text2gb/wiki_src2gb.txt indexes/wiki2gb "QMX-simd.noparamsForBuild"

echo "################################################################"


