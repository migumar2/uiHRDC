ln -s ../rlcsa/rlcsa_build   rlcsa_build 
ln -s ../rlcsa/rlcsa_locate  rlcsa_locate
ln -s ../rlcsa/rlcsa_display rlcsa_display

mkdir -p indexes
sh ./goBuild


	#rm -f Wa_log_rlcsa* Wb_log_rlcsa* P2_log_rlcsa* P5_log_rlcsa*
sh ./rlcsaLocate.sh

	#rm -f e80_log_rlcsa* e13000_log_rlcsa* 
sh ./rlcsaExtract.sh

