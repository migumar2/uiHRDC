ln -s ../../II_docs/BUILD_VBYTE_SANDERS_REPAIR_T   BUILD
ln -s ../../II_docs/SEARCH_VBYTE_SANDERS_REPAIR_T  SEARCH
mkdir indexes

rm -f B-LOG.dat*
python ../../../../../utils-py/starttime.py  B-LOG.dat "Build-starts!"   ##LOGS-ELAPSED-TIMES


echo "################################################################" 

#./BUILD ../../text2gb/wiki_src2gb.txt indexes/wiki2gb_bitmap1_k16 "samplerate=8;lenBitmapDiv=1"

#./BUILD ../../text2gb/wiki_src2gb.txt indexes/wiki2gb_bitmap1_k4 "samplerate=4;lenBitmapDiv=1"

./BUILD ../../text2gb/wiki_src2gb.txt indexes/wiki2gb_bitmap1_k16 "samplerate=16;lenBitmapDiv=1"
#./BUILD ../../text2gb/wiki_src2gb.txt indexes/wiki2gb_bitmap1_k64 "samplerate=64;lenBitmapDiv=1"  
./BUILD ../../text2gb/wiki_src2gb.txt indexes/wiki2gb_bitmap1_k128 "samplerate=128;lenBitmapDiv=1"  

echo "################################################################"

python ../../../../../utils-py/endtime.py    B-LOG.dat                  ##LOGS-ELAPSED-TIMES

