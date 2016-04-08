/* dfuds.cpp
 * Copyright (C) 2010, Diego Arroyuelo, all rights reserved.
 *
 * Implementation of the DFUDS tree representation
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

#define CLOSING (1)
#define NULLT ((unsigned int)-1)
#define ROOT (1)
#define DFUDS_HDR (79)
#include "dfuds.h"

/*TODO: replace call to close and related ones to higher level functions*/
namespace lz77index{
namespace basics{


dfuds::dfuds(unsigned int *par, unsigned int n, unsigned char *_label){//, unsigned int block_size){
    unsigned int block_size = 512;
    topology = new bp_hb(par, 2*n, block_size, true);
    if(topology==NULL)std::cout<<"Topology is NULL"<<std::endl;
    DPB = new static_doublebitmap_s(par, 2*n);
    Nnodes = n;
    label = _label;
    boost = new unsigned int[256];
    boost_pos = new unsigned char[256];
    //std::cout<<"computing boost"<<std::endl;
    this->compute_boost();
/*    for(p=ROOT; !bitget(topology->data, p); p++);
    
    for(p--, j=p; p>=1; p--, j--) 
       boost[label[j]] = topology->close(p)+1;*/
    /*std::cout<<"N:"<<n<<std::endl;
    for(unsigned int i=0;i<2*n;i++){
        std::cout<<i<<" -> "<<inspect(i)<<std::endl;
        if(!inspect(i)){
            std::cout<<"Close  : "<<i<<" "<<topology->close(i)<<std::endl;
            std::cout<<"Enclose: "<<i<<" "<<topology->enclose(i)<<std::endl;
        }
    }*/
}

dfuds::dfuds(){
    Nnodes = 0;
    topology = NULL;
    DPB = NULL;
    label = NULL;
    boost = new unsigned int[256];
    boost_pos = new unsigned char[256];
}
dfuds::~dfuds(){
    if(topology != NULL)
        delete topology;
    if(boost != NULL)
        delete [] boost;
    if(boost_pos != NULL)
        delete [] boost_pos;
    if(DPB != NULL)
        delete DPB;
    if(label != NULL)
        delete[] label;
}
void dfuds::compute_boost(){
    for(unsigned int i=0; i<256; i++){
        boost[i] = NULLT;
    }
    unsigned int d = this->degree(ROOT);
    //std::cout<<"Degree: "<<d<<std::endl;
    for(unsigned int i=0;i<d;i++){
        boost[label[i+1]] = this->child(ROOT,i);
        boost_pos[label[i+1]] = i;
    }
}

unsigned int dfuds::nodes(){
    return Nnodes;
}

unsigned int dfuds::root(){
    return ROOT;
}


unsigned int dfuds::parent(unsigned int x)
 {
    unsigned int p;
    unsigned int *data = topology->data; 
    
    if (x == ROOT) return NULLT; // root has no parent
    
    p = topology->open(x-1);    

    if ((p>=W) && (data[p/W]&(~((~0)<<(p%W))))==0) p-=(p%W+1);
		while ((p>=W) && data[p/W]==0)
			p-=32;
		while (p!=0 && !bitget(data,p)) p--;
	  //p = ((static_bitsequence_brw32 *)topology->bdata)->prev(p);
    
    return (unsigned int)p+1;
 }


unsigned int dfuds::parent_original(unsigned int x){
    unsigned int p, r;
        
    if (x == ROOT) return NULLT; // parent of root
    
    p = topology->open(x-1);
    
    r = topology->bdata->rank1(p);
    
    return (unsigned int)topology->bdata->select1(r)+1;
 }


unsigned int dfuds::child(unsigned int x, unsigned int i){
    unsigned int d = degree(x);
    if(i >= d) 
       return (unsigned int)NULLT; // there is no such child
    else
       return (unsigned int)topology->close(x+d-i-1)+1;
}


unsigned int dfuds::level_ancestor(unsigned int x, int delta){// brute force
    while (delta < 0) {
       x = parent(x); 
       delta++;
    }
    return x;
}


unsigned int dfuds::depth(unsigned int x){ // brute force
    unsigned int r = ROOT;
    unsigned int d = 0;
    
    while (x!=r) {
       x = parent(x);
       d++;
    }
    return d;
}


unsigned int dfuds::preorder_rank(unsigned int x){
    return topology->bdata->rank1(x-1); 
}

unsigned int dfuds::preorder_select(unsigned int p){
    if(p==0)
        return ROOT;
    else
        return (unsigned int)topology->bdata->select1(p)+1;
}

unsigned int dfuds::inspect(unsigned int p){
    return bitget(topology->data,p);
}


bool dfuds::isleaf(unsigned int x){
    return (inspect(x) == CLOSING && inspect(x-1) == CLOSING);
}
 

unsigned int dfuds::subtree_size(unsigned int x){
    unsigned int e, f;
    
    if (inspect(x)==CLOSING) return 1;
    e = topology->enclose(x);
    f = topology->close(e);
    return (f-x)/2 + 1; 
 }


unsigned int dfuds::first_child(unsigned int x){
    if (isleaf(x)) 
       return NULLT;
    else
       return child(x, 0);
 }


unsigned int dfuds::next_sibling(unsigned int x)
 {
    unsigned int i;
    
    i = topology->open(x-1)+1;
    
    if (inspect(i)==CLOSING) 
       return (unsigned int)NULLT;  // there is no next sibling
    else
       return (unsigned int)topology->close(i)+1;
 }


unsigned int dfuds::prev_sibling(unsigned int x)
 {
    unsigned int i;
    
    i = topology->open(x-1)-1;
    
    if (inspect(i)==CLOSING) 
       return (unsigned int)NULLT;  // there is no next sibling
    else
       return (unsigned int)topology->close(i)+1;
 }
 
 
bool dfuds::is_ancestor(unsigned int x, unsigned int y){
    return (x <= y) && (y <= (unsigned int)topology->close(topology->enclose(x))); 
}
 

unsigned int dfuds::distance(unsigned int x, unsigned int y){  // brute force
    unsigned int r = ROOT;
    unsigned int d = 0;
    
    while (y!=r && x!=y) {
       y = parent(y);
       d++;
    }
    return d;
}


unsigned int dfuds::degree(unsigned int x){
    unsigned int iaux = x, *data = topology->data;
    
    if ((data[x/W]&((~0)<<(x%32)))==0) x+=32-(x%W);
    while (data[x/W]==0)
       x+=32;
   
    while (!bitget(data, x))
       x++;
        
    return x-iaux; 
}


unsigned int dfuds::degree_original(unsigned int x){
    return (unsigned int)topology->bdata->select1(topology->bdata->rank1(x) + 1)-x;
}

unsigned int dfuds::child_rank(unsigned int x){
  unsigned int i;
  
  if (x==ROOT) return 1;

  i = topology->open(x);
  return (unsigned int)topology->bdata->select1(topology->bdata->rank1(i-1)+1) - i;
}



// obtains the label of a node. Assumes that the
// labels are stored in reverse order.
unsigned char dfuds::get_label(unsigned int x){
    unsigned int p;
 
    if (x==ROOT) return label[0];
    
    // find the "(" corresponding to node i in the definition of its parent
     p = topology->open(x-1);
    
    // the position of the letter corresponds to the number of "(" up to position p
    return label[topology->bdata->rank0(p)-1]; 
 }

//saves in ith the position in the array label of s
unsigned int dfuds::labeled_child(unsigned int x, unsigned char s, unsigned int* pos){
    unsigned int j;
    unsigned char tc=~s; 
    unsigned int r, d, last;
    if(x == ROOT){
        *pos = ((unsigned int)(boost_pos[s]))+1;
        return boost[s];
    }
    *pos = (unsigned int)(-1);    
    if(isleaf(x)) return NULLT; // node is a leaf
    
    r     = topology->bdata->rank0(x-1);
    d     = degree(x);
    last  = r+d-1;
    j     = r;
    //std::cout<<"Labels are: ";
    //for(unsigned int i=j;i<last;i++){
    //    std::cout<<(unsigned int)label[i]<<" ";
    //}
    //std::cout<<std::endl;
    // First, binary search on the children of node x
	while(last-j > 5) {
       unsigned int med = (last+j)/2;
       unsigned char let = label[med];
       if (let == s) break;
       if (let < s) j = med+1;
       else last = med-1;
    }
  
    // Then, sequential scan if there are only a few children
    while(j <= last) { // there is a child here
       tc = label[j];
       // is there a child by letter c?
       if (tc >= s) break;
       j++;
    }
    if(tc == s){
        *pos = j;
        return this->child(x,j-r);//topology->close(x+(j-r))+1;
    }else{ 
        return NULLT; // no child to go down by s
    }
}


unsigned int dfuds::size(){
    return sizeof(dfuds) +          // size of the class itself
    topology->size() +   	    // parentheses
    sizeof(unsigned char)*Nnodes +       // labels
    sizeof(unsigned int)*256 +           // boost
    DPB->size();
}

unsigned int dfuds::leftmost_leaf(unsigned int nid){
    //return 0;    
    return leaf_select(leaf_rank(nid-1)+1);//nid is always greater than 0, 0 is dummy?
}
unsigned int dfuds::rightmost_leaf(unsigned int nid){
    //if(nid==this->P->n-1)return nid;
    //if(bitget(P->data,nid))return nid;//node is a leaf
    //std::cout<<"RML:"<<nid<<std::endl;
    if(inspect(nid))return nid;//node is a leaf, because is a valid node and valid nodes setted to 1 are leaves 
    //std::cout<<"RML:"<<nid<<" "<<topology->close(nid)<<std::endl;
    //std::cout<<"LR:"<<leaf_rank(topology->close(nid))+1<<std::endl;
    /*return findclose(P,enclose(P,nid));*/
    //std::cout<<"Enclose: "<<topology->enclose(nid)<<std::endl;
    return topology->close(topology->enclose(nid));
    //return leaf_select(leaf_rank(topology->close(nid))+1);
}
unsigned int dfuds::leaf_rank(unsigned int nid){
    //std::cout<<"leaf rank: "<<nid<<" "<<DPB->rank(nid)<<std::endl;
    return DPB->rank(nid);
}
unsigned int dfuds::leaf_select(unsigned int leaf){
    return DPB->select(leaf);
}
unsigned int dfuds::leftmost_leaf_rank(unsigned int nid){
    return leaf_rank(nid-1)+1;
}
unsigned int dfuds::rightmost_leaf_rank(unsigned int nid){
    //if(inspect(nid))
    //    return leaf_rank(nid);
    //return leaf_rank(topology->close(nid))+1;
    return leaf_rank(rightmost_leaf(nid));
}
unsigned int dfuds::save(FILE* fp){
    unsigned char hdr = DFUDS_HDR;
    if(fwrite(&hdr,sizeof(unsigned char),1,fp)!=1){
        return 1;
    }
    if(fwrite(&Nnodes,sizeof(unsigned int),1,fp)!=1){
        return 2;
    }
    if(fwrite(label,sizeof(unsigned char),Nnodes,fp)!=Nnodes){
        return 3;
    }
    if(topology->save(fp)!=0){
        return 4;
    }
    if(DPB->save(fp)!=0){
        return 5;
    }
    return 0;
}
dfuds* dfuds::load(FILE* fp){
    unsigned char hdr;
    if(fread(&hdr,sizeof(unsigned char),1,fp)!=1 || hdr!=DFUDS_HDR){
        std::cerr<<"DFUDS: bad header"<<std::endl;
        return NULL;
    }
    dfuds* ret = new dfuds();
    if(fread(&(ret->Nnodes),sizeof(unsigned int),1,fp)!=1){
        std::cerr<<"DFUDS: reading nodes"<<std::endl;
        delete ret;
        return NULL;
    }
    ret->label = new unsigned char[ret->Nnodes];
    if(ret->label == NULL){
        std::cerr<<"DFUDS: allocating labels"<<std::endl;
        delete ret;
        return NULL;
    }
    if(fread(ret->label,sizeof(unsigned char),ret->Nnodes,fp) != ret->Nnodes){
        std::cerr<<"DFUDS: labels"<<std::endl;
        delete ret;
        return NULL;
    }
    ret->topology = bp_hb::load(fp);
    if(ret->topology == NULL){
        std::cerr<<"DFUDS: topology"<<std::endl;
        delete ret;
        return NULL;
    }
    ret->DPB = static_doublebitmap_s::load(fp);
    if(ret->DPB == NULL){
        std::cerr<<"DFUDS: doublebitmap"<<std::endl;
        delete ret;
        return NULL;
    }
    ret->DPB->bitmap_data = ret->topology->data;
    ret->compute_boost();
    return ret;     
}

}
}
