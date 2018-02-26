/* mapper.cpp
 * Copyright (C) 2008, Francisco Claude, all rights reserved.
 *
 * alphabet_mapper_cont definition
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
 
#include "mapper.h"
#include <cstdlib>

namespace lz77index{

mapper::mapper(){
    sigma_size = 0;
    a_map = NULL;
    a_unmap = NULL;
}
mapper::mapper(unsigned char* text, unsigned int len) { 
    sigma_size=0;
    a_map = new unsigned char[256];
    for(unsigned int i=0;i<256;i++){
        a_map[i]=0;
    }
    for(unsigned int i=0;i<len;i++){
        a_map[text[i]]=1;
    }
    for(unsigned int i=0;i<256;i++){
        if(a_map[i]==1){
            sigma_size++;
        }
    }
    a_unmap = new unsigned char[sigma_size];
    unsigned int t=0;
    for(unsigned int i=0;i<256;i++){
        if(a_map[i]==1){
            a_map[i]=t;
            a_unmap[t]=i;
            t++;
        }else{
            a_map[i]=255;    
        }
    }
}
mapper::~mapper(){
    if(a_map!=NULL)delete [] a_map;
    if(a_unmap!=NULL)delete [] a_unmap;
}
unsigned int mapper::map(unsigned char* s,unsigned int len){
    if(s!=NULL){
        for(unsigned int i=0;i<len;i++){
            s[i] = a_map[s[i]];
            if(s[i]>=sigma_size)return 1;//the string has letters not present in the text
        }
    }
    return 0;
}
unsigned int mapper::unmap(unsigned char* s,unsigned int len){
    if(s!=NULL){
        for(unsigned int i=0;i<len;i++){
            if(s[i]>=sigma_size)return 1;//the string is drawn from a bigger alphabet, error.
            s[i]=a_unmap[s[i]];
        }
    }
    return 0;
}
unsigned char mapper::mapchar(unsigned char s){
    return a_map[s];
}
unsigned char mapper::unmapchar(unsigned char s){
    if(s<sigma_size)
        return a_unmap[s];
    return 255;
}
unsigned int mapper::size(){
    return sizeof(mapper)+256*sizeof(unsigned char)+sigma_size*sizeof(unsigned char);
}
unsigned int mapper::sigma(){
    return sigma_size;
}
unsigned int mapper::save(FILE *fp){
    unsigned int wr;
    wr = fwrite(&(sigma_size),sizeof(unsigned int),1,fp);
    if(wr!=1) return 1;
    wr = fwrite(a_map,sizeof(unsigned char),256,fp);
    if(wr!=256) return 1;
    wr = fwrite(a_unmap,sizeof(unsigned char),sigma_size,fp);
    if(wr!=sigma_size) return 1;
    return 0;
}
mapper* mapper::load(FILE *fp){
    unsigned int val;
    mapper* ret = new mapper();
    if(fread(&val,sizeof(unsigned int),1,fp)!=1){
        delete ret;
        return NULL;
    }
    ret->sigma_size = val;
    ret->a_map = new unsigned char[256];
    ret->a_unmap = new unsigned char[ret->sigma_size];
    if(fread(ret->a_map,sizeof(unsigned char),256,fp)!=256){
        delete ret;
        return NULL;
    }
    if(fread(ret->a_unmap,sizeof(unsigned char),ret->sigma_size,fp)!=ret->sigma_size){
        delete ret;
        return NULL;
    }
    return ret;
}
}

