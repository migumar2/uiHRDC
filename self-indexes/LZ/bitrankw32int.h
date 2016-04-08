#ifndef BitRankW32Int_h
#define BitRankW32Int_h
#include "basics.h"
#include <cstdio>

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

namespace lz77index{
namespace basics{

typedef struct sbitRankW32Int{
    unsigned int *data;
    char owner;
    unsigned int integers;
    unsigned int factor,b,s;
    unsigned int *Rs;  					//superblock array
    unsigned int n;                  
} bitRankW32Int;
                                 //uso interno para contruir el indice rank
    unsigned int buildRankSub(bitRankW32Int * br,unsigned int ini,unsigned int fin);
    void buildRank(bitRankW32Int * br);            //crea indice para rank

    bitRankW32Int * createBitRankW32Int(unsigned int *bitarray, unsigned int n, char owner, unsigned int factor);
    void destroyBitRankW32Int(bitRankW32Int * br);            //destructor
    unsigned int isBitSet(bitRankW32Int * br, unsigned int i);
    unsigned int rank(bitRankW32Int * br, unsigned int i);           //Nivel 1 bin, nivel 2 sec-pop y nivel 3 sec-bit
    unsigned int lenght_in_bits(bitRankW32Int * br);
    unsigned int prev(bitRankW32Int * br, unsigned int start);       // gives the largest index i<=start such that IsBitSet(i)=true
    unsigned int bselect(bitRankW32Int * br, unsigned int x);         // gives the position of the x:th 1.
    unsigned int select0(bitRankW32Int * br, unsigned int x);        // gives the position of the x:th 0.
    unsigned int select1(bitRankW32Int * br, unsigned int x);        // gives the position of the x:th 1.
    unsigned int spaceRequirementInBits(bitRankW32Int * br);
    /*load-save functions*/
    int save(bitRankW32Int * br, FILE *f);
    int load(bitRankW32Int * br, FILE *f);
    bitRankW32Int * createBitRankW32IntFile(FILE *f, int *error);

}
}
#endif
