ln -s ../../II_docs/BUILDP_REPAIR_NOSKIP_NOTEXT BUILDP
ln -s ../../II_docs/BUILD_REPAIR_NOSKIP_NOTEXT  BUILD
ln -s ../../II_docs/SEARCH_REPAIR_NOSKIP_NOTEXT SEARCH
ln -s ../../II_docs/lib lib
mkdir indexes

echo "################################################################" 


#./BUILD ../../textos/text20gb.txt indexes/wiki20gb "filename=indexes/wiki20gb; lenBitmapDiv=1; maxMemRepair=900000000; replacesPerIter = 2000; repairBreak=0.01; path2convert=./lib/convert; path2build_bitmap=./lib/build_bitmap; path2build_index=./lib/build_index32b; path2compress_dictionary=./lib/compressDictionary"

#./BUILDP ../../20gb indexes/wiki20gb "filename=indexes/wiki20gb; lenBitmapDiv=1; maxMemRepair=900000000; replacesPerIter = 2000; repairBreak=0.01; path2convert=./lib/convert; path2build_bitmap=./lib/build_bitmap; path2build_index=./lib/build_index32b; path2compress_dictionary=./lib/compressDictionary"

#./BUILDP ../../20gb indexes/wiki20gb "filename=indexes/wiki20gb; lenBitmapDiv=1; maxMemRepair=2000000000; replacesPerIter = 1000; repairBreak=0.0004; path2convert=./lib/convert; path2build_bitmap=./lib/build_bitmap; path2build_index=./lib/build_index; path2compress_dictionary=./lib/compressDictionary"

#./BUILDP ../../20gb indexes/wiki20gb "filename=indexes/wiki20gb; lenBitmapDiv=1; maxMemRepair=2000000000; replacesPerIter = 1000; repairBreak=0.0000; path2convert=./lib/convert; path2build_bitmap=./lib/build_bitmap;  path2build_index=./lib/build_index_gonzalo; path2irepairgonzalo=./lib/irepair_bal; path2compress_dictionary=./lib/compressDictionary"

./BUILD ../../textos/text20gb.txt indexes/wiki20gb "filename=indexes/wiki20gb; lenBitmapDiv=1; maxMemRepair=2000000000; replacesPerIter = 1000; repairBreak=0.0000; path2convert=./lib/convert; path2build_bitmap=./lib/build_bitmap;  path2build_index=./lib/build_index_gonzalo; path2irepairgonzalo=./lib/irepair_bal; path2compress_dictionary=./lib/compressDictionary"

echo "################################################################"


