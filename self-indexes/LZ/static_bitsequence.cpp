/* static_bitsequence.cpp
 * Copyright (C) 2008, Francisco Claude, all rights reserved.
 *
 * static_bitsequence definition
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 */

#include "static_bitsequence.h"

namespace lz77index{
namespace basics{

unsigned int static_bitsequence::rank0(unsigned int i) {
	return i+1-rank1(i);
}

unsigned int static_bitsequence::rank1(unsigned int i) {
  if(i>=len) return ones;
  if(ones==0) return -1;
  unsigned int ini = 1;
  unsigned int fin = ones;
  while(ini<fin) {
    unsigned int pos = (ini+fin)/2;
    unsigned int bp = select1(pos);
    if(bp==i) return pos;
    if(bp<i)
      ini = pos+1;
    else
      fin = pos-1;
  }
	if(select1(ini)>i) return ini-1;
	return ini;
}

unsigned int static_bitsequence::select0(unsigned int i) {
  if(i>len-ones) return len;
  if(i==0) return -1;
  unsigned int ini = 0;
  unsigned int fin = len-1;
  while(ini<fin) {
    unsigned int pos = (ini+fin)/2;
    unsigned int br = rank0(pos);
    if(br<i)
      ini = pos+1;
    else
      fin = pos;
  }
	return ini;
}

unsigned int static_bitsequence::select1(unsigned int i) {
  if(i>ones) return len;
  if(i==0) return 0;
  unsigned int ini = 0;
  unsigned int fin = len-1;
  while(ini<fin) {
    unsigned int pos = (ini+fin)/2;
    unsigned int br = rank1(pos);
    if(br<i)
      ini = pos+1;
    else
      fin = pos;
  }
	return ini;
}

bool static_bitsequence::access(unsigned int i) {
  return (rank1(i)-(i!=0?rank1(i-1):0))>0;
}

unsigned int static_bitsequence::length() {
	return len;
}

unsigned int static_bitsequence::count_one() {
	return ones;
}

unsigned int static_bitsequence::count_zero() {
	return len-ones;
}

static_bitsequence * static_bitsequence::load(FILE * fp) {
  unsigned int r;
  if(fread(&r,sizeof(unsigned int),1,fp)!=1) return NULL;
  fseek(fp,-1*sizeof(unsigned int),SEEK_CUR);
  switch(r) {
    //case RRR02_HDR: return static_bitsequence_rrr02::load(fp);
    case BRW32_HDR: return static_bitsequence_brw32::load(fp);
    //case RRR02_LIGHT_HDR: return static_bitsequence_rrr02_light::load(fp);
  }
  return NULL;
}

}
}

