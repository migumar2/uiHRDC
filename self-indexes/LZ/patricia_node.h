/* patricia_node.h
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

#ifndef _PATRICIA_NODE_H
#define _PATRICIA_NODE_H

//#include <basics.h>
#include <iostream>
#include <vector>
namespace lz77index{

class patricia;

class patricia_node{
    public:
        virtual ~patricia_node(){};
        virtual bool isLeaf()=0;
        virtual unsigned int getStart()=0;
        virtual unsigned int getLength()=0;
        virtual unsigned int getSkip(int index)=0;
        virtual int getIndex(unsigned char chr)=0;
        virtual patricia_node* getNode(int index)=0;
        virtual void setSkip(int index, unsigned int skip)=0;
        virtual void setNode(int index, patricia_node* node)=0;
        virtual patricia_node* getLeaf()=0;
        virtual void addBranch(unsigned char chr, patricia_node* node, unsigned int skip=1)=0;
        virtual unsigned int add(unsigned char* text, unsigned int start, unsigned int length, unsigned int pos, bool reverse, unsigned int id)=0;
        virtual void print(int tab=0)=0;
        virtual void dfuds(unsigned int* bm, unsigned int* pos)=0;
        virtual void dfuds_labels(unsigned char* labels, unsigned int* pos)=0;
        virtual void dfuds_skips(unsigned int* skips, unsigned int* pos)=0;
        virtual unsigned int height()=0;
        virtual void leafIds(unsigned int* ids, unsigned int* pos)=0; //deberia ser friend
};

class patricia_leaf:public patricia_node{
    public:
        patricia_leaf(unsigned int start, unsigned int len, unsigned int id);
        virtual ~patricia_leaf();
        bool isLeaf();
        unsigned int getStart();
        unsigned int getLength();
        unsigned int getSkip(int index);
        int getIndex(unsigned char chr);
        patricia_node* getNode(int index);
        void setSkip(int index, unsigned int skip);
        void setNode(int index, patricia_node* node);
        patricia_node* getLeaf();
        void addBranch(unsigned char chr, patricia_node* node,unsigned int skip=1);
        unsigned int add(unsigned char* text, unsigned int start, unsigned int length, unsigned int pos, bool reverse, unsigned int id);
        void print(int tab=0);
        void dfuds(unsigned int* bm, unsigned int* pos);
        void dfuds_labels(unsigned char* labels, unsigned int* pos);
        void dfuds_skips(unsigned int* skips, unsigned int* pos);
        unsigned int height();
        void leafIds(unsigned int* ids, unsigned int* pos);        
    private:
        unsigned int id;
        unsigned int start;//represents the starting position of the node
        unsigned int len;//represents the length of the node
};

class patricia_inner:public patricia_node{
    public:
        patricia_inner();//creates an inner node
        virtual ~patricia_inner();
        bool isLeaf();
        unsigned int getStart();
        unsigned int getLength();
        unsigned int getSkip(int index);
        int getIndex(unsigned char chr);
        patricia_node* getNode(int index);
        void setSkip(int index, unsigned int skip);
        void setNode(int index, patricia_node* node);
        patricia_node* getLeaf();
        void addBranch(unsigned char chr, patricia_node* node, unsigned int skip=1);
        unsigned int add(unsigned char* text, unsigned int start, unsigned int length, unsigned int pos, bool reverse, unsigned int id);
        void print(int tab=0);
        void dfuds(unsigned int* bm, unsigned int* pos);
        void dfuds_labels(unsigned char* labels, unsigned int* pos);
        void dfuds_skips(unsigned int* skips, unsigned int* pos);
        unsigned int height();
        void leafIds(unsigned int* ids, unsigned int* pos); 
    private:
        std::vector<patricia_node*>* children;
        std::vector<unsigned int>* skips;
        std::vector<unsigned char>* labels;
};
unsigned int computeSkip(unsigned char* text, unsigned int start, unsigned int length, unsigned int pos, unsigned int start2, unsigned int length2, unsigned int maxskip, int f);

}

#endif /* _PATRICIA_H */
