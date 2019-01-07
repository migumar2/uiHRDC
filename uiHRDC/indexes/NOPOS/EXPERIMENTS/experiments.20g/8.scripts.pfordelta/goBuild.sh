ln -s ../../II_docs/BUILDP_PFORDELTA_NOTEXT  BUILDP
ln -s ../../II_docs/BUILD_PFORDELTA_NOTEXT  BUILD
ln -s ../../II_docs/SEARCH_PFORDELTA_NOTEXT SEARCH
mkdir indexes

rm -f B-LOG.dat*
python ../../../../../utils-py/starttime.py  B-LOG.dat "Build-starts!"   ##LOGS-ELAPSED-TIMES

echo "################################################################" 

#./BUILD ../textos/torsen.text2000mb.txt indexes/torsen2000mb "pfdThreshold=100" 

#./BUILD ../../textos/text20gb.txt indexes/wiki20gb "pfdThreshold=100" 
#./BUILDP ../../20gb indexes/wiki20gb "pfdThreshold=100 ; 8.pfordelta"

./BUILD ../../textos/text20gb.txt  indexes/wiki20gb "pfdThreshold=100 ; 8.pfordelta"

echo "################################################################"
python ../../../../../utils-py/endtime.py    B-LOG.dat                  ##LOGS-ELAPSED-TIMES

