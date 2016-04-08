/* static_doublebitmap_s.h
 * Copyright (C) 2009, Sebastian Kreft C., all rights reserved.
 *
 * static_doublebitmap_s definition
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

#ifndef _STATIC_DOUBLEBITMAP_S_H
#define _STATIC_DOUBLEBITMAP_S_H

#include "static_doublebitmap.h"

namespace lz77index{
namespace basics{
/** It uses the same idea of the normal bitmap, blocks, and superblocks
 *
 *  @author Sebastian Kreft C.
 */
class static_doublebitmap_s : public static_doublebitmap {
    public:
        static_doublebitmap_s(unsigned int* data, unsigned int n);
        ~static_doublebitmap_s();
        /*returns the number of 11 pairs found before pos*/
        unsigned int rank(unsigned int pos);
        /*return the pos in the bitmap where it holds rank(pos)=cant*/
        unsigned int select(unsigned int cant);
        /** Returns the size of the structure */
        unsigned int size();
        /** Saves the structure to a file */
        unsigned int save(FILE *fp);
        /** Loads the structure from a file */
        static static_doublebitmap_s* load(FILE * fp);
        unsigned int* bitmap_data;//dfuds should be friend
    protected:
        unsigned int  n;        // # of bits
        unsigned int  *sdata;   // superblock counters
        unsigned char *bdata;   // block counters
        static_doublebitmap_s();
};
}
}
#endif /* _STATIC_RANGE_CHAZELLE_H */
