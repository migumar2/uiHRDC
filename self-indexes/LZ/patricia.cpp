/* patricia.cpp
 * Copyright (C) 2009, Sebastian Kreft C., all rights reserved.
 *
 * patricia definition
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

#include "patricia.h"
#include <cstring>

namespace lz77index{

patricia::patricia(unsigned char* text, unsigned int tlen){
    this->root = new patricia_inner();
    this->text = text;
    this->tlen = tlen;
    this->strings = 0;
    this->_nodes = 1;
}
patricia::~patricia(){
    delete this->root;
    /*the text should be released by the caller*/
}

void patricia::add(unsigned int start){
    this->_nodes += this->root->add(this->text, start, this->tlen-start, 0, false,this->strings);//false means non reverse,strings=id
    this->strings += 1;
}
void patricia::addReverse(unsigned int start, unsigned int length){
    this->_nodes +=this->root->add(this->text, start, length, 0, true,this->strings);//true means reverse,strings=id
    this->strings += 1;
}
void patricia::print(){
    this->root->print();
}
unsigned int* patricia::dfuds(){
    unsigned int* bm = new unsigned int[(2*_nodes)/32+1];
    for(unsigned int i=0;i<(2*_nodes)/32+1;i++){
        bm[i] = 0;
    }
    unsigned int pos=1;
    this->root->dfuds(bm,&pos);
    return bm;
}
unsigned char* patricia::dfuds_labels(){
    unsigned char* label = new unsigned char[_nodes];
    for(unsigned int i=0;i<_nodes;i++){
        label[i] = 0;
    }
    label[0] = '\0';
    unsigned int pos=1;
    this->root->dfuds_labels(label,&pos);
    return label;
}
unsigned int* patricia::dfuds_skips(){
    unsigned int* skips = new unsigned int[_nodes];
    for(unsigned int i=0;i<_nodes;i++){
        skips[i] = 0;
    }
    unsigned int pos=1;
    this->root->dfuds_skips(skips,&pos);
    return skips;
}
unsigned int patricia::height(){
    return this->root->height();
}
unsigned int* patricia::leafIds(){
    unsigned int* ids = new unsigned int[this->strings];
    unsigned int pos = 0;
    this->root->leafIds(ids,&pos);
    return ids;
}
unsigned int patricia::ids(){
    return this->strings;
}
unsigned int patricia::nodes(){
    return this->_nodes;
}

}


