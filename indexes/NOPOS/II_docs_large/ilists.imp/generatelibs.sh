mkdir allbinaries
rm -f allbinaries/*

cd "1.5.riceRLE"                             ; make clean; make; cp *.a ../allbinaries/; cd ..
cd "6.vbyteP7zip_acabadoMerge2yN[Fork]"      ; make clean; make; cp *.a ../allbinaries/; cp lzmalib/lzmaEncoder ../allbinaries/ ; cd ..

#######
cd "4.Repair" ;
	ln -s ../allbinaries
	#Make libraries.	
	cd "2.integradoBB.ClaudeSolucion2013" ; make clean; make; cp *.a ../allbinaries/; cd ..	
	cd "3.integradoSkippingBB.ClaudeSolucion2013_v2(nowarn)" ; make clean; make; cp *.a ../allbinaries/; cd ..
	
	## NOW THE BINARIES For Repair Representations.
	cd "2.integradoBB.ClaudeSolucion2013/srcREPAIR"  ;  make clean; make ;  cd ../..
	cd "2.integradoBB.ClaudeSolucion2013/srcREPAIR/repairGonzalo.srcs"  ;  rm -f irepair ; make  ;  cd ../../..
	cd "2.integradoBB.ClaudeSolucion2013/srcREPAIR/repairGonzalo.srcs/bal" ; rm -f irepair ; make ; cd ../../../..
	## THIS ARE THE FINAL FILES YOU WILL NEED 
	cp "2.integradoBB.ClaudeSolucion2013/srcREPAIR/convert" ../allbinaries/
	cp "2.integradoBB.ClaudeSolucion2013/srcREPAIR/build_bitmap" ../allbinaries/
	cp "2.integradoBB.ClaudeSolucion2013/srcREPAIR/build_index" ../allbinaries/
	cp "2.integradoBB.ClaudeSolucion2013/srcREPAIR/repairGonzalo.srcs/irepair" ../allbinaries/
	cp "2.integradoBB.ClaudeSolucion2013/srcREPAIR/repairGonzalo.srcs/irepair" ../allbinaries/irepair_bal
	cp "2.integradoBB.ClaudeSolucion2013/srcREPAIR/build_index_gonzalo" ../allbinaries/
	cp "2.integradoBB.ClaudeSolucion2013/srcREPAIR/compressDictionary" ../allbinaries/
	
	cd ..
	rm -f "4.Repair/allbinaries"
#######

echo ""
echo "................................................................ ."
echo ". see allbinaries directory to find all compiled/needed binaries ."
echo ". copy them to the II_docs/lib directory ....................... ."
echo "................................................................ ."
echo ""

cp allbinaries/* ../lib
