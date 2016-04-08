 #include "kBitArrayLong.h"
 
/*-----------------------------------------------------------------
 Initilization of the kBitArray
 ---------------------------------------------------------------- */ 
 t_kBitArray create_kBitArrayLong (size_t size, uint elemSize) {
	 
 	t_kBitArray	 V = (t_kBitArray) malloc (sizeof(struct akbitArr));
 	
 	V->totalInts = ulong_len(elemSize,size);
 	V->data = (size_t *) malloc((V->totalInts) * sizeof(size_t));
	V->data[V->totalInts-1]=00000000; /** avoids valgrind to blame*/
 	V->size = size;
 	V->elemSize= elemSize;
 	printf("\nKbitVector[0,%zu), of elemSize = %u initialized\n",V->size,V->elemSize);
 	printf("\ntotalLongs = %zu, size = %zu  elemSize = %u\n",V->totalInts,V->size,V->elemSize);
 	return V;
}

size_t getKBitArrayLong(t_kBitArray V, register size_t i) {
	register uint eSize = V->elemSize;	
	return ( bitread64(V->data,i * eSize, eSize)); 
}

void setKBitArrayLong(t_kBitArray V, size_t i, size_t value){
	register uint eSize = V->elemSize;
		bitwrite64(V->data,i *eSize, eSize, value); 
}

/*-----------------------------------------------------------------
 freeing resources
 ---------------------------------------------------------------- */ 
void destroy_kBitArrayLong (t_kBitArray kBitArray) {
	//size_t total;
	//total = (kBitArray->totalInts) * sizeof(uint);
	
	free(kBitArray->data);
	free(kBitArray);	
	//printf("\n[destroying a Kbit array of size table]...Freed %zu bytes... RAM", total);		
}


size_t size_kBitArrayLong (t_kBitArray kBitArray) {
	return ((kBitArray->totalInts) * sizeof(size_t) + sizeof(struct akbitArr ));
	//return ((kBitArray->totalInts) * sizeof(uint));
}

