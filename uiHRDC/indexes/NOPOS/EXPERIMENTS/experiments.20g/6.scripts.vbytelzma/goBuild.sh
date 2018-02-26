ln -s ../../II_docs/BUILDP_VBYTE_LZMA_NOTEXT   BUILDP
ln -s ../../II_docs/BUILD_VBYTE_LZMA_NOTEXT   BUILD
ln -s ../../II_docs/SEARCH_VBYTE_LZMA_NOTEXT  SEARCH

ln -s ../../II_docs/lib lib
mkdir indexes

echo "################################################################" 

#./BUILD ../../textos/text20gb.txt indexes/wiki20gb "path2lzmaencoder=./lib/lzmaEncoder; minbcssize=10" 

#./BUILDP ../../20gb indexes/wiki20gb "path2lzmaencoder=./lib/lzmaEncoder; minbcssize=10"

./BUILD ../../textos/text20gb.txt indexes/wiki20gb "path2lzmaencoder=./lib/lzmaEncoder; minbcssize=10"

echo "################################################################"


