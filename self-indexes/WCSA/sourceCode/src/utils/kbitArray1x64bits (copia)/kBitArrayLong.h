#include "basics.h"
 
struct akbitArr {
	size_t *data;	   /* ulong * contains space for the array of kbit elements	*/
	size_t size ;      /* number of kbitElements    */
	size_t totalInts;  /* number of ints used */
	uint elemSize;     /* number of bits of each element*/	
};

typedef struct akbitArr *t_kBitArray;

	/*********/
	t_kBitArray create_kBitArrayLong (size_t size, uint elemSize);
	size_t getKBitArrayLong(t_kBitArray V,size_t i);
	void setKBitArrayLong(t_kBitArray V, size_t i, size_t value);
	void destroy_kBitArrayLong (t_kBitArray kBitArray);
	size_t size_kBitArrayLong (t_kBitArray kBitArray);





