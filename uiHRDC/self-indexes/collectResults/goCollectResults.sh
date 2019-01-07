echo "---------------------------------------------------------------------"
echo "GENERATING gnuplot .dat files for: SLP, WSLP, LZ77, LZEND, and RLCSA "
echo "from files in "
echo "    ../SLP/evaluation/results/ "
echo "    ../SLP/evaluation/results/ "
echo "    ../LZ/evaluation/results/  "
echo "    ../LZ/evaluation/results/  "
echo "    ../RLCSAM/scripts/         "
echo "and retrieving files (already in gnuplot-type files) for WCSA from"
echo "    ../WCSA/scripts.swcsa3.is/ "
echo "---------------------------------------------------------------------"
rm -rf slp wslp lz77 lzend rlcsa wcsa
mkdir slp wslp lz77 lzend rlcsa wcsa

#python collectResults.py <source dir > <prefix>
python collectResults.py     ../SLP/evaluation/results/ slp
		cp ../SLP/evaluation/results/?slp-LOG* slp
		cd slp
			mv Bslp-LOG.dat         B-LOG.dat
			mv Bslp-LOG.dat.elapsed B-LOG.dat.elapsed
			mv Sslp-LOG.dat         S-LOG.dat
			mv Sslp-LOG.dat.elapsed S-LOG.dat.elapsed
		cd ..		
python collectResultsWSLP.py ../SLP/evaluation/results/ wslp   
		cp ../SLP/evaluation/results/?wslp-LOG* wslp
		cd wslp
			mv Bwslp-LOG.dat         B-LOG.dat
			mv Bwslp-LOG.dat.elapsed B-LOG.dat.elapsed
			mv Swslp-LOG.dat         S-LOG.dat
			mv Swslp-LOG.dat.elapsed S-LOG.dat.elapsed
		cd ..	
	
python collectResults.py     ../LZ/evaluation/results/  lz77
		cp ../LZ/evaluation/results/?lz77-LOG* lz77
		cd lz77
			mv Blz77-LOG.dat         B-LOG.dat
			mv Blz77-LOG.dat.elapsed B-LOG.dat.elapsed
			mv Slz77-LOG.dat         S-LOG.dat
			mv Slz77-LOG.dat.elapsed S-LOG.dat.elapsed
		cd ..	
		
python collectResults.py     ../LZ/evaluation/results/  lzend
		cp ../LZ/evaluation/results/?lzend-LOG* lzend
		cd lzend
			mv Blzend-LOG.dat         B-LOG.dat
			mv Blzend-LOG.dat.elapsed B-LOG.dat.elapsed
			mv Slzend-LOG.dat         S-LOG.dat
			mv Slzend-LOG.dat.elapsed S-LOG.dat.elapsed
		cd ..	


#python collectResults.py <source dir > <basename=rlcsa>
python collectResultsRLCSA.py ../RLCSAM/scripts rlcsa
	cp ../RLCSAM/scripts/?-LOG* rlcsa 

# copy files for WCSA.
sh collectResultsWCSA.sh
	cp ../WCSA/scripts.swcsa3.is/?-LOG*        wcsa


echo "---------------------------------------------------------------------"
printf 'Result files in gnuplot-data format were collected\n'
printf 'See directories: slp, wslp, lz77, lzend, rlcsa, and wcsa\n'
echo "---------------------------------------------------------------------"

