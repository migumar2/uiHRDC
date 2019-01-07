ln -s ../../II_docs/BUILD_SIMPLE9_ESC_REPAIR_T   BUILD
ln -s ../../II_docs/SEARCH_SIMPLE9_ESC_REPAIR_T  SEARCH
mkdir indexes

rm -f B-LOG.dat*
python ../../../../../utils-py/starttime.py  B-LOG.dat "Build-starts!"   ##LOGS-ELAPSED-TIMES

echo "################################################################" 

./BUILD ../../text2gb/wiki_src2gb.txt indexes/wiki2gb "s9noparamsForBuild" 

echo "################################################################"

python ../../../../../utils-py/endtime.py    B-LOG.dat                  ##LOGS-ELAPSED-TIMES


