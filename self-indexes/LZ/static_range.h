/* static_range.h
 * Copyright (C) 2009, Sebastian Kreft C., all rights reserved.
 *
 * static_range definition
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

#ifndef _STATIC_RANGE_H
#define _STATIC_RANGE_H

#include <cstdlib>
#include <cstdio>
#include <iostream>
#include <vector>

#define RANGE_CHAZ_HDR 2
#define RANGE_NONE_HDR 3

namespace lz77index{

/** Base class for range structures
 * 
 *  @author Sebastian Kreft
 */
class static_range {
    public:
        static_range();
        virtual ~static_range();
        /** Locates the occurrences of stored pairs in range [x1..x2]x[y1..y2]. 
            Returns a vector containing all y coordinates of matching pairs*/
        virtual std::vector<unsigned int>* search(unsigned int x1, unsigned int x2, unsigned int y1, unsigned int y2)=0;
        /** Returns the number of occurences of stored pairs in range [x1..x2]x[y1..y2]. */
        virtual unsigned int count(unsigned int x1, unsigned int x2, unsigned int y1, unsigned int y2);
        /** Returns the size of the structure */
        virtual unsigned int size()=0;
        /** Saves the structure to a file */
        virtual unsigned int save(FILE *fp)=0;
        /** Loads the structure from a file */
        static static_range* load(FILE * fp);
};

}
#include "static_range_none.h"
#include "static_range_chazelle.h"

#endif /* _STATIC_RANGE_H */
