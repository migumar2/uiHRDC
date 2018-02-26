

echo "========================================================="
echo "Compiling Claude et al. inverted index representations =="
echo "========================================================="
cd II_docs
	cd ilists.imp
		#generates the package with each posting-list representation
		bash generatelibs.sh
		cd allbinaries
			echo "binaries built:..............................."
		    ls -l build_bitmap build_index build_index_gonzalo compressDictionary convert irepair irepair_bal lzmaEncoder
		    ls -l il_1.4golomb_nosampling_lenlist_64bit.a  il_1.5.riceRLE_64bit.a il_2.1.vbyte_bitmap_nosampling_64bit.a il_2.moffatBitmapSpireVbyte_64bit.a il_3.sandersBitmapByteCodes_64bit.a 
		    ls -l il_4.2.repair_64bit.a  il_4.3.1.repair_skipping_moffat_64bit.a il_4.3.2.repair_skipping_sanders_64bit.a il_4.3.repair_skipping_64bit.a 
		    ls -l il_5.lzend_64bit.a il_6.vbyte_lzma_64bit.a il_7.simple9_64bit.a il_8.pfordelta_64bit.a il_9.QMXCODING_64bit.a il_9.QMXCODINGSIMD_64bit.a 
		             
			sleep 10
			cd ..
		cd ..
		
	#Now creates the BUILD/SEARCH programs for each representation
	bash clean.sh	
	bash compile.sh
			echo "binaries built:..............................."
		ls -l BUILD_PFORDELTA_NOTEXT  BUILD_QMXCODING_NOTEXT   BUILD_QMXCODINGSIMD_NOTEXT   BUILD_REPAIR_NOSKIP_NOTEXT   BUILD_REPAIR_SKIP_MOFFAT_NOTEXT   BUILD_REPAIR_SKIP_NOTEXT   BUILD_REPAIR_SKIP_SANDERS_NOTEXT   BUILD_RICE_NOTEXT   BUILD_SIMPLE9_NOTEXT   BUILD_VBYTE_LZMA_NOTEXT  BUILD_VBYTE_NOTEXT 
		ls -l SEARCH_PFORDELTA_NOTEXT SEARCH_QMXCODING_NOTEXT  SEARCH_QMXCODINGSIMD_NOTEXT  SEARCH_REPAIR_NOSKIP_NOTEXT  SEARCH_REPAIR_SKIP_MOFFAT_NOTEXT  SEARCH_REPAIR_SKIP_NOTEXT  SEARCH_REPAIR_SKIP_SANDERS_NOTEXT  SEARCH_RICE_NOTEXT  SEARCH_SIMPLE9_NOTEXT  SEARCH_VBYTE_LZMA_NOTEXT  SEARCH_VBYTE_NOTEXT
		
        sleep 10
	
	cd ..


#now compile Eliasfano* source code. ==============
#[0V2014] Ottaviano & Venturini @ sigir 2014 paper
echo "============================================================"
echo "==compiling our - modified OV2014 source code           ===="
echo "============================================================"
cd EliasFano.OV14

echo "============================================================"
echo "== Compiling Partial time measurement tool:              ==="
echo "==       (II_docs.EliasFano_formato.ver.leeme)           ==="
echo "============================================================"
#IF NEEDED, generate id-based patterns from original text-patterns using "our search-programs"
cd II_docs.EliasFano_formato.ver.leeme
    ln -s ../../II_docs/lib/ lib
	sh compile.sh
		echo "binaries built:..............................."
        ls -l BUILD_VBYTE_NOTEXT SEARCH_VBYTE_NOTEXT

        sleep 10 
	cd ..

echo "============================================"
echo "compiling OV2014 - sources    =============="
echo "============================================"
cd partitioned_elias_fano
	#cd snappy-1.1.1
	#	./configure
	#	sudo make install		
	#	cd ..
		
	cd partitioned_elias_fano.nopos
		sh compile.sh
		echo "binaries built:..............................."
		ls -l create_freq_index create_wand_data queries

		sleep 10
		cd ../../

echo "========================================================="
cd ..



