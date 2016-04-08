/* dfuds.h
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

#ifndef __DFUDS_H__
#define __DFUDS_H__

#include "bp_hb.h"
#include "static_doublebitmap_s.h"

namespace lz77index{
namespace basics{

class dfuds {
private:
    unsigned char *label;
    unsigned int Nnodes;
    unsigned int *boost;
    unsigned char* boost_pos;
    static_doublebitmap_s* DPB;
    bp_hb *topology;
    void compute_boost();
public:
    /*labels of node start from 1*/
    dfuds(unsigned int *par, unsigned int n, unsigned char *_label);//, unsigned int block_size);
    dfuds();
    ~dfuds();
    
    /** nodes(): number of nodes in the tree */
    unsigned int nodes();
    
    /** root(): returns the root node */
    unsigned int root();
    
    /** parent(x): returns the parent of node x, using a sequential
        scan */
    unsigned int parent(unsigned int x);
    
    /** parent_original(x): returns the parent of node x, using the original
        DFUDS definition */
    unsigned int parent_original(unsigned int x);
    
    /** child(x, i): returns the i-th child of node x starting from 0 */
    unsigned int child(unsigned int x, unsigned int i);
    
    /** level_ancestor(x, delta): level-ancestor query. Delta is a negative number*/
    unsigned int level_ancestor(unsigned int x, int delta);  // brute force
    
    /** depth(x): returns the depth of node x in the tree */
    unsigned int depth(unsigned int x); // brute force
    
    /** preorder_rank(x): returns the preorder number of node x */
    unsigned int preorder_rank(unsigned int x);
    
    /** preorder_select(p): returns the node of x with preorder p */    
    unsigned int preorder_select(unsigned int p);
    
    /** inspect(p): inspect the DFUDS sequence at position p */
    unsigned int inspect(unsigned int p);
    
    /** isleaf(x): is x a leaf node? */
    bool isleaf(unsigned int x);
    
    /** subtree_size(x): size of the subtree rooted at node x */
    unsigned int subtree_size(unsigned int x);
    
    /** first_child(x): first child of node x */
    unsigned int first_child(unsigned int x);
    
    /** next_sibling(x): next sibling of node x */
    unsigned int next_sibling(unsigned int x);
    
    /** prev_sibling(x): previous sibling of node x */    
    unsigned int prev_sibling(unsigned int x);
    
    /** is_ancestor(x, y): is x an ancestor of node y?*/
    bool is_ancestor(unsigned int x, unsigned int y);
    
    /** distance(x, y): distance between nodes x and y */
    unsigned int distance(unsigned int x, unsigned int y);  // brute force
    
    /** degree(x, y): degree (number of children) of node x,
        implemented in a sequential fashion over the DFUDS sequence */
    unsigned int degree(unsigned int x);

    /** degree_original(x, y): degree (number of children) of node x,
        implemented as originally defined for DFUDS */    
    unsigned int degree_original(unsigned int x);
    
    /** child_rank(x): rank of node x among its siblings */
    unsigned int child_rank(unsigned int x);
    
    /** get_label(x): gets the label of node x, for labeled trees. */
    unsigned char get_label(unsigned int x);
    
    /** labeled_child(x, s): the child of node x by label s, for
        labeled trees */
    unsigned int labeled_child(unsigned int x, unsigned char s, unsigned int* pos);
    
    /** size(): size of the representation, in bytes */
    unsigned int size();

    unsigned int leftmost_leaf(unsigned int nid);
    unsigned int rightmost_leaf(unsigned int nid);
    unsigned int leaf_rank(unsigned int nid);
    unsigned int leaf_select(unsigned int leaf);
    unsigned int leftmost_leaf_rank(unsigned int nid);
    unsigned int rightmost_leaf_rank(unsigned int nid);
    unsigned int save(FILE* fp);
    static dfuds* load(FILE* fp);
};

}
}

#endif
