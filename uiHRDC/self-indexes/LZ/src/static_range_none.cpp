/* static_range_none.cpp
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

#include "static_range_none.h"
#include "basics.h"
#include <cstring>
#include <cstdio>

namespace lz77index{

static_range_none::static_range_none() {
    alen = 0;
    calen = 0;
    cabits = 0;
    carray = NULL;
}
static_range_none::~static_range_none() {
	//delete [] text;
    if(carray!=NULL)
        delete [] carray;
}
static_range_none::static_range_none(unsigned int* y,unsigned int len){
    alen = len;
    unsigned int maxval=0;
    for(unsigned int i=0;i<len;i++)
        if(y[i]>maxval)maxval=y[i];
    cabits = basics::bits(maxval);
    calen = basics::uint_len(len,cabits);
    carray = new unsigned int[calen];
    for(unsigned int i=0;i<calen;i++)carray[i]=0;    
    for(unsigned int i=0;i<alen;i++)
        basics::set_field(carray,cabits,i,y[i]);
    free(y);
}
std::vector<unsigned int>* static_range_none::search(unsigned int x1, unsigned int x2, unsigned int y1, unsigned int y2) {
    std::vector<unsigned int>* res = new std::vector<unsigned int>();
    //*count = 0;
    if(x2>=alen)x2=alen-1;
    if(y2>=alen)y2=alen-1;
    if(y2<y1)x1=x2+1;
    for(unsigned int x=x1;x<=x2;x++){
        unsigned int val = basics::get_field(carray,cabits,x);
        if(val>=y1 && val<=y2){
            //*count = *count + 1;
            res->push_back(val);//could be also x or both
        }
    }
    return res;
}

unsigned int static_range_none::size(){
    return sizeof(static_range_none)+sizeof(unsigned int)*(calen);
}
unsigned int static_range_none::save(FILE *fp){
    unsigned char wr = RANGE_NONE_HDR;
    wr = fwrite(&wr,sizeof(unsigned char),1,fp);
    if(wr!=1) return 1;
    if(fwrite(&alen,sizeof(unsigned int),1,fp)!=1)return 1;
    if(fwrite(&calen,sizeof(unsigned int),1,fp)!=1)return 1;
    if(fwrite(&cabits,sizeof(unsigned char),1,fp)!=1)return 1;
    if(fwrite(carray,sizeof(unsigned int),calen,fp)!=calen)return 1;
    return 0;
}
static_range_none* static_range_none::load(FILE *fp) {
    unsigned char rd;
    if(fread(&rd,sizeof(unsigned char),1,fp)!=1) return NULL;
    if(rd!=RANGE_NONE_HDR) return NULL;
    static_range_none* ret = new static_range_none();
    if(fread(&(ret->alen),sizeof(unsigned int),1,fp)!=1){
        delete ret;        
        return NULL;
    }
    if(fread(&(ret->calen),sizeof(unsigned int),1,fp)!=1){
        delete ret;        
        return NULL;
    }
    if(fread(&(ret->cabits),sizeof(unsigned char),1,fp)!=1){
        delete ret;        
        return NULL;
    }
    ret->carray = new unsigned int[ret->calen];
    if(fread(ret->carray,sizeof(unsigned int),ret->calen,fp)!=ret->calen){
        delete ret;        
        return NULL;
    }
    return ret;
}

}


