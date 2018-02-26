/* static_doublebitmap.h
 * Copyright (C) 2009, Sebastian Kreft C., all rights reserved.
 *
 * static_tree definition
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

#ifndef _STATIC_DOUBLEBITMAP_H
#define _STATIC_DOUBLEBITMAP_H

#include <cstdlib>
#include <cstdio>
#include "basics.h"
#include <iostream>

#define DBITMAP_S_HDR 2
#define DBITMAP_NONE_HDR 3

namespace lz77index{
namespace basics{

/** Base class for answering rank and select of two fixed bits (11) in a bitmap
 * 
 *  @author Sebastian Kreft
 */
class static_doublebitmap {
    public:
        static_doublebitmap();
        virtual ~static_doublebitmap();
        /*returns the number of 11 pairs found before pos*/
        virtual unsigned int rank(unsigned int pos)=0;
        /*return the pos in the bitmap where it holds rank(pos)=cant*/
        virtual unsigned int select(unsigned int cant)=0;
        /** Returns the size of the structure */
        virtual unsigned int size()=0;
        /** Saves the structure to a file */
        virtual unsigned int save(FILE *fp)=0;
        /** Loads the structure from a file */
        static static_doublebitmap* load(FILE * fp);
};
}
}
//#include <static_doublebitmap_none.h>
#include "static_doublebitmap_s.h"

#endif /* _STATIC_TREE_H */
