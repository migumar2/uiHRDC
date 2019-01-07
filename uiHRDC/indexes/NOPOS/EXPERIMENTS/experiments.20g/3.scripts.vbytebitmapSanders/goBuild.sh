ln -s ../../II_docs/BUILDP_VBYTE_SANDERS_NOTEXT BUILDP
ln -s ../../II_docs/BUILD_VBYTE_SANDERS_NOTEXT  BUILD
ln -s ../../II_docs/SEARCH_VBYTE_SANDERS_NOTEXT SEARCH
mkdir indexes

rm -f B-LOG.dat*
rm -f BB-LOG.dat*


echo "################################################################" 

python ../../../../../utils-py/starttime.py  B-LOG.dat "Build-starts!"   ##LOGS-ELAPSED-TIMES

	#./BUILD ../../textos/text20gb.txt indexes/wiki20gb_bitmap1_k16 "samplerate=16;lenBitmapDiv=1"
	#./BUILDP ../../20gb indexes/wiki20gb_bitmap1_k16 "samplerate=16;lenBitmapDiv=1 ; 3.vbytebitmapSanders"   

	./BUILD ../../textos/text20gb.txt indexes/wiki20gb_bitmap1_k16 "samplerate=16;lenBitmapDiv=1 ; 3.vbytebitmapSanders"   
	./BUILD ../../textos/text20gb.txt indexes/wiki20gb_bitmap1_k128 "samplerate=128;lenBitmapDiv=1; 3.vbytebitmapSanders"    

python ../../../../../utils-py/endtime.py    B-LOG.dat                  ##LOGS-ELAPSED-TIMES


python ../../../../../utils-py/starttime.py  BB-LOG.dat "Build-starts!"   ##LOGS-ELAPSED-TIMES


	#./BUILD ../../textos/text20gb.txt indexes/wiki20gb_bitmap1_k128 "samplerate=128;lenBitmapDiv=1"  
	#./BUILDP ../../20gb indexes/wiki20gb_bitmap1_k128 "samplerate=128;lenBitmapDiv=1; 3.vbytebitmapSanders"    
	./BUILD ../../textos/text20gb.txt indexes/wiki20gb_bitmap8_k16 "samplerate=16;lenBitmapDiv=8 ; 3.vbytebitmapSanders"   
	./BUILD ../../textos/text20gb.txt indexes/wiki20gb_bitmap8_k128 "samplerate=128;lenBitmapDiv=8; 3.vbytebitmapSanders"    

python ../../../../../utils-py/endtime.py    BB-LOG.dat                  ##LOGS-ELAPSED-TIMES

echo "################################################################"
