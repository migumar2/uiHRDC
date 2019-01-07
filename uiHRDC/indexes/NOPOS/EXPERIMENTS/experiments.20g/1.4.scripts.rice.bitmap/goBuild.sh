ln -s ../../II_docs/BUILDP_RICE_NOTEXT   BUILDP
ln -s ../../II_docs/BUILD_RICE_NOTEXT   BUILD
ln -s ../../II_docs/SEARCH_RICE_NOTEXT  SEARCH
mkdir indexes

rm -f B-LOG.dat*
rm -f BB-LOG.dat*

echo "################################################################" 

#./BUILD ../../textos/text20gb.txt indexes/wiki20gb_bitmap8 "lenBitmapDiv=8"  
#./BUILDP ../../20gb indexes/wiki20gb_bitmap8 "lenBitmapDiv=8"  

#./BUILDP ../../20gb indexes/wiki20gb_bitmap1 "lenBitmapDiv=1; 1.4.scripts.rice.bitmap"  


python ../../../../../utils-py/starttime.py  B-LOG.dat "Build-starts!"   ##LOGS-ELAPSED-TIMES
	./BUILD ../../textos/text20gb.txt indexes/wiki20gb_bitmap1 "lenBitmapDiv=1; 1.4.scripts.rice.bitmap"  
python ../../../../../utils-py/endtime.py    B-LOG.dat                  ##LOGS-ELAPSED-TIMES

python ../../../../../utils-py/starttime.py  BB-LOG.dat "Build-starts!"   ##LOGS-ELAPSED-TIMES
	./BUILD ../../textos/text20gb.txt indexes/wiki20gb_bitmap8 "lenBitmapDiv=8; 1.4.scripts.rice.bitmap"  
python ../../../../../utils-py/endtime.py    BB-LOG.dat                  ##LOGS-ELAPSED-TIMES

echo "################################################################"


