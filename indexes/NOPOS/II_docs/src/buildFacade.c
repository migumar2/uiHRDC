#include "buildFacade.h"

#include "utils/parseparamsII.c"  //parsing a string with parameters.
#include "utils/scdc.c"			  //decoding SCDC
#include "utils/errors.c"
#include "utils/ii.c"             //some auxiliar funcitons used during construction (also load/save)

#include "utils/iisvsETDCBlock.c" //getPositions() 


/***********************************************************************************/
/***********************************************************************************/

/** Building the index */

    /* Creates index from text[0..length-1]. Note that the index is an 
      opaque data type. Any build option must be passed in string 
      build_options, whose syntax depends on the index. The index must 
      always work with some default parameters if build_options is NULL. 
      The returned index is ready to be queried. */

int build_index (char *sourceFileBaseName, char *build_options, void **index) {
	int returnvalue;

	fprintf(stderr,"\n parameters received, ii_word-DocOriented:: \"%s\"\n",build_options); fflush(stderr);        
    returnvalue =  build_WordIndex (sourceFileBaseName, build_options, index);
    return returnvalue;    
}


    /* Creates index from a given basename
     * files that must exists are (the result of build_index).
     *    basename.posts
     *    basename.voc
     *    basename.const
     * The returned index is ready to be queried. 
     *  */
int build_index_from_postings (char *sourceFileBaseName, char *build_options, void **index) {
	int returnvalue;

	fprintf(stderr,"\n parameters received, ii_word-DocOriented:: \"%s\"\n",build_options); fflush(stderr);        
    returnvalue =  build_WordIndex_from_postings (sourceFileBaseName, build_options, index);
    return returnvalue;    
}

/**  Saves index on disk by using single or multiple files, having 
	proper extensions. */
int save_index (void *index, char *filename) {
	char *basename = filename;
	twcsa *wcsa=(twcsa *) index;

	fprintf(stderr,"\nSaving structures to disk: %s.*\n",basename);			
	fflush(stderr);

	// Saves the vocabulary of words
	saveVocabulary (index, basename);

	// Saves some configuration constants: maxNumOccs, sourceTextSize */
	saveIndexConstants (index, filename);
	
	#ifdef FREQ_VECTOR_AVAILABLE	
	// saves freqVector do disk
	saveFreqVector (wcsa->freqs,   wcsa->nwords, basename);	
	#endif
				
	// Saving the Compressed Structure of posting lists (il) 	
	int error = save_il(wcsa->ils, basename);
	IFERRORIL(error);
	fprintf(stderr,"\n \t**saved il_list");
	fflush(stderr);

	// Saving the Representation of the source text
	save_representation(wcsa->ct, filename);

	fprintf(stderr,"\n \t**saved compressed representation of the source text");
	fflush(stderr);

	return 0;
}



    /**  Loads index from one or more file(s) named filename, possibly 
      adding the proper extensions. */
int load_index(char *filename, void **index){

	twcsa *wcsa;
	wcsa = (twcsa *) malloc (sizeof (twcsa) * 1);
	void *Index = (void *) wcsa;
	int error;
	wcsa->text = NULL;
	
	// Inicializes the arrays used to detect if a char is valid or not.
	StartValid();
		
	/** 1 ** loads the vocabulary of words. zonewords, words vector, nwords */
	loadVocabulary (Index, filename);
		{	
		uint totaltmp=0;  //words
		totaltmp += ((((wcsa->nwords+1)* (wcsa->wordsData.elemSize))+W-1) /W) * (sizeof(uint));  //the pointers
		totaltmp += wcsa->wordsData.wordsZoneMem.size * sizeof(byte); //the characters of the words.	
		fprintf(stderr,"\n\t*Loaded Vocabulary of text: %u words, %d bytes\n", wcsa->nwords, totaltmp);
		}
		
	/** 2 ** Loads some configuration constants: sourceTextSize, maxNumOccs */
	loadIndexConstants(Index, filename);
	fprintf(stderr,"\t*Loaded  configuration constants: %lu bytes\n", (ulong) (2 * sizeof(uint ) + sizeof(ulong)) );

	#ifdef FREQ_VECTOR_AVAILABLE
	/** 3 ** Loading freq vector */
	{uint size; //the size of the vocabulary in #ofwords =>> already init in "loadvocabulary"
	loadFreqVector(&(wcsa->freqs), &size, (char *)filename);	
	fprintf(stderr,"\t*Loaded freq vector: %d bytes\n", wcsa->nwords * sizeof(uint) );
	}		
	#endif

	/** 4 ** Loading Compressed Structure of posting lists (il) */
	error = load_il((char*) filename,&(wcsa->ils));
 	IFERRORIL(error);
 	uint sizeil;
 	error = size_il(wcsa->ils,&sizeil);
 	IFERRORIL(error);
	fprintf(stderr,"\n \t*loaded compressed inverted lists structure: %d bytes\n", sizeil);
	
	/** 5 ** Loading the Representation of the source text */
	load_representation( &wcsa->ct,filename); 
	{
		uint size;
		size_representation(wcsa->ct, &size);	
		fprintf(stderr,"\n\t*Loaded (compressed) representation of the source Text: %u bytes\n", size);
	}
	
	(*index) = Index;			
	return 0;
}

	/** Frees the memory occupied by index. */
int free_index(void *index){
	twcsa *wcsa=(twcsa *) index;
	
	//the words.
	free (wcsa->wordsData.wordsZoneMem.zone);
	free (wcsa->wordsData.words); 
	
	#ifdef FREQ_VECTOR_AVAILABLE
	free(wcsa->freqs);
	#endif

	//delete inverted index represetation.
	if (wcsa->ils) free_il(wcsa->ils);
	
	if (wcsa->text) free(wcsa->text);
		
	//delete representation of the comrpessed text.
	free_representation(wcsa->ct);
	
	free(wcsa);
	return 0;
}

	/** Returns the size of the representation of the inverted lists **/
int index_getsize_il(void *index, long *sizeil) {
	twcsa *wcsa=(twcsa *)index;	 
	uint size_il_uint;
	int error = size_il(wcsa->ils, &size_il_uint);

	*sizeil = (long) size_il_uint;	
	return error;  
}
	
	/** Returns the size of the representation of the compressed text **/
int index_getsize_ct(void *index, long *sizect){
	twcsa *wcsa=(twcsa *)index;	  
	uint size_ct_uint;
	int error = size_representation(wcsa->ct, &size_ct_uint);
	*sizect = (long) size_ct_uint;
	return error;
}


	/** Gives the memory occupied by index in bytes. */
int index_size(void *index, ulong *size) {
	twcsa *wcsa=(twcsa *)index;
	*size=0;
	*size += sizeof(twcsa);

	uint totalVoc=0;  //Vocabulary of words.
	totalVoc += ((((wcsa->nwords+1)* (wcsa->wordsData.elemSize))+W-1) /W) * (sizeof(uint));  //the pointers
	totalVoc += wcsa->wordsData.wordsZoneMem.size * sizeof(byte); //the characters of the words.
	*size += totalVoc;
	
	#ifdef FREQ_VECTOR_AVAILABLE
	     *size += wcsa->nwords * sizeof(uint) ;  //freqs
	#endif
	
	uint sizeil; 
	int error = size_il(wcsa->ils, &sizeil);
	long sizeL = (long) size_il;
	if (sizeL <0){
		printf("\n\n [WARNING!! size is being reported badly (%ld bytes!) due to IL_postings_list_technique\n",sizeL);
	}
	IFERRORIL(error);
	*size += sizeil;

	uint sizerep;
	size_representation(wcsa->ct, &sizerep);
	*size += sizerep;	//the size of the representation of the source text 
	
	//printf("\n *************SIZE OF INDEX = %d bytes ",*size);
	return 0;	
}

/** returns detailled info of the data structures of the index **/
int index_info(void *index, char msg[]) {
	twcsa *wcsa=(twcsa *)index;
	char msgt[1000];
	int error;
	uint sizeil; 
	
	error = size_il(wcsa->ils, &sizeil);
	long sizeL = (long) size_il;
	if (sizeL <0){
		printf("\n\n [WARNING!! size is being reported badly (%ld bytes!) due to IL_postings_list_technique\n",sizeL);
	}
	IFERRORIL(error);
	
	ulong indexs;
	index_size(index,&indexs);
	
	ulong Text_length;
	get_length(index, &Text_length);	


	uint totalVoc=0;  //Vocabulary of words.
	totalVoc += ((((wcsa->nwords+1)* (wcsa->wordsData.elemSize))+W-1) /W) * (sizeof(uint));  //the pointers
	totalVoc += wcsa->wordsData.wordsZoneMem.size * sizeof(byte); //the characters of the words.
		
	uint sizerepres; //Compressed representation of the source text
	size_representation(wcsa->ct, &sizerepres);
		
	sprintf(msg,"\n\tSpace statistics for II-words-DocOriented\n");
	sprintf (msgt,"\t tindex structure = %lu bytes.\n",(ulong)sizeof(twcsa) );	strcat(msg,msgt);
    sprintf (msgt,"\t number of diff words= %u\n",wcsa->nwords );	strcat(msg,msgt);	
    sprintf (msgt,"\t number of docs (ndocs)= %u\n",wcsa->ndocs );	strcat(msg,msgt);	
	
	
	sprintf (msgt,"\t vocabulary of words = %u bytes.\n", totalVoc );	strcat(msg,msgt);
	sprintf (msgt,"\t Inverted list-structure = %u bytes.\n",sizeil );	strcat(msg,msgt);
	sprintf (msgt,"\t Text (compressed representation) = %u bytes.\n", sizerepres);	strcat(msg,msgt);
	sprintf (msgt,"\t Whole index = %lu bytes ", indexs);	strcat(msg,msgt);
	sprintf (msgt," ** Ratio = %2.3f %% **\n ",  100.0*indexs / Text_length);	strcat(msg,msgt);
	
	fprintf(stderr,"\n\tSpace statistics for II-words-DocOriented\n %s",msg);
	
	return 0;
}


/** Querying the index =============================================================*/
	
	/* Writes in numocc the number of occurrences of the substring 
	   pattern[0..length-1] found in the text indexed by index. */
int count (void *index, uchar *pattern, uint length, uint *numocc){
	uint integerPatterns[MAX_INTEGER_PATTERN_SIZE];
	uint integerPatternSize;	

	twcsa *wcsa=(twcsa *) index;
	parseTextIntoIntegers(wcsa, pattern, length, integerPatterns, &integerPatternSize);	
	//no words
	if (!integerPatternSize) {*numocc=0; return 0;} //not found
	
	#ifdef FREQ_VECTOR_AVAILABLE
	//only just one word
	if (integerPatternSize ==1) {*numocc= wcsa->freqs[integerPatterns[0]]; return 0;}
	#endif
	
	//a phrase;
	{	uint *occs;	
		getPositions (index, pattern, length, integerPatterns, integerPatternSize, &occs, (uint *) numocc);
		free(occs);
	}			
	return 0;
}

	/* Writes in numocc the number of occurrences of the substring 
	  pattern[0..length-1] in the text indexed by index. It also allocates
	  occ (which must be freed by the caller) and writes the locations of 
	  the numocc occurrences in occ, in arbitrary order.  */	  
int locate_PREV(void *index, uchar *pattern, uint length, uint **occ, uint *numocc){
	uint integerPatterns[MAX_INTEGER_PATTERN_SIZE];
	uint integerPatternSize;	
	twcsa *wcsa=(twcsa *) index;

//	fprintf(stderr,"\nTO LOCATE: %s, len=%u",pattern,length);
	parseTextIntoIntegers(wcsa, pattern, length, integerPatterns, &integerPatternSize);	
	
	if (!integerPatternSize) {*numocc=0; *occ=NULL; return 0;} //not found 
	  
	getPositions (index, pattern, length, integerPatterns, integerPatternSize, (uint **) occ, (uint *) numocc);
	
	{fflush(stderr);
	fprintf(stderr,"\n showing text on occurrences [numocc=%u]:",*numocc);
	uint i,n;
	n = (*numocc >6) ? 6 : *numocc;
	for (i=0; i< n; i++) {
		ulong pos = (*occ)[i];
		fprintf(stderr,"\n\t [%u: %lu]::>",i, pos);	fflush(stderr);fflush(stdout);	
		printTextInOffset(wcsa->ct, pos,40);
	}
	}
	fprintf(stderr,"\n");
	
	
	//fprintf(stderr,"\n** Pattern: %s appears %d times",pattern, *numocc);
	return 0;
}



	/* Writes in numocc the number of occurrences of the substring 
	  pattern[0..length-1] in the text indexed by index. It also allocates
	  occ (which must be freed by the caller) and writes the locations of 
	  the numocc occurrences in occ, in arbitrary order.  */	  
int locate(void *index, uchar *pattern, uint length, uint **occ, uint *numocc){
	uint integerPatterns[MAX_INTEGER_PATTERN_SIZE];
	uint integerPatternSize;	
	twcsa *wcsa=(twcsa *) index;

	parseTextIntoIntegers(wcsa, pattern, length, integerPatterns, &integerPatternSize);	
	
	if (!integerPatternSize) {*numocc=0; *occ=NULL; return 0;} //not found 

	  return index_listDocuments(index, integerPatterns, integerPatternSize, occ, numocc);
}



   /* Gives the length of the text indexed */
int get_length(void *index, ulong *length) {
	twcsa *wcsa=(twcsa *) index;
	*length = wcsa->sourceTextSize;
	return 0;
}


/** ***********************************************************************************
  * Accessing the indexed text  
  * ***********************************************************************************/


	/**  Allocates snippet (which must be freed by the caller) and writes 
	  the substring text[from..to] into it. Returns in snippet_length the 
	  length of the text snippet actually extracted (that could be less 
	  than to-from+1 if to is larger than the text size). 			   */
int extract (void *index, ulong from, ulong to, uchar **snippet, ulong *snippet_length) {    		
    //twcsa *wcsa=(twcsa *) index;    

   return -1;
}

  /** Displays the text (snippet) surrounding any occurrence of the 
    substring pattern[0..length-1] within the text indexed by index. 
    The snippet must include numc characters before and after the 
    pattern occurrence, totalizing length+2*numc characters, or less if 
    the text boundaries are reached. Writes in numocc the number of 
    occurrences, and allocates the arrays snippet_text and 
    snippet_lengths (which must be freed by the caller). The first is a 
    character array of numocc*(length+2*numc) characters, with a new 
    snippet starting at every multiple of length+2*numc. The second 
    gives the real length of each of the numocc snippets. */

int display (void *index, uchar *pattern, ulong length, ulong numc, 
        ulong *numocc, uchar **snippet_text, ulong **snippet_lengths) {

	//twcsa *wcsa=(twcsa *) index;
				
	return -1;
}

    /**  Obtains the length of the text indexed by index. */

int length (void *index, ulong *length) {
	return (get_length(index,length));
}

 	/**  Initializes the int2 and intn intersection functions to be used */
 	
int setDefaultIntersectionOptions(void *index, char *options) {
	twcsa *wcsa=(twcsa *) index;
	int error=0;
	error = setDefaultSearchOptions_il (wcsa->ils, options);
	return error;
}





/** ***********************************************************************************
	 CONSTRUCTION OF THE INDEX, from a given text file "inbasename".
    ***********************************************************************************/
int build_WordIndex (char *inbasename, char *build_options, void **index){
	twcsa *wcsa;
	wcsa = (twcsa *) malloc (sizeof (twcsa) * 1);
	*index = wcsa;
	wcsa->text = NULL;
	double t0, t1;
	t0 = getSYSTimeBF();

	//char path2repaircompressor[1000]="./src/repair64bit/repairCompressor";
	//wcsa->blockSize = DEFAULT_BLOCK_SIZE;
	//wcsa->q = DEFAULT_QGRAM_LEN;

	/** processing the parameters of the index:: blockSize, and q-gram-len (q) */
	{
		char delimiters[] = " =;";
		int j,num_parameters;
		char ** parameters;
		
		if (build_options != NULL) {
			parse_parameters_II(build_options,&num_parameters, &parameters, delimiters);
			for (j=0; j<num_parameters;j++) {

			  if ((strcmp(parameters[j], "blocksize") == 0 ) && (j < num_parameters-1) ) {
				//wcsa->blockSize = atoi(parameters[j+1]) * BLOCK_MULTIPLIER;	    
				j++;
			  } 
			  else if ((strcmp(parameters[j], "qgram") == 0 ) && (j < num_parameters-1) ) {
				//wcsa->q =atoi(parameters[j+1]);	    
				j++;
			  }
			  else if ((strcmp(parameters[j], "path2repaircompressor") == 0 ) && (j < num_parameters-1) ) {
				//strcpy(path2repaircompressor,parameters[j+1]);	    
				j++;
			  }
			  
			}
			free_parameters_II(num_parameters, &parameters);
		}
		//fprintf(stderr,"\n Parameters of II-blocks:: *basename = %s, blocksize = %d, q= %d",inbasename,wcsa->blockSize,wcsa->q);
		//fprintf(stderr,"\n \t path2repaircompressor= %s\n",path2repaircompressor);
	}

	/** 0 ** Inicializes the arrays used to detect if a char is valid or not. **/
	StartValid();
	
	
	/** 1 ** Loads the compressed text into memory. */
	t1 = getSYSTimeBF();
	fprintf(stderr,"\n... Entering LoadTextInmem:%s\n",inbasename);
	loadTextInMem(&(wcsa->text), &(wcsa->sourceTextSize),(char *)inbasename);	
		
	fprintf(stderr,"... Loaded Source Sequence: %lu bytes\n", wcsa->sourceTextSize); 
	fprintf(stderr,"... Done: %2.2f seconds (sys+usr time)\n\n", getSYSTimeBF() -t1); 
	fflush(stderr);

	/** 2 ** loads the array of document boundaries                           */
	uint ndocs;
	ulong *docboundaries;
	loadDocBeginngins(&docboundaries, &ndocs,(char *)inbasename);	
	wcsa->ndocs = ndocs; //just for statistics.

	
	/** 3 ** Parses the sequence and gathers the vocabulary of words (sorted alphanumerically) 
		the frecuency of such words: obtains "words", "nwords", and "wordsZone" 
		Sets also wcsa->freqs (freq of each word)
		Sets also wcsa->maxNumOccs (needed for malloc during extraction) */	
	
	fprintf(stderr,"\n... Entering CreateVocabularyOfWords (1st pass) \n"); fflush(stderr);
	CreateVocabularyOfWords(*index, docboundaries, ndocs);


	//shows the words parsed...
	{
		int i;
		fprintf(stderr,"\n\n Despues de sorting ....");	fflush(stderr);
		unsigned char *str;
		uint len;
//		for (i = 0; i<100; i++) {
		for (i = 0; ((uint)i)<wcsa->nwords; i++) {
			if ((i<10) || (((uint)i) >wcsa->nwords-5)) {
				getWord(wcsa,i,&str,&len);				
				fprintf(stderr,"\n freq[%6d]=%6u ",i,  wcsa->freqs[i]);
				fprintf(stderr,", words[%6d] = ",i);
				printWord(str,len);
			}
		}		
	}

	t1 = getSYSTimeBF();
	fprintf(stderr,"\n %u words have been parsed", wcsa->nwords);
	fprintf(stderr,"\n... Done: %2.2f seconds (sys+usr time)\n", getSYSTimeBF() -t1); 					
	
	/** 4 ** creates a temporal list of occurrences of each word (block-oriented).
					gives also the len of each list */
	{	
		//decompression of the source text and creation of occList[][] and lenList[]
		uint **occList; uint *lenList;
		
		t1 = getSYSTimeBF();
		fprintf(stderr,"\n... Entering createListsOfOccurrences (2nd pass) \n"); fflush(stderr);	
		
		createListsOfOccurrences (*index, &occList, &lenList, docboundaries, ndocs);
		
		fprintf(stderr,"\n %u lists of occurrences were created.", wcsa->nwords);fflush(stderr);
		fprintf(stderr,"\n... Done: %2.2f seconds (sys+usr time)\n", getSYSTimeBF() -t1);





#ifdef CIKM2011_HURRY
	free(wcsa->text);
	wcsa->text = NULL;
#endif
		//Preparing a "list of occurrences" that will be later indexed through build_il() **
		uint *source_il, sourcelen_il;
		uint maxPost = ndocs;
		uint nwords = wcsa->nwords;
		ulong source_il_ulong;

		t1 = getSYSTimeBF();
		fprintf(stderr,"\n... Entering prepareSourceFormatForIListBuilder \n"); fflush(stderr);	
		
		prepareSourceFormatForIListBuilder(nwords,maxPost,lenList, occList, &source_il, &source_il_ulong);

		/** FOR CIKM ILISTS_DO NOT STILL SUPPORT an ULONG HERE **/
		sourcelen_il = (uint)source_il_ulong;
		
		fprintf(stderr,"\n there are %lu uints in all the lists of occurrences: size [uint32] = %lu bytes.\n ", 
		               (ulong)sourcelen_il - nwords -2, (ulong) sizeof(uint)*(sourcelen_il - nwords -2));

		/*
	char fileuintpostings[256] = "postingsSequence.uint32";
	output_posting_lists_concatenated_DEBUGGING_ONLY (nwords, maxPost, lenList, occList,fileuintpostings);		
	*/
		
		
		{ char fileposts[2048];
			sprintf(fileposts,"%s.%s.%u","postinglists","posts", getpid());
			FILE *ff = fopen(fileposts,"w");
			fwrite(source_il, sizeof(uint), sourcelen_il,ff);
			fclose(ff);
			
		}		
/*		
	FILE *ff = fopen ("2gbnopositional.posts.uint32","w");
	fwrite (source_il, sizeof(uint), sourcelen_il, ff);
	fclose(ff);
*/	
	
		fprintf(stderr,"\n the lists of occurrences were formatted for build_il.");fflush(stderr);
		fprintf(stderr,"\n...Done: %2.2f seconds (sys+usr time)\n", getSYSTimeBF() -t1);

		
		t1 = getSYSTimeBF();
		fprintf(stderr,"\n**... entering BUILD INVERTED LIST REPRESENTATION!! \n"); fflush(stderr);	

		//compressing the lists of occurrences and setting wcsa->ils
		int error = build_il(source_il, sourcelen_il, build_options, &(wcsa->ils));  //source_il is freed inside!.
		IFERRORIL(error);							

		fprintf(stderr,"\n... Done: %2.2f seconds (sys+usr time)\n", getSYSTimeBF() -t1);
		
		{
			//frees memory for the posting lists
			uint i;
			for (i=0;i<wcsa->nwords;i++) free(occList[i]);
			free(occList);
			free(lenList);
		}
	
		/** 5 ** compressed representation of the source text */	
		{

#ifdef CIKM2011_HURRY
		uchar text_null[109] = "NULL-TEXT"; uint text_len_null=1;
		uint docbounds_null[2]= {0,10};
		uint ndoc_null = 1;
			build_representation (text_null, text_len_null, docbounds_null, ndoc_null, build_options, &wcsa->ct);
#endif
#ifndef CIKM2011_HURRY
			build_representation (wcsa->text, wcsa->sourceTextSize, docboundaries, ndocs, build_options, &wcsa->ct);
#endif

		
			unsigned char *document;
			uint doclen;
			extract_doc_representation (wcsa->ct, 0, &document, &doclen);
			fprintf(stderr,"\n =================== DOC 0 ======================");
			fprintf(stderr,"\n%s",document);
			fprintf(stderr,"\n =================== ***** ======================\n");
			free(document);
			//free(docbegsUL);
		

		}

	}
	
	#ifndef FREQ_VECTOR_AVAILABLE   //<----- not needed in advance, only during construction
		free(wcsa->freqs);
	#endif

	free(docboundaries);	
		
	ulong sizeI;
	index_size(*index, &sizeI);
	fflush(stderr); fflush(stdout);
	fprintf(stderr,"\n The index has already been built: %lu bytes!!\n", sizeI);
	fprintf(stderr,"\n... Done: OVERALL TIME = %2.2f seconds (sys+usr time)\n\n\n", getSYSTimeBF() -t0);
	fflush(stderr);
	fflush(stdout);
	return 0;
}













/** ***********************************************************************************
	 CONSTRUCTION OF THE INDEX, from a given text file "inbasename".
    ***********************************************************************************/
int build_WordIndex_from_postings (char *inbasename, char *build_options, void **index){
	twcsa *wcsa;
	wcsa = (twcsa *) malloc (sizeof (twcsa) * 1);
	*index = wcsa;
	void *Index = *index;
	wcsa->text = NULL;
	double t0, t1;
	t0 = getSYSTimeBF();

	/** processing the parameters of the index:: blockSize, and q-gram-len (q) */
	{
		char delimiters[] = " =;";
		int j,num_parameters;
		char ** parameters;
		
		if (build_options != NULL) {
			parse_parameters_II(build_options,&num_parameters, &parameters, delimiters);
			for (j=0; j<num_parameters;j++) {

			  if ((strcmp(parameters[j], "blocksize") == 0 ) && (j < num_parameters-1) ) {
				//wcsa->blockSize = atoi(parameters[j+1]) * BLOCK_MULTIPLIER;	    
				j++;
			  } 
			  else if ((strcmp(parameters[j], "qgram") == 0 ) && (j < num_parameters-1) ) {
				//wcsa->q =atoi(parameters[j+1]);	    
				j++;
			  }
			  else if ((strcmp(parameters[j], "path2repaircompressor") == 0 ) && (j < num_parameters-1) ) {
				//strcpy(path2repaircompressor,parameters[j+1]);	    
				j++;
			  }
			  
			}
			free_parameters_II(num_parameters, &parameters);
		}
		//fprintf(stderr,"\n Parameters of II-blocks:: *basename = %s, blocksize = %d, q= %d",inbasename,wcsa->blockSize,wcsa->q);
		//fprintf(stderr,"\n \t path2repaircompressor= %s\n",path2repaircompressor);
	}

	wcsa->freqs=NULL;


	/** 0 ** Inicializes the arrays used to detect if a char is valid or not. **/
	StartValid();
	
		
	/** 1 ** loads the vocabulary of words. zonewords, words vector, nwords */
	t1 = getSYSTimeBF();
	
	loadVocabulary (Index, inbasename);
		{	
		uint totaltmp=0;  //words
		totaltmp += ((((wcsa->nwords+1)* (wcsa->wordsData.elemSize))+W-1) /W) * (sizeof(uint));  //the pointers
		totaltmp += wcsa->wordsData.wordsZoneMem.size * sizeof(byte); //the characters of the words.	
		fprintf(stderr,"\n\t*Loaded Vocabulary: %u words, %d bytes", wcsa->nwords, totaltmp);
		}
		fprintf(stderr,"\n\t... Done: %2.2f seconds (sys+usr t)\n", getSYSTimeBF() -t1);		

	/** 2 ** Loads some configuration constants: sourceTextSize, maxNumOccs */
	loadIndexConstants(Index, inbasename);
	fprintf(stderr,"\n\t*Loaded  configuration constants: %lu bytes\n", (ulong) (2 * sizeof(uint ) + sizeof(ulong)) );		
/*
	//shows the words parsed...
	{
		int i;
		fprintf(stderr,"\n\n Despues de sorting ....");	fflush(stderr);
		unsigned char *str;
		uint len;
//		for (i = 0; i<100; i++) {
		for (i = 0; ((uint)i)<wcsa->nwords; i++) {
			if ((i<10) || (((uint)i) >wcsa->nwords-5)) {
				getWord(wcsa,i,&str,&len);				
				fprintf(stderr,"\n freq[%6d]=%6u ",i,  wcsa->freqs[i]);
				fprintf(stderr,", words[%6d] = ",i);
				printWord(str,len);
			}
		}		
	}

	t1 = getSYSTimeBF();
	fprintf(stderr,"\n %u words have been loaded", wcsa->nwords);
	fprintf(stderr,"\n... Done: %2.2f seconds (sys+usr time)\n", getSYSTimeBF() -t1); 					
*/	

	#ifdef FREQ_VECTOR_AVAILABLE
	/** 3 ** Loading freq vector */
	{uint size; //the size of the vocabulary in #ofwords =>> already init in "loadvocabulary"
	loadFreqVector(&(wcsa->freqs), &size, (char *)inbasename);	
	fprintf(stderr,"\t*Loaded freq vector: %d bytes\n", wcsa->nwords * sizeof(uint) );
	}		
	#endif	
	
	/** 5 ** Loading the Representation of the source text */
	load_representation( &wcsa->ct,inbasename); 
	{
		uint size;
		size_representation(wcsa->ct, &size);	
		fprintf(stderr,"\n\t*Loaded (compressed) representation of the source Text: %u bytes\n", size);
	}
		
	/** 4 ** Loading the uncompressed posting lists previously created by the indexer. */
	
		//Preparing a "list of occurrences" that will be later indexed through build_il() **
		uint *source_il, sourcelen_il;
		uint maxPost ; //just to check it loads OK ;)
		ulong source_il_ulong;

		t1 = getSYSTimeBF();
		fprintf(stderr,"\n... Loading the posting lists from disk \n"); fflush(stderr);	
		
		load_posting_lists_from_file(&maxPost, &source_il_ulong, &source_il, inbasename);

		/** FOR CIKM ILISTS_DO NOT STILL SUPPORT an ULONG HERE **/
		sourcelen_il = (uint)source_il_ulong;
		
		fprintf(stderr,"\n there are %lu uints in all the lists of occurrences: size [uint32] = %lu bytes.\n ", 
		               (ulong)sourcelen_il - wcsa->nwords -2, (ulong) sizeof(uint)*(sourcelen_il - wcsa->nwords -2));
		fprintf(stderr,"\n MAXPOST loaded = %u, source_il_len = %u \n\n",maxPost,sourcelen_il);
		fprintf(stderr,"\n NLISTS loaded = %u, MAXPOSTS_sET \n\n",source_il[0],source_il[1]);

		
		
		t1 = getSYSTimeBF();
		fprintf(stderr,"\n**... entering BUILD INVERTED LIST REPRESENTATION************!! \n\n"); fflush(stderr);	
/*
		{ char fileposts[2048];
			sprintf(fileposts,"%s.%s.%u","postinglistsXX","posts", getpid());
			FILE *ff = fopen(fileposts,"w");
			fwrite(source_il, sizeof(uint), sourcelen_il,ff);
			fclose(ff);
			
		}
*/
		//compressing the lists of occurrences and setting wcsa->ils
		int error = build_il(source_il, sourcelen_il, build_options, &(wcsa->ils));  //source_il is freed inside!.
		IFERRORIL(error);							

		fprintf(stderr,"\n... Done: %2.2f seconds (sys+usr time)\n", getSYSTimeBF() -t1);
		
	
	#ifndef FREQ_VECTOR_AVAILABLE   //<----- not needed in advance, only during construction
		free(wcsa->freqs);
	#endif

		
	ulong sizeI;
	index_size(*index, &sizeI);
	fflush(stderr); fflush(stdout);
	fprintf(stderr,"\n ---------------------------------------------");
	fprintf(stderr,"\n The index has already been built: %lu bytes!!\n", sizeI);
	fprintf(stderr,"\n... Done: OVERALL TIME = %2.2f seconds (sys+usr time)", getSYSTimeBF() -t0);
	fprintf(stderr,"\n ---------------------------------------------\n\n\n");
	fflush(stderr);
	fflush(stdout);
	return 0;
}







/** ****************************************************************
  * Querying the index WCSA
  * ***************************************************************/
///////////////////////////////////////////////////////////////////////////////////////
//					   FUNCTIONS NEEDED FOR SEARCHING FOR A PATTERN    					 //
///////////////////////////////////////////////////////////////////////////////////////


/** returns into word a pointer to wcsa->vocq.zone, for the i-th qgram */
void getWord(twcsa *wcsa, uint ith, unsigned char **word, uint *lenword) {
	register uint len, tmplen;
	len = bitread (wcsa->wordsData.words, (ith+1)* wcsa->wordsData.elemSize , wcsa->wordsData.elemSize);
	tmplen = bitread (wcsa->wordsData.words, (ith )* wcsa->wordsData.elemSize  , wcsa->wordsData.elemSize);
	len -=tmplen;
	*word= (unsigned char *) wcsa->wordsData.wordsZoneMem.zone + tmplen;	
	*lenword = len;
}




/** Given a word, retuns the position in "words[] array" or -1 if not found there. */
int parseWordIntoId(twcsa *wcsa, byte *aWord, uint len) { 		
	register uint min,max,p;	
	unsigned char *vocword;
	uint vocwordlen;
	// Binary search on the sorted array of words
	min = 0;
	max = (wcsa->nwords) - 1;
	while(min < max) {
		p = (min+max)/2;
		getWord(wcsa,p,&vocword,&vocwordlen);
		if(wtstrcompL(aWord, vocword, len, vocwordlen) >0 ) min = p+1;
		else max = p;
	}
	
	//getQgram(wcsa,min,qgram,len);
	getWord(wcsa,min,&vocword,&vocwordlen);
	if(!wtstrcompL(aWord, vocword, len, vocwordlen)) {
		 return min;
	}
	else {   //!SO NO WORD IS FOUND !!
		return -1;
	} 
}

int parseWordIntoIdDebug(twcsa *wcsa, byte *aWord, uint len) { 		
	register uint min,max,p;	
	unsigned char *vocword;
	uint vocwordlen;
	
	fprintf(stderr,"\n call to parseWordIntoIdDebug");
	fprintf(stderr,"\n wordsize = %u",len);
	fprintf(stderr,"\n word = ");
	printWord(aWord,len);
	fprintf(stderr,"\n");
		
	min = 0;
	max = (wcsa->nwords) - 1;
	while(min < max) {
		p = (min+max)/2;
		getWord(wcsa,p,&vocword,&vocwordlen);
		if(wtstrcompL(aWord, vocword, len, vocwordlen) >0 ) min = p+1;
		else max = p;
	}

	fprintf(stderr,"\n pos found = %u, max was %u",min,(wcsa->nwords) - 1);
	
	//getQgram(wcsa,min,qgram,len);
	getWord(wcsa,min,&vocword,&vocwordlen);

	fprintf(stderr,"\n Must compare (len = %u) ",len);
	printWord(aWord,len);
	fprintf(stderr,"\n with (len=%u) ",vocwordlen);
	printWord(aWord,len);
	

	if(!wtstrcompL(aWord, vocword, len, vocwordlen)) {
		 return min;
	}
	else {   //!SO NO WORD IS FOUND !!
		return -1;
	} 
}



/*------------------------------------------------------------------
 * Given a Sequence of words, retuns the position in "words[] array" of each word. 
 * memory for integerPattern is not allocate, sizeIntegers is usually a constant, and bounded previously
 * Returns ids of the words, and the number of ids in "integerPattern[]" 
 ------------------------------------------------------------------*/
void parseTextIntoIntegers(twcsa *wcsa, byte *textPattern, uint patLen, uint *integerPattern, uint *sizeIntegers) {
	
	byte *pbeg,*pend,*wordstart,*aWord;
	register unsigned long size;
	uint index =0; 
		 	
	pbeg = textPattern; 
	pend = pbeg + patLen;
							
	while (pbeg <pend) {  
		//parsing either a word or separator.			
		size=0;
		wordstart = pbeg;
		if (_Valid[*pbeg]) {   //alphanumerical data
			while ( (size<MAX_SIZE_OF_WORD) && (pbeg<pend)&& ( _Valid[*pbeg] )) {size++;pbeg++;}
	  }		    
		else {
			if (*pbeg != ' ') { //a separator comes starting in ' ' comes, so it is a new word					
				while ( (size<MAX_SIZE_OF_WORD) && (pbeg<pend) &&  (!_Valid[*pbeg] )) {size++;pbeg++;}
			}
			else {  //a  SPACE comes, so we have to test if next character is alphanumerical or not
				pbeg++;
				if (pbeg >= pend) {size++;}  // a unique BLANK at the end of the file.
				else {
					if (_Valid [*pbeg] ) {
						wordstart = pbeg;   //So skipping 1 blank character
						while ( (size<MAX_SIZE_OF_WORD) && (pbeg<pend)&& ( _Valid[*pbeg] )) {size++;pbeg++;}
					}
					else {   // a "separator word" ...
						size++; //the prev BLANK...
						while ( (size<MAX_SIZE_OF_WORD) && (pbeg<pend) &&  (!_Valid[*pbeg] )) {size++;pbeg++;}
					}//else {  // a "separator word"
				}//else ... not a unique BLANK AT THE END.
			}//else ... starting by a BLANK... 
		}
				  		
		//The parsed word is "aWord", and its length is "size"...
		aWord=wordstart;	
		
		// Binary search on the canonical words (wordsData)
		{
			uint len, tmplen;
			uchar *wcsaWord;
			register uint min,max,p;	
			min = 0;
			max = (wcsa->nwords) - 1;
			while(min < max) {
				p = (min+max)/2;
				
				{//preparing for strcompL
					len    = bitread (wcsa->wordsData.words, (p+1)* wcsa->wordsData.elemSize , wcsa->wordsData.elemSize);
					tmplen = bitread (wcsa->wordsData.words, (p )* wcsa->wordsData.elemSize  , wcsa->wordsData.elemSize);
					len -=tmplen;
					wcsaWord= (byte *) wcsa->wordsData.wordsZoneMem.zone + tmplen;
				}
				
				//if(strncmp((char*)aWord, (char*)wcsa->wordsData[p].word,size) > 0) min = p+1;
				if(wtstrcompL(aWord, wcsaWord, size, len) > 0) min = p+1;
				else max = p;


				//				{ //SHOW PROGRESS
				//					fprintf(stderr,"\n Patron = <<%s>>, curposWord= %d ==><<",aWord,p);
				//					printWord(wcsaWord,len); fprintf(stderr,">> len =%d",len);
				//				}
					
			}
			
			{//preparing for strcompL
				len    = bitread (wcsa->wordsData.words, (min+1)* wcsa->wordsData.elemSize, wcsa->wordsData.elemSize);
				tmplen = bitread (wcsa->wordsData.words, ( min )* wcsa->wordsData.elemSize, wcsa->wordsData.elemSize);
				len -=tmplen;
					wcsaWord= (byte *) wcsa->wordsData.wordsZoneMem.zone + tmplen;
			}

			//	if(!strncmp((char*)aWord, (char*)wcsa->wordsData[min].word, size)) {
			if(!wtstrcompL(aWord, wcsaWord, size, len)) {
				integerPattern[index++] = min ;//+1 ;  //<--
			}
			else {*sizeIntegers = 0; return;}  // a valid word that does not appear in the source text.
			
		}														
	}// end while
	*sizeIntegers = index;	
	
	//	//shows the parsed words:
	//	{uint i;
	//		printf("\n\n >>%s>> HA SIDO PARSEADO COMO:",textPattern);
	//		for (i=0; i<index;i++) {
	//				printf("<<%s>>",wcsa->wordsData[integerPattern[i] -1].word);
	//		}
	//		
	//	}	
}


/** ------------------------------------------------------------------
 * Given a Sequence of words, retuns the position in "words[] array" of each word. 
 * memory for integerPattern is not allocate, sizeIntegers is usually a constant, and bounded previously
 * Returns ids of the words, and the number of ids in "integerPattern[]" 
 * The memory of uint *integerPattern must not be freed by the caller 
 *------------------------------------------------------------------ */
int index_parseTextIntoIntegers(void *index, byte *textPattern, uint patLen, uint **integerPattern, uint *sizeIntegers) {
	static uint ids[MAX_INTEGER_PATTERN_SIZE];

	*integerPattern = ids;	
	twcsa *wcsa=(twcsa *) index;
	*sizeIntegers=0;
	parseTextIntoIntegers(wcsa, textPattern, patLen, ids, sizeIntegers);	

//	  fprintf(stderr,"\n:: IDtras parse = ");
//	  {uint k;
//		for (k=0;k<*sizeIntegers;k++)
//		fprintf(stderr,"[%u]",ids[k]);
//	  }
//	  fprintf(stderr,"");
//	  unsigned char * str; uint llen;
//	  getWord(wcsa,ids[0],&str,&llen);				
//	  printWord(str,llen);
//	  fprintf(stderr,")");
	
	return 0;
}




/** ------------------------------------------------------------------
 * displayTextOcurrences:
 * Shows in stdout, the text around the occurrences of a word/phrase
 * Returns also the number of occurrences.
 * radixDisplay is the numberOfWords from that are decoded previosly and after each occurrence
 *------------------------------------------------------------------ */
int displayTextOcurrences(void *index, uchar *pattern, ulong length, uint radixDisplay) {
	return 99;
}	
	
int  displayTextOcurrencesNoShow(void *index, uchar *pattern, ulong length, uint radixDisplay, ulong *numoccs) {
	return 99;
}





/** ------------------------------------------------------------------ 
 * Returns the list of documents containing the nids terms represented by ids[] 
 *------------------------------------------------------------------ */
int index_listDocuments(void *index, uint *ids, uint nids, uint **docs, uint *ndocs) {
	twcsa* wcsa = (twcsa *) index;	
	int retval;
	//intersecting lists or extract if only one
	uint *candDocs;

	if (nids ==1) {
		retval= extractList_il(wcsa->ils, ids[0], &candDocs, ndocs);
	}
	else if (nids ==2) {
		retval = intersect_2_il(wcsa->ils, ids[0],ids[1], ndocs, &candDocs);
	}
	else {
		retval = intersect_N_il (wcsa->ils, ids,nids , ndocs, &candDocs);
	}  
	
	*docs =candDocs;	
	return 0;
}




double getSYSTimeBF (void)
{
	double usertime, systime;
	struct rusage usage;

	getrusage (RUSAGE_SELF, &usage);
	usertime = (double) usage.ru_utime.tv_sec + (double) usage.ru_utime.tv_usec / ( ((double) 1000000.0));
	systime = (double) usage.ru_stime.tv_sec + 	(double) usage.ru_stime.tv_usec / ( ((double)1000000.0));
	return (usertime + systime);
	//return (usertime );
}
