# uiHRDC

uiHRDC is a C/C++ suite which includes a varied set of techniques for indexing
highly repetitive document collections. More precisely, it includes non-positional
and positional inverted indexes, which perform multiple forms of compression, and
three families of self-indexes. A more detailled description of all these techniques 
can be found in the paper **"Universal Indexes for Highly Repetitive Document 
Collections"** subtmitted to *Information Systems Journal in April 2016 by Claude, F; 
Fariña, A.; Martinez-Prieto, M.; and Navarro, G.*

This repository is organized as follows:

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
    - The `SLP` folder contains sources for our two SLP-based self indexes and
	  instructions for compiling and using them.
    - The `WCSA`: this folder contains sources for an adapted version of the Word 
	  Compressed Suffix Array, and instructions for compiling and using them.

Each folder contains its own README with additional details. Nevertheless, if you need
more information, please feel free to contact us:

- Francisco Claude: \<fclaude@recoded.cl\>
- Antonio Fariña: \<fari@udc.es\>
- Miguel A. Martínez-Prieto: \<migumar2@infor.uva.es\>
- Gonzalo Navarro:  \<gnavarro@dcc.uchile.cl\>
