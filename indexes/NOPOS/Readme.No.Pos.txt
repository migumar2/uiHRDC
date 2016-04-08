
The attached package contains the source code, dataset (20gb) and patterns used in our paper:
"Universal Indexes for Highly Repetitive Document Collections"
submitted to Information Systems Journal in April 2016 
by Claude, F; Fariña, F.; Martinez-Prieto, M.; and Navarro, G.

In particular, you can find the source code for the following techniques found there:
  - Rice, RiceB, Vbyte, VbyteB, VbyteCM, VbyteST, Simple9, PFD, QMX, 
    Vbyte-LZMA, Vbyte-Lzend, Repair, Repair-skip, Repair-skip-ST, Repari-skip-CM.

We provide a library with implementations for all those techniques so that we provide
compressed representations for a set os posting lists (ilists.imp) and also build/search
programs that allows you to create an index of a [repetitive] text collection, and also 
run queries for a set of patterns.

The structure of the package contains:
 - II_docs               <source code and 64bit-binaries >
       * ./ilists.imp
       * ./src
       * ./lib

 - EXPERIMENTS      <scripts> for running the experiments.   
       * ./experiments.20g  <-- scripts
       * ./golinks.sh       <-- prepares links to ../data (textos and patterns) 
                                needed for the scripts.
  - data 
       * ./go20gb.sh    <-- dataset 20gb
       * ./go200mb.sh   <-- dataset 200mb
       * ./Readme.data.txt

  - EliasFano.OV14
       * ./Readme.how.to.txt 

-----------------
Outline:
Section 0. Sets the dataset and query patterns that will be used by our
           scripts when running experiments.
Section 1. Explains how to run experiments in our paper.
Section 2. Explains the structure of the source code.
Section 3. Depicts the structure of the library of posting lists representations, and includes 
    pattern generation sources..

Apart from that, in directory "EliasFano.OV14", you will obtain scripts and instructions to run 
the experiments we run with Ottaviano&Venturini's source code (Sigir 2014). You will find a
*Readme.how.to.txt* file that explains you step-by-step how to run the experiments, and the 
prerrequisites of that software.

---------------------------
0. Preparing data directory
---------------------------
  ** assume ../textos   contains the source dataset
  ** assume ../patterns contains the source query patterns.

  ./go20gb.sh  creates links to the dataset used in our Inf. Systems. paper.
  ./go200mb.sh creates links to smallest test dataset (less memory is needed for building).

  If you want to launch our EXPERIMENTS (see Section 1). You *MUST* run script go20gb.sh
   - cd data
   - bash go20gb.sh
   - cd ..

------------------------
1. EXPERIMENTS directory
------------------------
If you simply want to run our experiments. Simply run:
   - bash golinks20gb.sh  
   - cd experiments.20g
   - bash goClean.sh; bash goBuildAll.sh; bash goSearch.sh

   Scripts will use the links set in ../data directory in the previous Section (to the largest or 
   smallest dataset).
   
   ** Please note that the indexing process is not "optimized at all" and it uses a lot of memory!.   
   ** Note ** Some techniques allow to parameterize the index construction and the intersection type at 
   searches. Look at EXPERIMENTS/experiments.20g/TECNIQUE/goBuild.sh && goSearch.sh, to see how they are being used.
   
Anyway, if you want to understand what it makes, Please have a look at 
"EXPERIMENTS/experiments.20g directory" (and here enter any subdirectory: i.e. 1.5.scritps.riceRLE).

   - Creating our non-positional inverted index for rice-RLE (1.5.scripts.riceRLE subdirectory) 
     is done via scripts "goBuild.sh". Searches for locate AND-QUERIES (Extract/Intersect2/IntersectN lists) 
     are done with script "goSearch.sh". You will find them in the corresponding sub-directories for each
     posting list representation type. 

  - Note that in the ../data folder, we set links to the source text file named:  "text20gb.txt", and the 
    "text20gb.txt.lens" and "text20gb.txt.DOCBOUNDARIES.ul" files.
    Note that, given a text file, you have to provide the corresponding ".DOCBOUNDARIES.ul" file... 
    Then the indexer will use it to create the inverted lists for each term in the text file.

    ** There is a function in the ../II_DOCS/src/utils/ii.c that loads the data in the file marking doc boundaries. 
       Note that each offset is marked with a ulong (8bytes) value... as I am compiling always with -m64 option.

            int loadDocBeginngins(ulong **offsets, uint *docs, char *basename) {
                int file;
                ssize_t readbytes;

                char *filename[2048];  
                sprintf(filename,"%s.%s", basename, DOCBOUNDARIES_FILE_EXT);

                fprintf(stderr,"\t Loading document boundaries (as integers) from disk %s...", filename);      
                if( (file = open(filename, O_RDONLY)) < 0) {
                    printf("Cannot open file %s\n", filename);exit(0);
                }  
                uint ndocs = (uint) (fileSize(filename)/sizeof(ulong) -1);              
                ulong *docBeginnings = (ulong *) malloc (sizeof(ulong) * (ndocs +1));

                readbytes = read(file, docBeginnings, sizeof(ulong) * (ndocs+1));      
                close(file);  
                *docs = ndocs;*offsets = docBeginnings;

                fprintf(stderr,"%u docs loaded \n", ndocs);      
                return 0;      
            }

    Therefore, once you have a file "x.txt" and the "x.txt.DOCBOUNDARIES.ul", you could use the scripts 
    in "experiments.20g/1.5.scritps.riceRLE" (or other subdirectory)  to create the indexes 
    (goBuild.sh) ... and to perform queries (goSearch.sh).

    You can see a sample query file in directory: "EXPERIMENTS/patterns.1strun"

	#numberOfFrases::     1000, #NumberOfWordsPerPhrase::  20002000 <not used>
		0        10 march 2003
		1        12 tennis grand
		2        10 quot style
		3        12 andre agassi
		<rank>	<len>	<pattern>

  ** there is also a program lens2ulongs (../data/patterns/lens2uints/lens2ulongsc.) 
    that will permit you to create the "x.txt.DOCBOUNDARIES.ul" if you have a file "x.txt.lens" 
    (containing 1 line that includes the lenght of each of the documents included in file "x.txt").

------------------------
2. II-DOCS directory
------------------------
This contains the source code that given a text and a file of doc_boundaries, permits to create 
the index and then run "AND-locate" queries  (extract / intersect 2 lists / intersect N lists depending
on the lenght of the pattern)

There are 2 main parts in our indexes:
  1- The programs to build/search: [A] the build program parsers the text and makes up the uncompressed 
  lists, and then uses a given "implementation of inverted lists representation" to create 
  compressed inverted lists. [B] The search program receives an index (path) and a file with patterns
  and runs the experiments for that index and that set of patterns.
  
  These two programs are in <src directory>  (buildII.c and buildSearcher.c)
  
  2- The library (following the interface in "src/interface_il.h") that holds the implementation of the 
  strategy to compress the inverted lists.  <lib directory>. 
      The binaries in this directory can be made by entering the directory <ilists.imp> and typing 
      "bash generatelibs.sh".
        ** "generatelibs.sh" script will:
              + enter all the subdirectories (each one containing an implementation for a compressed 
              representation of a set of posting lists), and 
              + then will build the corresponding "xx.a" library file.
              + finally it copies all the built files into "../lib" directory

The whole [build/search] package is built by calling "sh compile.sh". 
It will create indexer/searcher programs that include de different compressed representations for a 
set of posting lists: "RICE", "RICE_RLE", "VBYTE", "VBYTE_MOFFAT (cm)", "VBYTE_SANDERS (st)", "VBYTE_LZMA", 
"LZEND (vbyte-lzend)", "SIMPLE9", "PFORDELTA", "QMXCODING", "QMXCODINGSIMD", "REPAIR_NOSKIP  
(repair with no skipping info)", "REPAIR_SKIP", "REPAIR_SKIP_MOFFAT (cm)", "REPAIR_SKIP_SANDERS (st)". 
To create them, the indexer/searcher programs in the ./src directory are linked with the implementation 
of one representation for inverted lists: "lib/il_1.5.riceRLE_64bit.a", 
"lib/il_4.3.repair_skipping_64bit.a", "lib/il_2.1.vbyte_nosampling64bit.a", etc.  

Note that the building program of some representations (REPAIR*, VBYTE-LZMA) uses several external 
programs that are also included in the lib directory (and whose path must be passed as command line 
arguments for the corresponding indexer (BUILD* programs).

To summ up, if you want to create indexer/searcher programs you basically have to:
  [1]. enter ilist.imp directory and type: "bash generatelibs.sh"
  [2]. type "bash compile.sh"

  [3]. Resulting files
     $ ls lib/
         build_bitmap
         build_index
         build_index_gonzalo
         compressDictionary
         convert
         il_1.4golomb_nosampling_lenlist_64bit.a
         il_1.5.riceRLE_64bit.a
         il_2.1.vbyte_bitmap_nosampling_64bit.a
         il_2.moffatBitmapSpireVbyte_64bit.a
         il_3.sandersBitmapByteCodes_64bit.a
         il_4.2.repair_64bit.a
         il_4.3.1.repair_skipping_moffat_64bit.a
         il_4.3.2.repair_skipping_sanders_64bit.a
         il_4.3.repair_skipping_64bit.a
         il_5.lzend_64bit.a
         il_6.vbyte_lzma_64bit.a
         il_7.simple9_64bit.a
         il_8.pfordelta_64bit.a
         il_9.QMXCODING_64bit.a
         il_9.QMXCODINGSIMD_64bit.a
         irepair
         irepair_bal
         lzmaEncoder

     $ ls  BUILD_* SEARCH*
         BUILD_LZEND_IL_NOTEXT
         BUILD_PFORDELTA_NOTEXT
         BUILD_QMXCODING_NOTEXT
         BUILD_QMXCODINGSIMD_NOTEXT
         BUILD_REPAIR_NOSKIP_NOTEXT
         BUILD_REPAIR_SKIP_MOFFAT_NOTEXT
         BUILD_REPAIR_SKIP_NOTEXT
         BUILD_REPAIR_SKIP_SANDERS_NOTEXT
         BUILD_RICE_NOTEXT
         BUILD_RICE_RLE_NOTEXT
         BUILD_SIMPLE9_NOTEXT
         BUILD_VBYTE_LZMA_NOTEXT
         BUILD_VBYTE_MOFFAT_NOTEXT
         BUILD_VBYTE_NOTEXT
         BUILD_VBYTE_SANDERS_NOTEXT
         SEARCH_LZEND_IL_NOTEXT
         SEARCH_PFORDELTA_NOTEXT
         SEARCH_QMXCODING_NOTEXT
         SEARCH_QMXCODINGSIMD_NOTEXT
         SEARCH_REPAIR_NOSKIP_NOTEXT
         SEARCH_REPAIR_SKIP_MOFFAT_NOTEXT
         SEARCH_REPAIR_SKIP_NOTEXT
         SEARCH_REPAIR_SKIP_SANDERS_NOTEXT
         SEARCH_RICE_NOTEXT
         SEARCH_RICE_RLE_NOTEXT
         SEARCH_SIMPLE9_NOTEXT
         SEARCH_VBYTE_LZMA_NOTEXT
         SEARCH_VBYTE_MOFFAT_NOTEXT
         SEARCH_VBYTE_NOTEXT
         SEARCH_VBYTE_SANDERS_NOTEXT

Now you can enter EXPERIMENTS (Section 1) directory and run scripts for each variant.

---------------------------
2.1 II-DOCS_large directory
---------------------------
  In our paper we did some experiments with a 100gb dataset, this required to 
  make some "modifications" to the II_docs/src files, and also modifying the
  interface_il which required also changes in the library of representations 
  of posting lists. 
  You will find that new implementation in II_docs_large directory. Yet, only a few 
  techniques were adapted, (only Rice.RLE, Repair, Repair-skip, and vbyte-lzma).
  
  By running the scripts:
  II-Docs-large\ilists.imp>$ bash generatelibs.sh  (will make the libraries).
  II-Docs-large>$ bash compile.sh
  
  You will obtain files
         BUILD_REPAIR_NOSKIP_NOTEXT
         BUILD_REPAIR_SKIP_NOTEXT
         BUILD_RICE_RLE_NOTEXT
         BUILD_VBYTE_LZMA_NOTEXT

         SEARCH_REPAIR_NOSKIP_NOTEXT
         SEARCH_REPAIR_SKIP_NOTEXT
         SEARCH_RICE_RLE_NOTEXT
         SEARCH_VBYTE_LZMA_NOTEXT
  They work exactly as the previous ones.       
  
  
  





  
=====================================================================
=====================================================================
Additional information regarding:
 - the implementation of posting lists
 - BUILDP* programs !!!!!!!!!!!!
 - Generating patterns.
=====================================================================
=====================================================================

Apart from linking the implementation of each compressed representation of posting lists to 
create BUILD/SEARCH programs, it is also possible to test the inverted lists representations 
"alone"... (or to create a different representation and test it before linking it).


-------------------------------------------------------
3.1 II-DOCS/ilists.imp/ directory, and BUILDP* programs: 
-------------------------------------------------------
We are attaching also the source for the representation of inverted lists alone in directory 
"./ilists.imp" as well as several programs that you could use to test the inverted-list 
representation alone. You could have a look at: "/II_docs/ilists.imp/1.5.riceRLE/README.txt" 

** 3.1 Structure of "output.posts" files.
During the execution of BUILD_* indexers you will also obtain a sequence with the uncompressed uint32 
representation of the inverted lists corresponding to ALL the terms in the source file.
   
	  The following would be the data saved in the created file  ("output.posts" or similar 
	  obtained during the execution of the BUILD_* programs in Section 2")

	  [Nwords=N][maxPostvalue]
	  [lenPost1][post11][post12]...[post1k_1]  
	  [lenPost2][post21][post22]...[post2k_2]                         
	   ...                                                            
	  [lenPostN][postN1][postN2]...[postNk_N]             

 That is, there is 1 integer to indicate the number of words/terms in the file. 1 integer to 
 indicate the maxPostvalue (i.e number of documents), and then, for each term, and integer 
 representing the lenPost value (number of elements in the inverted lists), followed by 
 the values in that inverted list. Recall that all those integers are "32bit uint" values.

  -- You can use the output.posts file to:
     
     [1] Use BUILDP* builders instead of BUILD*.
          Manually do the next 3 steps
           a. Rename output.posts and call it 20gb.posts
           b. Copy index/wiki20gb.voc to      20gb.voc
           c. Copy index/wiki20gb.const to    20gb.const
        
        For example, (from EXPERIMENTS/experiments.20g/1.5.scritps.riceRLE/goBuild.sh)
        now you can use BUILDP as: 
        
         ./BUILDP ../../20gb indexes/wiki20gb "nooptions;  1.5.scripts.rice.rle"        
        instead of having to run           
         ./BUILD ../../textos/text20gb.txt indexes/wiki20gb "nooptions;  1.5.scripts.rice.rle"
         
         And the same 20gb.const, 20gb.posts, and 20gb.voc files can be used to create the rest
         of types of indexes. 
         
         This saves much time at indexing time (no parsing of text file is done), and memory 
         (the posting lists in 20gb.posts are read from disk, but the 20gb.txt file is no longer used).
         
     [2] test/debug the different posting list representations alone (without linking with buildII.c 
     and buildSearcher.c programs). See in each ilists.imp/ subdirectory that myIL and myILSearch 
     programs permit to "create a representation of lists from a source "output.posts" file, and also 
     to launch queries (E/I/N = Extract, Intersect2, IntersectN).
     Yet this option is only required at debugging.
                  
-------------------------------------------------------
3.2 Pattern generation: Directory others
-------------------------------------------------------
        
In addition we also include directories: 
	- ExtractorWords
	- extractor.phrases.randLengh

They allow us to create query-files (including either simple words and phrases) 
from a given source text.  

	See others/go.sh script.
	








