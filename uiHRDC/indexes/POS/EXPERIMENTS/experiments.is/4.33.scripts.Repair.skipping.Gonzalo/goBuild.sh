
#ln -s ../../II_docs/BUILD_REPAIR_SKIP_PLAIN   BUILD
#ln -s ../../II_docs/SEARCH_REPAIR_SKIP_PLAIN  SEARCH
#ln -s ../../II_docs/lib lib

ln -s ../../II_docs/BUILD_REPAIR_SKIP_REPAIR_T   BUILD
ln -s ../../II_docs/SEARCH_REPAIR_SKIP_REPAIR_T  SEARCH
ln -s ../../II_docs/lib lib
mkdir indexes

rm -f B-LOG.dat*
python ../../../../../utils-py/starttime.py  B-LOG.dat "Build-starts!"   ##LOGS-ELAPSED-TIMES

echo "################################################################" 

##./BUILD ../../text2gb/wiki_src2gb.txt indexes/wiki2gb "filename=indexes/wiki2gb; lenBitmapDiv=1; maxMemRepair=900000000; replacesPerIter = 1000; repairBreak=0.0100; path2convert=./lib/convert; path2build_bitmap=./lib/build_bitmap; path2build_index=./lib/build_index32b; path2compress_dictionary=./lib/compressDictionary"


./BUILD ../../text2gb/wiki_src2gb.txt indexes/wiki2gb "filename=indexes/wiki2gb; lenBitmapDiv=1; maxMemRepair=2000000000; replacesPerIter = 1000; repairBreak=0.0000005; path2convert=./lib/convert; path2build_bitmap=./lib/build_bitmap;  path2build_index=./lib/build_index_gonzalo; path2irepairgonzalo=./lib/irepair; path2compress_dictionary=./lib/compressDictionary"

#./BUILD ../../text2gb/wiki_src2gb.txt indexes/wiki2gb "filename=indexes/wiki2gb; lenBitmapDiv=1; maxMemRepair=2000000000; replacesPerIter = 1000; repairBreak=0.000000; path2convert=./lib/convert; path2build_bitmap=./lib/build_bitmap;  path2build_index=./lib/build_index; path2irepairgonzalo=./lib/irepair; path2compress_dictionary=./lib/compressDictionary"

echo "################################################################"

python ../../../../../utils-py/endtime.py    B-LOG.dat                  ##LOGS-ELAPSED-TIMES

