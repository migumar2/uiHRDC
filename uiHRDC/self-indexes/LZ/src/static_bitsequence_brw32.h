/* static_bitsequence_brw32.h
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

#ifndef _STATIC_BITSEQUENCE_BRW32_H
#define _STATIC_BITSEQUENCE_BRW32_H

#include <cstdlib>
#include <cstdio>
#include "basics.h"
#include "static_bitsequence.h"

namespace lz77index{
namespace basics{
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

/** Implementation of Rodrigo Gonzalez et al. practical rank/select solution [1]. 
 *  The interface was adapted.
 *  
 *  [1] Rodrigo Gonzalez, Szymon Grabowski, Veli Makinen, and Gonzalo Navarro.
 *      Practical Implementation of Rank and Select Queries. WEA05.
 *
 *  @author Rodrigo Gonzalez
 */
class static_bitsequence_brw32 : public static_bitsequence {
private:
  //bool owner;
    unsigned int n,integers;
    unsigned int factor,b,s;
    unsigned int *Rs; //superblock array

    unsigned int BuildRankSub(unsigned int ini,unsigned int fin); //uso interno para contruir el indice rank
	void BuildRank(); //crea indice para rank
    static_bitsequence_brw32();
  
public:
  unsigned int *data;
  static_bitsequence_brw32(unsigned int *bitarray, unsigned int n, unsigned int factor);
  ~static_bitsequence_brw32(); //destructor
  bool access(unsigned int i);
  unsigned int rank1(unsigned int i); //Nivel 1 bin, nivel 2 sec-pop y nivel 3 sec-bit

  unsigned int prev(unsigned int start); // gives the largest index i<=start such that IsBitSet(i)=true
  unsigned int prev2(unsigned int start); // gives the largest index i<=start such that IsBitSet(i)=true
  unsigned int next(unsigned int start); // gives the smallest index i>=start such that IsBitSet(i)=true
  unsigned int select0(unsigned int x); // gives the position of the x:th 1.
  unsigned int select1(unsigned int x); // gives the position of the x:th 1.
  unsigned int SpaceRequirementInBits();
  unsigned int SpaceRequirement();
  unsigned int size();
  
  /*load-save functions*/
  int save(FILE *f);
  static static_bitsequence_brw32 * load(FILE * fp);
};

}
}
#endif
