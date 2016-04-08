/* Multipattern random searches using set Horspool's algorithm adapted to 
search inside a text compressed with End-Tagged Dense Code.
Lightweight Natural Language Text Compression: Information Retrieval 2006
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
 Hash.c: Definition of a HashTable that uses linear hash
 ------------------------------------------------------------------------*/

#include "hashPIZZA.h"
	
    		  /* holds a hashTable of words  */

	tMemoryManagerPIZZA _memMgr; 	  /* Holds dynamic memory allocation for words. */
	
	unsigned long TAM_HASH;   /* # entries in the hash table    */
	unsigned long NumElem;    /* # elements already added to the hash table*/
	t_wordPIZZA  *hash; 





/*-----------------------------------------------------------------
 Initilization of data structures used by the hashTable
 ---------------------------------------------------------------- */
unsigned long initialize_hashPIZZA(unsigned long sizeVoc) {

	unsigned long i;

	fprintf(stderr,"\nHash Table need space at least for: %lu elements",sizeVoc);
	NumElem = 0;
	TAM_HASH = OCUP_HASH * sizeVoc;
	TAM_HASH = NearestPrimePIZZA(TAM_HASH);
	hash = (t_wordPIZZA *) malloc(TAM_HASH * sizeof(t_wordPIZZA));

	for(i = 0; i < TAM_HASH; i++)    {
		hash[i].word = NULL;
		hash[i].weight = 0;
		hash[i].size = 0;
		//hash[i].codeword = 0;
		hash[i].len=0;
	}
	fprintf(stderr,"\nHash Table initilized with: %lu elements",TAM_HASH);

	_memMgr = createMemoryManagerPIZZA();
	return TAM_HASH;
}

/*-----------------------------------------------------------------
 Frees the memory allocated by the hash table 
-----------------------------------------------------------------*/
void freeHashTablePIZZA() {
	destroyMemoryManagerPIZZA(_memMgr);
	free(hash);
}

/*------------------------------------------------------------------
 Find the nearest prime number over n. 
 ---------------------------------------------------------------- */
unsigned long NearestPrimePIZZA(unsigned long n)
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

/*------------------------------------------------------------------*
//   http://goanna.cs.rmit.edu.au/~hugh/software/zwh-ipl/
//	 Author J. Zobel, April 2001.
//	   Permission to use this code is freely granted, provided that this
//	   statement is retained. */
//
//	 Bitwise hash function.  Note that tsize does not have to be prime. */
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
unsigned long hashFunctionPIZZA(const unsigned char *aWord, unsigned int len)
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
//	 Author J. Zobel, April 2001.
//	   Permission to use this code is freely granted, provided that this
//	   statement is retained.
//
//	 String compare function. 
//	int scmp( char *s1, char *s2 )
//	{
//	    while( *s1 != '\0' && *s1 == *s2 )
//	    {
//		s1++;
//		s2++;
//	    }
//	    return( *s1-*s2 );
//	} */


/*------------------------------------------------------------------
 Modification of Zobel's scmp function compare two strings
 ---------------------------------------------------------------- */
inline int strcompPIZZA(const unsigned char *s1, const unsigned char *s2, register unsigned long ws1, unsigned long ws2) {
	 
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
 unsigned long insertElementPIZZA (const unsigned char *aWord, register unsigned long len,
                                         register unsigned long *addr) {
	
	if(*addr == TAM_HASH)
	{
		printf("Not enough memory, vocabulary exceeds maximun size !\n");
		exit(1);
	}

    getMemoryBlockPIZZA(_memMgr,( byte **)&(hash[*addr].word),len+1);
   
	strncpy ((char *) hash[*addr].word, (char *)aWord, len);

	hash[*addr].word[len]='\0';
	hash[*addr].len = len;
	hash[*addr].weight=0;
	NumElem++;
	
/**///	fprintf(stderr,"\n INSERTING word %s with len %d in position %ld\n",hash[*addr].word, hash[*addr].size, *addr); fflush(stderr);

	return *addr;
}

/*-----------------------------------------------------------------------
 Searches for a word in the hash table and returns its position in the
 vocabulary. It returns the next "available" position in the vocabulary,
 if the word is not in the hash table. That is: a "0-node" position.
 It also returns -using attribute returnedAddr- the position where word
 was found (or where it should go if it was inserted in next "insert call".
 -----------------------------------------------------------------------*/
unsigned long searchPIZZA (const unsigned char *aWord, register unsigned len,
								 unsigned long *returnedAddr){

	register unsigned long addr, Saddr;

	addr = hashFunctionPIZZA(aWord,len);

	Saddr = addr;

/**///	fprintf(stderr,"\n SEARCHING FOR [%s][%s] --> pos:%ld",aWord,len,hash[addr].word, addr);
	while((hash[addr].word  != NULL)&&((strcompPIZZA(hash[addr].word, aWord,  hash[addr].len, len)) != 0))  {
		addr = ((addr + JUMP) %TAM_HASH);
/**///		fprintf(stderr,"\n SEARCHING FOR [%s][%s] --> pos:%ld",aWord,hash[addr].word, addr);
	}

	*returnedAddr = addr;  // position returned
	

	if(hash[addr].word  == NULL) {
		return NumElem;	//Word was not found
	}
	
	return 0; 			//Word was found	
}





















// NEEDED FOR T.Huffman
/*-----------------------------------------------------------------------
 Loads the vocabulary used by the decompressor into a hash table. The key of the hash table 
 is the word,  and the value --> the relative position in the  ordered vocabulary.
 ------------------------------------------------------------------------*/
void loadCompressorHashTable(char * fileName) {	
	t_NodosEnNiveles tablaNodosEnNiveles;
	BufferRead V;	
	int v;
	unsigned char *text,*top, *nextbuf, *pbeg, *pend, *codeStart;
	char *voc;
	
	//the vocabulary of words read (words are read in decreasing frequency order)
	unsigned long *positionInTH;
	unsigned long numWords;
	
	voc = (char *) malloc(strlen(fileName)+4);
	strcpy(voc,fileName);
	strcat(voc,".voc");	
	
	// Inicialization of the buffers used to read the files

	v = open (voc, O_RDONLY);
	if (v == -1) {
		fprintf(stderr,"Cannot read vocabulary file %s\nSearch aborted.\n",voc);
		exit(0);
	}	

	  
	V = bufCreate(MAX_SIZE_OF_WORD+5);	  //a word being "cut"
	bufSetFile (V,v);
	
	
	/* loading the vocabulary into memory*/
	bufCurrent (V,(byte**)&text,(byte**)&top);
	pbeg = text; pend = top;


	{ //Reads value min_tree_level	
		unsigned char *c;
		c = pbeg;
		while (*pbeg !='\0') {
			pbeg++;
		}			
		tablaNodosEnNiveles.minNivel= (unsigned int) strtoul((char*)c, (char **)NULL, 10);
		pbeg++;	
		#ifdef LOGGING_ENABLED
			fprintf(stderr,"\n min nivel: %u",tablaNodosEnNiveles.minNivel);			
		#endif
	}

	{ //Reads value max_tree_level	
		unsigned char *c;
		c = pbeg;
		while (*pbeg !='\0') {
			pbeg++;
		}			
		tablaNodosEnNiveles.maxNivel= (unsigned int) strtoul((char*)c, (char **)NULL, 10);
		pbeg++;
		#ifdef LOGGING_ENABLED	
			fprintf(stderr,"\n max nivel: %u",tablaNodosEnNiveles.maxNivel);			
		#endif
	}
	
	//Allocates memory for first and base in the  "tablaNodosEnNiveles.maxNivel +1" levels of the tree.
	tablaNodosEnNiveles.first = (unsigned long *) calloc( (tablaNodosEnNiveles.maxNivel+1 ), sizeof (unsigned long));
	tablaNodosEnNiveles.base  = (unsigned long *) calloc( (tablaNodosEnNiveles.maxNivel+1 ), sizeof (unsigned long));
	
	//Reads first and base for each level of the tree.
	{ 
		register unsigned long i;
		for(i=tablaNodosEnNiveles.minNivel;i<=tablaNodosEnNiveles.maxNivel;i++){
			
			{ //Reading the value base[i]
				unsigned char *c;
				c = pbeg;
				while (*pbeg !='\0') {
					pbeg++;
				}			
				tablaNodosEnNiveles.base[i] = strtoul((char*)c, (char **)NULL, 10);
				pbeg++;	
			}
			{ //Reading the value first[i]
				unsigned char *c;
				c = pbeg;
				while (*pbeg !='\0') {
					pbeg++;
				}			
				tablaNodosEnNiveles.first[i] = strtoul((char*)c, (char **)NULL, 10);
				pbeg++;	
			}
		}	

		#ifdef LOGGING_ENABLED
			fprintf(stderr,"\n");	
			for (i=tablaNodosEnNiveles.minNivel; i<= tablaNodosEnNiveles.maxNivel; i++) {
				fprintf(stderr,"\n Base[%lu] -> %lu; first[%lu] --> %lu",i,tablaNodosEnNiveles.base[i],i,tablaNodosEnNiveles.first[i]);
			}
		#endif
	}					
	
	
	//initializes the hash_table
	initialize_hashPIZZA(tablaNodosEnNiveles.base[tablaNodosEnNiveles.minNivel] + 256- tablaNodosEnNiveles.first[tablaNodosEnNiveles.minNivel]); 
	positionInTH = (unsigned long *) malloc( (tablaNodosEnNiveles.base[tablaNodosEnNiveles.minNivel] + 256- tablaNodosEnNiveles.first[tablaNodosEnNiveles.minNivel] )* sizeof (unsigned long));
	
	
	fprintf(stderr,"\n PASO 1: HT INITIALIZATED ok\n"); fflush(stderr);

	//Reads one word at a time (j) and adds it to the vocabulary vector.
	{	unsigned long j;
		unsigned long RELPOS = 0;
		j=0;
		while(1){
		
			while ( pbeg < pend )   {
				//reads the word
				codeStart = pbeg;
				while (*pbeg !='\0') {
					pbeg++;
				}
				//adds the word to the vocabulary
				{	
					unsigned long addr;
	
					register unsigned long len;
					//unsigned char *w;
					//register int i;
			
					len = pbeg-codeStart;
					
					j = searchPIZZA ((unsigned char *)codeStart, len, &addr);			
						
					//adds the word to the hash Table
					insertElementPIZZA(codeStart, len,&addr);
					positionInTH[RELPOS]=addr;

					RELPOS ++;
				}
							
				j++;
				pbeg++;
				
			}
			
		nextbuf=pbeg;
		numWords = RELPOS;
		
		bufLoad (V, (unsigned char*)nextbuf);
		if (bufEmpty(V)) break;
		bufCurrent (V,(byte**)&text,(byte**)&top);
		pbeg = text; pend = top;
		}		
	}

	#ifdef LOGGING_ENABLED
		fprintf(stderr,"\n Number of words read: %ld",numWords);	
	#endif
	
	fprintf(stderr,"\n Loading vocabulary words into the hash table and building T.H. codes");			
	
	/* we need to apply the encoding process to set the codewords for each word */	
	{
		tablaNodosEnNiveles.base[tablaNodosEnNiveles.minNivel-1]= numWords;
		unsigned long x;
		unsigned long kk;
		unsigned long codigo;
		unsigned long nivel_atual;
		unsigned long wordsInlevel;
		
		codigo=0;
		x=0;
		nivel_atual = tablaNodosEnNiveles.maxNivel-1;
		while ( x < numWords ) {
			//codigo = tablaNodosEnNiveles.first[nivel_atual];
			wordsInlevel= tablaNodosEnNiveles.base[nivel_atual-1] - tablaNodosEnNiveles.base[nivel_atual];

			#ifdef LOGGING_ENABLED
				fprintf(stderr,"\n words in level [%ld ] = %ld", nivel_atual, wordsInlevel);
				fprintf(stderr,"\n x = %ld", x);
				fprintf(stderr,"\n nivel = %ld", nivel_atual);
			#endif

			for (kk=0;kk<(wordsInlevel); kk++) {
				hash[positionInTH[x]].size= nivel_atual;
				hash[positionInTH[x]].code  = (codigo<<(32-MULT(nivel_atual))  );
	            hash[positionInTH[x]].code = hash[positionInTH[x]].code | (0x80000000); //( (0x00000080) << ((nivel_atual -1)*8) );				
				//getBytes(hash[positionInTH[x]].codeword, hash[positionInTH[x]].code, nivel_atual);
				
				{   uint xxx=0;
					unsigned char *c;
					c= (unsigned char *) &(hash[positionInTH[x]].code);

					//save de codeword into the hash table.
					for (xxx=0; xxx<nivel_atual;xxx++) {
						hash[positionInTH[x]].codeword[xxx]= c[3-xxx];
					}

					//prints to stderr ...
//					if (x>760000) {
//						fprintf(stderr,"\n i=%ld codigoXX [%lu], len [%d], code",x,hash[positionInTH[x]].code, hash[positionInTH[x]].len);
//						for (xxx=0; xxx<nivel_atual;xxx++) {
//							fprintf(stderr,"[%d]",hash[positionInTH[x]].codeword[xxx]);
//						}
//					}
				}
				
				codigo++;
				if ((codigo&127)==0) codigo +=128;
				if ((codigo&32767) ==0) codigo +=32768;
				if ((codigo&8388607) ==0) codigo +=8388608;				

				x++;
				if (x==numWords) break;
			}
			
			codigo --;
			nivel_atual --;
			codigo = DESLOC(codigo);

			if (x<numWords) {
				while(tablaNodosEnNiveles.base[nivel_atual-1] == tablaNodosEnNiveles.base[nivel_atual]) {
					codigo = DESLOC(codigo);
					nivel_atual --;
					
				}			
			codigo++;
			}
	    }		
	}	
	

	/* freeing resources */
	bufDestroy(V);
	close(v);
	free(voc);	
	
		//{unsigned long x,xxx;	
		//	for (x=0;x<numWords;x++) {	
		//		if (x>760000) {
		//			fprintf(stderr,"\n i=%ld codigoXX [%lu], len [%d], code:",x,hash[positionInTH[x]].code, hash[positionInTH[x]].len);
		//			for (xxx=0; xxx<hash[positionInTH[x]].len;xxx++) {
		//				fprintf(stderr,"[%d]",hash[positionInTH[x]].codeword[xxx]);
		//			}
		//		}
		//	}			
		//}
	free(positionInTH);
	fprintf(stderr,"\n");

}
