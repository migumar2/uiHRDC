ln -s ../../II_docs/BUILD_VBYTE_REPAIR_T   BUILD
ln -s ../../II_docs/SEARCH_VBYTE_REPAIR_T  SEARCH
mkdir indexes

./BUILD ../../text2gb/wiki_src2gb.txt indexes/wiki2gb "no params"  

echo "################################################################" 
./BUILD ../../text2gb/wiki_src2gb.txt indexes/wiki2gb_ts1 "sample_ct=1"  
./BUILD ../../text2gb/wiki_src2gb.txt indexes/wiki2gb_ts2 "sample_ct=2"  
./BUILD ../../text2gb/wiki_src2gb.txt indexes/wiki2gb_ts8 "sample_ct=8"  
./BUILD ../../text2gb/wiki_src2gb.txt indexes/wiki2gb_ts32 "sample_ct=32"  
./BUILD ../../text2gb/wiki_src2gb.txt indexes/wiki2gb_ts64 "sample_ct=64"   

echo "################################################################"



