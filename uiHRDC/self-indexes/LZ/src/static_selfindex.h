/* static_selfindex.h
 * Copyright (C) 2009, Sebastian Kreft C., all rights reserved.
 *
 * static_selfindex definition
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

#ifndef __STATIC_SELFINDEX_H__
#define __STATIC_SELFINDEX_H__

#include <cstdio>
#include <iostream>
#include <vector>
#include "mapper.h"

#define SELFINDEX_LZ77_HDR 2
#define SELFINDEX_NONE_HDR 3
#define SELFINDEX_LZEND_HDR 4

namespace lz77index{
/** Base class for selindexes
 * 
 *  @author Sebastian Kreft
 */
class static_selfindex {
    public:
        static_selfindex();
        virtual ~static_selfindex();
        /** Locates the occurrences of pattern in the text. Returns a new allocated array and stores the number of elementes in count. The returned array could be arger than count.*/
        //std::vector<unsigned int>* locate(unsigned char* pattern, unsigned int length, unsigned int* count);
        std::vector<unsigned int>* locate(unsigned char* pattern, unsigned int length, unsigned int* count, unsigned int max_occs=0);//O MEANS NO MAX
        /** Returns the number of occurences of pattern in the text. */
        unsigned int count(unsigned char* pattern, unsigned int length);
        /** Returns true if the pattern exists*/
        bool exist(unsigned char* pattern, unsigned int length);
        /** Returns the substring of the text in a new allocated array*/
        unsigned char* display(unsigned int start, unsigned int end);
        /** Returns the length of the text*/
        unsigned int length();
        /** Returns the size of the index */
        virtual unsigned int size()=0;
        /** Saves the index to a file */
        virtual unsigned int save(FILE *fp)=0;
        /** Loads the index from a file */
        static static_selfindex* load(FILE * fp);
        static static_selfindex* load(const char* filename);
    protected:
        /** Locates the occurrences of pattern in the text. Returns a new allocated array and stores the number of elementes in count. The returned array could be arger than count.*/
        virtual std::vector<unsigned int>* _locate(unsigned char* pattern,unsigned int plen,unsigned int* count,unsigned int max_occs)=0;
        /** Returns the number of occurences of pattern in the text. */
        virtual unsigned int _count(unsigned char* pattern,unsigned int plen);
        /** Returns true if the pattern exists */
        virtual bool _exist(unsigned char* pattern,unsigned int plen);
        /** Returns the substring of the text in a new allocated array*/
        virtual unsigned char* _display(unsigned int start, unsigned int end)=0;
        unsigned int tlen;
        mapper* sigma_mapper;        
};

}

#include "static_selfindex_none.h"
#include "static_selfindex_lz77.h"
#include "static_selfindex_lzend.h"

#endif /* _STATIC_SELFINDEX_H */
