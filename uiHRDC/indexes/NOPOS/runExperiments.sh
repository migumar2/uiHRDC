echo "========================================================="
echo "Setting links to data and patterns directories =========="
echo "========================================================="

cd data
sh clean.sh
sh go20gb.sh
#sh go200mb.sh
cd ..


echo "========================================================="
echo "running Claude et al. tests - tests ====================="
echo "========================================================="
cd EXPERIMENTS
	sh clean.sh
	sh golinks.sh
	cd experiments.20g
		sh goBuildSearchClean.sh
		#sh goBuildSearchCleanX.sh   ##runs only some experiments
		cd ..
	sh clean.sh
	cd ..



#now process Eliasfano* tests. ==============
#[0V2014] Ottaviano & Venturini @ sigir 2014 paper
echo "============================================================"
echo "running OV2014 - preparing source data ====================="
echo "============================================================"
cd EliasFano.OV14
sh golinks.sh 

#IF NEEDED, generate id-based patterns from original text-patterns using "our search-programs"
cd II_docs.EliasFano_formato.ver.leeme
    ln -s ../../II_docs/lib/ lib
	sh compile.sh
	cd ..
cd 2.1.extract.ids.patterns
	sh goBuild.sh
	sh goSearch.sh
	sh clean.sh
	cd ..

echo "============================================"
echo "running OV2014 - tests ====================="
echo "============================================"
cd partitioned_elias_fano/partitioned_elias_fano.nopos
	sh compile.sh   
	echo "Running tests, please wait (log results to output.experiments.txt)"
	rm -f output.experiments.txt
	sh go20gbQueries.sh 1>>output.experiments.txt 2>>output.experiments.txt
	grep "Documents" output.experiments.txt
	grep "Time per occ" output.experiments.txt
	sh goCollectResults.sh

	rm -rf indexes 
	sh clean.sh
	cd ../../

sh clean.sh
cd ..


echo "========================================================="
echo "cleaning links to data and patterns directories =========="
echo "========================================================="
cd data
sh clean.sh
cd ..
cd EXPERIMENTS
sh clean.sh
cd ..


