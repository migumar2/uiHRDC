#ln -s ../../II_docs/BUILD_REPAIR_NOSKIP_NOTEXT  BUILD
#ln -s ../../II_docs/SEARCH_REPAIR_NOSKIP_NOTEXT SEARCH

ln -s ../../II_docs/BUILD_REPAIR_NOSKIP_REPAIR_T   BUILD
ln -s ../../II_docs/SEARCH_REPAIR_NOSKIP_REPAIR_T  SEARCH
ln -s ../../II_docs/lib lib
mkdir indexes

echo "################################################################" 


./BUILD ../../text2gb/wiki_src2gb.txt indexes/wiki2gb "filename=indexes/wiki2gb; lenBitmapDiv=1; maxMemRepair=2000000000; replacesPerIter = 1000; repairBreak=0.0000005; path2convert=./lib/convert; path2build_bitmap=./lib/build_bitmap;  path2build_index=./lib/build_index_gonzalo; path2irepairgonzalo=./lib/irepair; path2compress_dictionary=./lib/compressDictionary"

##./BUILD ../../text2gb/wiki_src2gb.txt indexes/wiki2gb "filename=indexes/wiki2gb; lenBitmapDiv=1; maxMemRepair=500000000; replacesPerIter = 1000; repairBreak=0.0100000; path2convert=./lib/convert; path2build_bitmap=./lib/build_bitmap; path2build_index=./lib/build_index32b; path2compress_dictionary=./lib/compressDictionary"

echo "################################################################"







