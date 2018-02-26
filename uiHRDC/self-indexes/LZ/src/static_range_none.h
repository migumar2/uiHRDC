/* static_range_none.h
 * Copyright (C) 2009, Sebastian Kreft C., all rights reserved.
 *
 * static_range_none definition
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

#ifndef _STATIC_RANGE_NONE_H
#define _STATIC_RANGE_NONE_H

#include "static_range.h"

namespace lz77index{

/** Range based on brute force.
 *  It uses n log n space and report occurences in time O(x2-x1)
 *
 *  @author Sebastian Kreft C.
 */
class static_range_none : public static_range {
    public:
        static_range_none(unsigned int* y, unsigned int len);
        ~static_range_none();
        /** Locates the occurrences of stored pairs in range [x1..x2]x[y1..y2]. 
            Returns an array in which A[2*i],A[2*i+1] is the ith matching pair */
        std::vector<unsigned int>* search(unsigned int x1, unsigned int x2, unsigned int y1, unsigned int y2);
        /** Returns the number of occurences of stored pairs in range [x1..x2]x[y1..y2]. */
        //virtual unsigned int count(unsigned int x1, unsigned int x2, unsigned int y1, unsigned int y2);
        /** Returns the size of the structure */
        unsigned int size();
        /** Saves the index to a file */
        unsigned int save(FILE *fp);
        /** Loads the index from a file */
        static static_range_none* load(FILE * fp);
    protected:
        static_range_none();
        unsigned int alen;
        unsigned int* carray;
        unsigned int calen;//the length of the compressed array
        unsigned char cabits;//number of bits
};

}

#endif /* _STATIC_RANGE_NONE_H */
