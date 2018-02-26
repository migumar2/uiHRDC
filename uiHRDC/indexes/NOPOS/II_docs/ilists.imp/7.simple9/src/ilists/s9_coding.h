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
//
// S9 Coding:
// In each block, we do not need to pad the last word with zero entries,
// because the coding method knows the block size and in the implementation,
// the decompression method will know where to stop.
// Each encoded word has a 4-bit header indicating which case was used to
// encode it. Thus, this limits the maximum integer that can be encoded to
// (2**28)-1.

//Ported to C, by Antonio Fariña (University of A Coruña) in May 2011.

#ifndef S9_CODING_H_
#define S9_CODING_H_


//public:
  /** compresses the "size" uints in "input".
   *  the encoded secuence is set into "output" and its size (number of uints) 
   *  returned
   *  Memory for "output" must be allocate by the caller
   */
  int s9_Compression(unsigned int* input, unsigned int* output, int size);
  
  /** decompresses the "size" uints in the compressed sequence of uints "input".
   *  the decoded secuence is set into "output".
   *  Memory for "output" must be allocate by the caller. Actually the caller must
   *  allocate at least size+28 uints!!  <-------------------------------------
   *  In addition it returns the number of uints processed from the "input" array.
   *    (this is not typically of interest, but could be of interest to move ptrs forward)
   */  
  int s9_Decompression(unsigned int* input, unsigned int* output, int size);

//private:
//  int s9_encode(unsigned int* _w, unsigned int* _p, int M);
//  int s9_decode(unsigned int* _w, unsigned int **_p);





inline int s9_encode(unsigned int* _w, unsigned int* _p, int M) {
	static int csize[9] = {1,2,3,4,5,7,9,14,28};
	static int conum[9] = {28,14,9,7,5,4,3,2,1};

  int _j;
  int _m;
  for (int _k = 0; _k < 9; _k++) {
    *_w = _k << 28;
    _m = (conum[_k] < (M)) ? conum[_k] : (M);
    for (_j = 0; (_j < _m) && (*(_p + _j) < static_cast<unsigned> (1 << csize[_k])); _j++) {
      *_w |= (*(_p + _j)) << (csize[_k] * _j);
    }

    if (_j == _m) {
      _p += _m;
      _w++;
      break;
    }
  }

  return _m;
}

inline int s9_decode(unsigned int* _w, unsigned int **pp) {
  static int conum[9] = {28,14,9,7,5,4,3,2,1};
  unsigned int * _p = *pp;
  int _k;
  _k = (*_w) >> 28;
  switch (_k) {
    case 0:
      *_p = (*_w) & 1;
      _p++;
      *_p = (*_w >> 1) & 1;
      _p++;
      *_p = (*_w >> 2) & 1;
      _p++;
      *_p = (*_w >> 3) & 1;
      _p++;
      *_p = (*_w >> 4) & 1;
      _p++;
      *_p = (*_w >> 5) & 1;
      _p++;
      *_p = (*_w >> 6) & 1;
      _p++;
      *_p = (*_w >> 7) & 1;
      _p++;
      *_p = (*_w >> 8) & 1;
      _p++;
      *_p = (*_w >> 9) & 1;
      _p++;
      *_p = (*_w >> 10) & 1;
      _p++;
      *_p = (*_w >> 11) & 1;
      _p++;
      *_p = (*_w >> 12) & 1;
      _p++;
      *_p = (*_w >> 13) & 1;
      _p++;
      *_p = (*_w >> 14) & 1;
      _p++;
      *_p = (*_w >> 15) & 1;
      _p++;
      *_p = (*_w >> 16) & 1;
      _p++;
      *_p = (*_w >> 17) & 1;
      _p++;
      *_p = (*_w >> 18) & 1;
      _p++;
      *_p = (*_w >> 19) & 1;
      _p++;
      *_p = (*_w >> 20) & 1;
      _p++;
      *_p = (*_w >> 21) & 1;
      _p++;
      *_p = (*_w >> 22) & 1;
      _p++;
      *_p = (*_w >> 23) & 1;
      _p++;
      *_p = (*_w >> 24) & 1;
      _p++;
      *_p = (*_w >> 25) & 1;
      _p++;
      *_p = (*_w >> 26) & 1;
      _p++;
      *_p = (*_w >> 27) & 1;
      _p++;
      break;
    case 1:
      *_p = (*_w) & 3;
      _p++;
      *_p = (*_w >> 2) & 3;
      _p++;
      *_p = (*_w >> 4) & 3;
      _p++;
      *_p = (*_w >> 6) & 3;
      _p++;
      *_p = (*_w >> 8) & 3;
      _p++;
      *_p = (*_w >> 10) & 3;
      _p++;
      *_p = (*_w >> 12) & 3;
      _p++;
      *_p = (*_w >> 14) & 3;
      _p++;
      *_p = (*_w >> 16) & 3;
      _p++;
      *_p = (*_w >> 18) & 3;
      _p++;
      *_p = (*_w >> 20) & 3;
      _p++;
      *_p = (*_w >> 22) & 3;
      _p++;
      *_p = (*_w >> 24) & 3;
      _p++;
      *_p = (*_w >> 26) & 3;
      _p++;
      break;
    case 2:
      *_p = (*_w) & 7;
      _p++;
      *_p = (*_w >> 3) & 7;
      _p++;
      *_p = (*_w >> 6) & 7;
      _p++;
      *_p = (*_w >> 9) & 7;
      _p++;
      *_p = (*_w >> 12) & 7;
      _p++;
      *_p = (*_w >> 15) & 7;
      _p++;
      *_p = (*_w >> 18) & 7;
      _p++;
      *_p = (*_w >> 21) & 7;
      _p++;
      *_p = (*_w >> 24) & 7;
      _p++;
      break;
    case 3:
      *_p = (*_w) & 15;
      _p++;
      *_p = (*_w >> 4) & 15;
      _p++;
      *_p = (*_w >> 8) & 15;
      _p++;
      *_p = (*_w >> 12) & 15;
      _p++;
      *_p = (*_w >> 16) & 15;
      _p++;
      *_p = (*_w >> 20) & 15;
      _p++;
      *_p = (*_w >> 24) & 15;
      _p++;
      break;
    case 4:
      *_p = (*_w) & 31;
      _p++;
      *_p = (*_w >> 5) & 31;
      _p++;
      *_p = (*_w >> 10) & 31;
      _p++;
      *_p = (*_w >> 15) & 31;
      _p++;
      *_p = (*_w >> 20) & 31;
      _p++;
      break;
    case 5:
      *_p = (*_w) & 127;
      _p++;
      *_p = (*_w >> 7) & 127;
      _p++;
      *_p = (*_w >> 14) & 127;
      _p++;
      *_p = (*_w >> 21) & 127;
      _p++;
      break;
    case 6:
      *_p = (*_w) & 511;
      _p++;
      *_p = (*_w >> 9) & 511;
      _p++;
      *_p = (*_w >> 18) & 511;
      _p++;
      break;
    case 7:
      *_p = (*_w) & 16383;
      _p++;
      *_p = (*_w >> 14) & 16383;
      _p++;
      break;
    case 8:
      *_p = (*_w) & ((1 << 28) - 1);
      _p++;
      break;
  }

  *pp = _p;
  return conum[_k];
}

#endif /* S9_CODING_H_ */
