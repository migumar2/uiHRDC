/* BinaryNode.h
 * Copyright (C) 2011, Rodrigo Canovas & Miguel A. Martinez-Prieto
 * all rights reserved.
 *
 * This class implements a binary node required for Hu-Tucker encoding.
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
 *
 * Contacting the authors:
 *   Rodrigo Canovas:  rcanovas@dcc.uchile.cl
 *   Miguel A. Martinez-Prieto:  migumar2@infor.uva.es
 */

#ifndef BINARYTRIE_H_INCLUDED
#define BINARYTRIE_H_INCLUDED

#include <BitString.h>
#include <libcdsBasics.h>
using namespace cds_utils;

#include<iostream>
using namespace std;

/** Auxiliar class to handle binary nodes.
 */

typedef struct
{ uint cbits;
  uint code;
} Tcode;

template <class T>
class BNode{
    public:
    int w; //weight
    int pos; //position
    bool type; //0=external, 1=internal
    int endpoint[2];
    BNode* children[2];
    T* obj;

    BNode();
    BNode(int v, int p, int t, T* o);
    ~BNode(){};
    void print();
    void code(int level, Tcode **codes, BitString **tree, bool *bits, int *tpos);
    void setEndpoints(int start, int end);
    void recPrint(int level=0);
};

template <class T>
BNode<T>::BNode(){
    w=-1;
    obj=0;
    pos=-1;
    type=0;
    endpoint[0]=endpoint[1]=-1;
    children[0]=children[1]=0;
}

template <class T>
BNode<T>::BNode(int v, int p, int t, T* o){
    w=v;
    obj=o;
    pos=p;
    type=t;
    endpoint[0]=endpoint[1]=-1;
    children[0]=children[1]=0;
}

template <class T>
void BNode<T>::setEndpoints(int start, int end){
    endpoint[0]=start;
    endpoint[1]=end;
}

template <class T>
void BNode<T>::code(int level, Tcode **codes, BitString **tree, bool *bits, int *tpos)
{
    (*tpos)++;

    if(children[0])
    {
	bits[level] = 0;
        children[0]->code(level+1, codes, tree, bits, tpos);

	bits[level] = 1;
        children[1]->code(level+1, codes, tree, bits, tpos);
    }
    else
    {
	uint code = 0;
	for (int i=1; i<=level; i++) if (bits[i-1]) bitset(&code, level-i);

	(*codes)[pos].code = code;
	(*codes)[pos].cbits = level;
    }

    (*tree)->setBit((*tpos), 1);
    (*tpos)++;
}

#endif // BINARYTRIE_H_INCLUDED
