 #include "kBitArray.h"
 
/*-----------------------------------------------------------------
 Initilization of the kBitArray
 ---------------------------------------------------------------- */ 
 t_kBitArray create_kBitArray (uint size, uint elemSize) {
 	uint bitsNeeded; 
 	t_kBitArray	 V;
 	
 	V = (t_kBitArray) malloc (sizeof(struct akbitArr));
 	bitsNeeded = size * elemSize;
 	V->totalInts = ((bitsNeeded+W-1)/W);
 	V->data = (uint *) malloc((V->totalInts) * sizeof(uint));
	V->data[V->totalInts-1]=0000; /** avoids valgrind to blame*/
 	V->size = size;
 	V->elemSize= elemSize;
 	printf("\nKbitVector[0,%d), of elemSize = %u initialized\n",V->size,V->elemSize);
 	printf("\ntotalInts = %u, size = %u  elemSize = %u\n",V->totalInts,V->size,V->elemSize);
 	return V;
}

uint getKBitArray(t_kBitArray V, register uint i) {
	register uint eSize = V->elemSize;
	register uint answ;
	register uint pos = i * eSize;
	mybitread(answ, V->data, pos, eSize);
	return (answ);
	//return ( bitread(V->data,i * eSize,eSize)); 
}

uint getKBitArraySinMacro(t_kBitArray V, register uint i) {
	register uint eSize = V->elemSize;
	return ( bitread(V->data,i * eSize,eSize)); 
}

void setKBitArray(t_kBitArray V, uint i, uint value){
	register uint eSize = V->elemSize;	
	register uint pos = i * eSize;
	mybitwrite(V->data, pos, eSize, value);
}

void setKBitArraySinMacro(t_kBitArray V, uint i, uint value){
	register uint eSize = V->elemSize;
	bitwrite(V->data,i *eSize, eSize, value);
}

/*-----------------------------------------------------------------
 freeing resources
 ---------------------------------------------------------------- */ 
void destroy_kBitArray (t_kBitArray kBitArray) {
	uint total;
	total = (kBitArray->totalInts) * sizeof(uint);
	
	free(kBitArray->data);
	free(kBitArray);	
	printf("\n[destroying a Kbit array of size table]...Freed %u bytes... RAM", total);		
}


