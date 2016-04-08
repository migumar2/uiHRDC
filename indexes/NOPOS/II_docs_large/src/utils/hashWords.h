/*-----------------------------------------------------------------------
 Hash: Definition of HashTable class (Linear Hash)
 ------------------------------------------------------------------------*/
#ifndef HASHWORDSINCLUDED
#define HASHWORDSINCLUDED	 
	#include <string.h>
	#include <stdlib.h>
	#include <math.h>
	#include <malloc.h>	
	#include <sys/types.h>
	#include <sys/stat.h>
	#include <fcntl.h>
	#include <unistd.h>
	
	#ifndef byte
		#define byte unsigned char
	#endif 
		
	#include "defValues.h"
	#include "fileInfo.h"
		
	#define W_JUMP 101  	   //jump done when a collision appears	
	#define W_OCUP_HASH 2.0   //index of occupation of the hash table
	#define W_SMALL_PRIME 1009 // a small prime number, used to compute a hash function
	#define W_SEED	1159241
	
	/* Type definitions */

		
	//struct WtzoneMem {        //a large block of memory to load a file into mem.
	//	byte *zone;  //block of mem.
	//	uint size;  //number of elements	 
	//	byte *nextFree;			 
	//};
	//typedef struct WtzoneMem t_WtZoneMem;

	struct WthashNode {
		  unsigned char *word;
		  unsigned long len;
		  unsigned long freq;
		  unsigned long id;
	};
	typedef struct WthashNode Wt_hashNode;
	
	struct WthashStr {
		Wt_hashNode *hash;		  /* the slots in the hash table 	*/
		unsigned long SIZE_HASH;  /* # entries in the hash table    */
		unsigned long NumElem;    /* # elements already added to the hash table*/
		//t_WtZoneMem zone;         /* memory zone to store the stopwords */		
		//uint qgramlen;	
		long zone_size;
	};
	
	typedef struct WthashStr *Wt_hash;
	
	
		
	// private:	
		unsigned long wtNearestPrime(unsigned long n);
		unsigned long wthashFunction (const unsigned char *aWord, unsigned int len, unsigned long sizeHash);	

	// public:
		
		Wt_hash wtinitialize_hash (unsigned long numberOfwords);
		
		void wtinsertElement (Wt_hash h, const unsigned char *aWord, register unsigned long len,
								      register unsigned long *addr);

		void wtinsertElement_id (Wt_hash h, const unsigned char *aWord, register unsigned long len, unsigned long id,
								      register unsigned long *addr);

		unsigned long wtsearch (Wt_hash h, const unsigned char *aWord, register unsigned long len,
									  unsigned long *returnedAddr);
		unsigned long wtinHashTable (Wt_hash h, const unsigned char *aWord, register unsigned long len,
									  unsigned long *returnedAddr);
		
		//permits to compare 2 strings of len ws1 and ws2 that do not end in '\0'
		int wtstrcompL(const byte *s1, const byte *s2, register ulong ws1, register ulong ws2);
							  
		void wtdestroy_hash (Wt_hash hash);


		void wthash2array_hash (Wt_hash h, unsigned char ***words, unsigned long *nwords);
	
		unsigned long wtmemused (Wt_hash hash);

#endif


















































