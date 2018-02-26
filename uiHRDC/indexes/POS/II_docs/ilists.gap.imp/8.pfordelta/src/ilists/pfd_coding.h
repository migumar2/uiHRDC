//PforDelta.
//Interface by Antonio Fariña (University of A Coruña) in May 2011.
//Cannot compress gaps >= 2^28! (due to S16-limitations).

#ifndef PFD_CODING_H_
#define PFD_CODING_H_
#define PFD_BS 128    //see unpack.h
#define PFD_BS2 (128 * 2)

//public:
  /** compresses the "size" uints in "input".
   *  the encoded secuence is set into "cdata" and its size (number of uints) 
   *  returned
   *  Memory for "cdata" must be allocate by the caller
   */
  int PFD_Compression(unsigned int* input, int size, unsigned int* cdata);
  
  /** decompresses the "size" uints internally representaed by the compressed sequence of uints "cdata".
   *  the decoded secuence is set into "output".
   *  Memory for "output" must be allocate by the caller. Actually the caller must
   *  allocate at least size uints!!  <-------------------------------------
   *  In addition it returns the number of uints processed from the "cdata" array.
   *    (this is not typically of interest, but could be of interest to move ptrs forward)
   */  
  int PFD_Decompression(unsigned int* input, int size, unsigned int* output);


#endif /* PFD_CODING_H_ */
