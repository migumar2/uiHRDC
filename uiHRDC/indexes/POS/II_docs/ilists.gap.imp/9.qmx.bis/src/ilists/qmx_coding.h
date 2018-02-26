//PforDelta.
//Interface by Antonio Fariña (University of A Coruña) in May 2011.
//Cannot compress gaps >= 2^28! (due to S16-limitations).

#ifndef QMX_CODING_H_
#define QMX_CODING_H_

#define QMX_BS2 (512*4)

#include <stdio.h>

//public:
  /** compresses the "size" uints in "input".
   *  the encoded secuence is set into "cdata" and its size (number of uints) 
   *  returned
   *  Memory for "cdata" must be allocated by the caller
   */
  int QMX_Compression(unsigned int* input, int size, unsigned int* cdata);
  
  /** decompresses the "size" uints internally representaed by the compressed sequence of uints "cdata".
   *  the decoded secuence is set into "output".
   *  Memory for "output" must be allocated by the caller. Actually the caller must
   *  allocate at least csize uints!!  <-------------------------------------
   *  In addition it returns the number of uints processed from the "cdata" array = "dsize".
   *    (this is not typically of interest, but could be of interest to move ptrs forward)
   */  
  int QMX_Decompression(unsigned int* input, int csize, unsigned int* output, int dsize);


#endif /* QMX_CODING_H_ */
