#include <sys/types.h>
#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/times.h>
#include <time.h>

#include <unistd.h>
#include <errno.h>
#include <string.h>

#include <stdint.h>
#include "kBitArrayLong.c"


size_t getLongRand(uint bits) {
	size_t x = (size_t) rand();
	size_t n = (x<<31L) | ((size_t)rand());
	n = n>>(62L - bits);
	return n;
} 

int check_kbitArray(size_t n, uint maxbits_random) {

	size_t i;
	
	size_t *V = new size_t[n];
	size_t maxval =0;
	
	int err=0;
	
	printf("\n now generating %zu random llong-integers", n);
	for (i=0; i<n; i++) {
		V[i] = getLongRand(maxbits_random);
		if (V[i] > maxval) maxval = V[i];
	}
	
	uint maxval_bits = bits(maxval);
	
	t_kBitArray kba = create_kBitArrayLong (n, maxval_bits);
	printf("\n\n Created kbitsArray with %zu llongs, and %zu bits each",n, (size_t) maxval_bits);
	printf("\n   it occupies: %zu bytes", size_kBitArrayLong(kba));
	
	printf("\n\n now adding those random values to the kbitsArray structure.");
	for (i=0; i<n;i++) {
		setKBitArrayLong(kba, i, V[i]);
	}
	
	printf("\n\n now recovering those values from the kbitsArray structure.");
	for (i=0; i<10; i++) {
		size_t val = getKBitArrayLong(kba, i);
		printf("\n i = %zu, rand = %12zu ,randkba= %12zu ",i,V[i],val);
	}
	
	for (i=0; i<n; i++) {
		size_t val = getKBitArrayLong(kba, i);
		if (V[i] != val) {
			printf("\n RECOVERING THE ORIGINAL LONG-INTS FAILED !!! (%zu != %zu)\n", V[i],val);
			break;
			err=1;
		}
	}	
	if (i==n)
		printf("\n RECOVERING THE ORIGINAL %zu LONG-INTS SUCCEEDED !!",n);
	
	destroy_kBitArrayLong(kba);
	delete [] V;	
		
	return err;
}


int main(int argc, char ** argv) {
	
	srand(time(NULL));
	uint i;
	
	for (i=2;i<62;i++) {
		int err= check_kbitArray(10000000, i);
		
		if (err) {
			printf("\n kBitArray_Long fails with random its of %d bits, please check",i);
			exit(0);
		}
	}
}
	
	
