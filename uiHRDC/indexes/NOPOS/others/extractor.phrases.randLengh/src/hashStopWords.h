/*-----------------------------------------------------------------------
 Hash: Definition of HashTable class (Linear Hash)
 ------------------------------------------------------------------------*/
#ifndef HASHSTOPWORDSINCLUDED
#define HASHSTOPWORDSINCLUDED
	 
	#include <string.h>
	#include <stdlib.h>
	#include <math.h>
	#include <malloc.h>
	
	#include <sys/types.h>
	#include <sys/stat.h>
	#include <fcntl.h>
	#include <unistd.h>

	#include "constants.h"
	
	#ifndef byte
		#define byte unsigned char
	#endif 
		
	#define ST_JUMP 17  		   //jump done when a collision appears	
	#define ST_OCUP_HASH 1.5	//index of occupation of the hash table
	#define ST_SMALL_PRIME 101 // a small prime number, used to compute a hash function
	#define ST_SEED	1009
	
	
	/* Type definitions */
	
	
	struct StzoneMem {        //a large block of memory to load a file into mem.
		byte *zone;  //block of mem.
		uint size;  //number of elements	 
		byte *nextFree;			 
	};
	typedef struct StzoneMem t_StZoneMem;

	struct sthashNode {
		  unsigned char *word;
		  unsigned long len;
	};
	typedef struct sthashNode stt_hashNode;
	
	struct sthashStr {
		stt_hashNode *hash;		  /* the slots in the hash table 	*/
		unsigned long SIZE_HASH;  /* # entries in the hash table    */
		unsigned long NumElem;    /* # elements already added to the hash table*/
		t_StZoneMem zone;         /* memory zone to store the stopwords */			
	};
	
	typedef struct sthashStr *stt_hash;
	
	
	// private:
	
		unsigned long stNearestPrime(unsigned long n);
		unsigned long sthashFunction (const unsigned char *aWord, unsigned int len, unsigned long sizeHash);
	
	// public:
		
		stt_hash stinitialize_hash (unsigned long numberOfStopwords, unsigned long stopWordsFileSize);
		
		void stinsertElement (stt_hash h, const unsigned char *aWord, register unsigned long len,
								      register unsigned long *addr);
		unsigned long stsearch (stt_hash h, const unsigned char *aWord, register unsigned len,
									  unsigned long *returnedAddr);
		unsigned long stinHashTable (stt_hash h, const unsigned char *aWord, register unsigned len,
									  unsigned long *returnedAddr);
		void stdestroy_hash (stt_hash hash);
	
		unsigned long stmemused (stt_hash hash);

		void loadStopWords(stt_hash *hash, char *stopwordsfile);
		
#endif

