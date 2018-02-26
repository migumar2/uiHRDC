

The attached package contains the source code, dataset (20gb) and patterns used in our paper:
"Universal Indexes for Highly Repetitive Document Collections"
submitted to Information Systems Journal in April 2016 
by Claude, F; Fariña, F.; Martinez-Prieto, M.; and Navarro, G.

In particular, you can find the source code for the following techniques found there:
  - EF-OPT, OPT-PFD, INTERPOLATIVE, VARINT

For this part, we used the implementations from [0V2014] Ottaviano & Venturini @ sigir 2014 paper.Yet, we have to provide the "input collection and query patterns" in the format their indexer and search programs require.

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
  ** assume ../EXPERIMENTS/textos   contains the source dataset
  ** assume ../EXPERIMENTS/patterns contains the source query patterns.

  - sh golinks.sh 

  - cd II_docs.EliasFano_formato.ver.leeme; sh compile; cd ..
  - cd 2.1.extract.ids.patterns
  - sh goBuild.sh
  - sh goSearch.sh.
  - cd ..
  - Now directory TESTDATA will contain the required files input files (dataset and patterns)
	./TESTDATA/20gb.docs
	./TESTDATA/20gb.freqs
	./TESTDATA/20gb.sizes

	./TESTDATA/wiki20g.wa.ids
	./TESTDATA/wiki20g.wb.ids
	./TESTDATA/wiki20g.p2.ids
	./TESTDATA/wiki20g.p5.ids
	
 2. Run OV2014 code:
   - cd partitioned_elias_fano
   - If not installed libboost/snappy --> follow download.sh script (or see info at  https://github.com/ot/partitioned_elias_fano)
   - cd partitioned_elias_fano.nopos  (we modified some files and added some scripts over [OV2014]).
      * sh compile.sh   
      * sh go20gbQueries.sh 1>>output.experiments.txt 2>>output.experiments.txt
  
       "output.experiments.txt" file will contain something like:

	< output for create index command>
		====================================================
		== CREATING 20gb OPT ===============================
		====================================================
		user@localhost:~/Escritorio/partitioned_elias_fano-sigir14/partitioned_elias_fano$ ./create_freq_index opt tmp/out 20gb.opt --check
		2015-04-12 03:18:05: Processing 881802 documents
		2015-04-12 03:18:05: semiasync_queue using 8 worker threads
		2015-04-12 03:23:02: Processed 534120 sequences, 1176903348 postings
		2015-04-12 03:23:17: opt collection built in 312.261 seconds
		{"type": "opt", "worker_threads": 8, "construction_time": 312.261, "construction_user_time": 2409.84}
		<TOP>: 329912725
		    m_params: 5
		    m_docs_sequences: 132564440
			m_endpoints: 873792
			    m_bits: 873784
			m_bitvectors: 131690640
			    m_bits: 131690632
		    m_freqs_sequences: 197348272
			m_endpoints: 906528
			    m_bits: 906520
			m_bitvectors: 196441736
			    m_bits: 196441728
		2015-04-12 03:23:17: Documents: 132564440 bytes, 0.901107 bits per element
		2015-04-12 03:23:17: Frequencies: 197348272 bytes, 1.34147 bits per element
		{"type": "opt", "docs_size": 132564440, "freqs_size": 197348272, "bits_per_doc": 0.901107, "bits_per_freq": 1.34147}
		{"type": "opt", "eps1": 0.03, "eps2": 0.3, "fix_cost": 64, "docs_avg_part": 140.77, "freqs_avg_part": 411.359}
		2015-04-12 03:23:19: Checking the written data, just to be extra safe...
		2015-04-12 03:23:35: Everything is OK!

	<output for search command>
		== Wa ==============================================
		2015-04-13 14:54:49: Loading index from indexes/20gb.opt
		2015-04-13 14:55:00: Performing opt queries [1000]
		2015-04-13 14:55:00: ---- opt and
		2015-04-13 14:55:00: Tot time (aprox) (micros): 2580
		2015-04-13 14:55:00: Time per occ (micros): 0.0188933 occs= 91020
		2015-04-13 14:55:00: Mean (micros per pat): 1.71967
		2015-04-13 14:55:00: Mean (micros per occ): 0.0188933
		2015-04-13 14:55:00: 50% quantile: 1
		2015-04-13 14:55:00: 90% quantile: 4
		2015-04-13 14:55:00: 95% quantile: 5.33333
		{"type": "opt", "query": "and", "tottime (aprox) (micros)": 2580, "noccs": 91020, "avg (micros per pat)": 1.71967, 
		"avg (micros per occ)": 0.0188933, "q50": 1, "q90": 4, "q95": 5.33333}

	------
	You can see the "size of the index by looking at line":
	  - 2015-04-12 03:23:17: Documents: 132564440 bytes, 0.901107 bits per element
	  $ grep "Documents" output.experiments.txt

	You can see "AND-QUERY time by looking at line": 
	  - 2015-04-13 14:55:00: Time per occ (micros): 0.0188933 occs= 91020
	  - 2015-04-13 14:55:00: Mean (micros per occ): 0.0188933
		(you get both number of ocurrences and micros per pattern here).
	  $ grep "Time per occ" output.experiments.txt
	
	Therefore, we obtain the EF-OPT technique obtains a document-index of size 132564440 bytes, and it solves our Wa queries 
	(low freq words) at around 0.0188933 microsec per occurrence. The number of occs is 91020.

	If you check the rest of file "output.experiments.txt", you obtain query-times for patterns Wb (frequent words), P2 (2-word AND),
	and P5 (5-word AND) for EF-OPT. 

	Apart from the results for:
	 * EF-OPT technique, you also obtain results for 
	 * EF-UNIF (non optimized see [OV2014]), 
	 * OPTPFOR
	 * INTERPOLATIVE
	 * VARINT



   

      
