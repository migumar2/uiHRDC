/* static_doublebitmap.cpp
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

#include "static_doublebitmap.h"
namespace lz77index{
namespace basics{

static_doublebitmap::static_doublebitmap() {
}
static_doublebitmap::~static_doublebitmap(){
}
static_doublebitmap* static_doublebitmap::load(FILE *fp) {
    unsigned char rd;
    if(fread(&rd,sizeof(unsigned char),1,fp)!=1) return NULL;
    fseek(fp,-1*sizeof(unsigned char),SEEK_CUR);
    switch(rd) {
        case DBITMAP_S_HDR: return static_doublebitmap_s::load(fp);
        //case DBITMAP_NONE_HDR: return static_doublebitmap_none::load(fp);
    }
    return NULL;
}

}
}

