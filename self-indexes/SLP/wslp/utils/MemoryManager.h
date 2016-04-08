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

#ifndef uchar
	#define uchar unsigned char
#endif 

#define LARGE_BLOCK_SIZE 1024*1024 			// Size of the blocks of memory that will be allocated
#define MAX_BLOCKS 2048						// Maximum number of blocks of size LARGE_BLOCK_SIZE that
											// can be allocated

	/*
	 * Definition of structure MemoryManager
	 */
	struct sMem {
		uchar *BLOCKS[MAX_BLOCKS]; 			//array of blocks of size LARGE_BLOCK_SIZE
		unsigned int  currentBlock;			//currentBlock in the array of blocks
		unsigned long remainderBytes; 		//number of bytes not yet assigned in BLOCKS[currentBlock]
		uchar *availableByte;				//pointer to next byte not yet assigned
	} ;

	typedef struct sMem *MemoryManager;
	
class MManager
{
public:
	static MemoryManager createMemoryManager(void);
	static void destroyMemoryManager (MemoryManager mm);
	static void getMemoryBlock (MemoryManager mm, uchar **dst, const unsigned int size);
	static void createNewMemoryBlock (MemoryManager mm);
};

#endif
