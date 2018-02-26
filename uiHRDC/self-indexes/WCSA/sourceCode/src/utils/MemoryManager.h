/*-----------------------------------------------------------------------
 File       : MemoryManager.h
 Function   : Reserves large blocks of memory and gives pointers to small
              portions of that block when requested.
              This improves performance since a unique "LARGE ALLOCATION"
              of memory is needed (a unique call to malloc).
              It is also responsible of freeing memory.
 Last change: 10/03/2004
 Purpose    : Improve hash performance.
 ------------------------------------------------------------------------*/

#ifndef MEMORYMANAGERINCLUDED
#define  MEMORYMANAGERINCLUDED      // only used for hashTable of stopwords
	
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <stdio.h>
#include <malloc.h>

#ifndef byte
	#define byte unsigned char
#endif 

#define LARGE_BLOCK_SIZE 1024*1024*64 			// Size of the blocks of memory that will be allocated
#define MAX_BLOCKS 8						// Maximum number of blocks of size LARGE_BLOCK_SIZE that
											// can be allocated

	/*
	 * Definition of structure MemoryManager
	 */
	struct sMem {
		byte *BLOCKS[MAX_BLOCKS]; 			//array of blocks of size LARGE_BLOCK_SIZE
		unsigned int  currentBlock;			//currentBlock in the array of blocks
		unsigned long remainderBytes; 		//number of bytes not yet assigned in BLOCKS[currentBlock]
		byte *availableByte;				//pointer to next byte not yet assigned
	} ;

	typedef struct sMem *MemoryManager;
	

	MemoryManager createMemoryManager(void);
	void destroyMemoryManager (MemoryManager mm);
	void getMemoryBlock (MemoryManager mm, byte **dst, const unsigned int size);
	void createNewMemoryBlock (MemoryManager mm);

#endif
