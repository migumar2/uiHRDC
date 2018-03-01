/* Words extractor -- 

Parses a text file and computes the different words in it.
Then, it applies a filter by frequency of word and/or the lenght
of the words, in such a way that only the "filtered words" are
output to the target file.

This target file is useful since it contains a set of patterns that
can be later searched inside the input text file. 

Copyright (C) 2005 Antonio Fariña.

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

Author's contact: Antonio Fariña, Dept. of Computer Science, University of
A Coruña. Campus de Elviña s/n. Spain  fari@udc.es
*/
   

/*-----------------------------------------------------------------------
 Hash: Definition of HashTable class (Linear Hash)
 ------------------------------------------------------------------------*/
 
/* 
   Thanks also to Justin Zobel for his fast hash functions
*/

#include "hash.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#define min(a, b) ((a) < (b)? (a): (b))

/*-----------------------------------------------------------------
 Initilization of data structures used by the hashTable
 ---------------------------------------------------------------- */
unsigned long initialize_hash (unsigned long sizeVoc) {

	unsigned long i;

	NumElem = 0;

	TAM_HASH = OCUP_HASH * sizeVoc;
	TAM_HASH = NearestPrime(TAM_HASH);
	hash = (t_word *) malloc(TAM_HASH * sizeof(t_word));

	for(i = 0; i < TAM_HASH; i++)    {
		hash[i].word = NULL;
		hash[i].weight = 0;
		hash[i].size = 0;
		hash[i].codeword = 0;
		hash[i].len=0;
	}
	fprintf(stderr,"\nHash Table initilized with: %lu elements",TAM_HASH);

	return TAM_HASH;
}


/*------------------------------------------------------------------
 Find the nearest prime number over n. --> Thanks to Edleno Silva de Moura
 ---------------------------------------------------------------- */
unsigned long int NearestPrime(unsigned int n)
{
    int p;  /* the prime number being sought */
    int i;  

    for (p = n; ; p++)
    {
        // checks if those values from 2 to $\sqrt{m}$ can be factors of $m$ */

        for (i = 2; i <= (int) sqrt((double) p) && p % i != 0; i++) ;

        if (p % i != 0)  /* No factors in that range, therefore a prime number was found */
        {
            break;
        }
    }

    return p;
} 


/*------------------------------------------------------------------*
//   http://goanna.cs.rmit.edu.au/~hugh/software/zwh-ipl/
//	/* Author J. Zobel, April 2001.
//	   Permission to use this code is freely granted, provided that this
//	   statement is retained. */
//
//	/* Bitwise hash function.  Note that tsize does not have to be prime. */
//	unsigned int bitwisehash(char *word, int tsize, unsigned int seed)
//	{
//	    char	c;
//	    unsigned int h;
//
//	    h = seed;
//	    for( ; ( c=*word )!='\0' ; word++ )
//	    {
//		h ^= ( (h << 5) + c + (h >> 2) );
//	    }
//	    return((unsigned int)((h&0x7fffffff) % tsize));
//	}
/*---------------------------------------------------------------- */

/*------------------------------------------------------------------
 Modification of Zobel's bitwise function to have into account the 
 lenght of the key explicetely 
 ---------------------------------------------------------------- */
unsigned long hashFunction (const unsigned char *aWord, unsigned int len)
{
    char c;
    register unsigned int h;
    register unsigned long last;
    last=((unsigned long) aWord) +len -1;

    h = SEED;
   
    for( ; ((unsigned long) aWord <=last ) ; )
    {
    	c=*(aWord++);
		//c=*aWord;
		h ^= ( (h << 5) + c + (h >> 2) );
    }
    return((unsigned int)((h&0x7fffffff) % TAM_HASH));
}


/*-----------------------------------------------------------------------
  compares two strings
 ---------------------------------------------------------------------*/

 /*------------------------------------------------------------------*
//   http://goanna.cs.rmit.edu.au/~hugh/software/zwh-ipl/
//	/* Author J. Zobel, April 2001.
//	   Permission to use this code is freely granted, provided that this
//	   statement is retained. */
//
//	/* String compare function. */
//	int scmp( char *s1, char *s2 )
//	{
//	    while( *s1 != '\0' && *s1 == *s2 )
//	    {
//		s1++;
//		s2++;
//	    }
//	    return( *s1-*s2 );
//	}
/*---------------------------------------------------------------- */


/*------------------------------------------------------------------
 Modification of Zobel's scmp function compare two strings
 ---------------------------------------------------------------- */
inline int strcomp(const unsigned char *s1, const unsigned char *s2, register unsigned long ws1, unsigned long ws2) {
	 
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
		
	    return( *s1-*s2 );
	 }
}

/*------------------------------------------------------------------
 Modification of Zobel's scmp function compare two strings
 ---------------------------------------------------------------- */
/* inline int strcomp_vOLD(const unsigned char *s1, const unsigned char *s2, unsigned long ws1, unsigned long ws2) {
	 
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
	    return( *s1-*s2 );
	 }
} */


/*-----------------------------------------------------------------------
 Inserts a new word in a given position of the hashTable (position previously computed)
 ---------------------------------------------------------------------*/
 unsigned long insertElement (const unsigned char *aWord, register unsigned long len,
                                         register unsigned long *addr) {
	
	if(*addr == TAM_HASH)
	{
		printf("Not enough memory, vocabulary exceeds maximun size !\n");
		exit(1);
	}

    getMemoryBlock(_memMgr,( char **)&(hash[*addr].word),len+1);
	strncpy ((char *) hash[*addr].word, (char *)aWord, len);

	hash[*addr].word[len]='\0';
	hash[*addr].weight=0;
	NumElem++;

	return *addr;
}

/*-----------------------------------------------------------------------
 Searches for a word in the hash table and returns its position in the
 vocabulary. It returns the next "available" position in the vocabulary,
 if the word is not in the hash table. That is: a "0-node" position.
 It also returns -using attribute returnedAddr- the position where word
 was found (or where it should go if it was inserted in next "insert call".
 -----------------------------------------------------------------------*/
unsigned long search (const unsigned char *aWord, register unsigned len,
								 unsigned long *returnedAddr){

	register unsigned long addr, Saddr;

	addr = hashFunction(aWord,len);

	Saddr = addr;

	while((hash[addr].word  != NULL)&&((strcomp(hash[addr].word, aWord,  hash[addr].len, len)) != 0))  {
		addr = ((addr + JUMP) %TAM_HASH);
	}

	*returnedAddr = addr;  // position returned

	if(hash[addr].word  == NULL) {
		return NumElem;	//Word was not found
	}
	
	return 0; 			//Word was found	
}

