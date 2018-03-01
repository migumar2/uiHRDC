//Some edits were done in uiHRDC code

////
// Copyright (c) 2008, WEST, Polytechnic Institute of NYU
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
//  1. Redistributions of source code must retain the above copyright notice,
//     this list of conditions and the following disclaimer.
//  2. Redistributions in binary form must reproduce the above copyright notice,
//     this list of conditions and the following disclaimer in the documentation
//     and/or other materials provided with the distribution.
//  3. Neither the name of WEST, Polytechnic Institute of NYU nor the names
//     of its contributors may be used to endorse or promote products derived
//     from this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
// OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
// LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
// DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
// THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
// Author(s): Torsten Suel, Jiangong Zhang, Jinru He
//
// If you have any questions or problems with our code, please contact:
// jhe@cis.poly.edu
//

////
// This is an implementation of PForDelta algorithm for sorted integer arrays.
// The PForDelta coding method is fast but compression efficiency is not as good as Rice
// coding. It is a blockwise coding, so you need to first set the block size to
// either 32, 64, 128, or 256. The default block size is 128. If the input
// buffer to the Compression() function is greater in size than the block size,
// any integers past the block size will be discarded.
// The meta data is stored as an uncompressed integer written at the beginning
// of the buffer; it includes the number of bits used per integer, the block
// size, and the number of integers coded without exceptions.
//
// 1. Original algorithm from:
//     http://citeseerx.ist.psu.edu/viewdoc/summary?doi=10.1.1.101.3316 and 
//     http://dx.doi.org/10.1109/ICDE.2006.150
//
// 2. Optimizacion from:
//     http://www2008.org/papers/pdf/p387-zhangA.pdf
//
// Alternative implementations:
//   * C++ http://code.google.com/p/poly-ir-toolkit/source/browse/trunk/src/compression_toolkit/pfor_coding.cc
//   * Java https://github.com/hyan/kamikaze/blob/master/src/main/java/com/kamikaze/pfordelta/PForDelta.java
//
// This code is based on an implementation in C++ of the Poly-IR-Toolkit. 
// It's available at http://code.google.com/p/poly-ir-toolkit/source/browse/trunk/src/compression_toolkit/pfor_coding.cc
//


#include "stdlib.h"
#include "stdio.h"
#include "s16head.h"
#include "unpack.h"


#define BS 128 
#define FRAC 0.10 
#define S 16 
#define PCHUNK 128

void pack(unsigned int *v, unsigned int b, unsigned int n, unsigned int *w);


int detailed_p4_encode(unsigned int **w, unsigned int* p, int num , int *chunk_size, int * exception_n)
{
	
	int i, j, t, s;

	unsigned int b = cnum[num];
	int bb_e;
	int bb_p;
	int p_low;
	unsigned int e_n = 0;
	int max_p = 0;
	int max_e = 0;
 
	unsigned int* out = (unsigned*)malloc(sizeof(unsigned)*PCHUNK*2);
	unsigned int* ex = (unsigned*)malloc(sizeof(unsigned)*PCHUNK*2);
	unsigned int* po = (unsigned*)malloc(sizeof(unsigned)*PCHUNK*2);

	unsigned int* tp = NULL;
	unsigned int *_pp, *_ww;

	if (b == 32)
	{
		(*w)[0] = ((b<<10)) + (0);
		*w +=1;	
		for (i = 0; i < PCHUNK ; i++)  (*w)[i] = p[i];
		*w += (PCHUNK);
		(*chunk_size) = 1 + BS;

		free(out);
		free(ex);
		free(po);
		return 0;
	}

	for (i = 0; i < PCHUNK ; i++)
	{
		if ( p[i] >= (1<<b) )		//exception
		{
			p_low = p[i] & ((1<<b)-1);
			out[i] = p_low;
			ex[e_n] = (p[i] >> b);
			po[(e_n++)] = i;               //          
		}
		else
			out[i] = p[i];
	}

	if (1)		// force to pass every time
	{
		/*get the gap of position*/
		for(j = e_n-1;j>0;j--)
		{
			po[j] = po[j] - po[j-1] ; 
			po[j] --;
		}
 	
		s = ((b * PCHUNK)>>5);
		tp = (*w);
		(*w)[0] = ((num<<10))+e_n;			// record b and number of exceptions into this value, in the other version we pick this value out and did not count it
		(*w) += 1;		
		for (i = 0; i < s; i++)  (*w)[i] = 0;
		pack(out, b, PCHUNK , *w);
		*w += s;

		unsigned int *all_array = (unsigned*)malloc(sizeof(unsigned)*PCHUNK*4) ;
		for(j=0;j<e_n;j++)		
		{
			all_array[j] = po[j];
			all_array[e_n+j] =ex[j];
		}
		for (_pp = all_array, _ww = (*w); _pp < &(all_array[2*e_n]); )
			s16_encode(&_ww, &_pp, &(all_array[2*e_n]) - _pp);

		(*chunk_size) = 1 + s + (_ww - (*w)) ;

		(*w) += (_ww - (*w)) ;
		
		(*exception_n) = e_n;

		free(out);
		free(ex);
		free(po);
		free(all_array);
		return (e_n);

	}
}


void pack(unsigned int *v, unsigned int b, unsigned int n, unsigned int *w)
{
  int i, bp, wp, s;

  for (bp = 0, i = 0; i < n; i++, bp += b)
  {
    wp = bp>>5;
    s = 32 - b - (bp & 31);
    if (s >= 0)
      w[wp] |= (v[i]<<s);
    else
    {
      s = -s;
      w[wp] |= (v[i]>>s);
      w[wp+1] = (v[i]<<(32-s));
    }
  }   
}

/*modified p4decode */
unsigned int *detailed_p4_decode(unsigned int *_p, unsigned int *_w,  unsigned int * all_array)
{
//	static int MAYOR=0;
	
  int i, s;
  unsigned int x;
  int flag = _w[0];
  (_w)++;
  
  unsigned int *_ww,*_pp;
  unsigned int b = ((flag>>10) & 31);
  unsigned int e_n = (flag & 1023) ;

  (unpack[b])(_p, _w);

  b = cnum[b];
  _w += ((b * BS)>>5);
  unsigned int _k = 0;
  unsigned int psum = 0;
  if(e_n != 0 )
  {
	  for (_pp = all_array, _ww = (unsigned int *)(_w); _pp < &(all_array[e_n*2]); )
	  {
		  S16_DECODE(_ww, _pp);
	  }

	  _w += (_ww - _w);
	  psum = all_array[0];

	  for(i=0;i<e_n;i++)
	  {
		  _p[psum] += (all_array[e_n+i]<<b);
		  psum += all_array[ i + 1] + 1;
//		  if (psum > MAYOR) {MAYOR=psum; printf("\n max psum = %i", MAYOR);}
	  }
  }
  return(_w);
}


