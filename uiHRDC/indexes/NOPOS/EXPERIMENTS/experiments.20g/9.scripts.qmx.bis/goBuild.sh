ln -s ../../II_docs/BUILDP_QMXCODINGSIMD_NOTEXT  BUILDP
ln -s ../../II_docs/BUILD_QMXCODINGSIMD_NOTEXT  BUILD
ln -s ../../II_docs/SEARCH_QMXCODINGSIMD_NOTEXT SEARCH
mkdir indexes

rm -f B-LOG.dat*
python ../../../../../utils-py/starttime.py  B-LOG.dat "Build-starts!"   ##LOGS-ELAPSED-TIMES

echo "################################################################" 

#./BUILD ../textos/torsen.text2000mb.txt indexes/torsen2000mb "pfdThreshold=1" 

#./BUILD ../../textos/text20gb.txt indexes/wiki20gb "pfdThreshold=1; 9.qmx" 
#./BUILDP ../../20gb indexes/wiki20gb "pfdThreshold=1 ; 9.qmx"

./BUILD ../../textos/text20gb.txt indexes/wiki20gb "pfdThreshold=1 ; 9.qmxbis"

echo "################################################################"
python ../../../../../utils-py/endtime.py    B-LOG.dat                  ##LOGS-ELAPSED-TIMES

