/* patricia.h
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

#ifndef _PATRICIA_H
#define _PATRICIA_H

//#include <basics.h>
#include <iostream>
#include "patricia_node.h"

namespace lz77index{

/** Patricia trie structure
 * 
 *  @author Sebastian Kreft
 */
class patricia {
    public:
        patricia(unsigned char* text, unsigned int tlen);
        ~patricia();
        /** adds the string text[start,start+length-1] to the trie */
        //virtual void add(uint start, uint length);
        /** adds the string text[start,tlength-1] to the trie */
        void add(unsigned int start);
        void addReverse(unsigned int start, unsigned int length);
        void print();
        unsigned int* dfuds();
        unsigned char* dfuds_labels();
        unsigned int* dfuds_skips();
        unsigned int height();
        unsigned int* leafIds();
        unsigned int ids();
        unsigned int nodes();
    private:
        unsigned char* text;
        unsigned int tlen;
        patricia_node* root;
        unsigned int strings;
        unsigned int _nodes;
};

}

#endif /* _PATRICIA_H */
