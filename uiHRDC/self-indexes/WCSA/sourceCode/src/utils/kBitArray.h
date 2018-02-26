#include "basics.h"
 
struct akbitArr {
	uint *data;	   /* uint * contains space for the array of kbit elements	*/
	uint size;     /* number of kbitElements    */
	uint elemSize; /* number of bits of each element*/	
	uint totalInts; /* number of ints used */
};

typedef struct akbitArr *t_kBitArray;

	/*********/
	t_kBitArray create_kBitArray (uint size, uint elemSize);
	uint getKBitArray(t_kBitArray V,uint i);
	void setKBitArray(t_kBitArray V, uint i, uint value);
	void destroy_kBitArray (t_kBitArray kBitArray);


	#define getKBitArrayMacro mybitread  //mybitread(answ, v, p, len)




