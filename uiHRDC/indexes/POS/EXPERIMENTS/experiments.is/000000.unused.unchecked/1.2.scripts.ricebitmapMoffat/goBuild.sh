ln -s ../../II_docs/BUILD_RICE_MOFFAT_REPAIR_T   BUILD
ln -s ../../II_docs/SEARCH_RICE_MOFFAT_REPAIR_T  SEARCH
mkdir indexes


echo "################################################################" 

./BUILD ../../textos/einstein.en.txt indexes/einstein450mb_bitmap8_k4  "samplerate=4 ;lenBitmapDiv=8"  
./BUILD ../../textos/einstein.en.txt indexes/einstein450mb_bitmap8_k32 "samplerate=32;lenBitmapDiv=8"  
./BUILD ../../textos/einstein.en.txt indexes/einstein450mb_bitmap1_k4  "samplerate=4 ;lenBitmapDiv=1"
./BUILD ../../textos/einstein.en.txt indexes/einstein450mb_bitmap1_k32 "samplerate=32;lenBitmapDiv=1"  

echo "################################################################"

