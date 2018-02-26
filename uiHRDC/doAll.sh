# Uncompresses Datasets
cd data
	sh prepare_data.sh
	cd ..

#Runs experiments for the self-indexes
cd self-indexes
	sh runAllExperiments.sh
	cd ..

#Runs experiments for the non-position and positional inverted indexes
cd indexes
	sh runAllExperiments.sh
	cd ..

	#cd NOPOS
	#	sh compileAllSources.sh
	#	sh runExperiments.sh
	#	cd ..
	#cd POS
	#	sh compileAllSources.sh
	#	sh runExperiments.sh
	#	cd ..

#Cleans unnecesary data (compressed Datasets)
cd data
	sh clean.sh
	cd ..

sleep 10
#Now generating benchmark/report/report.pdf that creates the main figures in the paper.

cd benchmark/report
	sh goReport.sh
	cd ../../






