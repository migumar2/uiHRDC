/* static_selfindex_none.cpp
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

#include "static_selfindex_none.h"
#include <cstring>
#include <cstdio>
#include "basics.h"
#include "utils.h"

namespace lz77index{

static_selfindex_none::static_selfindex_none() {
    tlen = 0;
    ctlen = 0;
    ctbits = 0;
    sigma_mapper = NULL;
    ctext = NULL;
}
static_selfindex_none::~static_selfindex_none() {
	//delete [] text;
    if(ctext!=NULL)
        delete [] ctext;
}
static_selfindex_none* static_selfindex_none::build(char* filename){
    unsigned int len;
    unsigned char* text = utils::readText(filename,&len);
    static_selfindex_none* ret = new static_selfindex_none(text,len);
    FILE* fd = fopen((std::string(filename)+".noneindex").c_str(),"w");
    ret->save(fd);
    fclose(fd);
    return ret;
}
static_selfindex_none::static_selfindex_none(unsigned char* text, unsigned int len) {
    this->tlen = len;
    this->sigma_mapper = new mapper(text,tlen);
    this->ctbits = lz77index::basics::bits(this->sigma_mapper->sigma()-1);
    this->ctlen = lz77index::basics::uint_len(this->tlen+1,this->ctbits);
    this->ctext = new unsigned int[this->ctlen];
    for(unsigned int i=0;i<ctlen;i++)ctext[i]=0;    
    //this->text = new unsigned char[tlen+1];
    for(unsigned int i=0;i<tlen;i++)
        lz77index::basics::set_field(this->ctext,this->ctbits,i,this->sigma_mapper->mapchar(text[i]));
        //(this->text)[i] = text[i];
}
std::vector<unsigned int>* static_selfindex_none::_locate(unsigned char* pattern, unsigned int plen,unsigned int* count,unsigned int max_occs) {
    std::vector<unsigned int>* occ_pos = new std::vector<unsigned int>();
    unsigned int next[256];
    for(int i=0; i<256; i++){
        next[i] = plen;    
    }
    for(unsigned int i=0; i<plen-1; i++){
        next[pattern[i]] = plen - 1 - i;
    }
    *count=0;
    unsigned int k = plen-1;
    unsigned int j = plen-1;
    while(k < tlen) {
        //if(text[k - (plen-1-j)] == pattern[j]){
        if(lz77index::basics::get_field(ctext,ctbits,k - (plen-1-j)) == pattern[j]){
            if(j==0){
                //report occurrence in k-plen+1
                //std::cout<<k-plen+1<<" ";
                occ_pos->push_back(k-plen+1);
                //*count = *count+1;
                k++;
                j = plen-1;
            }else{
                j--;
            }
        }else{
            //k = k + next[text[k]];
            k = k + next[lz77index::basics::get_field(ctext,ctbits,k)];
            j = plen-1;
        }
    }
    *count = occ_pos->size();
    return occ_pos;
}

unsigned char* static_selfindex_none::_display(unsigned int start, unsigned int end){
    if(end>=tlen || end<start)return NULL;
    unsigned char* sstr = new unsigned char[end-start+2];
    for(unsigned int i=start;i<=end;i++)
        sstr[i-start]=lz77index::basics::get_field(ctext,ctbits,i);
        //sstr[i-start]=text[i];
    sstr[end-start+1]='\0';
    return sstr;
}
unsigned int static_selfindex_none::size(){
    return sizeof(static_selfindex_none)+sizeof(unsigned int)*(ctlen)+sigma_mapper->size();
}
unsigned int static_selfindex_none::save(FILE *fp){
    unsigned char wr = SELFINDEX_NONE_HDR;
    wr = fwrite(&wr,sizeof(unsigned char),1,fp);
    if(wr!=1) return 1;
    if(fwrite(&tlen,sizeof(unsigned int),1,fp)!=1)return 1;
    if(fwrite(&ctlen,sizeof(unsigned int),1,fp)!=1)return 1;
    if(fwrite(&ctbits,sizeof(unsigned char),1,fp)!=1)return 1;
    if(fwrite(ctext,sizeof(unsigned int),ctlen,fp)!=ctlen)return 1;
    if(sigma_mapper->save(fp))return 1;
    return 0;
}
static_selfindex_none* static_selfindex_none::load(FILE *fp) {
    unsigned char rd;
    if(fread(&rd,sizeof(unsigned char),1,fp)!=1) return NULL;
    if(rd!=SELFINDEX_NONE_HDR) return NULL;
    static_selfindex_none* ret = new static_selfindex_none();
    if(fread(&(ret->tlen),sizeof(unsigned int),1,fp)!=1){
        delete ret;        
        return NULL;
    }
    if(fread(&(ret->ctlen),sizeof(unsigned int),1,fp)!=1){
        delete ret;        
        return NULL;
    }
    if(fread(&(ret->ctbits),sizeof(unsigned char),1,fp)!=1){
        delete ret;        
        return NULL;
    }
    ret->ctext = new unsigned int[ret->ctlen];
    if(fread(ret->ctext,sizeof(unsigned int),ret->ctlen,fp)!=ret->ctlen){
        delete ret;        
        return NULL;
    }
    ret->sigma_mapper=mapper::load(fp);
    if(ret->sigma_mapper==NULL){
        delete ret;        
        return NULL;
    }
    return ret;
}

}

