mkdir allbinaries
rm -f allbinaries/* 

cd "1.4Golomb_bitmap_NoSampling"               ; make clean; make; cp *.a ../allbinaries/; cd ..
cd "2.0.vbyte"                                  ; make clean; make; cp *.a ../allbinaries/; cd ..
cd "2moffatBitmapSpire"                        ; make clean; make; cp *.a ../allbinaries/; cd ..
cd "3SandersBitmapSpire"                       ; make clean; make; cp *.a ../allbinaries/; cd ..
cd "6.vbyteP7zip_acabadoMerge2yN[log_enabled]" ; make clean; make; cp *.a ../allbinaries/; cp lzmalib/lzmaEncoder ../allbinaries/ ; cd ..
cd "7.1.simple9e"                              ; make clean; make; cp *.a ../allbinaries/; cd ..
cd "8.pfordelta"                               ; make clean; make; cp *.a ../allbinaries/; cd ..
cd "9.qmx"                                     ; make clean; make; cp *.a ../allbinaries/; cd ..
cd "9.qmx.bis"                                 ; make clean; make; cp *.a ../allbinaries/; cd ..


#######
cd "4.repair" ;
	ln -s ../allbinaries
	#Make libraries.	
	cd "2.integradoBB.ClaudeSolucion2013.shortfiles.normal"            ; make clean; make; cp *.a ../allbinaries/; cd ..	
	cd "3.integradoSkippingBB.ClaudeSolucion2013_v2_shortfiles.normal" ; make clean; make; cp *.a ../allbinaries/; cd ..
	cd "4.integradoSkippingBB.Moffat.2015[ver.leeme_listo]"            ; make clean; make; cp *.a ../allbinaries/; cd ..
	cd "5.integradoSkippingBB.Sanders.2015[ver.leeme_listo]"           ; make clean; make; cp *.a ../allbinaries/; cd ..
	
	## NOW THE BINARIES For Repair Representations.
	cd "2.integradoBB.ClaudeSolucion2013.shortfiles.normal/srcREPAIR"  ;  make clean; make ;  cd ../..
	cd "2.integradoBB.ClaudeSolucion2013.shortfiles.normal/srcREPAIR/repairGonzalo.srcs"  ;  rm -f irepair ; make  ;  cd ../../..
	cd "2.integradoBB.ClaudeSolucion2013.shortfiles.normal/srcREPAIR/repairGonzalo.srcs/bal" ; rm -f irepair ; make ; cd ../../../..
	## THIS ARE THE FINAL FILES YOU WILL NEED 
	cp "2.integradoBB.ClaudeSolucion2013.shortfiles.normal/srcREPAIR/convert" ../allbinaries/
	cp "2.integradoBB.ClaudeSolucion2013.shortfiles.normal/srcREPAIR/build_bitmap" ../allbinaries/
	cp "2.integradoBB.ClaudeSolucion2013.shortfiles.normal/srcREPAIR/build_index" ../allbinaries/
	cp "2.integradoBB.ClaudeSolucion2013.shortfiles.normal/srcREPAIR/repairGonzalo.srcs/irepair" ../allbinaries/
	cp "2.integradoBB.ClaudeSolucion2013.shortfiles.normal/srcREPAIR/repairGonzalo.srcs/irepair" ../allbinaries/irepair_bal
	cp "2.integradoBB.ClaudeSolucion2013.shortfiles.normal/srcREPAIR/build_index_gonzalo" ../allbinaries/
	cp "2.integradoBB.ClaudeSolucion2013.shortfiles.normal/srcREPAIR/compressDictionary" ../allbinaries/
	
	cd ..
	rm -f "4.repair/allbinaries"
#######

echo ""
echo "................................................................ ."
echo ". see allbinaries directory to find all compiled/needed binaries ."
echo ". copy them to the ../lib directory ....................... ."
echo "................................................................ ."
echo ""

cp allbinaries/* ../lib

