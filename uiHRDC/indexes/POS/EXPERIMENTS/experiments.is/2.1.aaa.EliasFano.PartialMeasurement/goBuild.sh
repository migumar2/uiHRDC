ln -s ../../II_docs.EliasFano_formato.ver.leeme/BUILD_VBYTE_PLAIN   BUILD
ln -s ../../II_docs.EliasFano_formato.ver.leeme/SEARCH_VBYTE_PLAIN  SEARCH
mkdir indexes

rm -f B-LOG.dat*
python ../../../../../utils-py/starttime.py  B-LOG.dat "Build-starts!"   ##LOGS-ELAPSED-TIMES


echo "################################################################" 
./BUILD ../../text2gb/wiki_src2gb.txt indexes/wiki2gb_ts1 "sample_ct=1"   
echo "################################################################"

python ../../../../../utils-py/endtime.py    B-LOG.dat                  ##LOGS-ELAPSED-TIMES



