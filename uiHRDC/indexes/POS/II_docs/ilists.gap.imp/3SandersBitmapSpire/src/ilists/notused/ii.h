
/** **********************************************************************/
/** Saving/Loading the index to/from disk                               **/
/** **********************************************************************/

//		// * loads the compressed text (ETDC) into memory
//	int loadTextInMem (byte **text, uint *size, char *basename);
//		// * saves the compressed text (ETDC) to disk
//	int saveTextInMem (byte *text, uint size, char *basename);
//  
//		// * Loads the VOCABULARY of ETDC into the hash table, and sets also the
// 		// * pointers from byte *words vector.
//	int loadVocabulary (void *index, char* basename);
//  
//		// * Saves the VOCABULARY of ETDC  to disk
//	int saveVocabulary (void *index, char* basename);
//	
//		// * Saving freq vector 
//	int saveFreqVector(uint *V, uint size, char *basename); 	
//		// * Loading  freq vector 
//	int loadFreqVector(uint **V, uint *size, char *basename);
	
	
		// * Saving  lenList vector 
	int savelenListVector(uint *V, uint size, char *basename);
		// * Loading lenList vector 
	int loadlenListVector(uint *V, uint size, char *basename);
	
		// * Saving the lists of occurrences of all the words 
	int saveOccLists (void *ail, char *basename);
		// * Loading the lists of occurrences of all the words into "index"
	int loadOccLists (void *index, char *basename);
	
/** **********************************************************************/
/** Some functions used during index construction                        */
/** **********************************************************************/	

  		// * Computes the frequency of each id--word
  		//	* && sets the number of words in the source text: N
	int computeFreqs (void *index);
	
		// * Creates the lists of occurrences of all the words
  		// * Block-oriented: Only 1 occurrence per block is marked for each word.
  		// * A word belongs to a block if its associated codeword starts in that block.
  		// * returns also the len of each list.
	int createListsOfOccurrences(void *index, uint ***occList, uint **lenList);


		// ** Outputs the list of each posting list and its values.
		// *[Nwords=N]
		// *[maxPostvalue]
		// *[lenPost1][post11][post12]...[post1k´]
		// *[lenPost2][post21][post22]...[post2k´´]
		// * ... 
		// *[lenPostN][postN1][postN2]...[postNk´´´]		
	int saveAllPostingList(uint Nwords, uint *lenList, uint **occList, uint maxPost, char *basename);

		//recreates structures lenList, occList from above.
	int	parseAllPostingList(uint *nlists, uint **lenList, uint ***occList, 
                        uint *maxPostValue, uint *source, uint length);	
	
/** **********************************************************************/
/** Functions needed for searches                                       **/
/** **********************************************************************/	

		// * Uncompresses the list of occurrences of a given id-word
	int uncompressList (void *index, uint id, uint **list, uint *len);
	


