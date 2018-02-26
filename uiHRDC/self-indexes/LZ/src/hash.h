/* hash.h
 * Copyright (C) 2003, Gonzalo Navarro, all rights reserved.
 *
 * Closed hash table implementation
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

// Closed hash table that does not store the keys

// needs the maximum number of elements stored to be declared at creation time
// cannot remove elements
// does not store the key, rather, it reports all the collisioning values
// right value under collisions
// CANNOT STORE ZERO in the value

#ifndef HASHINCLUDED
#define HASHINCLUDED

#include "basics.h"
#include <stdio.h>
namespace lz77index{
namespace basics{


typedef struct shash{ 
	unsigned int size;    // # of table entries
	unsigned int bits;    // bits per entry
	unsigned int *table;    // data
} *hash;

typedef unsigned int handle;

// creates a table to store up to n values with guaranteed load factor.
// vbits = # of bits per entry, ENTRIES CANNOT HAVE VALUE ZERO
hash createHash (unsigned int n, unsigned int vbits, float factor);
  // frees the structure
void destroyHash (hash H);
  // inserts an entry 
void insertHash (hash H, unsigned int key, unsigned int elem);
  // looks for a key, returns first value (zero => no values)
  // writes in pos a handle to get next values
unsigned int searchHash (hash H, unsigned int key, handle *h);
  // gets following values using handle *h, which is rewritten
  // returns next value (zero => no more values)
unsigned int nextHash (hash H, handle *h);

void saveHash(FILE *f, hash H);

hash loadHash(FILE *f);


unsigned int sizeofHash(hash H);
// two large primes found with etc/hash.c
#define PRIME1 ((unsigned int)4294967279u)
#define PRIME2 ((unsigned int)4294967197u)
}
}

#endif
