/**
 *    offsets.h
 *    Copyright (C) 2011  Francisco Claude F.
 *
 *    This program is free software: you can redistribute it and/or modify
 *    it under the terms of the GNU General Public License as published by
 *    the Free Software Foundation, either version 3 of the License, or
 *    (at your option) any later version.
 *
 *    This program is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU General Public License for more details.
 *
 *    You should have received a copy of the GNU General Public License
 *    along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <algorithm>
using namespace std;

#define MYBS 0

inline uint exp_search(uint *offsets, uint ini, uint fin, uint val) {
  if(offsets[ini+1] > val) return ini;
  uint add = 1;
  uint inipadd = ini + add;
  while(inipadd<=fin) {
    if(offsets[inipadd] > val) {
#if MYBS > 1
      uint bs_ini = ini;
      uint bs_fin = inipadd;
      uint bs_med = (bs_ini+bs_fin)/2;
      while(bs_ini <= bs_fin) {
	if(offsets[bs_med] >= val) {
	  bs_fin = bs_med;
	} else {
	  if(bs_ini == bs_med) {
	    bs_fin = bs_ini-1;
	    break;
	  }
	  bs_ini = bs_med;
	}
	bs_med = (bs_ini+bs_fin)/2;
      }
      bs_fin++;
      return bs_fin;
#else
      uint * pos = upper_bound(offsets+ini,offsets+inipadd+1,val);
      pos--;
      return pos-offsets;
#endif
    }
    add *= 2;
    inipadd = ini + add;
  }
#if MYBS > 0
  uint bs_ini = ini;
  uint bs_fin = fin;
  uint bs_med = (bs_ini+bs_fin)/2;
  while(bs_ini <= bs_fin) {
    if(offsets[bs_med] >= val) {
      bs_fin = bs_med;
    } else {
      if(bs_ini == bs_med) {
	bs_fin = bs_ini-1;
	break;
      }
      bs_ini = bs_med;
    }
    bs_med = (bs_ini+bs_fin)/2;
      }
  bs_fin++;
  return bs_fin;
#else
  uint * pos = upper_bound(offsets+ini,offsets+fin+1,val);
  pos--;
  return pos-offsets;
#endif
}

inline uint * doc_offset_exp(uint *offsets, uint len_offs, uint * positions, uint len_posn) {
  uint * result = new uint[len_posn*2+1];
  result[0] = len_posn*2;
  sort(positions,positions+len_posn);
  uint ini = 0;
  for(uint i=1;i<=len_posn;i++) {
    uint pos = exp_search(offsets,ini,len_offs-1,positions[i-1]);
    result[2*i-1] = pos+1;
    result[2*i] = positions[i-1]-offsets[pos];
    ini = pos;
  }
  return result;
}

inline uint * doc_offset_exp(uint *offsets, uint * positions) {
  return doc_offset_exp(offsets+1,offsets[0],positions+1,positions[0]);
}
