/* bitarray.h
   Copyright (C) 2005, Rodrigo Gonzalez, all rights reserved.

   New RANK, SELECT, SELECT-NEXT and SPARSE RANK implementations.

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

#ifndef BitRankW32Int_h
#define BitRankW32Int_h

#include "basics.h"
/////////////
//Rank(B,i)// 
/////////////
//_factor = 0  => s=W*lgn
//_factor = P  => s=W*P
//Is interesting to notice
//factor=2 => overhead 50%
//factor=3 => overhead 33%
//factor=4 => overhead 25%
//factor=20=> overhead 5%


class BitRankW32Int {
private:
	
  uint factor,b,s;
  uint *Rs; //superblock array

	uint BuildRankSub(uint ini,uint fin); //uso interno para contruir el indice rank
	void BuildRank(); //crea indice para rank
public:
	uint *data;
    bool owner;
	
	uint n,integers;
  BitRankW32Int(uint *bitarray, uint n, bool owner, uint factor);
  ~BitRankW32Int(); //destructor
  bool IsBitSet(uint i);
  uint rank(uint i); //Nivel 1 bin, nivel 2 sec-pop y nivel 3 sec-bit

  uint prev(uint start); // gives the largest index i<=start such that IsBitSet(i)=true
  uint next(uint start); // gives the smallest index i>=start such that IsBitSet(i)=true
  uint select(uint x); // gives the position of the x:th 1.
  uint SpaceRequirementInBits();
  /*load-save functions*/
  int save(FILE *f);
  int load(FILE *f);
  BitRankW32Int(FILE *f, int *error); 
};

#endif

