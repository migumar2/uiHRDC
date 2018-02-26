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

#include "static_selfindex.h"
#include <cstring>

namespace lz77index{

static_selfindex::static_selfindex() {
	tlen = 0;
    sigma_mapper = NULL;
}
static_selfindex::~static_selfindex(){
    if(sigma_mapper!=NULL)delete sigma_mapper;
}
unsigned int static_selfindex::length() {
    return tlen;
}
std::vector<unsigned int>* static_selfindex::locate(unsigned char* pattern, unsigned int plen, unsigned int* count, unsigned int max_occs){
    //unsigned int plen = strlen((char*)pattern);
    if(sigma_mapper->map(pattern,plen+1)!=0){//the pattern has letters which are not present in the text
        *count=0;
        return NULL;
    }
    return _locate(pattern,plen,count,max_occs);
}
unsigned int static_selfindex::count(unsigned char* pattern, unsigned int plen){
    //unsigned int plen = strlen((char*)pattern);
    if(sigma_mapper->map(pattern,plen+1)!=0){//the pattern has letters which are not present in the text
        return 0;
    }
    return _count(pattern,plen);
    
}
unsigned char* static_selfindex::display(unsigned int start, unsigned int end){
    if(end >= this->tlen-1){//we don't want to extract the trailing '\0'
        end = this->tlen-2;
    }
    if(start>end)return NULL;
    unsigned char* sstr = _display(start,end);
    if(sstr!=NULL){
        //std::cout<<sstr<<std::endl;
        if(sigma_mapper->unmap(sstr,end-start+1)!=0){
            delete [] sstr;
            return NULL;
        }
    }
    return sstr;
}
bool static_selfindex::exist(unsigned char* pattern, unsigned int plen){
    //unsigned int plen = strlen((char*)pattern);
    if(sigma_mapper->map(pattern,plen+1)!=0){//the pattern has letters which are not present in the text
        return 0;
    }
    return _exist(pattern,plen);
    
}
unsigned int static_selfindex::_count(unsigned char* pattern, unsigned int plen) {
    unsigned int count;
    std::vector<unsigned int>* locate = this->_locate(pattern,plen,&count,0);
    delete locate;
    return count;
}
bool static_selfindex::_exist(unsigned char* pattern, unsigned int plen) {
    unsigned int count = _count(pattern,plen);
    return count>0;
}
static_selfindex* static_selfindex::load(FILE *fp) {
    unsigned char rd;
    if(fread(&rd,sizeof(unsigned char),1,fp)!=1) return NULL;
    fseek(fp,-1*sizeof(unsigned char),SEEK_CUR);
    switch(rd) {
        case SELFINDEX_LZ77_HDR: return static_selfindex_lz77::load(fp);
        case SELFINDEX_NONE_HDR: return static_selfindex_none::load(fp);
        case SELFINDEX_LZEND_HDR: return static_selfindex_lzend::load(fp);
    }
    return NULL;
}
static_selfindex* static_selfindex::load(const char* filename) {
    FILE* fd = fopen(filename,"r");
    static_selfindex* ret = load(fd);
    fclose(fd);
    return ret;
}

}

