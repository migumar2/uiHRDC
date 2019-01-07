ln -s ../../II_docs/BUILDP_VBYTE_LZMA_NOTEXT   BUILDP
ln -s ../../II_docs/BUILD_VBYTE_LZMA_NOTEXT   BUILD
ln -s ../../II_docs/SEARCH_VBYTE_LZMA_NOTEXT  SEARCH

ln -s ../../II_docs/lib lib
mkdir indexes

rm -f B-LOG.dat*
python ../../../../../utils-py/starttime.py  B-LOG.dat "Build-starts!"   ##LOGS-ELAPSED-TIMES

echo "################################################################" 

#./BUILD ../../textos/text20gb.txt indexes/wiki20gb "path2lzmaencoder=./lib/lzmaEncoder; minbcssize=10" 

#./BUILDP ../../20gb indexes/wiki20gb "path2lzmaencoder=./lib/lzmaEncoder; minbcssize=10"

./BUILD ../../textos/text20gb.txt indexes/wiki20gb "path2lzmaencoder=./lib/lzmaEncoder; minbcssize=10"

echo "################################################################"
python ../../../../../utils-py/endtime.py    B-LOG.dat                  ##LOGS-ELAPSED-TIMES

