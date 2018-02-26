/* patricia_node.cpp
 * Copyright (C) 2009, Sebastian Kreft C., all rights reserved.
 *
 * patricia_node definition
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

#include "patricia_node.h"
#include "basics.h"

#include <cstring>
#include <iostream>
#include <algorithm>


namespace lz77index{

patricia_leaf::patricia_leaf(unsigned int start, unsigned int len, unsigned int id){
    this->start = start;
    this->len = len;
    this->id = id;
}
patricia_leaf::~patricia_leaf(){
}
bool patricia_leaf::isLeaf(){
    return true;
}
unsigned int patricia_leaf::getStart(){
    return this->start;
}
unsigned int patricia_leaf::getLength(){
    return this->len;
}
unsigned int patricia_leaf::getSkip(int index){
    return 0;
}
int patricia_leaf::getIndex(unsigned char chr){
    return -1;
}
patricia_node* patricia_leaf::getNode(int index){
    return NULL;
}
void patricia_leaf::setSkip(int index, unsigned int skip){}
void patricia_leaf::setNode(int index, patricia_node* node){}
patricia_node* patricia_leaf::getLeaf(){
    return this;
}
void patricia_leaf::addBranch(unsigned char chr, patricia_node* node,unsigned int skip){}
unsigned int patricia_leaf::add(unsigned char* text, unsigned int start, unsigned int length, unsigned int pos, bool reverse, unsigned int id){return 0;}
void patricia_leaf::print(int tab){
    for(int i=0;i<tab;i++)std::cout<<"  ";
    std::cout<<this->start<<" "<<this->len<<std::endl;
}
void patricia_leaf::dfuds(unsigned int* bm, unsigned int* pos){
    //std::cout<<"0";debiese ser 1
    basics::bitset(bm,*pos);
    *pos = *pos+1; 
}
void patricia_leaf::dfuds_labels(unsigned char* label, unsigned int* pos){
    return;
}
void patricia_leaf::dfuds_skips(unsigned int* skip, unsigned int* pos){
    return;
}
unsigned int patricia_leaf::height(){
    return 1;
}
void patricia_leaf::leafIds(unsigned int* ids, unsigned int* pos){
    ids[*pos] = this->id;
    *pos = *pos + 1;
}
/**********************************************/
patricia_inner::patricia_inner(){
    this->children = new std::vector<patricia_node*>();
    this->skips = new std::vector<unsigned int>();
    this->labels = new std::vector<unsigned char>();
}
patricia_inner::~patricia_inner(){
    for(unsigned int i=0;i<this->children->size();i++)
        delete this->children->at(i);
    delete this->children;
    delete this->skips;
    delete this->labels;
}
bool patricia_inner::isLeaf(){
    return false;
}
unsigned int patricia_inner::getStart(){
    return 0;
}
unsigned int patricia_inner::getLength(){
    return 0;
}
unsigned int patricia_inner::getSkip(int index){
    return this->skips->at(index);
}
int patricia_inner::getIndex(unsigned char chr){
    for(unsigned int i=0;i<this->labels->size();i++){
        if(this->labels->at(i)==chr)return i;
    }
    return -1;
}
patricia_node* patricia_inner::getNode(int index){
    return this->children->at(index);
}
void patricia_inner::setSkip(int index, unsigned int skip){
    (*(this->skips))[index]=skip;
}
void patricia_inner::setNode(int index, patricia_node* node){
    (*(this->children))[index] = node;
}
patricia_node* patricia_inner::getLeaf(){
    return this->getNode(0)->getLeaf();
}
void patricia_inner::addBranch(unsigned char chr, patricia_node* node, unsigned int skip){
    int index = lower_bound(labels->begin(), labels->end(), chr) - labels->begin();
    this->children->insert(this->children->begin()+index, node);
    this->labels->insert(this->labels->begin()+index, chr);
    this->skips->insert(this->skips->begin()+index, skip);
    /*this->children->push_back(node);
    this->labels->push_back(chr);
    this->skips->push_back(skip);*/
}
unsigned int patricia_inner::add(unsigned char* text, unsigned int start, unsigned int length, unsigned int pos, bool reverse, unsigned int id){
    unsigned int skip, nskip;
    patricia_node *node, *nnode, *lnode, *dnode;
    int f=reverse?-1:1;
    int index = this->getIndex(text[start+f*pos]);
    /*there is no branch starting with text[start+pos] in the trie*/
    if(index==-1){
        lnode = new patricia_leaf(start, length, id);
        this->addBranch(text[start+f*pos], lnode, 1);
        return 1;
    }else{
        skip = this->getSkip(index);
        node = this->getNode(index);
        if(node->isLeaf()){
            nskip = computeSkip(text, start, length, pos, node->getStart(), node->getLength(), 0, f);
            /*we only have one node in the branch so the skip may increase*/
            lnode = new patricia_leaf(start, length, id);
            nnode = new patricia_inner();
            /*Check if the char is within the bounds otherwise it would be $=\0 */
            nnode->addBranch(pos+nskip<length?text[start+f*(pos+nskip)]:'\0',lnode);
            nnode->addBranch(pos+nskip<node->getLength()?text[node->getStart()+f*(pos+nskip)]:'\0',node);
            this->setNode(index,nnode);
            this->setSkip(index,nskip);
            return 2;
        }else{
            /*at least two nodes in the branch, so skip can not increase*/
            dnode = node->getLeaf();
            nskip = computeSkip(text, start, length, pos, dnode->getStart(), dnode->getLength(), skip, f);
            //std::cout<<"nskip: "<<nskip<<" "<<skip<<" "<<start<<" "<<length<<" "<<pos<<std::endl;
            if(nskip==skip){
                /*we add recursively into the subtrie*/
                //std::cout<<"recursivo"<<std::endl;
                return node->add(text, start, length, pos+skip, reverse, id);
            }else{
                /*we create new nodes*/
                lnode = new patricia_leaf(start, length, id);
                nnode = new patricia_inner();
                /*Check if the char is within the bounds otherwise it would be $=\0 */
                nnode->addBranch(pos+nskip<length?text[start+f*(pos+nskip)]:'\0',lnode,1);
                nnode->addBranch(pos+nskip<dnode->getLength()?text[dnode->getStart()+f*(pos+nskip)]:'\0',node,skip-nskip);
                this->setNode(index,nnode);
                this->setSkip(index,nskip);
                return 2;
            }
        }
    }
}
unsigned int computeSkip(unsigned char* text, unsigned int start, unsigned int length, unsigned int pos, unsigned int start2, unsigned int length2, unsigned int maxskip, int f){
    unsigned int max_skip = std::min(length,length2)-pos;
    if(maxskip!=0)max_skip = std::min(max_skip, maxskip); 
    for(unsigned int skip=1; skip<=max_skip; skip++){
        if(text[start+f*(pos+skip-1)] != text[start2+f*(pos+skip-1)])
            return skip-1;
    }
    return max_skip;
}
void patricia_inner::print(int tab){
    for(int i=0;i<tab;i++)std::cout<<"  ";
    for(unsigned int i=0;i<this->children->size();i++)
        std::cout<<(unsigned int)this->labels->at(i)<<" "<<this->skips->at(i)<<" | ";
    std::cout<<std::endl;
    for(unsigned int i=0;i<this->children->size();i++)
        this->children->at(i)->print(tab+1);
}
void patricia_inner::dfuds(unsigned int* bm, unsigned int* pos){
    //for(unsigned int i=0;i<this->children->size();i++)
    //    basics::bitset(bm,*pos+i);std::cout<<"1"; debiese ser 0
    *pos = *pos + this->children->size();
    basics::bitset(bm,*pos);//std::cout<<"0"; debiese ser 1
    //std::cout<<"bit setted at:"<<*pos<<std::endl;
    *pos = *pos+1;
    for(unsigned int i=0;i<this->children->size();i++)
        this->children->at(i)->dfuds(bm,pos);
}
void patricia_inner::dfuds_labels(unsigned char* label, unsigned int* pos){
    for(unsigned int i=0;i<this->children->size();i++)
        label[*pos+i] = this->labels->at(i); 
    *pos = *pos + this->children->size();
    for(unsigned int i=0;i<this->children->size();i++)
        this->children->at(i)->dfuds_labels(label,pos);
}
void patricia_inner::dfuds_skips(unsigned int* skip, unsigned int* pos){
    for(unsigned int i=0;i<this->children->size();i++)
        skip[*pos+i] = this->skips->at(i); 
    *pos = *pos + this->children->size();
    for(unsigned int i=0;i<this->children->size();i++)
        this->children->at(i)->dfuds_skips(skip,pos);
}
unsigned int patricia_inner::height(){
    unsigned int max_h = 0;    
    for(unsigned int i=0;i<this->children->size();i++){
        unsigned int h = this->children->at(i)->height();
        if(h>max_h)max_h = h;
    }
    return 1 + max_h; 
}
void patricia_inner::leafIds(unsigned int* ids, unsigned int* pos){
    for(unsigned int i=0;i<this->children->size();i++){
        this->children->at(i)->leafIds(ids,pos);      
    }
}

}

