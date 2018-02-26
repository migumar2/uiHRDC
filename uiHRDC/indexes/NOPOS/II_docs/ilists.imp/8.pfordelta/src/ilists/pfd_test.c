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

#include "pfd_coding.h"

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
int check_pfd(size_t n, uint maxbits_random) {

	size_t i;
	
	
	
	
	uint *V    = new uint[n+ PFD_BS2];
	uint *Diff = new uint[n+ PFD_BS2];
	uint *C    = new uint[n+ PFD_BS2];
	uint *D    = new uint[n+ PFD_BS];
	
	
	
	
	size_t maxval =0;
	
	int err=0;
	
	printf("\n now generating %zu random integers", n);
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
		V[i] = Diff[i]+1;
		//V[i] = Diff[i];
		
	delete []Diff;
#endif
	
	////	

	//s9_coding *s9 = new s9_coding();

	printf("\n\n Created array  with %zu uints, and at most %zu bits each",n, (size_t) maxbits_random);
	for (i=0; i< ((10<n)?10:n); i++) {	
		printf("\n %u ", V[i]);
	}


	printf("\n\n now compressing them with PFDelta.");
fflush(stderr);fflush(stdout);

	int N = PFD_Compression(V, (int) n, C);
	printf("\n\n %zu integers were compressed within %d uints", n,N);

	uint *C2    = new uint[N];
	for (i=0;i<N;i++) C2[i]=C[i];
	delete [] C;
	
	
	printf("\n\n now recovering those values: PFDelta decoding");
	double t1 = getTime();
	int d= PFD_Decompression(C2, (int) n, D);
	printf("\n decompression = %2.5f seconds (%i ints)", getTime()-t1, d); 
	

	for (i=0; i<n; i++) {		
		if (V[i] != D[i]) {
			printf("\n RECOVERING THE ORIGINAL INTS FAILED i=%zu pf %zu !!! (%zu != %zu)\n", i, n, (size_t) V[i], (size_t) D[i]);
			break;
			err=1;
		}
	
	}	
	if (i==n)
		printf("\n RECOVERING THE ORIGINAL %zu LONG-INTS SUCCEEDED !!",n);

	delete [] V;	
	delete [] C2;	
	delete [] D;	
	//delete s9;
	return err;
}


int main(int argc, char ** argv) {
	
	srand(time(NULL));
	uint i=28;
	int err;

	 	err =check_pfd(1, 28);
		if (err) {
			printf("\n s9 check failed!, please check");
			exit(0);
		}

		err= check_pfd(1000, i);
		if (err) {
			printf("\n s9 check failed!, please check");
			exit(0);
		}
	
	
//	for (i=2;i<62;i++) {
		err= check_pfd(10000000, i);
		if (err) {
			printf("\n s9 check failed!, please check");
			exit(0);
		}


	 	err =check_pfd(100000000, i);
		if (err) {
			printf("\n s9 check failed!, please check");
			exit(0);
		}
 	
	 	
			
//	}
}
	
	
