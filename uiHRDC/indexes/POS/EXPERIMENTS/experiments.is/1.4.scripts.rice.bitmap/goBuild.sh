ln -s ../../II_docs/BUILD_RICE_REPAIR_T   BUILD
ln -s ../../II_docs/SEARCH_RICE_REPAIR_T  SEARCH
mkdir indexes


echo "################################################################" 

#./BUILD ../../text2gb/wiki_src2gb.txt indexes/wiki2gb_bitmap8 "lenBitmapDiv=8" & 
./BUILD ../../text2gb/wiki_src2gb.txt indexes/wiki2gb_bitmap1 "lenBitmapDiv=1"  

echo "################################################################"

