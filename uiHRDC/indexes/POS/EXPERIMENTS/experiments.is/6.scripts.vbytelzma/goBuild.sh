ln -s ../../II_docs/BUILD_VBYTE_LZMA_REPAIR_T   BUILD
ln -s ../../II_docs/SEARCH_VBYTE_LZMA_REPAIR_T  SEARCH
ln -s ../../II_docs/lib lib
mkdir indexes

rm -f B-LOG.dat*
python ../../../../../utils-py/starttime.py  B-LOG.dat "Build-starts!"   ##LOGS-ELAPSED-TIMES

echo "################################################################" 

./BUILD ../../text2gb/wiki_src2gb.txt indexes/wiki2gb "path2lzmaencoder=./lib/lzmaEncoder; minbcssize=10" 

echo "################################################################"

python ../../../../../utils-py/endtime.py    B-LOG.dat                  ##LOGS-ELAPSED-TIMES
