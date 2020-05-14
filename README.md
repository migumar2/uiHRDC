# uiHRDC

__uiHRDC__ is a C/C++ reproducibility framework which comprises a varied set of techniques 
for indexing highly repetitive document collections, and all scripts required to 
replicate the experimental setup proposed in:

- F. Claude, A. Fariña, M.A. Martinez-Prieto, and G. Navarro. Universal Indexes for Highly 
Repetitive Document Collections. Information Systems. Volume 61, pages 1-23, 2016. 
(https://doi.org/10.1016/j.is.2016.04.002)

uiHRDC includes non-positional and positional inverted indexes, which perform multiple forms 
of compression, and three families of self-indexes.  A more detailled description of all 
these techniques can be found in the aforementioned paper.

This repository contains a Dockerfile which creates the reproducibility environment,
including all dependencies required to compile and run our (self-)indexes. On the other
hand, the folder `uiHRDC` organizes the corresponding sources, and also provides some test 
collections and query patterns to evaluate different retrieval operations.

More information about each proposed technique can be found in its directory. Nevertheless, 
if you have any doubt or need more information, please feel free to contact us:

- Antonio Fariña:              fari@udc.es
- Miguel A. Martínez-Prieto:   migumar2@infor.uva.es
- Gonzalo Navarro:             gnavarro@dcc.uchile.cl
- Francisco Claude:            fclaude@recoded.cl


---------
NOTE: We were also invited to write reproducibility companion paper in Information Systems. In 
this second paper we give a brief summary of the techniques in the previous paper, and
include further details regarding how our experiments can be reproduced by using our __uiHRDC__ 
framework. For more details please see/cite: 

- A. Fariña, M.A. Martinez-Prieto, F. Claude, G. Navarro, J.J Lastra-Díaz, N. Prezza, and D. Seco. On the Reproducibility of 
Experiments of Indexing Repetitive Document Collections. Information Systems, volume 83, pages 181-194, 2019. (https://doi.org/10.1016/j.is.2019.03.007)


