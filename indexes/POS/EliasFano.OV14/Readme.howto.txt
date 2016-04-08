

The attached package contains the source code, dataset (2gb) and patterns used in our paper:
"Universal Indexes for Highly Repetitive Document Collections"
submitted to Information Systems Journal in April 2016 
by Claude, F; Fariña, F.; Martinez-Prieto, M.; and Navarro, G.

In particular, you can find the source code for the following techniques found there:
  - EF-OPT, OPT-PFD, INTERPOLATIVE, VARINT

For this part, we used the implementations from [0V2014] Ottaviano & Venturini @ sigir 2014 paper.
Yet, we have to provide the "input collection and query patterns" in the format their indexer 
and search programs require.

Apart from that, in our positional scenario, we avoided modifying their code as much as possible,
therefore, for measuring times, we followed this strategy:
   - [a] Measure the time needed for parsing the patterns (into sequence of int ids) using our code:
        ** after that timing, we also obtain, for each query, a sequence of id-patterns to look for in next step.
   - [b] Measure intersection time using [OV2014] code: 
        ** after that timing, we also obtain, for each query, a sequence of offsets for the occurrences. We use it in next step.
   - [c] Measure the time needed to run "merge2occs" process: 
        Given a sequence of absolute offsets <occ1>,<occ2>, ... , and an array markingthe offset of beginning 
        of each document, it returns a sequence <doc-id, rel-occ1><doc-id, rel-occ2>...
        This timing is also done with our code. 

   * The final times for each query must be obtained as:  ([a]+[c]) + [b]
     - To compute ([a]+[c]) we use the script "./2.1.aaa.EliasFano.PartialMeasurement/goSearch.sh"
     - To compute [b], we will use "./partitioned.EF.pos/go2gblog.sh" script

The original source code for [OV2014] can be found here:  https://github.com/ot/partitioned_elias_fano

 **   Authors had this "pre-requisites"
 **   The code is tested on Linux with GCC 4.9 and OSX Mavericks with Clang.
 **   The following dependencies are needed for the build.
 **   CMake >= 2.8, for the build system
 **   Boost >= 1.42

We tested it in ubuntu 14.04 LTS with:
  - cmake 2.8.12.2
  - g++ 4.8.2
  - libboost 1.46.1
  - snappy-1.1.1

-------------------------------------------
In order to run our experiments you can:

1. Obtain formated input dataset and query patterns.
  ** assume ../data/text2gb   contains the source dataset
  ** assume ../data/patterns contains the source query patterns.

  - sh golinks.sh 

  - cd II_docs.EliasFano_formato.ver.leeme; sh compile; cd ..
  - cd "2.1.aaa.EliasFano.PartialMeasurement"
  - sh goBuild.sh
  - sh goSearch.sh
  - sh clean.sh
  - cd ..
  - Now directory TESTDATA will contain the required files input files (dataset and patterns)
	./TESTDATA/2gb.docs
	./TESTDATA/2gb.freqs
	./TESTDATA/2gb.sizes

	./TESTDATA/2g.wa.ids
	./TESTDATA/2g.wb.ids
	./TESTDATA/2g.p2.ids
	./TESTDATA/2g.p5.ids

  **** In addition, files  "2.1.aaa.EliasFano.PartialMeasurement/*.dat" contain the query times for [a]+[c] steps ****
	
 2. Run [OV2014] code:
   - cd partitioned_elias_fano
   - If not installed libboost/snappy --> follow download.sh script (or see info at  https://github.com/ot/partitioned_elias_fano)
   - cd partitioned.EF.pos  (we modified some files and added some scripts over [OV2014]).
      * sh compile.sh 
      * script -a output.experiments.txt #initiates script writing to output.experiments.txt
      * sh go2gbPOS.sh
      * exit   ##closes the script -a
      * ## the same as 3 previous lines would be $sh go2gbPOS.sh 1>>output.experiments.txt 2>>output.experiments.txt
  
       "output.experiments.txt" file will contain something like:

	< output for create index command>
		====================================================
		== CREATING 2gb OPT ===============================
		====================================================
		user@localhost:~/Escritorio/POS/EliasFano.OV14/partitioned_elias_fano/partitioned.EF.pos$ ./create_freq_index opt tmp/out 2gb.opt --check
		2016-04-07 12:56:30: Processing 462872628 documents
		2016-04-07 12:56:30: semiasync_queue using 8 worker threads
		2016-04-07 12:59:08: Processed 250550 sequences, 462872628 postings
		2016-04-07 12:59:34: opt collection built in 184.309 seconds
		{"type": "opt", "worker_threads": 8, "construction_time": 184.309, "construction_user_time": 1274.21}
		<TOP>: 638474181
		    m_params: 5
		    m_docs_sequences: 638083632
			m_endpoints: 512680
			    m_bits: 512672
			m_bitvectors: 637570944
			    m_bits: 637570936
		    m_freqs_sequences: 390536
			m_endpoints: 157064
			    m_bits: 157056
			m_bitvectors: 233464
			    m_bits: 233456
		2016-04-07 12:59:34: Documents: 638083632 bytes, 11.0282 bits per element
		2016-04-07 12:59:34: Frequencies: 390536 bytes, 0.00674978 bits per element
		{"type": "opt", "docs_size": 638083632, "freqs_size": 390536, "bits_per_doc": 11.0282, "bits_per_freq": 0.00674978}
		{"type": "opt", "eps1": 0.03, "eps2": 0.3, "fix_cost": 64, "docs_avg_part": 136.82, "freqs_avg_part": 45062}
		2016-04-07 12:59:35: Checking the written data, just to be extra safe...
		2016-04-07 12:59:43: Everything is OK!
		2016-04-07 12:59:44: Reading sizes...
		2016-04-07 12:59:54: Storing max weight for each list...
		2016-04-07 13:00:05: 250550 list processed

	<output for search command>
	
		== Wa ==============================================
		2016-04-07 13:00:14: Loading index from indexes/2gb.opt
		2016-04-07 13:00:14: Performing opt queries [1000]
		2016-04-07 13:00:14: ---- opt and
		2016-04-07 13:00:14: Tot time (micros): 3279.67
		2016-04-07 13:00:14: Time per occ (micros): 0.0236828 occs= 99228
		2016-04-07 13:00:14: Mean (micros per pat): 2.35
		2016-04-07 13:00:14: Mean (micros per occ): 0.0236828
		2016-04-07 13:00:14: 50% quantile: 1
		2016-04-07 13:00:14: 90% quantile: 5.33333
		2016-04-07 13:00:14: 95% quantile: 8.66667
		{"type": "opt", "query": "and", "tottime (micros)": 3279.67, "noccs": 99228, "avg (micros per pat)": 2.35, "avg (micros per occ)": 0.0236828, "q50": 1, "q90": 5.33333, "q95": 8.66667}

	------
	You can see the "size of the index by looking at line":
	  - 2016-04-07 12:59:34: Documents: 638083632 bytes, 11.0282 bits per element

	  $ grep "Documents" output.experiments.txt

	You can see "AND-QUERY time by looking at line": 
	  - 2016-04-07 13:00:14: Time per occ (micros): 0.0236828 occs= 99228
	  - 2016-04-07 13:00:14: Mean (micros per occ): 0.0236828	
		(you get both number of ocurrences and micros per pattern here).
	  $ grep "Time per occ" output.experiments.txt
	
	Therefore, we obtain the EF-OPT technique obtains a document-index of size 132564440 bytes, and it solves our Wa queries 
	(low freq words) at around 0.0236828 microsec per occurrence. The number of occs is 99228.

	  **** NOTE THAT this times correspond to step [B] in the search process. ****
	  **** You can use the spreedshet "results.ods" to sum times ([a]+[c]) + [B] 

	If you check the rest of file "output.experiments.txt", you obtain query-times for patterns Wb (frequent words), P2 (2-word AND),
	and P5 (5-word AND) for EF-OPT. 

	Apart from the results for:
	 * EF-OPT technique, you also obtain results for 
	 * EF-UNIF (non optimized see [OV2014]), 
	 * OPTPFOR
	 * INTERPOLATIVE
	 * VARINT

------------------


   

      
