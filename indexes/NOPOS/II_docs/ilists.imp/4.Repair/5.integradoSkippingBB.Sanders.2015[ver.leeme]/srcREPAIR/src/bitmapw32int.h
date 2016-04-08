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

#ifndef BitMapW32Int_h
#define BitMapW32Int_h

#include "basic.h"


class BitMapW32Int {
private:
	uint *data;
    bool owner;	
public:
	uint n,integers;
  BitMapW32Int( uint *bitarray, uint _n, bool owner);
  BitMapW32Int(FILE *f, int *error); 

  ~BitMapW32Int(); //destructor
  bool IsBitSet(uint i);

  uint SpaceRequirementInBits();
  int save(FILE *f);
  int load(FILE *f);
};

#endif

