ln -s ../../II_docs/BUILD_RICE_SANDERS_REPAIR_T   BUILD
ln -s ../../II_docs/SEARCH_RICE_SANDERS_REPAIR_T  SEARCH
mkdir indexes


echo "################################################################" 

./BUILD ../../textos/einstein.en.txt indexes/einstein450mb_bitmap8_k16 "samplerate=16;lenBitmapDiv=8"&  
./BUILD ../../textos/einstein.en.txt indexes/einstein450mb_bitmap8_k64 "samplerate=64;lenBitmapDiv=8"&  
./BUILD ../../textos/einstein.en.txt indexes/einstein450mb_bitmap1_k16 "samplerate=16;lenBitmapDiv=1"&  
./BUILD ../../textos/einstein.en.txt indexes/einstein450mb_bitmap1_k64 "samplerate=64;lenBitmapDiv=1"  

echo "################################################################"

