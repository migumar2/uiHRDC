ln -s ../../II_docs/BUILDP_VBYTE_NOTEXT   BUILDP
ln -s ../../II_docs/BUILD_VBYTE_NOTEXT   BUILD
ln -s ../../II_docs/SEARCH_VBYTE_NOTEXT  SEARCH
mkdir indexes


echo "################################################################" 

#./BUILD ../../textos/text20gb.txt indexes/wiki20gb_bitmap8 "lenBitmapDiv=8"  
#./BUILDP ../../20gb indexes/wiki20gb_bitmap8 "lenBitmapDiv=8"  

#./BUILDP ../../20gb indexes/wiki20gb_bitmap1 "lenBitmapDiv=1; 2.1.scripts.vbytebitmap"  
./BUILD ../../textos/text20gb.txt indexes/wiki20gb_bitmap1 "lenBitmapDiv=1; 2.1.scripts.vbytebitmap"  
echo "################################################################"
