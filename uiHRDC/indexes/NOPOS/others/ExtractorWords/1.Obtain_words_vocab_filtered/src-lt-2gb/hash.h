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

#include <string.h>
#include <stdlib.h>
#include <math.h>

#include <malloc.h>

#include "MemoryManager.h"

#define JUMP 101  		 //jump done when a collision appears
#define OCUP_HASH 2	 	 //index of occupation of the hash table
#define SMALL_PRIME 1009 // a small prime number, used to compute a hash function
#define SEED	1159241
/* Type definitions */

#define MIN(a,b) (a < b) ? a : b

struct Nword {
	  unsigned char *word;
	  unsigned long size;
	  unsigned long len;
	  unsigned long weight;
	  unsigned long codeword;
};

typedef struct Nword t_word;

// private:

	MemoryManager _memMgr; 	  /* Holds dynamic memory reserve for words. */

	unsigned long TAM_HASH;   /* # entries in the hash table    */
	unsigned long NumElem;    /* # elements already added to the hash table*/

	unsigned long initialize_hash (unsigned long tamArq);
	unsigned long NearestPrime (unsigned int n);
	unsigned long hashFunction (const unsigned char *aWord, unsigned int len);

// public:
	t_word  *hash;     		  /* holds a hashTable of words  */
	
	unsigned long initialize_hash  (unsigned long tamArq);
	unsigned long insertElement (const unsigned char *aWord, register unsigned long len,
										register unsigned long *addr);
	unsigned long search (const unsigned char *aWord, register unsigned len,
									unsigned long *returnedAddr);


