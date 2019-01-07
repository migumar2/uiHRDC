ln -s ../rlcsa/rlcsa_build   rlcsa_build 
ln -s ../rlcsa/rlcsa_locate  rlcsa_locate
ln -s ../rlcsa/rlcsa_display rlcsa_display

mkdir -p indexes

##BUILDs INDEXES

	rm -f B-LOG.dat*
	python ../../../utils-py/starttime.py  B-LOG.dat "Build-starts!"   ##LOGS-ELAPSED-TIMES
sh ./goBuild
	python ../../../utils-py/endtime.py    B-LOG.dat                  ##LOGS-ELAPSED-TIMES


##PERFORMS INDEXES	

	rm -f S-LOG.dat*
	python ../../../utils-py/starttime.py  S-LOG.dat "Search-starts!"   ##LOGS-ELAPSED-TIMES
	
	#rm -f Wa_log_rlcsa* Wb_log_rlcsa* P2_log_rlcsa* P5_log_rlcsa*
sh ./rlcsaLocate.sh

	#rm -f e80_log_rlcsa* e13000_log_rlcsa* 
sh ./rlcsaExtract.sh

	python ../../../utils-py/endtime.py    S-LOG.dat                  ##LOGS-ELAPSED-TIMES
