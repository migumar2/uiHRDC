
// implements canonical Huffman 

#ifndef HUFFINCLUDED
#define HUFFINCLUDED

#include "basics.h"
#define SORTED 1
#define UNSORTED 0

typedef struct
   { uint max,lim;   // maximum symbol (0..max), same excluding zero freqs
     uint depth; // max symbol length
     union
       { uint *spos; // symbol positions after sorting by decr freq (enc)
	 uint *symb; // symbols sorted by freq (dec)
       } s;
     uint *num;  // first pos of each length (dec), number of each length (enc)
     uint *fst;  // first code (numeric) of each length (dec)
     size_t total; // total length to achieve, in bits
   } THuff;

	// Creates Huffman encoder given symbols 0..lim with frequencies 
	// freq[i], ready for compression
	// sorted --> are the symbols already sorted ?

THuff createHuff (uint *freq, uint lim, uint sorted);

	// Encodes symb using H, over stream[ptr...lim] (ptr and lim are
	// bit positions of stream). Returns the new ptr.
	
size_t encodeHuff (THuff H, uint symb, uint *stream, size_t ptr);

	// Decodes *symb using H, over stream[ptr...lim] (ptr and lim are
	// bit positions of stream). Returns the new ptr.
	
size_t decodeHuff (THuff *H, uint *symb, uint *stream, size_t ptr);

	//Prepares a Huffman tree for decoding (changes in spos & symb)
	
void prepareToDecode(THuff *H);

	// Writes H in file f
	
void saveHuff (THuff H, FILE *f);

	// Size of H written on file
	
uint sizeHuffDisk (THuff H);

	//Size of H in memory
uint sizeHuff (THuff H);
	
	// Frees H
	
void freeHuff (THuff H);

	// Loads H from file f, prepared for encoding or decoding depending
	// on enc
	
THuff loadHuff (FILE *f, int enc);

//Decodes a code starting in position ptr from stream. Returns the ranking in the
//vector of symbols.

#define decodeNormalHuffMacro(H, symbol, stream, ptr) \
   { uint pos; \
     int d; \
     pos = 0; \
     d = 0; \
     while (pos < H->fst[d]) \
        { pos = (pos << 1) | bitget(stream,ptr); \
          ptr++; d++; \
        } \
    symbol = (H->s.symb[ H->num[d] + pos - H->fst[d] ]); \
   }


#endif

