
// implements canonical Huffman !! Just for decoding when symbols were sorted before creating huffman

#ifndef HUFFDECINCLUDED
#define HUFFDECINCLUDED

#include "basics.h"
#define SORTED 1
#define UNSORTED 0

typedef struct
   { //uint lim;   
     uint depth; // max symbol length
     uint *num;  // first pos of each length (dec), number of each length (enc)
     uint *fst;  // first code (numeric) of each length (dec)        
   } THuffDec;


//typedef struct
//   { uint max,lim;   // maximum symbol (0..max), same excluding zero freqs
//     uint depth; // max symbol length
//     union
//       { uint *spos; // symbol positions after sorting by decr freq (enc)
//	 uint *symb; // symbols sorted by freq (dec)
//       } s;
//     uint *num;  // first pos of each length (dec), number of each length (enc)
//     uint *fst;  // first code (numeric) of each length (dec)
//     uint total; // total length to achieve, in bits
//   } THuff;


	// Decodes *symb using H, over stream[ptr...lim] (ptr and lim are
	// bit positions of stream). Returns the new ptr.
int decodeHuffDec (THuffDec *H, uint *symb, uint *stream, uint ptr);

	// Writes H in file f	
void saveHuffDec (THuffDec H, FILE *f);

	// Frees H	
void freeHuffDec (THuffDec H);

	// the number of bytes used by HuffDecman struct.
uint sizeHuffDec (THuffDec H);

	// Loads H from file f, prepared for encoding or decoding depending
	// on enc
	
THuffDec loadHuffDec (FILE *f, int enc);


//Decodes a code starting in position ptr from stream. Returns the ranking in the
//vector of symbols.
#define decodeHuffDecMacro(H, symb, stream, ptr) \
   { uint pos; \
     int d; \
     pos = 0; \
     d = 0; \
     while (pos < H->fst[d]) \
        { pos = (pos << 1) | bitget(stream,ptr); \
          ptr++; d++; \
        } \
     fflush(stdout); \
     symb = H->num[d]+pos-H->fst[d]; \
   }

#endif
















//#define decodeHuffDecMacroVALIDWORDS decodeHuffDecMacro

//Decodes a code starting in position ptr from stream. Returns the ranking in the
//vector of symbols. 
//Saves space, as the last level for num[] and fst[] are not needed.
//             at expenses of an extra-IF (the last line).
#define decodeHuffDecMacroVALIDWORDS(H, symb, stream, ptr, depth) \
   { uint pos; \
     int d; \
     pos = 0; \
     d = 0; \
     while ((pos < H->fst[d])  && (d< depth)) \
        { pos = (pos << 1) | bitget(stream,ptr); \
          ptr++; d++; \
        } \
     fflush(stdout); \
     symb = (depth==d) ? pos : H->num[d]+pos-H->fst[d]; \
   }


//Decodes a code starting in position ptr from stream. Returns the ranking in the
//vector of symbols.
#define decodeHuffDecMacroVariantWordxx(fst,num, symb, stream, ptr, depth) \
   { uint pos; \
     int d; \
     pos = 0; \
     d = 0; \
     while ((pos < fst[d])  && (d< depth)) \
        { pos = (pos << 1) | bitget(stream,ptr); \
          ptr++; d++; \
        } \
     fflush(stdout); \
     symb = (depth==d) ? pos : num[d] + pos - fst[d]; \
   }



//Decodes a code starting in position ptr from stream. Returns the ranking in the
//vector of symbols.
// #define decodeHuffDecMacroVariantWord(fst,symb, stream, ptr, depth) \
//    { uint pos; \
//      int d; \
//      pos = 0; \
//      d = 0; \
//      while ((d< depth) && (pos < fst[d*2])  ) \
//         { pos = (pos << 1) | bitget(stream,ptr); \
//           ptr++; d++; \
//         } \
//      symb = (depth==d) ? pos : fst[d*2+1] + pos - fst[d*2]; \
//    }


/** Decodes a variant of a word from a stream of compressed bits, starting in the ptr-th bit
	The starting bucket in fstnum is "offbucket" [fst|num|fst|num|fst|num]
	*/
#define decodeHuffDecMacroVariantWordPos(fstnum, offbucket, symb, stream, ptr, depth) \
   { uint pos; \
     register uint d; \
     pos = 0; \
     d = 0; \
     while ((d< depth) && (pos < fstnum[offbucket + d*2])  ) \
        { pos = (pos << 1) | bitget(stream,ptr); \
          ptr++; d++; \
        } \
     symb = (depth==d) ? pos : fstnum[offbucket+d*2+1] + pos - fstnum[offbucket+d*2]; \
   }
#define decodeHuffDecMacroVariantWordPos2(HV, symb, stream, ptr, idCanonical) \
   { uint pos; \
     register uint d; \
     uint *offfstnum = HV.offsetNumAndFst; \
     uint *fstnum = HV.zoneNumFst; \
	 register uint offbucket = offfstnum[idCanonical]; \
	 register uint depth = (offfstnum[idCanonical+1] - offbucket)/2; \
     pos = 0; \
     d = 0; \
     while ((d< depth) && (pos < fstnum[offbucket + d*2])  ) \
        { pos = (pos << 1) | bitget(stream,ptr); \
          ptr++; d++; \
        } \
     symb = (depth==d) ? pos : fstnum[offbucket+d*2+1] + pos - fstnum[offbucket+d*2]; \
   }
   
   
     //fst[0] = zone[0  ];
	 //fst[1] = zone[1*2];
	 //fst[i] = zone[i*2];

     //num[0] = zone[0  +1];
	 //num[1] = zone[1*2+1];
	 //num[i] = zone[i*2+1];



//#define decodeHuffDecMacroVariantWordPos2bits(HV, symb, stream, ptr, idCanonical) \
//   { uint pos; \
//     register uint d; \
//     uint sizeBuckbits= HV.sizeBuckbits; \
//     uint dirbElemSize = HV.dirbElemSize; \
//     uint sizeFstbits = HV.sizeFstbits; \
//     uint sizeNumbits = HV.sizeNumbits; \
//     uint *dirb = HV.Dirb; \
//     uint *zonefstnum = HV.zoneMem; \
//	 register uint offbucket; \
//	 offbucket = bitread (dirb, idCanonical*dirbElemSize, dirbElemSize); \
//	 register uint depth; \
//	 depth = bitread (dirb, (idCanonical+1)*dirbElemSize, dirbElemSize); \
//	 depth = (depth - offbucket)/sizeBuckbits; \
//     pos = 0; \
//     d = 0; \
//     register uint currfst; \
//     currfst = bitread (zonefstnum, (offbucket + d*sizeBuckbits), sizeFstbits); \
//     while ((pos < currfst)  ) \
//        { pos = (pos << 1) | bitget(stream,ptr); \
//          ptr++; d++; \
//          if (d<depth) \
//          	currfst = bitread (zonefstnum, (offbucket + d*sizeBuckbits), sizeFstbits); \
//          else break; \
//        } \
//    if (depth==d) \
//     	symb=pos; \
//    else { \
//    	uint currNum; \
//    	currNum = bitread (zonefstnum, (offbucket + d*sizeBuckbits+ sizeFstbits), sizeNumbits); \
//     symb = currNum + pos - currfst; \
//    } \
//  }



// int decodeHuffDecVariantWord (uint *fst , uint *symb, uint *stream, uint ptr, uint depth);


