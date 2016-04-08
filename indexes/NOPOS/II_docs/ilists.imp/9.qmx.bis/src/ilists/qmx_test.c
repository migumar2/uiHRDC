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

#include <sys/time.h>
#include <sys/resource.h>
#include <unistd.h>

#include "compress_qmx.h"

#define GET_TIME_DIVIDER ((double) 1.0 )	 //getTime deals with "seconds" as time unit.
#define GET_TIME_UNIT "sec"
#define MSEC_TIME_DIVIDER  ((double) 1000.0) //1 sec = 1000 msec
#define MSEC_TIME_UNIT 	  "msec"
#define SEC_TIME_DIVIDER  ((double) 1.0	)	 //1 sec = 1 sec
#define SEC_TIME_UNIT 	  "sec"

double getTime (void)
{
	double usertime, systime;
	struct rusage usage;

	getrusage (RUSAGE_SELF, &usage);
	usertime = (double) usage.ru_utime.tv_sec * GET_TIME_DIVIDER +
		(double) usage.ru_utime.tv_usec / ( ((double) 1000000.0) / GET_TIME_DIVIDER);
	systime = (double) usage.ru_stime.tv_sec * GET_TIME_DIVIDER +
		(double) usage.ru_stime.tv_usec / ( ((double)1000000.0) / GET_TIME_DIVIDER);
	//return (usertime + systime);
	return (usertime );
}

size_t getLongRand(uint bits) {
	size_t x = (size_t) rand();
	size_t n = (x<<31L) | ((size_t)rand());
	n = n>>(62L - bits);
	return n;
} 

int cmpuints (const void *A, const void *B) {
	uint *a = (uint *) A;
	uint *b = (uint *) B;
	return ((*a)>= (*b));
}

//set maxbits_random <=32
//n = number of integers in the array.
int check_qmx(size_t n, uint maxbits_random) {

	size_t i;
	
	uint *V = new uint[n+8];
	uint *Diff = new uint[n+8];
	uint *C = new uint[500000000];
	uint *D = new uint[n+28];
	size_t maxval =0;
	
	int err=0;
	
	printf("\n now generating %zu random integers", n);fflush(stdout);
	for (i=0; i<n; i++) {
		V[i] = (uint) getLongRand(maxbits_random);
		//V[i]++;
		if (V[i] > maxval) maxval = V[i];
	}
	qsort(V, n, sizeof(uint), cmpuints);

#ifdef ENCODE_DIFFERENCES	
	size_t j=0;
	Diff[j++]= V[0];
	for (i=1;i<n;i++) {
		if (V[i]== V[i-1]) continue;
		Diff[j++] = (V[i]-V[i-1]);
	}
	
	n=j;
	for (i=0;i<n;i++)
		V[i] = Diff[i];
		
	delete []Diff;
#endif
	
	////	

	

	printf("\n\n Created array  with %zu uints, and at most %zu bits each",n, (size_t) maxbits_random);fflush(stdout);
//	for (i=0; i<n; i++) {	
//		printf("\n %u ", V[i]);
//	}

	printf("\n\n now compressing them with qmx.");fflush(stdout);
	compress_qmx qmx;
	
	ulong N1 = n; ulong N2;
	qmx.encodeArray(V, N1, C, &N2);

//	qmx.encodeArray(V, N1/2, C, &N2);
//	ulong N22;
//	qmx.encodeArray(&V[N1/2], N1- N1/2, &C[N2], &N22);
//	N2 +=N22;
	
	//int N = s9_Compression(V, C, (int) n);
	int N = N2;
	printf("\n\n %zu integers were compressed within %d uints", n,N);fflush(stdout);
	printf("\n ratio = %2.2f", 100.0 * N2/N1);
	
	printf("\n\n now recovering those values: qmx decoding");fflush(stdout);
	double t1 = getTime();
	//s9_Decompression(C, D, (int) n);
	
	qmx.decodeArray(C, N2, D, N1);
	
	printf("\n decompression = %2.5f seconds", getTime()-t1); fflush(stdout);
	

	for (i=0; i<n; i++) {		
		if (V[i] != D[i]) {
			printf("\n RECOVERING THE ORIGINAL INTS FAILED i=%zu pf %zu !!! (%zu != %zu)\n", i, n, (size_t) V[i], (size_t) D[i]);fflush(stdout);
			break;
			err=1;
		}
	
	}	
	if (i==n)
		printf("\n RECOVERING THE ORIGINAL %zu LONG-INTS SUCCEEDED !!",n);fflush(stdout);
	
	delete [] V;	
	delete [] C;	
	delete [] D;	
	//delete s9;
	return err;
}


int main(int argc, char ** argv) {
	
	srand(time(NULL));
	uint i=28;
	
//	for (i=2;i<62;i++) {
		int err= check_qmx(1000, i);
		if (err) {
			printf("\n qmx check failed!, please check");
			exit(0);
		}
	 	
			
//	}
}
	
	
