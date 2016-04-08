//PforDelta.
//Interface by Antonio Fariña (University of A Coruña) in May 2011.
//Cannot compress gaps >= 2^28! (due to S16-limitations).

#include "qmx_coding.h"
#include "compress_qmx.c"


/* compresses size integers from $input$.
 * stores the compressed data in $output$ and returns its size (as numb of uints)
 * NOTE: Fari 2011. 
 *    I guess $input$ must be an array of (size + PFD_BS2) uints.
 *    cdata must have space to store $size$ integers.            
 */ 
int QMX_Compression(unsigned int* input, int size, unsigned int* cdata) {

	//printf("\n\n now compressing them with qmx.");fflush(stdout);
	static compress_qmx qmx;
	
	unsigned long cSize;
	qmx.encodeArray(input, (unsigned long)size, cdata, &cSize);
	
	//delete qmx;

	return cSize;// /sizeof(unsigned int);
}


/* decompresses size integers from $cdata$.
 * stores the decompressed data in $output$ and returns the number of uints 
 * processed from $cdata$ = $N$;
 * NOTE: Fari 2011. 
 *    I guess output must be an array of (size + PFD_BS) uints.
 *    and cdata contains $N$ integers.  
 */ 
int QMX_Decompression(unsigned int* cdata, int csize, unsigned int* output, int dsize) {
	
//printf("\n\n now decompressing them with qmx.");fflush(stdout);
	static compress_qmx qmx;
	
	qmx.decodeArray(cdata, (unsigned long) csize, output, (unsigned long) dsize);
	
	//delete qmx;

	return dsize;
}
