

/** **********************************************************************/
/** Saving/Loading the index to/from disk                               **/
/** **********************************************************************/

		// * loads the source text into memory
	int loadTextInMem (byte **text, uint *size, char *basename);

		// * document beginnings within the source text are marked in offsets 
	int loadDocBeginngins(uint **offsets, uint *docs, char *basename);	

		// * parses the text, gathers the vocabulary, freqs[], and maxNumOccs
	int CreateVocabularyOfWords (void *index, uint *docboundaries, uint ndocs);
	
		// * Loads the VOCABULARY of q-grams, and sets also the
 		// * pointers from byte *words vector.
	int loadVocabulary (void *index, char* basename);
  
		// * Saves the VOCABULARY of qgrams  to disk
	int saveVocabulary (void *index, char* basename);
	
		// * document beginnings within the sequence of words: Sids[]
	int loadDocOffsetsSids(void *index, char *basename);	
		// * document beginnings within the sequence of words: Sids[]
	int saveDocOffsetsSids(void *index, char *basename);	
	
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



/** ------------------------------------------------------------------
  * Creates the lists of occurrences of all the words
  * Doc-oriented: Only 1 occurrence per doc is marked for each word.
  * A word belongs to a doc if its associated codeword starts in that doc.
  * Creates also "doc_offsets_sids": For each doc, gives the rank in the
  *     sequence of word-ids of the first word in the ith-document
  * ------------------------------------------------------------------*/
int createListsOfOccurrences(void *index, uint ***occList, uint **lenList, 
								uint **doc_offsets_sids,
								uint *docboundaries, uint ndocs);
  
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
										uint **occList, uint **formatedList, uint *formatedLen);


int output_posting_lists_concatenated_DEBUGGING_ONLY (uint nwords, 
					uint maxPost, uint *lenList, uint **occList, char *filename);
					
										
int getPositions (void *index,  uchar *pattern, ulong length, uint *ids, uint nids, uint **list, uint *len);




/** for debugging **/
void printWord(uchar *str, uint len);
