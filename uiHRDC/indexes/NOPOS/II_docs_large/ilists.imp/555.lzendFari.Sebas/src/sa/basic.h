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

#define mask31 0x0000001F

#define max(x,y) ((x)>(y)?(x):(y))
#define min(x,y) ((x)<(y)?(x):(y))


/*numero de bits del entero de la maquina*/
#define W 32
/* W-1 */
#define Wminusone 31
/*numero de bits del entero de la maquina*/
#define WW 64
/*bits para hacer la mascara para contar mas rapido*/
#define bitsM 8
/*bytes que hacen una palabra */
#define BW 4
#ifndef uchar
#define uchar unsigned char
#endif
#ifndef ulong
#define ulong unsigned long
#endif
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
#define bitget(e,p) ((((e)[(p)/W] >> ((p)%W))) & 1)
/* sets bit p in e */
#define bitset(e,p) ((e)[(p)/W] |= (1<<((p)%W)))
/* cleans bit p in e */
#define bitclean(e,p) ((e)[(p)/W] &= ~(1<<((p)%W)))

/* numero de enteros necesarios para representar e elementos de largo n */
#define enteros(e,n) ((e)*(n))/W+(((e)*(n))%W > 0)

inline ulong GetField(ulong *A, register  ulong len, register ulong index) {
  register ulong i=index*len/W, j=index*len-W*i, result;
  if (j+len <= W)
    result = (A[i] << (W-j-len)) >> (W-len);
  else {
    result = A[i] >> j;
    result = result | (A[i+1] << (WW-j-len)) >> (W-len);
  }
  return result;
}

inline void SetField(ulong *A,register ulong len, register ulong index,register  ulong x) {
   ulong i=index*len/W, j=index*len-i*W;
   ulong mask = ((j+len) < W ? ~0u << (j+len) : 0) | ((W-j) < W ? ~0u >> (W-j) : 0);
   A[i] = (A[i] & mask) | x << j;
   if (j+len>W) {
      mask = ((~0u) << (len+j-W));
      A[i+1] = (A[i+1] & mask)| x >> (W-j);
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

