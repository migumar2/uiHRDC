

echo "========================================================="
echo "Compiling Claude et al. inverted index representations =="
echo "========================================================="
cd II_docs
	cd ilists.gap.imp
		#generates the package with each posting-list representation
		bash generatelibs.sh
		cd allbinaries
			echo "binaries built:..............................."
		    ls -l build_bitmap build_index build_index_gonzalo compressDictionary convert irepair irepair_bal lzmaEncoder
		    ls -l il_1.4golomb_nosampling_lenlist_64bit.a il_2.0.vbyte_64bit.a il_2.moffatBitmapSpireVbyte_64bit.a il_3.sandersBitmapByteCodes_64bit.a 
		    ls -l il_4.2.repair_64bit.a  il_4.3.repair_skipping_64bit_2015.a il_4.3.2.repair_skipping_sanders_64bit.a il_4.3.1.repair_skipping_moffat_64bit.a
		    ###
		    ls -l il_7.1.simple9escape.64bit.a il_8.pfordelta_64bit.a il_9.QMXCODING_64bit.a  il_9.QMXCODINGSIMD_64bit.a
		             
			sleep 10
			cd ..
		cd ..
		
	#Now creates the BUILD/SEARCH programs for each representation
	bash clean.sh	
	bash compile.sh
		echo "binaries built:..............................."
		ls -l BUILD_RICE_REPAIR_T  BUILD_VBYTE_REPAIR_T  BUILD_VBYTE_MOFFAT_REPAIR_T  BUILD_VBYTE_SANDERS_REPAIR_T  BUILD_REPAIR_NOSKIP_REPAIR_T  BUILD_REPAIR_SKIP_MOFFAT_REPAIR_T  BUILD_REPAIR_SKIP_REPAIR_T  BUILD_REPAIR_SKIP_SANDERS_REPAIR_T  BUILD_VBYTE_LZMA_REPAIR_T  BUILD_SIMPLE9_ESC_REPAIR_T  BUILD_PFORDELTA_REPAIR_T  BUILD_QMXCODING_REPAIR_T  BUILD_QMXCODINGSIMD_REPAIR_T  BUILD_VBYTE_PLAIN

		ls -l SEARCH_RICE_REPAIR_T  SEARCH_VBYTE_REPAIR_T  SEARCH_VBYTE_MOFFAT_REPAIR_T  SEARCH_VBYTE_SANDERS_REPAIR_T  SEARCH_REPAIR_NOSKIP_REPAIR_T  SEARCH_REPAIR_SKIP_MOFFAT_REPAIR_T  SEARCH_REPAIR_SKIP_REPAIR_T  SEARCH_REPAIR_SKIP_SANDERS_REPAIR_T  SEARCH_VBYTE_LZMA_REPAIR_T  SEARCH_SIMPLE9_ESC_REPAIR_T  SEARCH_PFORDELTA_REPAIR_T  SEARCH_QMXCODING_REPAIR_T  SEARCH_QMXCODINGSIMD_REPAIR_T  SEARCH_VBYTE_PLAIN
		
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
        ls -l BUILD_VBYTE_PLAIN SEARCH_VBYTE_PLAIN

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
		
	cd partitioned.EF.pos
		sh compile.sh
		echo "binaries built:..............................."
		ls -l create_freq_index create_wand_data queries

		sleep 10
		cd ../../

echo "========================================================="
cd ..



