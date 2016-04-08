
// Implements operations over a bitmap

#ifndef BITMAPINCLUDED
#define BITMAPINCLUDED

#include "basics.h"

typedef struct sbitmap
   { uint *data;
     uint n;        // # of bits
     uint pop;	    // # bits set
     uint *sdata;   // superblock counters
     uint sSize;	//		size of sdata vector
     byte *bdata;   // block counters
     uint bSize; 	//     size of bdata vector
     uint mem_usage;
   } *bitmap;


	// creates a bitmap structure from a bitstring, which gets owned
bitmap createBitmap (uint *string, uint n);
	// rank(i): how many 1's are there before position i, not included
uint rank (bitmap B, uint i);
	// select(i): position of i-th 1
uint bselect (bitmap B, uint i);
	// destroys the bitmap, freeing the original bitstream
void destroyBitmap (bitmap B);
	// popcounts 1's in x
uint popcount (register uint x);

void saveBitmap (char *filename, bitmap b);
bitmap loadBitmap (char *filename, uint *string, uint n);



////EDU'S functions included here.
//bitmap createBitmapEdu (uint *string, uint n);
//uint popcountEdu (register uint x);     //which is identical to popcount.
//uint rank1Edu(bitmap B, unsigned int position);
//unsigned int isActiveBit(uint *V, uint position);
void showBitVector(uint * V, int vectorSize);

#endif


