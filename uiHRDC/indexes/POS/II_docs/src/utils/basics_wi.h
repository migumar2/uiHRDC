
 // Basics
 
#ifndef BASICSINCLUDED
#define BASICSINCLUDED

  // Includes 

#include <sys/types.h>
#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/times.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>

  // Memory management
/*
#define malloc(n) Malloc(n)
#define free(p) Free(p)
#define realloc(p,n) Realloc(p,n)


void *Malloc (size_t n);
void Free (void *p);
void *Realloc (void *p, size_t n);
*/
  // Data types

#ifndef byte
	#define byte unsigned char
#endif 

//typedef unsigned char byte;
// typedef unsigned int uint;

//typedef int bool;
//#define true 1
//#define false 0

#define wimax(x,y) ((x)>(y)?(x):(y))
#define wimin(x,y) ((x)<(y)?(x):(y))

  // Bitstream management

//#define WI32 (8*sizeof(uint))
#define WI32 (32)

	// bits needed to represent a number between 0 and n
uint bits_wi (uint n);
        // returns e[p..p+len-1], assuming len <= WI32
uint bitread_wi (uint *e, uint p, uint len);
        // writes e[p..p+len-1] = s, assuming len <= WI32
void bitwrite_wi (uint *e, uint p, uint len, uint s);
    // writes e[p..p+len-1] = 0, no assumption on len
        
    /**/ //FARI. WITH ASSUMPTION ON LEN, OR IT CRASHES 
         //NOt WORKING UPON THE LIMIT OF THE STARTING uint.
void bitzero_wi (uint *e, uint p, uint len);
	// reads bit p from e
#define bitget_wi(e,p) (((e)[(p)/WI32] >> ((p)%WI32)) & 1)
	// sets bit p in e
#define bitset_wi(e,p) ((e)[(p)/WI32] |= (1<<((p)%WI32)))
	// cleans bit p in e
#define bitclean(_wie,p) ((e)[(p)/WI32] &= ~(1<<((p)%WI32)))



/* bitRead and bitWrite as MACROS */
	// returns e[p..p+len-1], assuming len <= WI32
	//mybitread (uint returned value, uint *e, uint p, uint len)
#define mybitread_wi(answ, v, p, len) \
   { uint *e ; \
   	 e=v;\
     e += p/WI32; p %= WI32; \
     answ = *e >> p; \
     if (len == WI32) \
	  { if (p) answ |= (*(e+1)) << (WI32-p); \
	  } \
     else { if (p+len > WI32) answ |= (*(e+1)) << (WI32-p); \
            answ &= (1<<len)-1; \
	  } \
   }


  	// writes e[p..p+len-1] = s, len <= WI32
	//void bitwrite (uint *e, uint p, uint len, uint s)
#define mybitwrite_wi(v, p, len, s) \
   { uint *e ; \
   	 e=v; \
    e += p/WI32; p %= WI32; \
     if (len == WI32) \
	  { *e |= (*e & ((1<<p)-1)) | (s << p); \
            if (p) { \
            	e++; \
            	*e = (*e & ~((1<<p)-1)) | (s >> (WI32-p)); \
        	} \
	  } \
     else { if (p+len <= WI32) \
	       { *e = (*e & ~(((1<<len)-1)<<p)) | (s << p); \
	       } \
	       else { \
	    	*e = (*e & ((1<<p)-1)) | (s << p); \
            e++; len -= WI32-p; \
            *e = (*e & ~((1<<len)-1)) | (s >> (WI32-p)); \
           } \
	  } \
   } 

#endif
