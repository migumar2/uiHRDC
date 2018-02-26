/* static_permutation_mrrr.cpp
 * Copyright (C) 2008, Francisco Claude, all rights reserved.
 *
 * Permutation
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


#include "static_permutation_mrrr2.h"

namespace lz77index{
namespace basics{

static_permutation_mrrr2::static_permutation_mrrr2(unsigned int * elems, unsigned int nelems, unsigned int t, static_bitsequence_builder * bmb) {
  permutation = createPerm2(elems, nelems, t, bmb);
}

static_permutation_mrrr2::static_permutation_mrrr2() {
}

static_permutation_mrrr2::~static_permutation_mrrr2() {
  destroyPerm2(permutation);
}

unsigned int static_permutation_mrrr2::size() {
  return sizeof(static_permutation)+sizeofPerm2(permutation);
}

unsigned int static_permutation_mrrr2::pi(unsigned int i) {
  return getelemPerm2(permutation,i);
}

unsigned int static_permutation_mrrr2::rev_pi(unsigned int i) {
  return inversePerm2(permutation,i);
}

unsigned int static_permutation_mrrr2::save(FILE *fp) {
	unsigned int wr = STATIC_PERMUTATION_MRRR2_HDR;
	wr = fwrite(&wr,sizeof(unsigned int),1,fp);
	if(wr!=1) return 1;
    return savePerm2(permutation,fp);
}

static_permutation_mrrr2 * static_permutation_mrrr2::load(FILE *fp) {
  unsigned int rd;
  if(fread(&rd,sizeof(unsigned int),1,fp)!=1) return NULL;
  if(rd!=STATIC_PERMUTATION_MRRR2_HDR) return NULL;
  static_permutation_mrrr2 * ret = new static_permutation_mrrr2();
  ret->permutation = loadPerm2(fp);
  return ret;
}

}
}
