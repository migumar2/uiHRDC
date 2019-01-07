ln -s ../../II_docs/BUILDP_REPAIR_SKIP_MOFFAT_NOTEXT BUILDP
ln -s ../../II_docs/BUILD_REPAIR_SKIP_MOFFAT_NOTEXT  BUILD
ln -s ../../II_docs/SEARCH_REPAIR_SKIP_MOFFAT_NOTEXT SEARCH
ln -s ../../II_docs/lib lib

echo "################################################################" 
echo "compiling sampleK.c and appendLastLine.c used at query-time"
gcc -o appendLastLine appendLastLine.c
gcc -o sampleK sampleK.c
echo "################################################################" 

mkdir indexes

rm -f B-LOG.dat*
python ../../../../../utils-py/starttime.py  B-LOG.dat "Build-starts!"   ##LOGS-ELAPSED-TIMES

echo "################################################################" 


#./BUILD ../../textos/text20gb.txt indexes/wiki20gb "filename=indexes/wiki20gb; lenBitmapDiv=1; maxMemRepair=900000000; replacesPerIter = 2000; repairBreak=0.01; path2convert=./lib/convert; path2build_bitmap=./lib/build_bitmap;  path2build_index=./lib/build_index_gonzalo; path2irepairgonzalo=./lib/irepair; path2compress_dictionary=./lib/compressDictionary"

#./BUILDP ../../20gb indexes/wiki20gb "filename=indexes/wiki20gb; lenBitmapDiv=1; maxMemRepair=2000000000; replacesPerIter = 1000; repairBreak=0.0000004; path2convert=./lib/convert; path2build_bitmap=./lib/build_bitmap;  path2build_index=./lib/build_index_gonzalo; path2irepairgonzalo=./lib/irepair; path2compress_dictionary=./lib/compressDictionary" 

./BUILD ../../textos/text20gb.txt indexes/wiki20gb "filename=indexes/wiki20gb; lenBitmapDiv=1; maxMemRepair=2000000000; replacesPerIter = 1000; repairBreak=0.0000004; path2convert=./lib/convert; path2build_bitmap=./lib/build_bitmap;  path2build_index=./lib/build_index_gonzalo; path2irepairgonzalo=./lib/irepair; path2compress_dictionary=./lib/compressDictionary" 




#./BUILDP ../../20gb indexes/wiki20gb_b "filename=indexes/wiki20gb_b; lenBitmapDiv=1; maxMemRepair=2000000000; replacesPerIter = 1000; repairBreak=0.000; path2convert=./lib/convert; path2build_bitmap=./lib/build_bitmap;  path2build_index=./lib/build_index_gonzalo; path2irepairgonzalo=./lib/irepair; path2compress_dictionary=./lib/compressDictionary"


#rm E.dat
#./SEARCH indexes/wiki20gb E  1 "int2=skip;intn=skip" < ../../patterns/wiki20g.words.f1_1000
#./SEARCH indexes/wiki20gb_b E  1 "int2=skip;intn=skip" < ../../patterns/wiki20g.words.f1_1000
#mv E.dat E.sizes.dat

echo "################################################################"
python ../../../../../utils-py/endtime.py    B-LOG.dat                  ##LOGS-ELAPSED-TIMES
