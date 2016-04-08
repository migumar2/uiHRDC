
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
MemoryManager createMemoryManager(void) {
	MemoryManager mm;
	mm = (MemoryManager) malloc (sizeof(struct sMem));
	mm->currentBlock=0;
	createNewMemoryBlock(mm);	
	return mm;
}

/*------------------------------------------------------------------
 Destructor method
 ------------------------------------------------------------------ */
void destroyMemoryManager (MemoryManager mm){
	register int i;
	for (i=0; i<=mm->currentBlock;i++) free(mm->BLOCKS[i]);
	printf("\n[destroying MemManager] ...Freed %u bytes... RAM", LARGE_BLOCK_SIZE* (mm->currentBlock+1));
	free(mm);

}

/*------------------------------------------------------------------
 createNewMemoryBlock method
 Allocates a new memory block of size "LARGE_BLOCK_SIZE" and adds it to
 vector BLOCKS
 ------------------------------------------------------------------ */

void createNewMemoryBlock (MemoryManager mm) {
	mm->BLOCKS[mm->currentBlock] = (byte *) malloc (LARGE_BLOCK_SIZE);

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
void getMemoryBlock (MemoryManager mm, byte **dst, const unsigned int size) {
	if (mm->remainderBytes < size)	{
		mm->currentBlock++;
		createNewMemoryBlock(mm);
		//fprintf(stderr,"\new memory block");
	}

	*dst = mm->availableByte;			//points to a free size-block
	mm->remainderBytes -= (size);
	mm->availableByte  += (size);
}


/*------------------------------------------------------------------
 main, to make unit proofs
 ------------------------------------------------------------------ */
/*
int main(int argc, char* argv[])
{  byte *word, *word2;
	unsigned int size;
int i;
	MemoryManager memMgr;
	memMgr=createMemoryManager();

	size = 100;
	getMemoryBlock(memMgr,&word,size);

	fprintf(stderr,"pasei getblock\n");
	strcpy((char *)(word), "01234567890123456789012345678901234567890123456789");
	word[50]='\0';
	fprintf(stderr,"pasei strcpy \n");
	fprintf(stderr,"\n%s",word);
	getMemoryBlock(memMgr,&word2,size);
	strcpy((char *)(word2), "soy la word 2");

	for (i=0;i<100000;i++) {
		size = 89;
		getMemoryBlock(memMgr,&word,size);
	}

	fprintf(stderr,"\n  final %s",word2);

	destroyMemoryManager(memMgr);

} */

