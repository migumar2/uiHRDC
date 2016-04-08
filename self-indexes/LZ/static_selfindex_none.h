/* static_selfindex_none.h
 * Copyright (C) 2009, Sebastian Kreft C., all rights reserved.
 *
 * static_selfindex_none definition
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

#ifndef __STATIC_SELFINDEX_NONE_H__
#define __STATIC_SELFINDEX_NONE_H__

#include "static_selfindex.h"

namespace lz77index{
/** "Selfindex" that uses BMH to find occurrences.
 *  It's not really a selfindex, it is used just for testing.
 *
 *  @author Sebastian Kreft C.
 */
class static_selfindex_none : public static_selfindex {
    public:
        ~static_selfindex_none();
        unsigned int size();
        /** Saves the index to a file */
        unsigned int save(FILE *fp);
        /** Loads the index from a file */
        static static_selfindex_none* load(FILE * fp);
        static static_selfindex_none* build(char* filename);
    protected:
        static_selfindex_none(unsigned char* text,unsigned int len);
        /** Locates the occurrences of pattern in the text. Returns a new allocated array and stores the number of elementes in count. The returned array could be arger than count.*/
        std::vector<unsigned int>* _locate(unsigned char* pattern,unsigned int plen,unsigned int* count,unsigned int max_occs);
        /** Return the number of occurences of pattern in the text. */
        //virtual unsigned int count(unsigned char* pattern);
        /** Returns the substring of the text in a new allocated array*/
        unsigned char* _display(unsigned int start, unsigned int end);
        /** Returns the size of the index */
        static_selfindex_none();
        //unsigned char* text;
        unsigned int* ctext;
        unsigned int ctlen;//the length of the compressed text
        unsigned char ctbits;//number of bits
};

}
#endif /* _STATIC_SELFINDEX_NONE_H */
