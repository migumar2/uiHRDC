ln -s ../../II_docs_large/BUILDP_VBYTE_LZMA_NOTEXT   BUILDP
ln -s ../../II_docs_large/BUILD_VBYTE_LZMA_NOTEXT   BUILD
ln -s ../../II_docs_large/SEARCH_VBYTE_LZMA_NOTEXT  SEARCH

ln -s ../../II_docs_large/lib lib
mkdir indexes

echo "################################################################" 


#./BUILD ../../../textos/suel100gb.txt indexes/wiki100gb "path2lzmaencoder=./lib/lzmaEncoder; minbcssize=10" 

./BUILDP ../wiki100gb indexes/wiki100gb "path2lzmaencoder=./lib/lzmaEncoder; minbcssize=8; 6.scripts.vbyte.lzma"

echo "################################################################"


