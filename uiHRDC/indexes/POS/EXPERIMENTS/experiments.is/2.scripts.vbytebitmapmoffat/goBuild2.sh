ln -s ../../II_docs/BUILD_VBYTE_MOFFAT_REPAIR_T   BUILD
ln -s ../../II_docs/SEARCH_VBYTE_MOFFAT_REPAIR_T  SEARCH
mkdir indexes

rm -f B-LOG.dat*
python ../../../../../utils-py/starttime.py  B-LOG.dat "Build-starts!"   ##LOGS-ELAPSED-TIMES


echo "################################################################" 
 
./BUILD ../../text2gb/wiki_src2gb.txt indexes/wiki2gb_bitmap1_k4  "samplerate=4 ;lenBitmapDiv=8"
./BUILD ../../text2gb/wiki_src2gb.txt indexes/wiki2gb_bitmap1_k32 "samplerate=32;lenBitmapDiv=8"  

echo "################################################################"

python ../../../../../utils-py/endtime.py    B-LOG.dat                  ##LOGS-ELAPSED-TIMES
