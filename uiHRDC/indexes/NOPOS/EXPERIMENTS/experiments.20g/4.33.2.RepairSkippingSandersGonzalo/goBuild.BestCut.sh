#~ #!/bin/bash
#for COLLECTION in "bt" "brain" "lu" "sp" "sppm" "sweep3d" "comet" "control" "info" "obs" "plasma" ; do
#	../WPH_construccion/CWPH /home/alberto/compression/puntoFlotante/revistaDCC/data/$COLLECTION.bin indexes/$COLLECTION;
#done

ln -s ../../II_docs/BUILDP_REPAIR_SKIP_MOFFAT_NOTEXT BUILDP
ln -s ../../II_docs/BUILD_REPAIR_SKIP_MOFFAT_NOTEXT  BUILD
ln -s ../../II_docs/SEARCH_REPAIR_SKIP_MOFFAT_NOTEXT SEARCH
ln -s ../../II_docs/lib lib
mkdir indexes

echo "################################################################" 


rm -f E.dat

## best cut = 0.0000004 ??
##for CUT in "0.005" "0.0005" "0.00005" "0.000005" "0.0000010" "0.0000005" "0.0000002" "0.00000010" "0.00000005" "0.000000005" "0.0000000005" ; do
for CUT in "0.0000006"  "0.0000005"  "0.0000004"  "0.0000003" "0.0000002" ; do
                                                              
	TEXTO1="Repair de Gonzalo variando corte = "
	TEXTOFINAL=$TEXTO1" "$CUT
	echo "##################################" >> E.dat
	echo $TEXTOFINAL >> E.dat

	./BUILDP ../../20gb indexes/wiki20gb$CUT "filename=indexes/wiki20gb$CUT; lenBitmapDiv=1; maxMemRepair=2000000000; replacesPerIter = 1000; repairBreak=$CUT; path2convert=./lib/convert; path2build_bitmap=./lib/build_bitmap;  path2build_index=./lib/build_index_gonzalo; path2irepairgonzalo=./lib/irepair; path2compress_dictionary=./lib/compressDictionary" ;

	./SEARCH indexes/wiki20gb$CUT E  1 "int2=skip;intn=skip" < ../../patterns/wiki20g.words.f1_1000 ;

done

mv E.dat E.sizes.dat
echo "################################################################"
