#!/bin/bash
rm -f E.dat
rm -f N.dat
ln -s ../../II_docs/SEARCH_REPAIR_SKIP_MOFFAT_NOTEXT SEARCH
ln -s ../../II_docs/lib lib
echo "################################################################" 

SAMPLEK="1 4 16 64"

###
	for i in $SAMPLEK;
	do
		echo "- [Wa]-- II-REPAIR-SKIPPING-SAMPLING-MOFFAT SAMPLEK = $i -----------------------"
		./sampleK indexes/wiki20gb $i
		./SEARCH indexes/wiki20gb E  500 "int2=skip;intn=skip" < ../../patterns/wiki20g.words.f1_1000
		./appendLastLine E.dat sampleK=$i
	done
	mv  E.dat  E_Wa.4.3.1repairG.skipping.moffat.dat 

###
	for i in $SAMPLEK;
	do
		echo "- [Wb]-- II-REPAIR-SKIPPING-SAMPLING-MOFFAT SAMPLEK = $i -----------------------"
		./sampleK indexes/wiki20gb $i
		./SEARCH indexes/wiki20gb E  500 "int2=skip;intn=skip" < ../../patterns/wiki20g.words.f1001_100k
		./appendLastLine E.dat sampleK=$i
	done
	mv  E.dat E_Wb.4.3.1repairG.skipping.moffat.dat

###
	for i in $SAMPLEK;
	do
		echo "- [P2]-- II-REPAIR-SKIPPING-SAMPLING-MOFFAT SAMPLEK = $i -----------------------"
		./sampleK indexes/wiki20gb $i
		./SEARCH indexes/wiki20gb N   4 "int2=skipE;intn=skipE" < ../../patterns/wiki20g_2_2
		./appendLastLine N.dat sampleK=$i
	done
	mv  N.dat N_p2.4.3.1repairG.skipping.moffat.dat 

###
	for i in $SAMPLEK;
	do
		echo "- [P4]-- II-REPAIR-SKIPPING-SAMPLING-MOFFAT SAMPLEK = $i -----------------------"
		./sampleK indexes/wiki20gb $i
		./SEARCH indexes/wiki20gb N   4 "int2=skipE;intn=skipE" < ../../patterns/wiki20g_5_5
		./appendLastLine N.dat sampleK=$i
	done
	mv  N.dat N_p5.4.3.1repairG.skipping.moffat.dat 


echo "################################################################"


#./BUILDP ../../20gb indexes/wiki20gbbitmap "filename=indexes/wiki20gbbitmap; lenBitmapDiv=8; maxMemRepair=2000000000; replacesPerIter = 1000; repairBreak=0.0000004; path2convert=./lib/convert; path2build_bitmap=./lib/build_bitmap;  path2build_index=./lib/build_index_gonzalo; path2irepairgonzalo=./lib/irepair; path2compress_dictionary=./lib/compressDictionary" 



#echo "################################################################" 
#./SEARCH indexes/torsen200mb I 1  "int2=skip;intn=skip" < ../patterns/torsen200mb_1_1
#./SEARCH indexes/torsen2000mb E 1 "int2=skip;intn=skip" < ../patterns/torsen2000mb_1_1_10k
#./SEARCH indexes/torsen2000mb I 1 "int2=skip;intn=skip" < ../patterns/torsen2000mb_2_2_10k
#./SEARCH indexes/torsen2000mb N 1 "int2=skip;intn=skip" < ../patterns/torsen2000mb_2_5_10k
#echo "################################################################"
#cp E.dat E.4.3.repair.skipping.dat
#cp I.dat I.4.3.repair.skipping.dat
#cp N.dat N.4.3.repair.skipping.dat
