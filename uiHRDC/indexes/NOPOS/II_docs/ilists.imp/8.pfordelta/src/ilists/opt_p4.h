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





#include "pf.h"
#define BS 128
using namespace std;

//int dnum[17] = {0,1,2,3,4,5,6,7,8,9,10,11,12,13,16,20,32};

void p4_encode(unsigned int *doc_id, int npos, int b,unsigned int *buf , int *size, int *ex_n)
{
	int i = 0;
	unsigned int *ww = buf;
	detailed_p4_encode(&ww, &(doc_id[i]), b, size,ex_n);
}

/*
*	when list_size is too small, not good to use this function
*/
int OPT4(unsigned int *doc_id,unsigned int list_size,unsigned int *aux)
{
	int i,j,l;
	for(i=0; i<2*BS; i++)
	{
		doc_id[i+list_size] = 0 ;		// pack the input, avoid garbage data in the end
	}
	int size = 0;
	int ex_n = 0;
	int csize = 0; 	// compressed size in bytes
			
	int chunk_size = 0;																					
	int b = -1, temp_en = 0;
	int offset = 0;
	for(j=0;j<list_size;j+=BS)				// for each chunk
	{
		chunk_size = 999999999;
		b = -1;
		// get the smallest chunk size by trying all b's
		for(l=0;l<16;l++)
				{
					p4_encode(doc_id+j, BS, l, aux+offset, &size, &ex_n);
					if(chunk_size > size * 4)			// int bytes
					{
						chunk_size = size *4;
						b = l;
						temp_en = ex_n;
					}		
				}	

				csize += chunk_size;
				//printf("encode:%u\n", b);
				p4_encode(doc_id + j, BS, b, aux + offset, &size, &ex_n);
				offset += size;
	}
	
	return csize;	
}
