ln -s ../../II_docs/BUILD_VBYTE_LZMA_REPAIR_T   BUILD
ln -s ../../II_docs/SEARCH_VBYTE_LZMA_REPAIR_T  SEARCH
ln -s ../../II_docs/lib lib
mkdir indexes

echo "################################################################" 

./BUILD ../../text2gb/wiki_src2gb.txt indexes/wiki2gb "path2lzmaencoder=./lib/lzmaEncoder; minbcssize=10" 

echo "################################################################"


