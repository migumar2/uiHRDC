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
 Hash.h: Definition of a HashTable that uses linear hash
 ------------------------------------------------------------------------*/
//Needed for TH.

#ifndef HASHINCLUDED
#define HASHINCLUDED
	
	
	#include <string.h>
	#include <stdlib.h>
	#include <math.h>
	#include <stdio.h>
	#include <malloc.h>
	
	#include "MemoryManagerPIZZA.h"
	#include "BufferedReaderPIZZA.h"


	#define DESLOC(c) (c>> 8)
	#define MULT(c) ((c<<3))
	
	/* Structure of Huffman tree */
	struct NodosEnNiveles {
	   unsigned int maxNivel;
	   unsigned int minNivel;
	   unsigned long  *first;
	   unsigned long  *base;
	
	} ;
	typedef struct NodosEnNiveles t_NodosEnNiveles;
	
	
	//
	




	
	#define JUMP 101  		 //jump done when a collision appears
	#define OCUP_HASH 2	 	 //index of occupation of the hash table
	#define SMALL_PRIME 1009 // a small prime number, used to compute a hash function
	#define SEED	1159241
	/* Type definitions */
	
	#define MIN(a,b) (a < b) ? a : b
	
	struct NwordPIZZA {
		  unsigned char *word;
		  unsigned long size;
		  unsigned long len;
		  unsigned long weight;
		  unsigned long code;
		  byte codeword[4];
	};
	
	typedef struct NwordPIZZA t_wordPIZZA;
	
	// variables and functions
	
	
		unsigned long NearestPrimePIZZA(unsigned long n);
		unsigned long hashFunctionPIZZA (const unsigned char *aWord, unsigned int len);
	
	// public:
		
		unsigned long initialize_hashPIZZA  (unsigned long tamArq);
		void freeHashTablePIZZA();
		unsigned long insertElementPIZZA (const unsigned char *aWord, register unsigned long len,
											register unsigned long *addr);
		unsigned long searchPIZZA (const unsigned char *aWord, register unsigned len,
										unsigned long *returnedAddr);
	
		void loadCompressorHashTablePIZZA(char * fileName);
	
	


#endif
