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

#include "static_range_chazelle.h"
#include <cstring>
#include <cstdio>
namespace lz77index{

static_range_chazelle::static_range_chazelle() {
    R = NULL;
}
static_range_chazelle::~static_range_chazelle() {
    basics::destroyRange(R);
}
static_range_chazelle::static_range_chazelle(unsigned int* y,unsigned int len){
    R = basics::createRange(y,len);
}
static void processOcc(void* data,unsigned int y){
    std::vector<unsigned int>* res = (std::vector<unsigned int>*)data;
    res->push_back(y);
    //unsigned int* count = (unsigned int*)data;
    //*count = *count + 1;    
}
std::vector<unsigned int>* static_range_chazelle::search(unsigned int x1, unsigned int x2, unsigned int y1, unsigned int y2) {
    //*count = 0;
    std::vector<unsigned int>* res = new std::vector<unsigned int>();
    basics::searchRange(R,x1,x2,y1,y2,processOcc,res);
    //*count = res->size(); 
    return res;
}

unsigned int static_range_chazelle::size(){
    return sizeof(static_range_chazelle)+basics::sizeOfRange(R);//sizeof R
}
unsigned int static_range_chazelle::save(FILE *fp){
    unsigned char wr = RANGE_CHAZ_HDR;
    wr = fwrite(&wr,sizeof(unsigned char),1,fp);
    if(wr!=1) return 1;
    if(basics::saveRange(R,fp)!=0)return 1;
    return 0;
}
static_range_chazelle* static_range_chazelle::load(FILE *fp) {
    unsigned char rd;
    if(fread(&rd,sizeof(unsigned char),1,fp)!=1) return NULL;
    if(rd!=RANGE_CHAZ_HDR) return NULL;
    static_range_chazelle* ret = new static_range_chazelle();
    ret->R = basics::loadRange(fp);
    if(ret->R==NULL){
        std::cerr<<"The returned range is NULL"<<std::endl;
        delete ret;        
        return NULL;
    }
    return ret;
}

}


