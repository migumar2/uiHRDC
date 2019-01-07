ln -s ../../II_docs/BUILD_QMXCODING_REPAIR_T  BUILD
ln -s ../../II_docs/SEARCH_QMXCODING_REPAIR_T SEARCH
mkdir indexes

rm -f B-LOG.dat*
python ../../../../../utils-py/starttime.py  B-LOG.dat "Build-starts!"   ##LOGS-ELAPSED-TIMES

echo "################################################################" 

#./BUILD ../textos/torsen.text2000mb.txt indexes/torsen2000mb "pfdThreshold=100" 

#./BUILD ../../textos/text20gb.txt indexes/wiki20gb "pfdThreshold=100" 
./BUILD ../../text2gb/wiki_src2gb.txt indexes/wiki2gb "QMXnoparamsForBuild"

echo "################################################################"

python ../../../../../utils-py/endtime.py    B-LOG.dat                  ##LOGS-ELAPSED-TIMES


