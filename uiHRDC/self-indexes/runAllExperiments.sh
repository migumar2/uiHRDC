echo "---------------------------------------------------------------------"
echo "---------------------------------------------------------------------"
echo "This script will run all the experiments for the self-indexes used in"
echo "the paper; that is: WCSA, SLP, WSLP, LZ77, LZEND, and RLCSA. "
echo ""
echo "Basically, we enter directories WCSA, LZ, RLCSAM, and SLP. Then, for " 
echo "each directory, we run the corresponding script *runExperiments.sh*, "
echo "which will: "
echo "  (1): prepare links to ../data directory (incl. texts and patterns) "
echo "  (2): compile the source code "
echo "  (3): build the required self-indexes for the source data "
echo "  (4): run search experiments (measures locate & extract times)"
echo "        (output files with the results of the experiments are created)"
echo "  (5): clean the un-needed data/links/temp-files and keep the results."
echo "" 

echo "Once all the experiments have completed, we process all the files "
echo "that contain the experimental results, and generate GNUPLOT-type  "
echo "data files. This is done by entering -collectResults- directory,  "
echo "and running script -goCollectResults.sh-. "
echo ""
echo "The generated GNUPLOT-type data files will be created within folder"
echo "collectResults, and subdirectories wcsa, lz77,lzend, rlcsa, slp, "
echo "and wslp."
echo "---------------------------------------------------------------------"
echo "---------------------------------------------------------------------"
echo ""

cd WCSA
  sh runExperiments.sh
  cd ..

cd LZ  
  sh runExperiments.sh
  cd ..
  
cd RLCSAM  
  sh runExperiments.sh
  cd ..

cd SLP  
  sh runExperiments.sh
  cd ..

cd collectResults
	sh goCollectResults.sh
	cd ..

