# uiHRDC

__uiHRDC__ is a C/C++ reproducibility framework which comprises a varied set of techniques 
for indexing highly repetitive document collections, and all scripts required to 
replicate the experimental setup proposed in __F. Claude, A. Fariña, M.A. Martinez-Prieto, 
and G. Navarro. Universal Indexes for Highly Repetitive Document Collections. Information 
Systems 61:1-23, 2016__. 

uiHRDC includes non-positional and positional inverted indexes, which perform multiple forms 
of compression, and three families of self-indexes.  A more detailled description of all 
these techniques can be found in the aforementioned paper.

This repository contains a Dockerfile which creates the reproducibility environment,
including all dependencies required to compile and run our (self-)indexes. On the other
hand, the folder `uiHRDC` organizes the corresponding sources, and also provides some test 
collections and query patterns to evaluate different retrieval operations.

More information about each proposed technique can be found in its directory. Nevertheless, 
if you have any doubt or need more information, please feel free to contact us:

- Francisco Claude: \<fclaude@recoded.cl\>
- Antonio Fariña: \<fari@udc.es\>
- Miguel A. Martínez-Prieto: \<migumar2@infor.uva.es\>
- Gonzalo Navarro:  \<gnavarro@dcc.uchile.cl\>
