ln -s ../../II_docs/BUILDP_VBYTE_SANDERS_NOTEXT BUILDP
ln -s ../../II_docs/BUILD_VBYTE_SANDERS_NOTEXT  BUILD
ln -s ../../II_docs/SEARCH_VBYTE_SANDERS_NOTEXT SEARCH
mkdir indexes

echo "################################################################" 

#./BUILD ../../textos/text20gb.txt indexes/wiki20gb_bitmap1_k16 "samplerate=16;lenBitmapDiv=1"
#./BUILDP ../../20gb indexes/wiki20gb_bitmap1_k16 "samplerate=16;lenBitmapDiv=1 ; 3.vbytebitmapSanders"   
./BUILD ../../textos/text20gb.txt indexes/wiki20gb_bitmap1_k16 "samplerate=16;lenBitmapDiv=1 ; 3.vbytebitmapSanders"   


#./BUILD ../../textos/text20gb.txt indexes/wiki20gb_bitmap1_k128 "samplerate=128;lenBitmapDiv=1"  
#./BUILDP ../../20gb indexes/wiki20gb_bitmap1_k128 "samplerate=128;lenBitmapDiv=1; 3.vbytebitmapSanders"    
./BUILD ../../textos/text20gb.txt indexes/wiki20gb_bitmap1_k128 "samplerate=128;lenBitmapDiv=1; 3.vbytebitmapSanders"    

echo "################################################################"

