ln -s ../../II_docs_large/BUILDP_REPAIR_NOSKIP_NOTEXT BUILDP
ln -s ../../II_docs_large/BUILD_REPAIR_NOSKIP_NOTEXT  BUILD
ln -s ../../II_docs_large/SEARCH_REPAIR_NOSKIP_NOTEXT SEARCH
ln -s ../../II_docs_large/lib lib
mkdir indexes

echo "################################################################" 


./BUILDP ../../wiki100gb indexes/wiki100gb "filename=indexes/wiki100gb; lenBitmapDiv=1; maxMemRepair=2000000000; replacesPerIter = 1000; repairBreak=0.0000001; path2convert=./lib/convert; path2build_bitmap=./lib/build_bitmap;  path2build_index=./lib/build_index_gonzalo; path2irepairgonzalo=./lib/irepair; path2compress_dictionary=./lib/compressDictionary"


echo "################################################################"



