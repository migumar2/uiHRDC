ln -s ../../II_docs/BUILDP_RICE_NOTEXT   BUILDP
ln -s ../../II_docs/BUILD_RICE_NOTEXT   BUILD
ln -s ../../II_docs/SEARCH_RICE_NOTEXT  SEARCH
mkdir indexes


echo "################################################################" 

#./BUILD ../../textos/text20gb.txt indexes/wiki20gb_bitmap8 "lenBitmapDiv=8"  
#./BUILDP ../../20gb indexes/wiki20gb_bitmap8 "lenBitmapDiv=8"  

#./BUILDP ../../20gb indexes/wiki20gb_bitmap1 "lenBitmapDiv=1; 1.4.scripts.rice.bitmap"  
./BUILD ../../textos/text20gb.txt indexes/wiki20gb_bitmap1 "lenBitmapDiv=1; 1.4.scripts.rice.bitmap"  

echo "################################################################"

