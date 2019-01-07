ln -s ../../II_docs/BUILD_VBYTE_REPAIR_T   BUILD
ln -s ../../II_docs/SEARCH_VBYTE_REPAIR_T  SEARCH
mkdir indexes

rm -f B-LOG.dat*
python ../../../../../utils-py/starttime.py  B-LOG.dat "Build-starts!"   ##LOGS-ELAPSED-TIMES

./BUILD ../../text2gb/wiki_src2gb.txt indexes/wiki2gb "no params"  

echo "################################################################" 
./BUILD ../../text2gb/wiki_src2gb.txt indexes/wiki2gb_ts1 "sample_ct=1"  
./BUILD ../../text2gb/wiki_src2gb.txt indexes/wiki2gb_ts2 "sample_ct=2"  
./BUILD ../../text2gb/wiki_src2gb.txt indexes/wiki2gb_ts8 "sample_ct=8"  
./BUILD ../../text2gb/wiki_src2gb.txt indexes/wiki2gb_ts32 "sample_ct=32"  
./BUILD ../../text2gb/wiki_src2gb.txt indexes/wiki2gb_ts64 "sample_ct=64"   
./BUILD ../../text2gb/wiki_src2gb.txt indexes/wiki2gb_ts256 "sample_ct=256"   
./BUILD ../../text2gb/wiki_src2gb.txt indexes/wiki2gb_ts4k "sample_ct=4096"   

echo "################################################################"

python ../../../../../utils-py/endtime.py    B-LOG.dat                  ##LOGS-ELAPSED-TIMES



