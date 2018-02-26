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
 File       : MemoryManager
 Function   : Reserves large blocks of memory and gives pointers to small
              portions of that block when requested.
              This improves performance since a unique "LARGE ALLOCATION"
              of memory is needed (a unique call to malloc).
              It is also responsible of freeing memory.
 Purpose    : Improve hash performance.
 ------------------------------------------------------------------------*/
 
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <stdio.h>

#include <malloc.h>

#define byte char
#define LARGE_BLOCK_SIZE 1048576*64 	// Size of the blocks of memory that will be allocated
#define MAX_BLOCKS 8				// Maximum number of blocks of size LARGE_BLOCK_SIZE that
										// can be allocated 


	struct sMem {
		byte *BLOCKS[MAX_BLOCKS]; 		//array of blocks of size LARGE_BLOCK_SIZE
		unsigned int  currentBlock;	//currentBlock in the array of blocks
		unsigned long remainderBytes; //number of bytes not yet assigned in BLOCKS[currentBlock]
		byte *availableByte;				//pointer to next byte not yet assigned 
	} ;
	
	typedef struct sMem *MemoryManager;
	
		 
	/* Creates a new MemoryManager */
	MemoryManager createMemoryManager(void);
	/*Frees the allocated memory*/
	void destroyMemoryManager (MemoryManager mm);
	 /*Allocates a new memory block of size "LARGE_BLOCK_SIZE" and adds it to  vector BLOCKS */
	void createNewMemoryBlock (MemoryManager mm);
	/* returns a pointer "dst" to a free block of memory of size "size" */
	void getMemoryBlock (MemoryManager mm, byte **dst, const unsigned int size);

