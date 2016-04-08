/* static_doublebitmap_s.cpp
 * Copyright (C) 2009, Sebastian Kreft C., all rights reserved.
 *
 * static_doublebitmap_s definition
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

#include "static_doublebitmap_s.h"
#include <cstdio>

namespace lz77index{
namespace basics{

const unsigned char dpopc[] = {
    0,0,0,1,0,0,1,2,0,0,0,1,1,1,2,3,0,0,0,1,0,0,1,2,1,1,1,2,2,2,3,4,
    0,0,0,1,0,0,1,2,0,0,0,1,1,1,2,3,1,1,1,2,1,1,2,3,2,2,2,3,3,3,4,5,
    0,0,0,1,0,0,1,2,0,0,0,1,1,1,2,3,0,0,0,1,0,0,1,2,1,1,1,2,2,2,3,4,
    1,1,1,2,1,1,2,3,1,1,1,2,2,2,3,4,2,2,2,3,2,2,3,4,3,3,3,4,4,4,5,6,
    0,0,0,1,0,0,1,2,0,0,0,1,1,1,2,3,0,0,0,1,0,0,1,2,1,1,1,2,2,2,3,4,
    0,0,0,1,0,0,1,2,0,0,0,1,1,1,2,3,1,1,1,2,1,1,2,3,2,2,2,3,3,3,4,5,
    1,1,1,2,1,1,2,3,1,1,1,2,2,2,3,4,1,1,1,2,1,1,2,3,2,2,2,3,3,3,4,5,
    2,2,2,3,2,2,3,4,2,2,2,3,3,3,4,5,3,3,3,4,3,3,4,5,4,4,4,5,5,5,6,7
};

unsigned int dpopcount (register unsigned int x){
    int t=0;
    if(((x&(0x03<<7))>>7)==0x03)t++;
    if(((x&(0x03<<15))>>15)==0x03)t++;
    if(((x&(0x03<<23))>>23)==0x03)t++;
    return dpopc[x&0xFF] + dpopc[(x>>8)&0xFF] + dpopc[(x>>16)&0xFF] + dpopc[x>>24] + t;
}
unsigned int dpopcount8 (register unsigned int x){
    return dpopc[x & 0xff];
}
static_doublebitmap_s::static_doublebitmap_s(unsigned int* data, unsigned int n) {
    unsigned int ns, nb, words;
    unsigned int i,j,pop,bpop,pos;
    this->n = n;
    this->bitmap_data = data;
    words = (n+W-1)/W;
    ns = (n+256-1)/256;
    nb = 256/W; // adjustments
    this->bdata = new unsigned char[ns*nb];
    this->sdata = new unsigned int[ns];
    for(unsigned int i=0;i<ns*nb;i++){
        this->bdata[i] = 0;
    }
    pop = 0; pos = 0;
    for (i=0;i<ns;i++) { 
       bpop = 0;
       this->sdata[i] = pop;
       for (j=0;j<nb;j++) { 
          if (pos == words) break;
          this->bdata[pos++] = bpop;
          bpop += dpopcount(*data++);
          if((i>0||j>0) && (bitget(this->bitmap_data,i*256+j*32-1) && bitget(this->bitmap_data,i*256+j*32)))bpop++;
       }
       pop += bpop;
    }
    /*std::cout<<n<<std::endl;
    for(int k=0;k<ns*nb;k++)std::cout<<this->bdata[k]<<" ";std::cout<<"_________"<<std::endl;
    for(int k=0;k<ns;k++)std::cout<<this->sdata[k]<<" ";std::cout<<"________"<<std::endl;*/
}

static_doublebitmap_s::static_doublebitmap_s() {
    this->bitmap_data=NULL;
    this->sdata=NULL;
    this->bdata=NULL;
    this->n=0;
    //DPB = NULL;
}
static_doublebitmap_s::~static_doublebitmap_s() {
    //if(this->bitmap_data!=NULL)delete this->bitmap_data;
    if(this->bdata!=NULL)delete[] (this->bdata);
    if(this->sdata!=NULL)delete[] (this->sdata);
}
unsigned int static_doublebitmap_s::rank(unsigned int pos){
    pos++;
    unsigned int t=0;
    unsigned int i, j;
    i=pos>>8;j=pos>>5;
    //FIXME: deberia estar esta conidcion? le agregue le pos-j*32>0 <=> pos%32>0
    if((j>0) && (pos-j*32>0) && (bitget(this->bitmap_data,j*32-1) && bitget(this->bitmap_data,j*32)))t++;
    return this->sdata[i] + this->bdata[j] + dpopcount(this->bitmap_data[j] & ((1<<(pos&0x1F))-1)) + t;
}
/*FIXME: cant should be lower than the number of occurrences*/
unsigned int static_doublebitmap_s::select(unsigned int cant){
    //std::cout<<std::endl<<cant<<" *"<<std::endl;
    //return 0;
    unsigned int x=cant;
    //std::cout<<"x: "<<x<<std::endl;
    unsigned int s = 256;
    unsigned int l = 0, n = this->n, r = n/s,left;
    unsigned int mid = (l+r)/2;
    unsigned int j;//ones, j;
    unsigned int rankmid = this->sdata[mid];
    if (x==0) return 0;
    //std::cout<<"pase: "<<std::endl;
    // first, binary search on the superblock array
    while (l <= r) {
       if (rankmid < x)
          l = mid+1;
       else
          r = mid-1;
       mid = (l+r)/2;
       rankmid = this->sdata[mid];
    }
    //std::cout<<"l: "<<l<<" r: "<<r<<" mid: "<<mid<<std::endl; 
    // sequential search using dpopcount over an int, we use the block table
    left = mid*8;
    x -= rankmid;
//std::cout<<"x: "<<x<<" left: "<<left<<std::endl; 
    unsigned int k;
    for(k=1;k<8;k++){
        if(left+k == (n+W-1)/W) break;
        if(this->bdata[left+k] > x) break;
    }
    left=left+k-1;
    //left--;
    //std::cout<<"x: "<<x<<" left: "<<left<<std::endl;
    x-= this->bdata[left];
    // sequential search using dpopcount over a char
    j = this->bitmap_data[left];
    left = left*32;
    if(x==0)return left;
    //return left;
    //std::cout<<"x: "<<x<<" left: "<<left<<std::endl; 
    /*if(left>0&&bitget(this->bitmap_data,left-1)&&bitget(this->bitmap_data,left))t++;
    rankmid = dpopcount8(j) + t;
    unsigned int sj = j;
    unsigned int k=0;
    if (rankmid < x) {
       j = j>>8;
       x -= rankmid;
       left += 8;
    //std::cout<<"x1: "<<x<<" left: "<<left<<std::endl; 
       rankmid = dpopcount8(j);
       if (rankmid < x) {
          j = j>>8;
          x -= rankmid;
          left += 8;
    //std::cout<<"x2: "<<x<<" left: "<<left<<std::endl; 
          rankmid = dpopcount8(j);
          if (rankmid < x) {
             j = j>>8;
             x -= rankmid;
             left += 8;
    //std::cout<<"x3: "<<x<<" left: "<<left<<std::endl; 
          }
       }
    }
    return left;*/
    // finally sequential search bit per bit
    //left-=8;
    //std::cout<<"x: "<<x<<" left: "<<left<<"j: "<<j<<std::endl;
    if(left>0&&bitget(this->bitmap_data,left-1)&&bitget(this->bitmap_data,left))x--;
    
    while(x>0) {
        //std::cout<<"j: "<<j<<" left: "<<left<<std::endl; 
        //std::cout<<"--x: "<<x<<" j&0x3: "<<(j&0x3)<<" j: "<<j<<" 0x3:"<<0x03<<std::endl; 
       if((j&0x3)==0x3)x--;//std::cout<<" j&0x3: "<<(j&0x3)<<std::endl;}
       j = j>>1;
       left++;
        //std::cout<<"x*: "<<x<<" left: "<<left<<"j: "<<j<<std::endl; 
    }
    return left;
}
unsigned int static_doublebitmap_s::size(){
    unsigned int ns, nb;
    ns = (this->n+256-1)/256;
    nb = 256/W; 
    return sizeof(static_doublebitmap_s)+ns*nb*sizeof(unsigned char)+ns*sizeof(unsigned int);
}
unsigned int static_doublebitmap_s::save(FILE *fp){
    unsigned char hdr = DBITMAP_S_HDR;
    if(fwrite(&hdr,sizeof(unsigned char),1,fp)!=1){
        return 1;
    }
    if(fwrite(&n,sizeof(unsigned int),1,fp)!=1){
        return 2;
    }
    unsigned int ns, nb, words;
    words = (n+W-1)/W;
    ns = (n+256-1)/256;
    nb = 256/W;
    if(fwrite(sdata,sizeof(unsigned int),ns,fp)!=ns){
        return 3;
    }
    if(fwrite(bdata,sizeof(unsigned char),ns*nb,fp)!=ns*nb){
        return 4;
    }
    return 0;
}
static_doublebitmap_s* static_doublebitmap_s::load(FILE *fp) {
    unsigned char hdr;
    if(fread(&hdr,sizeof(unsigned char),1,fp)!=1 || hdr!=DBITMAP_S_HDR)
        return NULL;
    static_doublebitmap_s* ret = new static_doublebitmap_s();
    if(fread(&(ret->n),sizeof(unsigned int),1,fp)!=1){
        delete ret;
        return NULL;
    }
    unsigned int ns, nb, words;
    words = (ret->n+W-1)/W;
    ns = (ret->n+256-1)/256;
    nb = 256/W;
    ret->sdata = new unsigned int[ns];
    if(ret->sdata==NULL){
        delete ret;
        return NULL;
    }
    if(fread(ret->sdata,sizeof(unsigned int),ns,fp)!=ns){
        delete ret;
        return NULL;
    }
    ret->bdata = new unsigned char[ns*nb];
    if(ret->bdata == NULL){
        delete ret;
        return NULL;
    }
    if(fread(ret->bdata,sizeof(unsigned char),ns*nb,fp)!=ns*nb){
        delete ret;
        return NULL;
    }
    return ret;
}

}
}
