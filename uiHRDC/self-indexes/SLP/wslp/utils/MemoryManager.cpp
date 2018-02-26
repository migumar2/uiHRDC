
/*-----------------------------------------------------------------------
 File       : MemoryManager.cpp
 Function   : Reserves large blocks of memory and gives pointers to small
              portions of that block when requested.
              This improves performance since a unique "LARGE ALLOCATION"
              of memory is needed (a unique call to malloc).
              It is also responsible of freeing memory.
 Last change: 10/03/2004
 Purpose    : Improve hash performance.
 ------------------------------------------------------------------------*/
#include "MemoryManager.h"


/*------------------------------------------------------------------
 Constructor method
 ------------------------------------------------------------------ */
MemoryManager 
MManager::createMemoryManager(void) {
	MemoryManager mm;
	mm = (MemoryManager) malloc (sizeof(struct sMem));
	mm->currentBlock=0;
	createNewMemoryBlock(mm);	
	return mm;
}

/*------------------------------------------------------------------
 Destructor method
 ------------------------------------------------------------------ */
void 
MManager::destroyMemoryManager (MemoryManager mm){
	register int i;
	for (i=0; i<=mm->currentBlock;i++) free(mm->BLOCKS[i]);
	free(mm);

}

/*------------------------------------------------------------------
 createNewMemoryBlock method
 Allocates a new memory block of size "LARGE_BLOCK_SIZE" and adds it to
 vector BLOCKS
 ------------------------------------------------------------------ */

void 
MManager::createNewMemoryBlock (MemoryManager mm) {
	mm->BLOCKS[mm->currentBlock] = (uchar *) malloc (LARGE_BLOCK_SIZE);

	if (mm->BLOCKS[mm->currentBlock] == NULL) {
		fprintf(stderr, "\nERROR...\nUnable to allocate enough memory. Exitting...\n");
		exit(0);
	}

	mm->remainderBytes = LARGE_BLOCK_SIZE;
	mm->availableByte	= mm->BLOCKS[mm->currentBlock];  //points to the begining of the block
}


/*------------------------------------------------------------------
 getBlock method
 returns a pointer to a free block of memory of size "size"
 ------------------------------------------------------------------ */
void 
MManager::getMemoryBlock (MemoryManager mm, uchar **dst, const unsigned int size) {
	if (mm->remainderBytes < size)	{
		mm->currentBlock++;
		createNewMemoryBlock(mm);
	}

	*dst = mm->availableByte;			//points to a free size-block
	mm->remainderBytes -= (size);
	mm->availableByte  += (size);
}

