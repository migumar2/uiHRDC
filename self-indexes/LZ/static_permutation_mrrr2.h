/* static_permutation_mrrr.h
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

#ifndef _STATIC_PERMUTATION_MRRR2_H
#define _STATIC_PERMUTATION_MRRR2_H

#include "basics.h"
#include "static_permutation.h"
#include "perm2.h"

namespace lz77index{
namespace basics{

/** Wrapper for Diego Arroyuelo's implementation of Munro et al.'s permutations.
 *  @author Francisco Claude
 */
class static_permutation_mrrr2 : public static_permutation {
  public:
    static_permutation_mrrr2(unsigned int * elems, unsigned int nelems, unsigned int t, static_bitsequence_builder * bmb);
    ~static_permutation_mrrr2();
    /** Computes the i-th element of the permutation */
    unsigned int pi(unsigned int i);
    /** Computes the inverse of i */
    unsigned int rev_pi(unsigned int i);
    /** Saves the permutation to fp, returns 0 in case of success */
    unsigned int save(FILE *fp);
    /** Returns the size of the permutation */
    unsigned int size();
    /** Loads a static_permutation from fp */
    static static_permutation_mrrr2 * load(FILE *fp);
  protected:
    perm2 permutation;
    static_permutation_mrrr2();
};

}
}
#endif
