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

// Author(s): Torsten Suel, Jiangong Zhang, Jinru He
//
// If you have any questions or problems with our code, please contact:
// jhe@cis.poly.edu

//Ported to C, by Antonio Fariña (University of A Coruña) in May 2011.


#include "s9_coding.h"

#define ESCAPE_S9 ((1u<<28u)-1)

int s9_Compression(unsigned int* input, unsigned int* output, int size) {
  int left = size;
  int ret;
  unsigned int escape = ESCAPE_S9;
  unsigned* tmp = output;
  
  while (left > 0) {
	if ( (*input) >= ESCAPE_S9 ) {
		//printf("\nvalue %u greather that escape = %u", *input, ESCAPE_S9);
        ret = s9_encode(tmp, &escape, left);
        left -= ret;
        if( ret != 1) {printf("\n\n deberia ser 1, pero es %u\n\n\n",ret ); }
        
        tmp++;
        
        *tmp = *input;
        input ++;
        tmp ++; 	
	}
	else {
//		printf("\nvalue %u less that escape = %u", *input, ESCAPE_S9);
		ret = s9_encode(tmp, input, left);
		input += ret;
		left -= ret;
		tmp++;
	}
  }

  return tmp - output;
}


int s9_Decompression(unsigned int* input, unsigned int* output, int size) {
  unsigned int *_p = output;
  int num;
  unsigned* tmp = input;

  int left = size;
  while (left > 0) {
    num = s9_decode(tmp,&_p);
    //printf("\n num = %u",num);
    if ((num == 1) && (*(_p-1) == ESCAPE_S9)) {
		tmp++;
		*(_p-1) = *tmp;
	}
    tmp++;
    left -= num;
  }

  return tmp - input;
}

//const int csize[9] = {1,2,3,4,5,7,9,14,28};
//const int conum[9] = {28,14,9,7,5,4,3,2,1};

/*
int s9_Compression(unsigned int* input, unsigned int* output, int size) {
  int left = size;
  int ret;
  unsigned* tmp = output;
  while (left > 0) {
    ret = s9_encode(tmp, input, left);
    input += ret;
    left -= ret;
    tmp++;
  }

  return tmp - output;
}


int s9_Decompression(unsigned int* input, unsigned int* output, int size) {
  unsigned int *_p = output;
  int num;
  unsigned* tmp = input;

  int left = size;
  while (left > 0) {
    num = s9_decode(tmp,&_p);
    tmp++;
    left -= num;
  }

  return tmp - input;
}
*/

