ln -s ../../II_docs/BUILDP_VBYTE_NOTEXT   BUILDP
ln -s ../../II_docs/BUILD_VBYTE_NOTEXT   BUILD
ln -s ../../II_docs/SEARCH_VBYTE_NOTEXT  SEARCH
mkdir indexes

rm -f B-LOG.dat*
rm -f BB-LOG.dat*


echo "################################################################" 

python ../../../../../utils-py/starttime.py  BB-LOG.dat "Build-starts!"   ##LOGS-ELAPSED-TIMES
	./BUILD ../../textos/text20gb.txt indexes/wiki20gb_bitmap8 "lenBitmapDiv=8; 2.1.scripts.vbytebitmap8"   
	#./BUILDP ../../20gb indexes/wiki20gb_bitmap8 "lenBitmapDiv=8"  
python ../../../../../utils-py/endtime.py    BB-LOG.dat                  ##LOGS-ELAPSED-TIMES


python ../../../../../utils-py/starttime.py  B-LOG.dat "Build-starts!"   ##LOGS-ELAPSED-TIMES
	#./BUILDP ../../20gb indexes/wiki20gb_bitmap1 "lenBitmapDiv=1; 2.1.scripts.vbytebitmap"  
	./BUILD ../../textos/text20gb.txt indexes/wiki20gb_bitmap1 "lenBitmapDiv=1; 2.1.scripts.vbytebitmap1" 
python ../../../../../utils-py/endtime.py    B-LOG.dat                  ##LOGS-ELAPSED-TIMES

echo "################################################################"
