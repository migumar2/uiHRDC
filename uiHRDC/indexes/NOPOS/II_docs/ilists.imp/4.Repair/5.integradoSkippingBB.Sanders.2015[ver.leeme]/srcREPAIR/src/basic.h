/* basics.h
   Copyright (C) 2005, Rodrigo Gonzalez, all rights reserved.

   Some preliminary stuff

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 2.1 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with this library; if not, write to the Free Software
   Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA

*/


#ifndef BASICSINCLUDED
#define BASICSINCLUDED

#include <sys/types.h>
#include <sys/resource.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/times.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>


#define mask31 0x0000001F

#define MAX(x,y) ((x)>(y)?(x):(y))
#define MIN(x,y) ((x)<(y)?(x):(y))

#define uint unsigned int

/*numero de bits del entero de la maquina*/
#define WW32 32
/* WW32-1 */
#define Wminusone 31
/*numero de bits del entero de la maquina*/
#define WW64 64
/*bits para hacer la mascara para contar mas rapido*/
#define bitsM 8
/*bytes que hacen una palabra */
#define BW 4
#ifndef uchar
#define uchar unsigned char
#endif
/*#ifndef uint
#define uint unsigned int
#endif*/
#define size_uchar 256

const unsigned char __popcount_tab[] =
{
0,1,1,2,1,2,2,3,1,2,2,3,2,3,3,4,1,2,2,3,2,3,3,4,2,3,3,4,3,4,4,5,
1,2,2,3,2,3,3,4,2,3,3,4,3,4,4,5,2,3,3,4,3,4,4,5,3,4,4,5,4,5,5,6,
1,2,2,3,2,3,3,4,2,3,3,4,3,4,4,5,2,3,3,4,3,4,4,5,3,4,4,5,4,5,5,6,
2,3,3,4,3,4,4,5,3,4,4,5,4,5,5,6,3,4,4,5,4,5,5,6,4,5,5,6,5,6,6,7,
1,2,2,3,2,3,3,4,2,3,3,4,3,4,4,5,2,3,3,4,3,4,4,5,3,4,4,5,4,5,5,6,
2,3,3,4,3,4,4,5,3,4,4,5,4,5,5,6,3,4,4,5,4,5,5,6,4,5,5,6,5,6,6,7,
2,3,3,4,3,4,4,5,3,4,4,5,4,5,5,6,3,4,4,5,4,5,5,6,4,5,5,6,5,6,6,7,
3,4,4,5,4,5,5,6,4,5,5,6,5,6,6,7,4,5,5,6,5,6,6,7,5,6,6,7,6,7,7,8,
};

const unsigned char select_tab[] =
{
0,1,2,1,3,1,2,1,4,1,2,1,3,1,2,1,5,1,2,1,3,1,2,1,4,1,2,1,3,1,2,1,
6,1,2,1,3,1,2,1,4,1,2,1,3,1,2,1,5,1,2,1,3,1,2,1,4,1,2,1,3,1,2,1,
7,1,2,1,3,1,2,1,4,1,2,1,3,1,2,1,5,1,2,1,3,1,2,1,4,1,2,1,3,1,2,1,
6,1,2,1,3,1,2,1,4,1,2,1,3,1,2,1,5,1,2,1,3,1,2,1,4,1,2,1,3,1,2,1,
8,1,2,1,3,1,2,1,4,1,2,1,3,1,2,1,5,1,2,1,3,1,2,1,4,1,2,1,3,1,2,1,
6,1,2,1,3,1,2,1,4,1,2,1,3,1,2,1,5,1,2,1,3,1,2,1,4,1,2,1,3,1,2,1,
7,1,2,1,3,1,2,1,4,1,2,1,3,1,2,1,5,1,2,1,3,1,2,1,4,1,2,1,3,1,2,1,
6,1,2,1,3,1,2,1,4,1,2,1,3,1,2,1,5,1,2,1,3,1,2,1,4,1,2,1,3,1,2,1,
};


/* bits needed to represent a number between 0 and n */
inline uint bits (uint n){
  uint b = 0;
  while (n) { b++; n >>= 1; }
  return b;
}

/* reads bit p from e */
#define bitget(e,p) ((((e)[(p)/WW32] >> ((p)%WW32))) & 1)
/* sets bit p in e */
#define bitset(e,p) ((e)[(p)/WW32] |= (1<<((p)%WW32)))
/* cleans bit p in e */
#define bitclean(e,p) ((e)[(p)/WW32] &= ~(1<<((p)%WW32)))

/* numero de enteros necesarios para representar e elementos de largo n */
#define enteros(e,n) ((e)*(n))/WW32+(((e)*(n))%WW32 > 0)

inline uint GetField(uint *A, register  uint len, register uint index) {
  //register uint i=index*len/WW32, j=index*len-WW32*i, result;
  register ulong i=(index/WW32)*len+((index%WW32)*len)/WW32, j=((index%WW32)*len)%WW32,result;
  if (j+len <= WW32)
    result = (A[i] << (WW32-j-len)) >> (WW32-len);
  else {
    result = A[i] >> j;
    result = result | (A[i+1] << (WW64-j-len)) >> (WW32-len);
  }
  return result;
}

inline void SetField(uint *A,register uint len, register uint index,register  uint x) {
   //uint i=index*len/WW32, j=index*len-i*WW32;
   register ulong i=(index/WW32)*len+((index%WW32)*len)/WW32, j=((index%WW32)*len)%WW32;
   uint mask = ((j+len) < WW32 ? ~0u << (j+len) : 0) | ((WW32-j) < WW32 ? ~0u >> (WW32-j) : 0);
   A[i] = (A[i] & mask) | x << j;
   if (j+len>WW32) {
      mask = ((~0u) << (len+j-WW32));
      A[i+1] = (A[i+1] & mask)| x >> (WW32-j);
   }
}


//being e a bitmap (a uint *)
// returns e[p..p+len-1], assuming len <= W
inline uint bitread_32 (uint *e, uint p, uint len)

   { uint answ;
     e += p/WW32; p %= WW32;
     answ = *e >> p;
     if (len == WW32)
	  { if (p) answ |= (*(e+1)) << (WW32-p);
	  }
     else { if (p+len > WW32) answ |= (*(e+1)) << (WW32-p);
            answ &= (1<<len)-1;
	  }
     return answ;
   }

//being e a bitmap (a uint *)
// writes e[p..p+len-1] = s, len <= W
inline void bitwrite_32 (register uint *e, register uint p, 
	       register uint len, register uint s)

   { e += p/WW32; p %= WW32;
     if (len == WW32)
	  { *e |= (*e & ((1<<p)-1)) | (s << p);
            if (!p) return;
            e++;
            *e = (*e & ~((1<<p)-1)) | (s >> (WW32-p));
	  }
     else { if (p+len <= WW32)
	       { *e = (*e & ~(((1<<len)-1)<<p)) | (s << p);
		 return;
	       }
	    *e = (*e & ((1<<p)-1)) | (s << p);
            e++; len -= WW32-p;
            *e = (*e & ~((1<<len)-1)) | (s >> (WW32-p));
	  }
   }



inline unsigned GetFieldW32(unsigned *A,register unsigned index) {
  return A[index];
}

inline void SetField32(unsigned *A, register unsigned index,register unsigned x) {
  A[index]=x;
}

inline unsigned GetFieldW16(unsigned *A,register unsigned index) {
  register unsigned i=index/2, j=(index&1)<<4, result;
  result = (A[i] << (16-j)) >> (16);
  return result;
}

inline unsigned GetFieldW4(unsigned *A,register unsigned index) {
  register unsigned i=index/8, j=(index&0x7)<<2;
  /*register unsigned i=index/8, j=index*4-32*i; */
  return (A[i] << (28-j)) >> (28);
}


inline uint popcount (register int x){
  return __popcount_tab[(x >>  0) & 0xff]  + __popcount_tab[(x >>  8) & 0xff]  + __popcount_tab[(x >> 16) & 0xff] + __popcount_tab[(x >> 24) & 0xff];
}

inline uint popcount16 (register int x){
  return __popcount_tab[x & 0xff]  + __popcount_tab[(x >>  8) & 0xff];
}

inline uint popcount8 (register int x){
  return __popcount_tab[x & 0xff];
}

inline int len_uchar(uchar *s) {
  int i;
  for (i=0;;i++) 
    if (s[i] == '\0') return i;
}

#endif

