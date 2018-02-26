/* hash.cpp
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

// Closed hash table
#include "hash.h"
#include <cstdlib>

#define bitsW (5)
namespace lz77index{
namespace basics{

inline static unsigned int bitget_go(unsigned int *e, unsigned int p, unsigned int len){ 
	unsigned int answ;
	e += p >> bitsW; p &= (1<<bitsW)-1;
	answ = *e >> p;
	if (len == W)
	{ 
		if(p) 
			answ |= (*(e+1)) << (W-p);
	}
	else{ 
		if (p+len > W) 
			answ |= (*(e+1)) << (W-p);
		answ &= (1<<len)-1;
	}
	return answ;
}
// writes e[p..p+len-1] = s, len <= W
inline static void bitput (register unsigned int *e, register unsigned int p, register unsigned int len, register unsigned int s){ 
	e += p >> bitsW; 
	p &= (1<<bitsW)-1;
	if (len == W){ 
		*e |= (*e & ((1<<p)-1)) | (s << p);
		if (!p) 
			return;
		e++;
		*e = (*e & ~((1<<p)-1)) | (s >> (W-p));
	}
	else{ 
		if (p+len <= W){ 
			*e = (*e & ~(((1<<len)-1)<<p)) | (s << p);
			return;
		}
		*e = (*e & ((1<<p)-1)) | (s << p);
		e++; 
		len -= W-p;
		*e = (*e & ~((1<<len)-1)) | (s >> (W-p));
	}
}

  // creates a table to store up to n values with guaranteed load factor.
  // vbits = # of bits per entry, ENTRIES CANNOT HAVE ZERO VALUE

hash createHash (unsigned int n, unsigned int vbits, float factor){ 
	hash H = (hash)malloc (sizeof(struct shash));
	int i,N;
	if (n == 0){free(H); return NULL;}
	N = (int)(n*factor); 
	if (N <= (int)n) 
		N =(int)(n+1);
	H->size = (1 << bits(N-1)) - 1;
	H->bits = vbits;
	H->table = (unsigned int *) malloc ((((H->size+1)*vbits+W-1)/W)*sizeof(unsigned int));
#ifdef INDEXREPORT
	printf ("     Also created hash table of %i bits\n",
			(((H->size+1)*vbits+W-1)/W)*W);
#endif
	for (i=(((H->size+1)*vbits+W-1)/W)-1;i>=0;i--) H->table[i] = 0;
	return H;
}

// frees the structure
void destroyHash (hash H){ 
	if (H == NULL) 
		return;
	free (H->table);
	free (H);
}

void saveHash(FILE *f, hash H){
	unsigned int indicator;
	if(H){
		indicator = 1;
		fwrite(&indicator, sizeof(unsigned int), 1, f);
		fwrite(&H->size, sizeof(unsigned int), 1, f);
		fwrite(&H->bits, sizeof(unsigned int), 1, f);
		fwrite(H->table, sizeof(unsigned int), ((H->size+1)*H->bits+W-1)/W, f);
	}
	else {
		indicator = 0;
		fwrite(&indicator, sizeof(unsigned int), 1, f);
	}
}   
   
hash loadHash(FILE *f){
	hash H = NULL;
	unsigned int indicator;
	fread(&indicator, sizeof(unsigned int), 1, f);
	if(indicator){
		H = (hash)malloc(sizeof(struct shash));
		fread(&H->size, sizeof(unsigned int), 1, f);
		fread(&H->bits, sizeof(unsigned int), 1, f);
		H->table = (unsigned int *)malloc((((H->size+1)*H->bits+W-1)/W)*sizeof(unsigned int));
		fread(H->table, sizeof(unsigned int), ((H->size+1)*H->bits+W-1)/W, f);      
	}
	return H;
}
   
// inserts an entry, not prepared for overflow
void insertHash (hash H, unsigned int key, unsigned int value){ 
	unsigned int pos = (key*PRIME1) & H->size;
	if (bitget_go(H->table, pos*H->bits, H->bits) != 0){ 
		do 
			pos = (pos + PRIME2) & H->size;
		while (bitget_go(H->table,pos*H->bits,H->bits) != 0);
	}
	bitput(H->table,pos*H->bits,H->bits,value);
}

// looks for a key, returns first value (zero => no values)
// writes in pos a handle to get next values
unsigned int searchHash (hash H, unsigned int key, handle *h){ 
	*h = (key*PRIME1) & H->size;
	return bitget_go(H->table,*h*H->bits,H->bits);
}

// gets following values using handle *pos, which is rewritten
// returns next value (zero => no more values)
unsigned int nextHash (hash H, handle *h){ 
	*h = (*h +PRIME2) & H->size;
	return bitget_go(H->table,*h*H->bits,H->bits);
}

unsigned int sizeofHash(hash H){
	if(H){
		unsigned int aux;
		aux = (((unsigned int)(H->size+1))*H->bits+W-1)/W;
		return sizeof(struct shash) + aux*sizeof(unsigned int);
	}
	else
		return 0;
}
}
}
