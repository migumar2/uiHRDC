//PforDelta.
//Interface by Antonio Fariña (University of A Coruña) in May 2011.
//Cannot compress gaps >= 2^28! (due to S16-limitations).

#include "pfd_coding.h"
#include "opt_p4.h"


/* compresses size integers from $input$.
 * stores the compressed data in $output$ and returns its size (as numb of uints)
 * NOTE: Fari 2011. 
 *    I guess $input$ must be an array of (size + PFD_BS2) uints.
 *    cdata must have space to store $size$ integers.            
 */ 
int PFD_Compression(unsigned int* input, int size, unsigned int* cdata) {
	int cSize = OPT4(input, size, cdata);
	return cSize/sizeof(uint);
}


/* decompresses size integers from $cdata$.
 * stores the decompressed data in $output$ and returns the number of uints 
 * processed from $cdata$ = $N$;
 * NOTE: Fari 2011. 
 *    I guess output must be an array of (size + PFD_BS) uints.
 *    and cdata contains $N$ integers.  
 */ 
int PFD_Decompression(unsigned int* cdata, int size, unsigned int* output) {
	static unsigned int all_array[2048];		// extra array for coding

	int i;
	unsigned int *_ww = cdata;
	for (i = 0; i*BS < size ; i++)
	{
		/* _ww = detailed_p4_decode(docid_check, _ww, all_array); */		// this is fast
		_ww = 	detailed_p4_decode(output + BS * i, _ww,  all_array);	// this is slow
	}
	return _ww-cdata;
}
