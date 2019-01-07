rm -f ALL-LOG*



# Uncompresses Datasets
  cd data
  	sh prepare_data.sh
  	cd ..


## starts timing for the experiments #################################################
python ./utils-py/starttime.py  ALL-LOG.dat "Build-starts!"       ##LOGS-ELAPSED-TIMES

	#Runs experiments for the non-position and positional inverted indexes
	cd indexes
		sh runAllExperiments.sh
		cd ..


	#Runs experiments for the self-indexes
	 cd self-indexes
		sh runAllExperiments.sh
		cd ..

python ./utils-py/endtime.py  ALL-LOG.dat                         ##LOGS-ELAPSED-TIMES
## ends timing for the experiments #################################################



#Cleans unnecesary data (compressed Datasets)
cd data
	sh clean.sh
	cd ..

sleep 10
#Now generating benchmark/report/report.pdf that creates the main figures in the paper.

 cd benchmark/report
 	sh goReport.sh
 	cd ../../






