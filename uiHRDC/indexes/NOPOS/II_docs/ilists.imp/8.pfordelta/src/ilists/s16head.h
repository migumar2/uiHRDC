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
// This is an implementation of Simple16 algorithm for sorted integer arrays.
// The basic ideas are based on papers from:
//   1. http://www2008.org/papers/pdf/p387-zhangA.pdf 
//   2. http://www2009.org/proceedings/pdf/p401.pdf 
//
// The maximum possible integer value Simple16 can encode is < 2^28 (this is 
// dertermined by the Simple16 algorithm itself). Therefore, in order to use
// Simple16, the application must write their own code to encode numbers in 
// the range of [2^28, 2^32). A simple way is just write those numbers as 
// 32-bit integers (that is, no compression for very big numbers). 
//
// Alternative implementations:
//   * C++ http://code.google.com/p/poly-ir-toolkit/source/browse/trunk/src/compression_toolkit/s16_coding.cc
//   * Java https://github.com/hyan/kamikaze/blob/master/src/main/java/com/kamikaze/pfordelta/Simple16.java
//
// This code is based on an implementation in C++ of the Poly-IR-Toolkit. 
// It's available at http://code.google.com/p/poly-ir-toolkit/source/browse/trunk/src/compression_toolkit/s16_coding.cc
//





void s16_encode(unsigned int **_w, unsigned int **_p, unsigned int m)
{ 
int cnum[16] = {28, 21, 21, 21, 14, 9, 8, 7, 6, 6, 5, 5, 4, 3, 2, 1};
int cbits[16][28] = { {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
   {2,2,2,2,2,2,2,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0},
   {1,1,1,1,1,1,1,2,2,2,2,2,2,2,1,1,1,1,1,1,1,0,0,0,0,0,0,0},
   {1,1,1,1,1,1,1,1,1,1,1,1,1,1,2,2,2,2,2,2,2,0,0,0,0,0,0,0},
   {2,2,2,2,2,2,2,2,2,2,2,2,2,2,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
   {4,3,3,3,3,3,3,3,3,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
   {3,4,4,4,4,3,3,3,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
   {4,4,4,4,4,4,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
   {5,5,5,5,4,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
   {4,4,5,5,5,5,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
   {6,6,6,5,5,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
   {5,5,6,6,6,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
   {7,7,7,7,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
   {10,9,9,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
   {14,14,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
   {28,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0} };

  unsigned int _k, _j, _m, _o;

  for (_k = 0; _k < 16; _k++) 
  { 
    (**_w) = _k<<28; 
    _m = (cnum[_k] < m)? cnum[_k]:m; 
    for (_j = 0, _o = 0; (_j < _m) && (*((*_p)+_j) < (1<<cbits[_k][_j])); ) 
    { 
      (**_w) += ((*((*_p)+_j))<<_o); 
      _o += cbits[_k][_j]; 
      _j++;
    } 
    if (_j == _m) 
    { 
      (*_p) += _m; 
      (*_w)++; 
      break; 
    } 
  } 
}


/* more optimized handcoded edition */

#define S16_DECODE(_w, _p)	\
{ \
  _k = (*_w)>>28; \
  switch(_k) \
  { \
    case 0: \
      *_p = (*_w) & 1;     _p++; \
      *_p = (*_w>>1) & 1;  _p++; \
      *_p = (*_w>>2) & 1;  _p++; \
      *_p = (*_w>>3) & 1;  _p++; \
      *_p = (*_w>>4) & 1;  _p++; \
      *_p = (*_w>>5) & 1;  _p++; \
      *_p = (*_w>>6) & 1;  _p++; \
      *_p = (*_w>>7) & 1;  _p++; \
      *_p = (*_w>>8) & 1;  _p++; \
      *_p = (*_w>>9) & 1;  _p++; \
      *_p = (*_w>>10) & 1;  _p++; \
      *_p = (*_w>>11) & 1;  _p++; \
      *_p = (*_w>>12) & 1;  _p++; \
      *_p = (*_w>>13) & 1;  _p++; \
      *_p = (*_w>>14) & 1;  _p++; \
      *_p = (*_w>>15) & 1;  _p++; \
      *_p = (*_w>>16) & 1;  _p++; \
      *_p = (*_w>>17) & 1;  _p++; \
      *_p = (*_w>>18) & 1;  _p++; \
      *_p = (*_w>>19) & 1;  _p++; \
      *_p = (*_w>>20) & 1;  _p++; \
      *_p = (*_w>>21) & 1;  _p++; \
      *_p = (*_w>>22) & 1;  _p++; \
      *_p = (*_w>>23) & 1;  _p++; \
      *_p = (*_w>>24) & 1;  _p++; \
      *_p = (*_w>>25) & 1;  _p++; \
      *_p = (*_w>>26) & 1;  _p++; \
      *_p = (*_w>>27) & 1;  _p++; \
      break; \
    case 1: \
      *_p = (*_w) & 3;     _p++; \
      *_p = (*_w>>2) & 3;  _p++; \
      *_p = (*_w>>4) & 3;  _p++; \
      *_p = (*_w>>6) & 3;  _p++; \
      *_p = (*_w>>8) & 3;  _p++; \
      *_p = (*_w>>10) & 3;  _p++; \
      *_p = (*_w>>12) & 3;  _p++; \
      *_p = (*_w>>14) & 1;  _p++; \
      *_p = (*_w>>15) & 1;  _p++; \
      *_p = (*_w>>16) & 1;  _p++; \
      *_p = (*_w>>17) & 1;  _p++; \
      *_p = (*_w>>18) & 1;  _p++; \
      *_p = (*_w>>19) & 1;  _p++; \
      *_p = (*_w>>20) & 1;  _p++; \
      *_p = (*_w>>21) & 1;  _p++; \
      *_p = (*_w>>22) & 1;  _p++; \
      *_p = (*_w>>23) & 1;  _p++; \
      *_p = (*_w>>24) & 1;  _p++; \
      *_p = (*_w>>25) & 1;  _p++; \
      *_p = (*_w>>26) & 1;  _p++; \
      *_p = (*_w>>27) & 1;  _p++; \
      break; \
    case 2: \
      *_p = (*_w) & 1;     _p++; \
      *_p = (*_w>>1) & 1;  _p++; \
      *_p = (*_w>>2) & 1;  _p++; \
      *_p = (*_w>>3) & 1;  _p++; \
      *_p = (*_w>>4) & 1;  _p++; \
      *_p = (*_w>>5) & 1;  _p++; \
      *_p = (*_w>>6) & 1;  _p++; \
      *_p = (*_w>>7) & 3;  _p++; \
      *_p = (*_w>>9) & 3;  _p++; \
      *_p = (*_w>>11) & 3;  _p++; \
      *_p = (*_w>>13) & 3;  _p++; \
      *_p = (*_w>>15) & 3;  _p++; \
      *_p = (*_w>>17) & 3;  _p++; \
      *_p = (*_w>>19) & 3;  _p++; \
      *_p = (*_w>>21) & 1;  _p++; \
      *_p = (*_w>>22) & 1;  _p++; \
      *_p = (*_w>>23) & 1;  _p++; \
      *_p = (*_w>>24) & 1;  _p++; \
      *_p = (*_w>>25) & 1;  _p++; \
      *_p = (*_w>>26) & 1;  _p++; \
      *_p = (*_w>>27) & 1;  _p++; \
      break; \
    case 3: \
      *_p = (*_w) & 1;     _p++; \
      *_p = (*_w>>1) & 1;  _p++; \
      *_p = (*_w>>2) & 1;  _p++; \
      *_p = (*_w>>3) & 1;  _p++; \
      *_p = (*_w>>4) & 1;  _p++; \
      *_p = (*_w>>5) & 1;  _p++; \
      *_p = (*_w>>6) & 1;  _p++; \
      *_p = (*_w>>7) & 1;  _p++; \
      *_p = (*_w>>8) & 1;  _p++; \
      *_p = (*_w>>9) & 1;  _p++; \
      *_p = (*_w>>10) & 1;  _p++; \
      *_p = (*_w>>11) & 1;  _p++; \
      *_p = (*_w>>12) & 1;  _p++; \
      *_p = (*_w>>13) & 1;  _p++; \
      *_p = (*_w>>14) & 3;  _p++; \
      *_p = (*_w>>16) & 3;  _p++; \
      *_p = (*_w>>18) & 3;  _p++; \
      *_p = (*_w>>20) & 3;  _p++; \
      *_p = (*_w>>22) & 3;  _p++; \
      *_p = (*_w>>24) & 3;  _p++; \
      *_p = (*_w>>26) & 3;  _p++; \
      break; \
    case 4: \
      *_p = (*_w) & 3;     _p++; \
      *_p = (*_w>>2) & 3;  _p++; \
      *_p = (*_w>>4) & 3;  _p++; \
      *_p = (*_w>>6) & 3;  _p++; \
      *_p = (*_w>>8) & 3;  _p++; \
      *_p = (*_w>>10) & 3;  _p++; \
      *_p = (*_w>>12) & 3;  _p++; \
      *_p = (*_w>>14) & 3;  _p++; \
      *_p = (*_w>>16) & 3;  _p++; \
      *_p = (*_w>>18) & 3;  _p++; \
      *_p = (*_w>>20) & 3;  _p++; \
      *_p = (*_w>>22) & 3;  _p++; \
      *_p = (*_w>>24) & 3;  _p++; \
      *_p = (*_w>>26) & 3;  _p++; \
      break; \
    case 5: \
      *_p = (*_w) & 15;     _p++; \
      *_p = (*_w>>4) & 7;  _p++; \
      *_p = (*_w>>7) & 7;  _p++; \
      *_p = (*_w>>10) & 7;  _p++; \
      *_p = (*_w>>13) & 7;  _p++; \
      *_p = (*_w>>16) & 7;  _p++; \
      *_p = (*_w>>19) & 7;  _p++; \
      *_p = (*_w>>22) & 7;  _p++; \
      *_p = (*_w>>25) & 7;  _p++; \
      break; \
    case 6: \
      *_p = (*_w) & 7;     _p++; \
      *_p = (*_w>>3) & 15;  _p++; \
      *_p = (*_w>>7) & 15;  _p++; \
      *_p = (*_w>>11) & 15;  _p++; \
      *_p = (*_w>>15) & 15;  _p++; \
      *_p = (*_w>>19) & 7;  _p++; \
      *_p = (*_w>>22) & 7;  _p++; \
      *_p = (*_w>>25) & 7;  _p++; \
      break; \
    case 7: \
      *_p = (*_w) & 15;     _p++; \
      *_p = (*_w>>4) & 15;  _p++; \
      *_p = (*_w>>8) & 15;  _p++; \
      *_p = (*_w>>12) & 15;  _p++; \
      *_p = (*_w>>16) & 15;  _p++; \
      *_p = (*_w>>20) & 15;  _p++; \
      *_p = (*_w>>24) & 15;  _p++; \
      break; \
    case 8: \
      *_p = (*_w) & 31;     _p++; \
      *_p = (*_w>>5) & 31;  _p++; \
      *_p = (*_w>>10) & 31;  _p++; \
      *_p = (*_w>>15) & 31;  _p++; \
      *_p = (*_w>>20) & 15;  _p++; \
      *_p = (*_w>>24) & 15;  _p++; \
      break; \
    case 9: \
      *_p = (*_w) & 15;     _p++; \
      *_p = (*_w>>4) & 15;  _p++; \
      *_p = (*_w>>8) & 31;  _p++; \
      *_p = (*_w>>13) & 31;  _p++; \
      *_p = (*_w>>18) & 31;  _p++; \
      *_p = (*_w>>23) & 31;  _p++; \
      break; \
    case 10: \
      *_p = (*_w) & 63;     _p++; \
      *_p = (*_w>>6) & 63;  _p++; \
      *_p = (*_w>>12) & 63;  _p++; \
      *_p = (*_w>>18) & 31;  _p++; \
      *_p = (*_w>>23) & 31;  _p++; \
      break; \
    case 11: \
      *_p = (*_w) & 31;     _p++; \
      *_p = (*_w>>5) & 31;  _p++; \
      *_p = (*_w>>10) & 63;  _p++; \
      *_p = (*_w>>16) & 63;  _p++; \
      *_p = (*_w>>22) & 63;  _p++; \
      break; \
    case 12: \
      *_p = (*_w) & 127;     _p++; \
      *_p = (*_w>>7) & 127;  _p++; \
      *_p = (*_w>>14) & 127;  _p++; \
      *_p = (*_w>>21) & 127;  _p++; \
      break; \
    case 13: \
      *_p = (*_w) & 1023;     _p++; \
      *_p = (*_w>>10) & 511;  _p++; \
      *_p = (*_w>>19) & 511;  _p++; \
      break; \
    case 14: \
      *_p = (*_w) & 16383;     _p++; \
      *_p = (*_w>>14) & 16383;  _p++; \
      break; \
    case 15: \
      *_p = (*_w) & ((1<<28)-1);     _p++; \
      break; \
  }\
  _w++; \
}





