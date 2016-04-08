
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
 
#include "hashWords.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>




/*-----------------------------------------------------------------
 Initilization of data structures used by the hashTable
 ---------------------------------------------------------------- */
Wt_hash wtinitialize_hash (unsigned long numberOfwords) {
	Wt_hash h;
	unsigned long i;
	//unsigned long maxQgramsLenSizeBytes = numberOfQgrams * (qgramLen+1);
	
	h = (Wt_hash) malloc(sizeof(struct WthashStr));
	h->SIZE_HASH = (ulong) (W_OCUP_HASH * numberOfwords);
	h->SIZE_HASH = wtNearestPrime(h->SIZE_HASH);
	h->hash = (Wt_hashNode *) malloc(h->SIZE_HASH * sizeof(Wt_hashNode));
	h->NumElem = 0;
	h->zone_size = 0;
	
	//h->zone.size = maxQgramsLenSizeBytes;
	//h->zone.zone = (byte *) malloc (sizeof(byte)* maxQgramsLenSizeBytes);
	//h->zone.nextFree = h->zone.zone;
	//h->qgramlen = qgramLen; 
	
	for (i = 0; i < h->SIZE_HASH; i++)    {
	 	h->hash[i].word = NULL;
	 	h->hash[i].len = 0;
	 	h->hash[i].freq = 0;
	}
	printf("\nHash Table initilized with: %lu elements\n",h->SIZE_HASH);
	return h;	
}


//copies the words from the htq to "words" and sets nwords
void wthash2array_hash (Wt_hash h, unsigned char ***words, unsigned long *nwords){
	unsigned long i,n,curr;
	unsigned char **ws;
	
	n = h->NumElem;
	ws = (unsigned char **) malloc (sizeof(unsigned char *) * n);

	curr = 0;
	for (i=0;i<h->SIZE_HASH; i++) {
			if (h->hash[i].word != NULL)
				ws[curr++] = h->hash[i].word;
	}
	
	*words = ws;
	*nwords = n;
}	


unsigned long wtmemused (Wt_hash hash) {
	unsigned long mem;
	mem= sizeof(struct WthashStr);
	//fprintf(stderr,"\nmem hash: %9d",mem); 
	mem+= hash->SIZE_HASH * sizeof(Wt_hashNode);
	//fprintf(stderr,"\nmem hash: %9d",mem); 
	mem+= hash->zone_size;	
	//fprintf(stderr,"\nmem hash: %9d",mem); 
	return mem;
}


/*------------------------------------------------------------------
 Find the nearest prime number over n.
 ---------------------------------------------------------------- */
unsigned long wtNearestPrime(unsigned long n)
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
unsigned long wthashFunction (const unsigned char *aWord, unsigned int len, unsigned long sizeHash)
{
    char	c;
    register unsigned int h;
    register unsigned long last;
    last=((unsigned long) aWord) +len -1;


    h = W_SEED;
    //last=aWord+len;

    for( ; ((unsigned long) aWord <=last ) ; )
    //for(c=*aWord; (aWord++)<last ;  )
    {
    	c=*(aWord++);
		//c=*aWord;
		h ^= ( (h << 5) + c + (h >> 2) );
    }
    return((unsigned int)((h&0x7fffffff) % sizeHash));
}


/*-----------------------------------------------------------------------
  compares two strings. Zobel's hash function -> modified
 ---------------------------------------------------------------------*/

/* Author J. Zobel, April 2001.
	http://www.seg.rmit.edu.au/code/zwh-ipl/
   Permission to use this code is freely granted, provided that this
   statement is retained. */

inline int wtstrcomp(const unsigned char *s1, const unsigned char *s2, register unsigned long ws1, unsigned long ws2) {
	 
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

//permits to compare 2 strings of len ws1 and ws2 that do not end in '\0'
int wtstrcompL(const byte *s1, const byte *s2, register ulong ws1, register ulong ws2) {
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
void wtinsertElement (Wt_hash h, const unsigned char *aWord, register unsigned long len, register unsigned long *addr) {

	if(h->NumElem >= h->SIZE_HASH -1)   //loses 1 slot, but ensures that "search function" does not enter an infinity loop
	{
		printf("\n\nHash table full!! Change size (MAX_NUM_OF_DIFFERENT_Q_GRAMS) and recompile !\n");
		exit(1);
	}

    //getMemoryBlock(_memMgr,( byte **)&(h->hash[*addr].word),len+1);
    
    h->hash[*addr].word = (byte *) malloc(sizeof(byte) * (len+1));
    //h->hash[*addr].word = h->zone.nextFree;
    //h->zone.nextFree += (len+1);
    h->zone_size += len+1;

	strncpy ((char *) h->hash[*addr].word, (char *)aWord, len);

	h->hash[*addr].word[len]= '\0';
	h->hash[*addr].len =len;
	h->hash[*addr].freq = 1;
	h->NumElem++;
	
}

/*-----------------------------------------------------------------------
 Search for a word in the hash table and returns its position in the
 vocabulary. It returns the next "available" position in the vocabulary,
 if the word is not in the hash table. That is: a "0-node" position.
 It also returns -using attribute returnedAddr- the position where word
 was found (or where it should go if it was inserted in next "insert call".
 -----------------------------------------------------------------------*/
unsigned long wtsearch (Wt_hash h, const unsigned char *aWord, register unsigned long len,
								 unsigned long *returnedAddr){

	register unsigned long addr, Saddr;
	
 	//fprintf(stderr,"\n searching for  [%s], [%d], sizehash= %ld",aWord,len,h->SIZE_HASH);
	addr = wthashFunction(aWord,(uint) len, h->SIZE_HASH);
	Saddr = addr;

	Wt_hashNode *hash;
	hash = h->hash;
	
	while((hash[addr].word  != NULL)&&((wtstrcomp(hash[addr].word, aWord,  hash[addr].len, len)) != 0))  {	
		//fprintf(stderr,"\nComprueba [%s], [%d]",hash[addr].word,strcomp(hash[addr].word, aWord, len));
		addr = ((addr + W_JUMP) % h->SIZE_HASH);
	}

	*returnedAddr = addr;
	
	if(hash[addr].word  == NULL) {
		return 0;				//Word was not found
	}
	else {
		return 1; //Word was found 
	}
}



/*-----------------------------------------------------------------------
  Tells if a word appears or not in the hash table.
 -----------------------------------------------------------------------*/
unsigned long wtinHashTable (Wt_hash h, const unsigned char *aWord, register unsigned long len, unsigned long *returnedAddr){
	
	unsigned long searched;
	unsigned long nothing;
	searched = wtsearch(h,aWord,len,&nothing);
	*returnedAddr=nothing;
	return (searched);
}

/*------------------------------------------------------------------
 Destructor method
 ------------------------------------------------------------------ */
void wtdestroy_hash (Wt_hash hash){
	unsigned long mem=0;
	mem += sizeof(struct WthashStr) + hash->SIZE_HASH * sizeof(Wt_hashNode);	
	//free(hash->zone.zone);

	unsigned long i;
	for (i=0;i<hash->SIZE_HASH; i++) {
			if (hash->hash[i].word != NULL)
				free(hash->hash[i].word);
	}
	free(hash->hash);
		
	mem +=hash->zone_size;
	free(hash);	
	//printf("\n[destroying hash table]...Freed %ld bytes... RAM", mem);	
}
