ln -s ../../II_docs/BUILDP_VBYTE_MOFFAT_NOTEXT  BUILDP
ln -s ../../II_docs/BUILD_VBYTE_MOFFAT_NOTEXT  BUILD
ln -s ../../II_docs/SEARCH_VBYTE_MOFFAT_NOTEXT SEARCH
mkdir indexes

rm -f B-LOG.dat*
rm -f BB-LOG.dat*



echo "################################################################" 

python ../../../../../utils-py/starttime.py  BB-LOG.dat "Build-starts!"   ##LOGS-ELAPSED-TIMES

	#./BUILD ../../textos/text20gb.txt indexes/wiki20gb_bitmap8_k4 "samplerate=4; lenBitmapDiv=8"  
	#./BUILDP ../../20gb indexes/wiki20gb_bitmap8_k4 "samplerate=4; lenBitmapDiv=8; 2.scripts.vbytebitmapMoffat"   
	./BUILD ../../textos/text20gb.txt indexes/wiki20gb_bitmap8_k4 "samplerate=4; lenBitmapDiv=8; 2.scripts.vbytebitmapMoffat"   

	#./BUILD ../../textos/text20gb.txt indexes/wiki20gb_bitmap8_k32 "samplerate=32; lenBitmapDiv=8"  
	#./BUILDP ../../20gb indexes/wiki20gb_bitmap8_k32 "samplerate=32; lenBitmapDiv=8; 2.scripts.vbytebitmapMoffat"   
	./BUILD ../../textos/text20gb.txt indexes/wiki20gb_bitmap8_k32 "samplerate=32; lenBitmapDiv=8; 2.scripts.vbytebitmapMoffat"   

python ../../../../../utils-py/endtime.py    BB-LOG.dat                  ##LOGS-ELAPSED-TIMES



python ../../../../../utils-py/starttime.py  B-LOG.dat "Build-starts!"   ##LOGS-ELAPSED-TIMES

	./BUILD ../../textos/text20gb.txt indexes/wiki20gb_bitmap1_k4 "samplerate=4; lenBitmapDiv=1; 2.scripts.vbytebitmapMoffat"   
	./BUILD ../../textos/text20gb.txt indexes/wiki20gb_bitmap1_k32 "samplerate=32; lenBitmapDiv=1; 2.scripts.vbytebitmapMoffat"   

python ../../../../../utils-py/endtime.py    B-LOG.dat                  ##LOGS-ELAPSED-TIMES

echo "################################################################"



#echo "################################################################" 
#./BUILD ../textos/torsen.text2000mb.txt indexes/torsen2000mb_bitmap8_k4  "samplerate=4 ;lenBitmapDiv=8"&  
#./BUILD ../textos/torsen.text2000mb.txt indexes/torsen2000mb_bitmap8_k32 "samplerate=32;lenBitmapDiv=8"&  
#./BUILD ../textos/torsen.text2000mb.txt indexes/torsen2000mb_bitmap1_k4  "samplerate=4 ;lenBitmapDiv=1"&  
#./BUILD ../textos/torsen.text2000mb.txt indexes/torsen2000mb_bitmap1_k32 "samplerate=32;lenBitmapDiv=1"  
#echo "################################################################"

