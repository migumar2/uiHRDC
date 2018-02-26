

/** **********************************************************************/
/** Saving/Loading the index to/from disk                               **/
/** **********************************************************************/

		// * loads the source text into memory
	int loadTextInMem (byte **text, ulong *size, char *basename);

		// * parses the text, gathers the vocabulary, freqs[], and maxNumOccs
	int CreateVocabularyOfWords (void *index, ulong *docboundaries, uint ndocs);
	
		// * Loads the VOCABULARY of q-grams, and sets also the
 		// * pointers from byte *words vector.
	int loadVocabulary (void *index, char* basename);
  
		// * Saves the VOCABULARY of qgrams  to disk
	int saveVocabulary (void *index, char* basename);
	
		// * document beginnings within the source text are marked in offsets 
	int loadDocBeginngins(uint **offsets, uint *docs, char *basename);	
	
		// * Saving freq vector 
	int saveFreqVector(uint *V, uint size, char *basename); 	
		// * Loading  freq vector 
	int loadFreqVector(uint **V, uint *size, char *basename);
	
		// * Saving the lists of occurrences of all the words 
	int saveOccLists (void *index, char *basename);
		// * Loading the lists of occurrences of all the words into "index"
	int loadOccLists (void *index, char *basename);
	
/** **********************************************************************/
/** Some functions used during index construction                        */
/** **********************************************************************/	



		// * Creates the lists of occurrences of all the words
  		// * Block-oriented: Only 1 occurrence per block is marked for each word.
  		// * A word belongs to a block if its associated codeword starts in that block.
  		// * returns also the len of each list.
	int createListsOfOccurrences(void *index, uint ***occList, uint **lenList, ulong *docboundaries, uint ndocs);

		//  * Prepares the source uint array that should be passed to build_il()
		//  * Inputs: lenList[], occList[][], nwords and maxPost
		//  * Output: formatedList[] and formatedLen
		//  * Format of the created source array.
		//  * 	// [Nwords=N]
		//		// [maxPostvalue]
		//		// [lenPost1][post11][post12]...[post1k]
		//		// [lenPost2][post21][post22]...[post2k]
		//		//  ... 
		//		// [lenPostN][postN1][postN2]...[postNk]
int prepareSourceFormatForIListBuilder (uint nwords, uint maxPost, uint *lenList, 
										uint **occList, uint **formatedList, ulong *formatedLen);

int load_posting_lists_from_file(uint *maxPost, ulong *n_il, uint **source_il, char *basename); // if they were created previously

int output_posting_lists_concatenated_DEBUGGING_ONLY (uint nwords, 
					uint maxPost, uint *lenList, uint **occList, char *filename);


int getPositions (void *index,  uchar *pattern, ulong length, uint *ids, uint nids, uint **list, uint *len);



/** *** randomize an array of IDS *****/
int int_rand_range_IDS(uint  a,uint b);
void makeRandom (uint *V, uint n);


/** for debugging **/
void printWord(uchar *str, uint len);

