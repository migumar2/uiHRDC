echo "---------------------------------------------------------------------"
echo "---------------------------------------------------------------------"
echo "This script will run all the experiments for the inverted indexes (II)"
echo "in the paper; that is both non-positional and positional II."
echo "For each type of II, we encode the postings lists differentially, and"
echo "then use different techniques to represent d-gaped posting lists. In"
echo "particular we use:"
echo ""
echo "   NON-POSITIONAL SCENARIO       POSITIONAL SCENARIO              "
echo "  ---------------------------  ----------------------             "
echo "   RICE                         RICE                              "
echo "   RICE-BITMAP                                                    "
echo "   VBYTE                        VBYTE                             "
echo "   VBYTE-CM                     VBYTE-CM                          "
echo "   VBYTE-ST                     VBYTE-ST                          "
echo "   VBYTE-BITMAP                                                   "
echo "   VBYTE-BITMAP-CM                                                "
echo "   VBYTE-BITMAP-ST                                                "
echo "   SIMPLE-9                     SIMPLE-9                          "
echo "   PFORDELTA                                                      "
echo "   QMX-SIMD                     QMX-SIMD                          "
echo "                                                                  "
echo "   *ELIAS-FANO-OPT              *ELIAS-FANO-OPT                   "
echo "   *OPT-PFD                     *OPT-PFD                          "
echo "   *INTERPOLATIVE               *INTERPOLATIVE                    "
echo "   *VARINT-G8IU                 *VARINT-G8IU                      "
echo "                                                                  "
echo "   RICE-RLE                                                       "
echo "   VBYTE-LZMA                   VBYTE-LZMA                        "
echo "   VBYTE-LZEND                                                    "
echo "   REPAIR                       REPAIR                            "
echo "   REPAIR-SKIPPING              REPAIR-SKIPPING                   "
echo "   REPAIR-SKIPPING-CM           REPAIR-SKIPPING-CM                "
echo "   REPAIR-SKIPPING-ST           REPAIR-SKIPPING-ST                "
echo ""
echo "Variants marked with * were modified from Ottaviano-Venturini paper"
echo "at SIGIR-2014. S9,PFORDELTA, and QMX were integrated from existing "
echo "sources. The others are our own-implementations. "
echo "To run the experiments, basically, we have to enter directories NOPOS"
echo "and POS. Then, for each directory, we run the corresponding script"
echo " *runExperiments.sh*. This script will:"
echo "  (1): prepare links to ../data directory (incl. texts and patterns) "
echo "  (2): compile the source code "
echo "  (3): build the required inverted indexes for the source data "
echo "  (4): run search experiments (measures locate & extract times)"
echo "        (output files with the results of the experiments are created"
echo "         following the format of GNUPLOT data files."
echo "  (5): clean the un-needed data/links/temp-files and keep the results."
echo "" 

echo "Recall, that once all the experiments have completed, many GNUPLOT-type"
echo " data files will be available within the directories of the experiments."
echo "---------------------------------------------------------------------"
echo "---------------------------------------------------------------------"
echo ""


rm -f NOPOS/NOPOS-LOG*
python ../utils-py/starttime.py  NOPOS/NOPOS-LOG.dat "Build-starts!"   ##LOGS-ELAPSED-TIMES
 cd NOPOS
   sh compileAllSources.sh
	  sh runExperiments.sh
	  cd ..
python ../utils-py/endtime.py  NOPOS/NOPOS-LOG.dat                     ##LOGS-ELAPSED-TIMES


rm -f POS/POS-LOG*
python ../utils-py/starttime.py  POS/POS-LOG.dat "Build-starts!"       ##LOGS-ELAPSED-TIMES
cd POS
 	sh compileAllSources.sh
 	sh runExperiments.sh
 	cd ..
python ../utils-py/endtime.py  POS/POS-LOG.dat                         ##LOGS-ELAPSED-TIMES



##   echo "NON-POSITIONAL"; ls -lR NOPOS |grep E_Wa ;sleep 5; echo "POSITIONAL"; ls -lR POS |grep E_Wa
