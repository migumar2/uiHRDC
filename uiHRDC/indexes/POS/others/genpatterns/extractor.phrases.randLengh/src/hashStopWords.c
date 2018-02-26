
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
 
#include "hashStopWords.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>

/*
uint fileSize(char *filename) {
	struct stat st;
    if (stat (filename,&st) != 0) { 
    	fprintf (stderr,"Error: cannot stat file %s\n",filename);
		fprintf (stderr," errno = %i\n",errno);
	 	exit(1);
	}
    return st.st_size;	
}
*/
/*------------------------------------------------------------------
 Loads the stopwords into a hash table.
 ----------------------------------------------------------------- */
void loadStopWords(stt_hash *hash, char *stopwordsfile) {
	unsigned long stopWordsFileSize;
	int f_in;
	
	byte aStopword[MAX_SIZE_OF_WORD+1];
	unsigned int aStopwordSize;
	byte c;
	uint numberOfStopwords;
	
	unsigned int counter=0;	
	unsigned long addrInTH,i;	
	stt_hash stopwordshash;
		
	stopWordsFileSize = fileSize(stopwordsfile);

	// Loads the list of stopwords into memory (they are used to filter the text
	if( (f_in = open(stopwordsfile, O_RDONLY)) < 0) {
		printf("Cannot read file %s\n", stopwordsfile);
		exit(0);
	}	
	
	//reads the number of stopwords in the file (the first line)
	aStopwordSize = 0;
	while(read(f_in, &c, sizeof(char)))  {
		if(c == 13) continue;
		if(c == '\n') {
			aStopword[aStopwordSize] = '\0';	
			break;
		} else {
			aStopword[aStopwordSize] = c;
			aStopwordSize++;	
		}						
	}
	numberOfStopwords = atoi((char * )aStopword);

	stopwordshash = stinitialize_hash(numberOfStopwords, stopWordsFileSize); //creates the hash table
	
	//reads each stopword
	aStopwordSize = 0;
	while(read(f_in, &c, sizeof(char))) {		
		if(c == 13) continue;
		if(c == '\n') {
			aStopword[aStopwordSize] = '\0';
			i = stsearch (stopwordshash,aStopword, aStopwordSize, &addrInTH );
			stinsertElement (stopwordshash, aStopword, aStopwordSize, &addrInTH);
			//fprintf(stderr,"\n stopWord[%d] = <<%s>> ",counter,stopwordshash->hash[addrInTH].word);	
			counter++;
			aStopwordSize = 0;
		} else {
			aStopword[aStopwordSize] = c;
			aStopwordSize++;	
		}	
	}		
	close(f_in);	
	printf("\n\tLoaded %u stopwords. Hash table has %u elements and  %u slots.",numberOfStopwords,stopwordshash->NumElem, stopwordshash->SIZE_HASH);
	*hash = stopwordshash ;		
}


/*-----------------------------------------------------------------
 Initilization of data structures used by the hashTable
 ---------------------------------------------------------------- */

stt_hash stinitialize_hash (unsigned long numberOfStopwords, unsigned long stopWordsFileSize) {
	stt_hash h;
	unsigned long i;
	
	h = (stt_hash) malloc(sizeof(struct sthashStr));
	h->SIZE_HASH = (ulong) (ST_OCUP_HASH * numberOfStopwords);
	h->SIZE_HASH = stNearestPrime(h->SIZE_HASH);
	h->hash = (stt_hashNode *) malloc(h->SIZE_HASH * sizeof(stt_hashNode));
	h->NumElem = 0;
	h->zone.size = stopWordsFileSize;
	h->zone.zone = (byte *) malloc (sizeof(byte)* stopWordsFileSize);
	h->zone.nextFree = h->zone.zone; 

	for (i = 0; i < h->SIZE_HASH; i++)    {
	 	h->hash[i].word = NULL;
	 	h->hash[i].len = 0;
	}
	//printf("\nHash Table initilized with: %lu elements\n",h->SIZE_HASH);

	return h;	
}

unsigned long stmemused (stt_hash hash) {
	unsigned long mem;
	mem= sizeof(struct sthashStr);
	mem+= hash->SIZE_HASH * sizeof(stt_hashNode);
	mem+= hash->zone.size;
	return mem;
}


/*------------------------------------------------------------------
 Find the nearest prime number over n.
 ---------------------------------------------------------------- */
unsigned long stNearestPrime(unsigned long n)
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
unsigned long sthashFunction (const unsigned char *aWord, unsigned int len, unsigned long sizeHash)
{
    char	c;
    register unsigned int h;
    register unsigned long last;
    last=((unsigned long) aWord) +len -1;


    h = ST_SEED;
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

inline int ststrcomp(const unsigned char *s1, const unsigned char *s2, register unsigned long ws1, unsigned long ws2) {
	 
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


/*-----------------------------------------------------------------------
 Insert a new word in a position of the hashTable (position previously computed)
 ---------------------------------------------------------------------*/
void stinsertElement (stt_hash h, const unsigned char *aWord, register unsigned long len, register unsigned long *addr) {

	if(h->NumElem >= h->SIZE_HASH -1)   //loses 1 slot, but ensures that "search function" does not enter an infinity loop
	{
		printf("\n\nHash table full!! Change size and recompile !\n");
		exit(1);
	}

    //getMemoryBlock(_memMgr,( byte **)&(h->hash[*addr].word),len+1);
    
    h->hash[*addr].word = h->zone.nextFree;
    h->zone.nextFree += (len+1);

	strncpy ((char *) h->hash[*addr].word, (char *)aWord, len);

	h->hash[*addr].word[len]='\0';
	h->hash[*addr].len =len;
	h->NumElem++;
	
}

/*-----------------------------------------------------------------------
 Search for a word in the hash table and returns its position in the
 vocabulary. It returns the next "available" position in the vocabulary,
 if the word is not in the hash table. That is: a "0-node" position.
 It also returns -using attribute returnedAddr- the position where word
 was found (or where it should go if it was inserted in next "insert call".
 -----------------------------------------------------------------------*/
unsigned long stsearch (stt_hash h, const unsigned char *aWord, register unsigned len,
								 unsigned long *returnedAddr){

	register unsigned long addr, Saddr;
	
 	//fprintf(stderr,"\n searching for  [%s], [%d], sizehash= %ld",aWord,len,h->SIZE_HASH);
	addr = sthashFunction(aWord,len, h->SIZE_HASH);
	Saddr = addr;

	stt_hashNode *hash;
	hash = h->hash;
	
	while((hash[addr].word  != NULL)&&((ststrcomp(hash[addr].word, aWord,  hash[addr].len, len)) != 0))  {	
		//fprintf(stderr,"\nComprueba [%s], [%d]",hash[addr].word,strcomp(hash[addr].word, aWord, len));
		addr = ((addr + ST_JUMP) % h->SIZE_HASH);
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
unsigned long stinHashTable (stt_hash h, const unsigned char *aWord, register unsigned len, unsigned long *returnedAddr){
	
	unsigned long searched;
	unsigned long nothing;
	searched = stsearch(h,aWord,len,&nothing);
	*returnedAddr=nothing;
	return (searched);
}

/*------------------------------------------------------------------
 Destructor method
 ------------------------------------------------------------------ */
void stdestroy_hash (stt_hash hash){
	unsigned long mem=0;
	mem += sizeof(struct sthashStr) + hash->SIZE_HASH * sizeof(stt_hashNode);	
	free(hash->hash);
	free(hash->zone.zone);
	free(hash);	
	printf("\n\t[destroying hash table]...Freed %ld bytes... RAM", mem);	
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
