/* static_selfindex.cpp
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

#include "static_range.h"
#include <cstring>

namespace lz77index{

static_range::static_range() {
}
static_range::~static_range(){
}
unsigned int static_range::count(unsigned int x1, unsigned int x2, unsigned int y1, unsigned int y2){
    unsigned int count;
    std::vector<unsigned int>* res = search(x1,x2,y1,y2);
    count = res->size();
    delete res;
    return count;
}
static_range* static_range::load(FILE *fp) {
    unsigned char rd;
    if(fread(&rd,sizeof(unsigned char),1,fp)!=1) return NULL;
    fseek(fp,-1*sizeof(unsigned char),SEEK_CUR);
    switch(rd) {
        case RANGE_CHAZ_HDR: return static_range_chazelle::load(fp);
        case RANGE_NONE_HDR: return static_range_none::load(fp);
    }
    return NULL;
}

}

