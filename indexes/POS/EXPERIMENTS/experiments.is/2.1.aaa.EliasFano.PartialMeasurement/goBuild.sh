ln -s ../../II_docs.EliasFano_formato.ver.leeme/BUILD_VBYTE_PLAIN   BUILD
ln -s ../../II_docs.EliasFano_formato.ver.leeme/SEARCH_VBYTE_PLAIN  SEARCH
mkdir indexes


echo "################################################################" 
./BUILD ../../text2gb/wiki_src2gb.txt indexes/wiki2gb_ts1 "sample_ct=1"   
echo "################################################################"



