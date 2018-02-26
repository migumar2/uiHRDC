# uiHRDC

This directory encompasses a varied collection of C/C++ implementations of indexes for 
indexing highly repetitive document collections. More precisely, it includes non-positional
and positional inverted indexes, which perform multiple forms of compression, and
three families of self-indexes. Besides, it includes some test data collections and 
different sets of queries to evaluate pattern location snippet extraction performance.
Finally, it comprises some scripts to run build and test processes, and also for
compiling result files and obtain a final report of the experiment.

Sub-directories are organized as follows:

  + `benchmark`: this directory contains some simple scripts to compile result files
	   into different graphics which are then included in a $\LaTeX$ file. A final
	   PDF report is obtaained.

  + `data`: this directory organizes all experimental data used in our evaluation. It
          is organized as follows:
    - The `intervals` folder contains files which describe boundary text positions used
          for snippet extraction.
    - The `patterns` folder contains files which describe patterns of different length
          to be searched in the text.
    - The `text` folder contains repetitive text collections for evaluating our techniques.
    
  + `indexes`: this directory organizes all sources for the non-positional and positional
               inverted indexes presented in our paper:
    - The `NOPOS` folder contanins all source code for our compressed non-positional
           indexes and instructions for using them.
    - The `POS` folder contanins all source code for our compressed positional indexes 
           and instructions for using them.

  + `self-indexes`: this directory organizes all sources for the three families of
     self-indexes considered in our paper:
    - The `LZ` folder contains sources for optimized versiones of LZ77 and LZ-end
          self-indexes, and instructions for compiling and using them.
    - The `RLCSA`: this folder contains sources for a release of RLCSA ( <http://jltsiren.kapsi.fi/rlcsa>)
	  , and instructions for compiling and using it.
    - The `SLP` folder contains sources for our two SLP-based self indexes and
	  instructions for compiling and using them.
    - The `WCSA`: this folder contains sources for an adapted version of the Word 
	  Compressed Suffix Array, and instructions for compiling and using it.

Each folder contains its own README with additional details. Nevertheless, if you have any 
doubt or need more information, please feel free to contact us:

- Francisco Claude: \<fclaude@recoded.cl\>
- Antonio Fariña: \<fari@udc.es\>
- Miguel A. Martínez-Prieto: \<migumar2@infor.uva.es\>
- Gonzalo Navarro:  \<gnavarro@dcc.uchile.cl\>
