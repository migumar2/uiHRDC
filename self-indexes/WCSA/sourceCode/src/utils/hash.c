
/* DYNAMIC END-TAGGED DENSE CODE. -- 
A dynamic word-based byte oriented compressor for text files based on 
dynamic End-Tagged Dense Code.

Brisaboa, N. R., Faria, A., Navarro, G., Param, J. R. 
Simple, Fast, and Efficient Natural Language Adaptive Compression. 
11th International Symposium on String Processing and Information Retrieval (SPIRE'04) - LNCS 3246. A. Apostolico, M. Melucci (Ed.), pp. 230-241. 
Padova (Italia), 2004. 

Copyright (C) 2005 Antonio Faria.

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

Author's contact: Antonio Faria, Dept. of Computer Science, University of
A Corua. Campus de Elvia s/n. Spain  fari@udc.es
*/

/*-----------------------------------------------------------------------
 Hash: Definition of HashTable class (Linear Hash)
 ------------------------------------------------------------------------*/
 
#include "hash.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>


/*-----------------------------------------------------------------
 Initilization of data structures used by the hashTable
 ---------------------------------------------------------------- */
t_hash initialize_hash (unsigned long sizeVoc) {
	t_hash h;
	unsigned long i;

	h = (t_hash) malloc(sizeof(struct hashStr));
	h->SIZE_HASH = (unsigned long) (OCUP_HASH * sizeVoc);
	h->SIZE_HASH = NearestPrime(h->SIZE_HASH);
	h->hash = (t_hashNode *) malloc(h->SIZE_HASH * sizeof(t_hashNode));
	h->NumElem = 0;

	//creates the memory manager that is used to reserve small pieces of memory (for words)
	h->_memMgr=createMemoryManager();


	for (i = 0; i < h->SIZE_HASH; i++)    {
	 	h->hash[i].word = NULL;
	 	h->hash[i].len = 0;
		h->hash[i].posInVoc = 0;
	}
	printf("\nHash Table initilized with: %lu elements\n",h->SIZE_HASH);

	return h;	
}


/*------------------------------------------------------------------
 Find the nearest prime number over n.
 ---------------------------------------------------------------- */
unsigned long NearestPrime(unsigned long n)
{
    long position;  /* the prime number being sought */
    long index;

    for (position = n; ; position++)
    {
        // checks if those values from 2 to $\sqrt{m}$ can be factors of $m$ */
        for (index = 2; index <= (long) sqrt((double) position) && position % index != 0; index++) ;

        if (position % index != 0)  /* No factors in that range, therefore a prime number was found */
        {
            break;
        }
    }
    return position;
}



/*-----------------------------------------------------------------------
 Computes a hash function over a string "aWord", it returns the position
 where "aWord" should go in the hash table if no collision ocurrs.
 ---------------------------------------------------------------------*/
unsigned long hashFunction (const unsigned char *aWord, unsigned int len, unsigned long sizeHash)
{
    char	c;
    register unsigned int h;
    //register unsigned long last;
    //last=((unsigned long) aWord) +len -1;
    register unsigned char *last;
    last = (unsigned char *) (aWord +(len -1));


    h = SEED;
    //for( ; ((unsigned long) aWord <=last ) ; )
    for( ; ( aWord <=last ) ; )
    {
    	c=*(aWord++);
		h ^= ( (h << 5) + c + (h >> 2) );
    }
    return((unsigned int)((h&0x7fffffff) % sizeHash));
}




/*-----------------------------------------------------------------------
 Computes a hash function over a string "aWord", it returns the position
 where "aWord" should go in the hash table if no collision ocurrs.
 ---------------------------------------------------------------------*/
unsigned long hashFunction1 (const unsigned char *aWord, uint len, unsigned long sizeHash)
{
    register unsigned char c;
    register uint h, i;
    h = SEED;

    for( i=0; i<len; i++ )
    {
    	c=aWord[i];
		h ^= ( (h << 5) + c + (h >> 2) );
    }
    return((unsigned int)((h&0x7fffffff) % sizeHash));
}

// or even

unsigned long hashFunction2 (const unsigned char *aWord, uint len, uint sizeHash)
{
    register unsigned char c;
    register uint h, i;
    h = SEED;

    for( i=0; i<len; i++ )
    {
    	c=aWord[i];
		h ^= ( (h << 5) + c + (h >> 2) );
    }
    return((unsigned int)((h&0x7fffffff) % sizeHash));
}



/*-----------------------------------------------------------------------
  compares two strings
 ---------------------------------------------------------------------*/

/* Author J. Zobel, April 2001.
	http://www.seg.rmit.edu.au/code/zwh-ipl/
   Permission to use this code is freely granted, provided that this
   statement is retained. */



 int strcomp(const unsigned char *s1, const unsigned char *s2, register unsigned long ws1, unsigned long ws2) {	 
	 if (ws1 !=ws2) {
	 		return -1;
	 }
	 
	 {  register unsigned long iters;	    
	 	 iters=1;
	    while( iters<ws1 && *s1 == *s2 )
	    {
			s1++;
			s2++;
			iters++;
	    }
	    //fprintf(stderr,"\nDevuelve [%d]",*s1-*s2);
	    return( *s1-*s2 );
	 }
}

inline int strcomp3(const unsigned char *s1, const unsigned char *s2, unsigned long ws1, unsigned long ws2) {
	 
	 if (ws1 !=ws2) {
	 		return -1;
	 }
	 
	 {  register unsigned long iters;
	    register unsigned long end;
	    end = MIN(ws1,ws2);
	 	 iters=1;
	    while( iters<end && *s1 == *s2 )
	    {
			s1++;
			s2++;
			iters++;
	    }
	    //fprintf(stderr,"\nDevuelve [%d]",*s1-*s2);
	    return( *s1-*s2 );
	 }
}


/* Author J. Zobel, April 2001.
	http://www.seg.rmit.edu.au/code/zwh-ipl/
   Permission to use this code is freely granted, provided that this
   statement is retained. */

//int strcomp(const unsigned char *s1, const unsigned char *s2, unsigned long ws) {
//    register unsigned long i;
//    i=0;
//    while( i < ws-1 && *s1 == *s2 )*/
//    {
//			s1++;
//			s2++;
//			i++;
//    }
//    return( *s1-*s2 );
//}

//int strcompL(const byte *s1, const byte *s2, register ulong ws1, register ulong ws2) {
//	register ulong iters = 1;	    
//
//	while( iters<ws1 && *s1 == *s2 ){
//		s1++;
//		s2++;
//		iters++;
//	}
//	  // so     iters == ws1   OR   *s1 != *s2
//	if (ws1 == iters) {
//			if (ws2 == ws1)
//				return 0;  // s1 equals to s2
//			else 
//				return -1;   // s1 < s2
//	}
//	else { //(ws1 > iters) so s1 != s2
//		return( *s1-*s2);
//	}	 
//}


//permits to compare 2 strings of len ws1 and ws2 that do not end in '\0'
int strcompL(const byte *s1, const byte *s2, register ulong ws1, register ulong ws2) {
	register ulong iters = 0;	    

	while( iters<ws1 && iters<ws2 && *s1 == *s2 ){
		s1++;
		s2++;
		iters++;
	}

	if (ws1 == iters) {
			if (ws2 == ws1)
				return 0;  // s1 equals to s2
			else 
				return -1;   // w1 < w2  and  *s1_i == *s2_i for i=0 to iters-1
	}
	else 
	if (ws2 == iters) {
			if (ws2 == ws1)
				return 0;  // s1 equals to s2
			else 
				return +1;   // w2 < w1  and  *ws1 is '\0'
	}
	else { //*s1 != *s2
		return (*s1-*s2);
	}	
	
}
	

/*-----------------------------------------------------------------------
 Insert a new word in a position of the hashTable (position previously computed)
 ---------------------------------------------------------------------*/
void insertElement (t_hash h, const unsigned char *aWord, register unsigned long len, register unsigned long *addr) {
    //fprintf(stderr,"\n Entra en la funcin [%s], [%ld]",aWord, len);

	if(h->NumElem >= h->SIZE_HASH -1)   //loses 1 slot, but ensures that "search function" does not enter an infinity loop
	{
		printf("\n\nHash table full!! Change size and recompile !\n");
		exit(1);
	}

    getMemoryBlock(h->_memMgr,( byte **)&(h->hash[*addr].word),len+1);
    //fprintf(stderr,"\n tras obter memoria");

	strncpy ((char *) h->hash[*addr].word, (char *)aWord, len);

	h->hash[*addr].word[len]='\0';
	h->hash[*addr].len =len;
	h->hash[*addr].posInVoc = h->NumElem;
	h->NumElem++;

	//fprintf(stderr,"\n####inserted word [%s] ",h->hash[*addr].word);
	
	//return *addr;
}

/*-----------------------------------------------------------------------
 Search for a word in the hash table and returns its position in the
 vocabulary. It returns the next "available" position in the vocabulary,
 if the word is not in the hash table. That is: a "0-node" position.
 It also returns -using attribute returnedAddr- the position where word
 was found (or where it should go if it was inserted in next "insert call".
 -----------------------------------------------------------------------*/
unsigned long search (t_hash h, const unsigned char *aWord, register unsigned len,
								 unsigned long *returnedAddr){

	register unsigned long addr, Saddr;
	
 	//fprintf(stderr,"\n searching for  [%s], [%d], sizehash= %ld",aWord,len,h->SIZE_HASH);
	addr = hashFunction(aWord,len, h->SIZE_HASH);
	Saddr = addr;

	t_hashNode *hash;
	hash = h->hash;
	
	while((hash[addr].word  != NULL)&&((strcomp(hash[addr].word, aWord,  hash[addr].len, len)) != 0))  {	
		//fprintf(stderr,"\nComprueba [%s], [%d]",hash[addr].word,strcomp(hash[addr].word, aWord, len));
		addr = ((addr + JUMP) % h->SIZE_HASH);
	}

	*returnedAddr = addr;
	
	if(hash[addr].word  == NULL) {
		return h->NumElem;				//Word was not found
	}
	else {
		return h->hash[addr].posInVoc; //Word was found in this position in the vocabulary
	}
}



/*-----------------------------------------------------------------------
  Tells if a word appears or not in the hash table.
 -----------------------------------------------------------------------*/
unsigned long inHashTable (t_hash h, const unsigned char *aWord, register unsigned len, unsigned long *returnedAddr){
	
	unsigned long searched;
	unsigned long nothing;
	searched = search(h,aWord,len,&nothing);
	*returnedAddr=nothing;
	return (searched < (h->NumElem) );
}

/*------------------------------------------------------------------
 Destructor method
 ------------------------------------------------------------------ */
void destroy_hash (t_hash hash){
	unsigned long mem=0;
	mem += sizeof(struct hashStr) + hash->SIZE_HASH * sizeof(t_hashNode);	
	free(hash->hash);
	destroyMemoryManager(hash->_memMgr); //frees words and variants 
//	free(hash->_memMgr);
	free(hash);	
	printf("\n[destroying hash table]...Freed %ld bytes... RAM", mem);	
}




/*------------------------------------------------------------------
 main, to make unit proofs
 ------------------------------------------------------------------ */
/*
int main(int argc, char* argv[])
{   byte a[10]= "word1";
	byte b[10]= "word2";
	byte c[10]= "word3";
	byte d[10]= "word4";
	byte e[10]= "word5";
	byte f[10]= "word6";
	byte * w;
	unsigned int size;
	unsigned long i,addrInTH;
	
	t_hash hash;
	
	_memMgr=createMemoryManager();

	hash = initialize_hash (2);
	
	w=a;
	i = search (hash,w, strlen(w), &addrInTH );
	insertElement (hash, w, strlen(w), &addrInTH);
	fprintf(stderr,"\n i = %ld, addrInTh = %ld ",i,addrInTH);
	fprintf(stderr,"\n word in hash[%ld]= %s , freq = %ld, posinvoc =%ld",addrInTH, hash->hash[addrInTH].word, hash->hash[addrInTH].freq, hash->hash[addrInTH].posInVoc);	

	w=b;
	i = search (hash,w, strlen(w), &addrInTH );
	insertElement (hash, w, strlen(w), &addrInTH);
	fprintf(stderr,"\n i = %ld, addrInTh = %ld ",i,addrInTH);
	fprintf(stderr,"\n word in hash[%ld]= %s , freq = %ld, posinvoc =%ld",addrInTH, hash->hash[addrInTH].word, hash->hash[addrInTH].freq, hash->hash[addrInTH].posInVoc);	

	w=c;
	i = search (hash,w, strlen(w), &addrInTH );
	insertElement (hash, w, strlen(w), &addrInTH);
	fprintf(stderr,"\n i = %ld, addrInTh = %ld ",i,addrInTH);
	fprintf(stderr,"\n word in hash[%ld]= %s , freq = %ld, posinvoc =%ld",addrInTH, hash->hash[addrInTH].word, hash->hash[addrInTH].freq, hash->hash[addrInTH].posInVoc);	

	w=d;
	i = search (hash,w, strlen(w), &addrInTH );
	insertElement (hash, w, strlen(w), &addrInTH);
	fprintf(stderr,"\n i = %ld, addrInTh = %ld ",i,addrInTH);
	fprintf(stderr,"\n word in hash[%ld]= %s , freq = %ld, posinvoc =%ld",addrInTH, hash->hash[addrInTH].word, hash->hash[addrInTH].freq, hash->hash[addrInTH].posInVoc);	

//	w=e;
//	i = search (hash,w, strlen(w), &addrInTH );
//	insertElement (hash, w, strlen(w), &addrInTH);
//	fprintf(stderr,"\n i = %ld, addrInTh = %ld ",i,addrInTH);
//	fprintf(stderr,"\n word in hash[%ld]= %s , freq = %ld, posinvoc =%ld",addrInTH, hash->hash[addrInTH].word, hash->hash[addrInTH].freq, hash->hash[addrInTH].posInVoc);	

//	w=f;
//	i = search (hash,w, strlen(w), &addrInTH );
//	insertElement (hash, w, strlen(w), &addrInTH);
//	fprintf(stderr,"\n i = %ld, addrInTh = %ld ",i,addrInTH);
//	fprintf(stderr,"\n word in hash[%ld]= %s , freq = %ld, posinvoc =%ld",addrInTH, hash->hash[addrInTH].word, hash->hash[addrInTH].freq, hash->hash[addrInTH].posInVoc);	

	fprintf(stderr,"\n in: %s  hash ? = %d",a,inHashTable(hash,a,strlen(a)) );
	fprintf(stderr,"\n in: %s  hash ? = %d",e, inHashTable(hash,e,strlen(e)) );
	fprintf(stderr,"\n in: %s  hash ? = %d",b, inHashTable(hash,b,strlen(b)) );

	destroy_hash(hash);
	destroyMemoryManager(_memMgr);
	printf("\n\n");
}
*/
