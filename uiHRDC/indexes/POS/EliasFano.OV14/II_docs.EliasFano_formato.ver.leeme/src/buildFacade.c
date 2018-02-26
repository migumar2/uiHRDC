#include "buildFacade.h"


#include "utils/parseparamsII.c"  //parsing a string with parameters.
#include "utils/scdc.c"			  //decoding SCDC
#include "utils/errors.c"
#include "utils/ii.c"             //some auxiliar funcitons used during construction (also load/save)



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

/**  Saves index on disk by using single or multiple files, having 
	proper extensions. */
int save_index (void *index, char *filename) {
	char *basename = filename;
	twcsa *wcsa=(twcsa *) index;

	fprintf(stderr,"\nSaving structures to disk: %s.*\n",basename);			
	fflush(stderr);

	// Saves the compressed text from SCDC and its vocabulary
	//saveTextInMem (wcsa->text, wcsa->textSize, basename);   
	saveVocabulary (index, basename);

	// Saves some configuration constants: maxNumOccs, sourceTextSize */
	saveIndexConstants (index, filename);
	
	#ifdef FREQ_VECTOR_AVAILABLE	
	// saves freqVector do disk
	saveFreqVector (wcsa->freqs,   wcsa->nwords, basename);	
	#endif
	
	// saves the doc_offsets_sids array.
	saveDocOffsetsSids(index,filename);			
				
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
		totaltmp += ((((wcsa->nwords+1)* (wcsa->wordsData.elemSize))+WI32-1) /WI32) * (sizeof(uint));  //the pointers
		totaltmp += wcsa->wordsData.wordsZoneMem.size * sizeof(byte); //the characters of the words.	
		fprintf(stderr,"\n\t*Loaded Vocabulary of text: %u words, %d bytes\n", wcsa->nwords, totaltmp);
		}
		
	/** 2 ** Loads some configuration constants: sourceTextSize, maxNumOccs */
	loadIndexConstants(Index, filename);
	fprintf(stderr,"\t*Loaded  configuration constants: %lu bytes\n", (ulong) (2 * sizeof(uint)) );
	fprintf(stderr,"\t\t\t: ndocs = %lu bytes\n", (ulong) wcsa->ndocs );

	#ifdef FREQ_VECTOR_AVAILABLE
	/** 3 ** Loading freq vector */
	{uint size; //the size of the vocabulary in #ofwords =>> already init in "loadvocabulary"
	loadFreqVector(&(wcsa->freqs), &size, (char *)filename);	
	fprintf(stderr,"\t*Loaded freq vector: %lu bytes\n", (ulong) wcsa->nwords * sizeof(uint) );
	}		
	#endif

	/** 4 saves the doc_offsets_sids array. */
	loadDocOffsetsSids(Index,filename);	  //wcsa->doc_offsets_sids[]
	
	//fprintf(stderr,"\n The %u ranks of the document beginnings are:", wcsa->ndocs);
	//{
	//  int i;
	//  for (i=0;i<=wcsa->ndocs;i++) 		fprintf(stderr,"[%u-th-> %u]",i,wcsa->doc_offsets_sids[i]); 
	//}	

	/** 5 ** Loading Compressed Structure of posting lists (il) */
	error = load_il((char*) filename,&(wcsa->ils));
 	IFERRORIL(error);
 	uint sizeil;
 	error = size_il(wcsa->ils,&sizeil);
 	IFERRORIL(error);
	fprintf(stderr,"\n \t*loaded compressed inverted lists structure: %d bytes\n", sizeil);
	
	/** 6 ** Loading the Representation of the source text */
	load_representation( &wcsa->ct,filename); 
	{
		uint size;
		size_representation(wcsa->ct, &size);	
		fprintf(stderr,"\n\t*Loaded (compressed) representation of the source Text: %u bytes\n", size);
	}
	
	(*index) = Index;	
	//checkDocMerge(Index) ;exit(0);
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

	if (wcsa->doc_offsets_sids) free(wcsa->doc_offsets_sids);

	if (wcsa->ils) free_il(wcsa->ils);
	if (wcsa->text) free(wcsa->text);
		
	//delete wcsa->rp;
	free_representation(wcsa->ct);
	
	free(wcsa);
	return 0;
}

	/** Returns the size of the representation of the inverted lists **/
int index_getsize_il(void *index, uint *sizeil) {
	twcsa *wcsa=(twcsa *)index;	 
	int error = size_il(wcsa->ils, sizeil);
	return error;  
}
	
	/** Returns the size of the representation of the compressed text **/
int index_getsize_ct(void *index, uint *sizect){
	twcsa *wcsa=(twcsa *)index;	  
	int error = size_representation(wcsa->ct, sizect);
	return error;
}


	/** Gives the memory occupied by index in bytes. */
int index_size(void *index, ulong *size) {
	twcsa *wcsa=(twcsa *)index;
	*size=0;
	*size += sizeof(twcsa);

	uint totalVoc=0;  //Vocabulary of words.
	totalVoc += ((((wcsa->nwords+1)* (wcsa->wordsData.elemSize))+WI32-1) /WI32) * (sizeof(uint));  //the pointers
	totalVoc += wcsa->wordsData.wordsZoneMem.size * sizeof(byte); //the characters of the words.
	*size += totalVoc;
	
	#ifdef FREQ_VECTOR_AVAILABLE
	*size += wcsa->nwords * sizeof(uint) ;  //freqs
	#endif


	uint sizeil; 
	int error = size_il(wcsa->ils, &sizeil);
	IFERRORIL(error);
	*size += sizeil;

	uint sizerep;
	size_representation(wcsa->ct, &sizerep);
	*size += sizerep;	//the size of the representation of the source text 
	
	*size += (wcsa->ndocs+1) * sizeof(uint) ;  // doc_offsets_sids array.

	//printf("\n *************SIZE OF INDEX = %d bytes ",*size);
	return 0;	
}

/** returns detailled info of the data structures of the index **/
int index_info(void *index, char msg[]) {
	twcsa *wcsa=(twcsa *)index;
	char msgt[1000];
	uint sizeil; 
	int error = size_il(wcsa->ils, &sizeil);
	IFERRORIL(error);
	
	ulong indexs;
	index_size(index,&indexs);
	
	ulong Text_length;
	get_length(index, &Text_length);	


	uint totalVoc=0;  //Vocabulary of words.
	totalVoc += ((((wcsa->nwords+1)* (wcsa->wordsData.elemSize))+WI32-1) /WI32) * (sizeof(uint));  //the pointers
	totalVoc += wcsa->wordsData.wordsZoneMem.size * sizeof(byte); //the characters of the words.
	
	uint size_doc_offsets_sids = sizeof (uint) * (wcsa->ndocs+1);
		
	uint sizerepres; //Compressed representation of the source text
	size_representation(wcsa->ct, &sizerepres);
		
	sprintf(msg,"\n\tSpace statistics for II-words-DocOriented\n");
	sprintf (msgt,"\t tindex structure = %lu bytes.\n",(ulong)sizeof(twcsa) );	strcat(msg,msgt);
    sprintf (msgt,"\t number of diff words= %u\n",wcsa->nwords );	strcat(msg,msgt);	
    sprintf (msgt,"\t number of docs (ndocs)= %u\n",wcsa->ndocs );	strcat(msg,msgt);	
	
	
	sprintf (msgt,"\t vocabulary of words = %u bytes.\n", totalVoc );	strcat(msg,msgt);
	sprintf (msgt,"\t Mapping Docs-2-wordposition = %u bytes.\n", size_doc_offsets_sids);	strcat(msg,msgt);
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
  /*
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
*/
	return 99;
}




	/* Writes in numocc the number of occurrences of the substring 
	  pattern[0..length-1] in the text indexed by index. It also allocates
	  occ (which must be freed by the caller) and writes the locations of 
	  the numocc occurrences in occ, in arbitrary order.  */	  
/*
int locate(void *index, uchar *pattern, uint length, uint **occs, uint *noccs){
	uint integerPatterns[MAX_INTEGER_PATTERN_SIZE];
	uint integerPatternSize,occurrences;	
	twcsa *wcsa=(twcsa *) index;

//	fprintf(stderr,"\nTO LOCATE: %s, len=%u",pattern,length);
	parseTextIntoIntegers(wcsa, pattern, length, integerPatterns, &integerPatternSize);	
	
	if (!integerPatternSize) {*noccs=0; *occs=NULL; return 0;} //not found 

	uint *ids = integerPatterns;
	uint nids = integerPatternSize;
	
	int retval;
	//intersecting lists or extract if only one
	uint *candoccs;
	
	if (nids ==1) {
		retval= extractList_il(wcsa->ils, ids[0], &candoccs, noccs);
	}
	else if (nids ==2) {
		//printf("\n\n\n call to intersect 2 \n\n\n");  fflush(stderr);fflush(stdout);
		retval = intersect_2_il(wcsa->ils, ids[0],ids[1], noccs, &candoccs);
	}
	else {
		retval = intersect_N_il (wcsa->ils, ids,nids , noccs, &candoccs);
		//*noccs=0; candoccs=NULL; retval=0;
	}  
	
		////// SHOWS THE RESULTS OF THE SEARCH
		////{int i;
		//// FILE *log = fopen("log.il.txt","w");
		//// uint *l,n;
		//// for (i=0;i<nids;i++) {
		////	l=NULL;
		////	fprintf(log,"\n-------------------------- list %u -----------------\n",ids[i]);
		////	retval= extractList_il(wcsa->ils, ids[i], &l, &n);
		////	uint j;
		////	for (j=0;j<n;j++)
		////	  fprintf(log, "%u\n",l[j]);
		////	if (l) free (l);
		//// 
		//// }
		//// fclose (log);
		////}
			

		//	ulong tot=0;
		//	for (int i=0;i<*noccs;i++){ printf("\n occ[%3d] = %9u ..... %15lu",i,candoccs[i],tot); tot+=candoccs[i];}
				
	// ** Now using the doc-boundaries, 
	// ** Adds to occs (in positions noccs+0, noccs+1, noccs+2, ... the doc-ids for each occ]
	if (*noccs)
	  addDocIdsToOccs(&candoccs, *noccs, wcsa->doc_offsets_sids, wcsa->ndocs);	
	*occs =candoccs;	

	//if (*noccs != wcsa->freqs[ids[0]]) exit(0);
	return 0;
	
}
*/


int locate(void *index, uchar *pattern, uint length, uint **occs, uint *noccs){
	uint integerPatterns[MAX_INTEGER_PATTERN_SIZE];
	uint integerPatternSize,occurrences;	
	twcsa *wcsa=(twcsa *) index;

//	fprintf(stderr,"\nTO LOCATE: %s, len=%u",pattern,length);
	parseTextIntoIntegers(wcsa, pattern, length, integerPatterns, &integerPatternSize);	
	
	if (!integerPatternSize) {*noccs=0; *occs=NULL; return 0;} //not found 

	uint *ids = integerPatterns;
	uint nids = integerPatternSize;
	
	int retval;
	//intersecting lists or extract if only one
	uint *candoccs;
	
	if (nids ==1) {
		retval= extractList_il(wcsa->ils, ids[0], &candoccs, noccs);
	}
	else if (nids ==2) {
		//printf("\n\n\n call to intersect 2 \n\n\n");  fflush(stderr);fflush(stdout);
		retval = intersect_2_il(wcsa->ils, ids[0],ids[1], noccs, &candoccs);
	}
	else {
		retval = intersect_N_il (wcsa->ils, ids,nids , noccs, &candoccs);
		//*noccs=0; candoccs=NULL; retval=0;
	}  
	
			/*// SHOWS THE RESULTS OF THE SEARCH
			{int i;
			 FILE *log = fopen("log.il.txt","w");
			 uint *l,n;
			 for (i=0;i<nids;i++) {
				l=NULL;
				fprintf(log,"\n-------------------------- list %u -----------------\n",ids[i]);
				retval= extractList_il(wcsa->ils, ids[i], &l, &n);
				uint j;
				for (j=0;j<n;j++)
				  fprintf(log, "%u\n",l[j]);
				if (l) free (l);
			 
			 }
			 fclose (log);
			}
			 **/

		//	ulong tot=0;
		//	for (int i=0;i<*noccs;i++){ printf("\n occ[%3d] = %9u ..... %15lu",i,candoccs[i],tot); tot+=candoccs[i];}
				
	// ** Now using the doc-boundaries, 
	// ** Adds to occs (in positions noccs+0, noccs+1, noccs+2, ... the doc-ids for each occ]
	if (*noccs)
	  addDocIdsToOccs(&candoccs, *noccs, wcsa->doc_offsets_sids, wcsa->ndocs);	
	*occs =candoccs;	

	//if (*noccs != wcsa->freqs[ids[0]]) exit(0);
	return 0;
	
}

/*******************/

/*
 * Hace el parseTextIntoIntegers y el extract/intersect, pero no el addDOIDSTOOCCS.
 * Devuelve occs[] y noccs, que serán usados para llamar a locate2 
 *        (donde se miden tiempos de parse y addDOCIDSTOOCCS solamente)
 */
int locate1(void *index, uchar *pattern, uint length, uint **occs, uint *noccs){
	uint integerPatterns[MAX_INTEGER_PATTERN_SIZE];
	uint integerPatternSize,occurrences;	
	twcsa *wcsa=(twcsa *) index;

	parseTextIntoIntegers(wcsa, pattern, length, integerPatterns, &integerPatternSize);	
	//if (!integerPatternSize)
	//	printf("\n pattern of len %u <<%s>> could not be parsed into ids (skipped)\n",length,pattern); 
		
	if (!integerPatternSize) {*noccs=0; *occs=NULL; return 0;} //not found 

	uint *ids = integerPatterns; uint nids = integerPatternSize;
	
	int retval;	uint *candoccs;	
	if (nids ==1) {
		retval= extractList_il(wcsa->ils, ids[0], &candoccs, noccs);
	}
	else if (nids ==2) {
		retval = intersect_2_il(wcsa->ils, ids[0],ids[1], noccs, &candoccs);
	}
	else {
		retval = intersect_N_il (wcsa->ils, ids,nids , noccs, &candoccs);
	}  
	
			//prints the result of the extract/intersect of lists.
			{int i;
			 FILE *log = fopen("log.il.txt","w");
			 uint *l,n;
			 fprintf(log,"NOCCS=%u\n\t",*noccs);
			 for (i=0;i<*noccs;i++) {
				  fprintf(log, "[%u]",candoccs[i]);			 
			 }
			 fclose (log);
			}

/* SKIPS ADDDOCIDSTOOCCS				
	// ** Now using the doc-boundaries, 
	// ** Adds to occs (in positions noccs+0, noccs+1, noccs+2, ... the doc-ids for each occ]
	
	if (*noccs)
	  addDocIdsToOccs(&candoccs, *noccs, wcsa->doc_offsets_sids, wcsa->ndocs);	
*/
	*occs =candoccs;	
	return 0;	
}

/*
 * Hace el parseTextIntoIntegers y el addDOIDSTOOCCS. 
 * Nota, no llama a extract/intersect puesto que usará los valores occs[0..noccs-1] obtenidos con "locate1"
 * Así podemos medir tiempos de parse y addDOCIDSTOOCCS solamente, para sumárselos a los tiempos 
 * de intersección que obtiene el partitioned-elias-fano index (sigir-2014-paper).
 */

int locate2(void *index, uchar *pattern, uint length, uint **occs, uint *noccs){
	uint integerPatterns[MAX_INTEGER_PATTERN_SIZE];
	uint integerPatternSize,occurrences;	
	twcsa *wcsa=(twcsa *) index;

	parseTextIntoIntegers(wcsa, pattern, length, integerPatterns, &integerPatternSize);		
	if (!integerPatternSize) {*noccs=0; *occs=NULL; return 0;} //not found 

	uint *ids = integerPatterns; uint nids = integerPatternSize;
	
	uint *candoccs;	

	/* SKIPS CALL TO EXTRACT/INTERSECT, as it uses the results from *occs.
	int retval;	
		if (nids ==1) {
			retval= extractList_il(wcsa->ils, ids[0], &candoccs, noccs);
		}
		else if (nids ==2) {
			retval = intersect_2_il(wcsa->ils, ids[0],ids[1], noccs, &candoccs);
		}
		else {
			retval = intersect_N_il (wcsa->ils, ids,nids , noccs, &candoccs);
		}  
	*/
	candoccs= *occs;
					
	// ** Now using the doc-boundaries, 
	// ** Adds to occs (in positions noccs+0, noccs+1, noccs+2, ... the doc-ids for each occ]
	if (*noccs)
	  addDocIdsToOccs(&candoccs, *noccs, wcsa->doc_offsets_sids, wcsa->ndocs);	
	*occs =candoccs;	

	return 0;	
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
    twcsa *wcsa=(twcsa *) index;    

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

	twcsa *wcsa=(twcsa *) index;
				
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
	 CONSTRUCTION OF THE INDEX 
    ***********************************************************************************/
int build_WordIndex (char *inbasename, char *build_options, void **index){
	twcsa *wcsa;
	wcsa = (twcsa *) malloc (sizeof (twcsa) * 1);
	*index = wcsa;
	wcsa->text = NULL;

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
	loadTextInMem(&(wcsa->text), &(wcsa->textSize),(char *)inbasename);	
	wcsa->sourceTextSize = wcsa->textSize;
	fprintf(stderr,"... Loaded Source Sequence: %u bytes\n", wcsa->textSize); fflush(stderr);
fflush(stderr);
	/** 2 ** loads the array of document boundaries                           */
	uint ndocs;
	uint *docboundaries;
	loadDocBeginngins(&docboundaries, &ndocs,(char *)inbasename);	
	wcsa->ndocs = ndocs; //just for statistics.

	
	/** 3 ** Parses the sequence and gathers the vocabulary of words (sorted alphanumerically) 
		the frecuency of such words: obtains "words", "nwords", and "wordsZone" 
		Sets also wcsa->freqs (freq of each word)
		Sets also wcsa->maxNumOccs (needed for malloc during extraction) */	
	
	fprintf(stderr,"... Entering CreateVocabularyOfWords \n"); fflush(stderr);
	CreateVocabularyOfWords(*index, docboundaries, ndocs);


	//shows the words parsed...
	{
		int i;
		fprintf(stderr,"\n\n Después de sorting ....");	fflush(stderr);
		unsigned char *str;
		uint len;
//		for (i = 0; i<100; i++) {
		for (i = 0; i<wcsa->nwords; i++) {
			if ((i<15) || (i>wcsa->nwords-5)) {
				getWord(wcsa,i,&str,&len);				
				fprintf(stderr,"\n freq[%6d]=%6u ",i,  wcsa->freqs[i]);
				fprintf(stderr,", words[%6d] = ",i);
				printWord(str,len);
			}
		}		
	}

	fprintf(stderr,"\n %u words have been parsed", wcsa->nwords);
					
	
	/** 4 ** creates a temporal list of occurrences of each word (block-oriented).
					gives also the len of each list */
	{	
		//decompression of the source text and creation of occList[][] and lenList[]
		uint **occList; uint *lenList;    
		uint *doc_offsets_sids;
		
		uint **tfd; uint *idsindoc;
		createListsOfOccurrences (*index, &occList, &tfd, &idsindoc, &lenList, &doc_offsets_sids, docboundaries, ndocs);
		wcsa->doc_offsets_sids=doc_offsets_sids;




		uint ndocs_old = ndocs;
		ndocs = doc_offsets_sids[ndocs];


		{
			//outputs DOC-POSTINGS.
			//[1][nDOCS]
			//[LEN1][ID1][ID2][...ID_LEN1]
			//[LEN2][ID1][ID2][...ID_LEN2]
			//[LEN3][ID1][ID2][...ID_LEN3]
			
			FILE *f = fopen ("out.docs","w");
			uint uno=1;
			fwrite(&uno,sizeof(uint),1,f);
			fwrite(&ndocs,sizeof(uint),1,f);
			uint nwords = wcsa->nwords;
			ulong i,j;	

			for (i=0;i<nwords;i++) {
				fwrite(&(lenList[i]),sizeof(uint),1,f);
				for (j=0;j<lenList[i];j++) {
					uint docid=occList[i][j];
					fwrite(&docid,sizeof(uint),1,f);
				}
			}
			fclose(f);
		}	
		
		{
			//outputs TERM-FREQ-PER-DOC.
			//[LEN1][freqinID1][freqinID2][...freqinID_LEN1]
			//[LEN2][freqinID1][freqinID2][...freqinID_LEN2]
			//[LEN3][freqinID1][freqinID2][...freqinID_LEN3]
			
			FILE *f = fopen ("out.freqs","w");
			//fwrite(&i,sizeof(uint)1,f);
			//fwrite(&ndocs,sizeof(uint)1,f);
			uint nwords = wcsa->nwords;
			ulong i,j;	
			for (i=0;i<nwords;i++) {
				fwrite(&(lenList[i]),sizeof(uint),1,f);
				for (j=0;j<lenList[i];j++) {
					uint frqINdoc=tfd[i][j];
					fwrite(&frqINdoc,sizeof(uint),1,f);
				}
			}
			fclose(f);
		}	

		{
			//outputs NUM_IDS_PER_DOC.
			//[ndocs][ids_in_1]...[ids_in_ndocs]
			
		
			FILE *f = fopen ("out.sizes","w");
			//fwrite(&i,sizeof(uint),1,f);
			fwrite(&ndocs,sizeof(uint),1,f);
			ulong i;	uint uno=1;

			for (i=0;i<ndocs;i++) {
//				fwrite(&(idsindoc[i]),sizeof(uint),1,f);    //NO FUNCIONA EN EL POSICIONAL, PUES TENGO QUE HACER QUE CADA POSICIÓN SEA UN DOC DIFERENTE
				fwrite(&(uno),sizeof(uint),1,f);
			}
			fclose(f);
		}	

		fprintf(stderr,"\n files out.docs, out.freqs, out.sizes created successfully\n");
		//exit(0);


		ndocs = ndocs_old;



		

		fprintf(stderr,"\n %u lists of occurrences were created.", wcsa->nwords);fflush(stderr);
		//fprintf(stderr,"\n The ranks of the document beginnings are:");
		//{
		//  int i;
		//  for (i=0;i<=ndocs;i++) 		fprintf(stderr,"[%u-th-> %u]",i,wcsa->doc_offsets_sids[i]);
		//}

		//Preparing a "list of occurrences" that will be later indexed through build_il() **
		uint *source_il, sourcelen_il;
		//uint maxPost = ndocs; //!!
		uint maxPost = doc_offsets_sids[ndocs];
		uint nwords = wcsa->nwords;
		prepareSourceFormatForIListBuilder(nwords,maxPost,lenList, occList, &source_il, &sourcelen_il);

/*	
	char fileuintpostings[256] = "postingsSequence.uint32";
	output_posting_lists_concatenated_DEBUGGING_ONLY (nwords, maxPost, lenList, occList,fileuintpostings);	
*/
	
	
	FILE *ff = fopen ("postings.posts","w");
	fwrite (source_il, sizeof(uint), sourcelen_il, ff);
	fclose(ff);
	
#ifdef WRITE_POSTING_LIST		
		FILE *ff = fopen ("postings.posts","w");
		fwrite (source_il, sizeof(uint), sourcelen_il, ff);
		fclose(ff);
#endif		
		
		fprintf(stderr,"\n there are %lu uints in all the lists of occurrences: size [uint32] = %lu bytes.\n ", (ulong)sourcelen_il - nwords -2, (ulong) sizeof(uint)*(sourcelen_il - nwords -2));
		fprintf(stderr,"\n maxPostValue = %u",maxPost);
		  
		/*{ char fileposts[256];
			sprintf(fileposts,"%s.%s.%u","POSTING_LISTS","posts", getpid());
			FILE *ff = fopen(fileposts,"w");
			fwrite(source_il, sizeof(uint), sourcelen_il,ff);
			fclose(ff);
			
		}		
		for (int x=0;x<10;x++) printf("\n%u --> %u",x,source_il[x]);
		exit(0);
		
		*/
		
		fprintf(stderr,"\n the lists of occurrences were formatted for build_il.");fflush(stderr);


		//compressing the lists of occurrences and setting wcsa->ils
		int error = build_il(source_il, sourcelen_il, build_options, &(wcsa->ils));  //source_il is freed inside!.
		IFERRORIL(error);							


		{
			//frees memory for the posting lists
			uint i;
			for (i=0;i<wcsa->nwords;i++) free(occList[i]);
			free(occList);
			free(lenList);
		}
	
		/** 5 ** compressed representation of the source text */	
		{
			//unsigned long *docbegsUL; 
			unsigned int i;
			//docbegsUL = (ulong *) malloc (sizeof(ulong) * (ndocs+1));
			//for (i=0;i<=ndocs;i++) docbegsUL[i] = docboundaries[i];			
			//build_representation (wcsa->text, wcsa->textSize, docbegsUL, ndocs, build_options, &wcsa->ct);

			fprintf(stderr,"\nNow compressing the text: %lu bytes", (ulong)wcsa->textSize);fflush(stderr);
			
			build_representation (wcsa->text, wcsa->textSize, docboundaries, ndocs, build_options, &wcsa->ct);
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
	len = bitread_wi (wcsa->wordsData.words, (ith+1)* wcsa->wordsData.elemSize , wcsa->wordsData.elemSize);
	tmplen = bitread_wi (wcsa->wordsData.words, (ith )* wcsa->wordsData.elemSize  , wcsa->wordsData.elemSize);
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
					len    = bitread_wi (wcsa->wordsData.words, (p+1)* wcsa->wordsData.elemSize , wcsa->wordsData.elemSize);
					tmplen = bitread_wi (wcsa->wordsData.words, (p )* wcsa->wordsData.elemSize  , wcsa->wordsData.elemSize);
					len -=tmplen;
					wcsaWord= (byte *) wcsa->wordsData.wordsZoneMem.zone + tmplen;
				}
				
				//if(strncmp((char*)aWord, (char*)wcsa->wordsData[p].word,size) > 0) min = p+1;
				if(wtstrcompL(aWord, wcsaWord, size, len) > 0) min = p+1;
				else max = p;


							//	{ //SHOW PROGRESS
							//		fprintf(stderr,"\n \tPatron = <<%s>>, curposWord= %d ==><<",aWord,p);
							//		printWord(wcsaWord,len); fprintf(stderr,">> len =%d",len);
							//	}
					
			}
			
			{//preparing for strcompL
				len    = bitread_wi (wcsa->wordsData.words, (min+1)* wcsa->wordsData.elemSize, wcsa->wordsData.elemSize);
				tmplen = bitread_wi (wcsa->wordsData.words, ( min )* wcsa->wordsData.elemSize, wcsa->wordsData.elemSize);
				len -=tmplen;
					wcsaWord= (byte *) wcsa->wordsData.wordsZoneMem.zone + tmplen;
			}

			//	if(!strncmp((char*)aWord, (char*)wcsa->wordsData[min].word, size)) {
			if(!wtstrcompL(aWord, wcsaWord, size, len)) {
			    //fprintf(stderr,"\n********************************************************");
				//fprintf(stderr,"\n found word \"%s\", ------>id = %u",aWord,min);
				integerPattern[index++] = min ;//+1 ;  //<--
			}
			else {*sizeIntegers = 0; return;}  // a valid word that does not appear in the source text.
			
		}														
	}// end while
	*sizeIntegers = index;	
/*	
		//shows the parsed words:
		{uint i;
			fprintf(stderr,"\n\n >>%s>> HA SIDO PARSEADO COMO:",textPattern);
		  for (i=0; i<index;i++) {
				{//preparing for strcompL
					uint len, tmplen;
					uchar *wcsaWord;
					register uint min,max,p;	
					p=integerPattern[i];
					len    = bitread_wi (wcsa->wordsData.words, (p+1)* wcsa->wordsData.elemSize , wcsa->wordsData.elemSize);
					tmplen = bitread_wi (wcsa->wordsData.words, (p )* wcsa->wordsData.elemSize  , wcsa->wordsData.elemSize);
					len -=tmplen;
					wcsaWord= (byte *) wcsa->wordsData.wordsZoneMem.zone + tmplen;

					fprintf(stderr,"\n id= %d, word --> ",integerPattern[i]);
					printWord(wcsaWord,len); 
					//printf("<<%s>>",wcsa->wordsData[integerPattern[i] -1].word);

				}				
			}
			fprintf(stderr,"\nThe new pattern of ids is: ");fflush(stderr);fflush(stdout);
			for (i=0;i<index;i++) fprintf(stderr,"[%u]",integerPattern[i]);

			fprintf(stderr,"\n\n");fflush(stderr);fflush(stdout);
		}	
*/

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


/*
/** ------------------------------------------------------------------ 
 * Returns the list of documents containing the nids terms represented by ids[] 
 *------------------------------------------------------------------ */
int  index_listDocuments(void *index, uint *ids, uint nids, uint **occs, uint *noccs) {
  return 99;
}
/*
int  index_listDocuments(void *index, uint *ids, uint nids, uint **occs, uint *noccs) {
	twcsa* wcsa = (twcsa *) index;	
	int retval;
	//intersecting lists or extract if only one
	uint *candoccs;
	
	if (nids ==1) {
		retval= extractList_il(wcsa->ils, ids[0], &candoccs, noccs);
	}
	else if (nids ==2) {
		retval = intersect_2_il(wcsa->ils, ids[0],ids[1], noccs, &candoccs);
	}
	else {
		retval = intersect_N_il (wcsa->ils, ids,nids , noccs, &candoccs);
	}  
					
	// ** Now using the doc-boundaries, 
	// ** Adds to occs (in positions noccs+0, noccs+1, noccs+2, ... the doc-ids for each occ]
	addDocIdsToOccs(&candoccs, *noccs, wcsa->doc_offsets_sids, wcsa->ndocs);	
	*occs =candoccs;	

	//if (*noccs != wcsa->freqs[ids[0]]) exit(0);
	return 0;
}
*/


/** ------------------------------------------------------------------ 
 * Returns the list of documents containing the nids terms represented by ids[] 
 *------------------------------------------------------------------ */
int index_listDocuments2(void *index, uint *ids, uint nids, uint **occs, uint *noccs){
  return 99;
}
/*
int index_listDocuments2(void *index, uint *ids, uint nids, uint **occs, uint *noccs) {
	twcsa* wcsa = (twcsa *) index;	
	int retval;
	//intersecting lists or extract if only one
	uint *candDocs;
	
	if (nids ==1) {
		retval= extractList_il(wcsa->ils, ids[0], &candDocs, noccs);
	}
	else if (nids ==2) {
		retval = intersect_2_il(wcsa->ils, ids[0],ids[1], noccs, &candDocs);
	}
	else {
		retval = intersect_N_il (wcsa->ils, ids,nids , noccs, &candDocs);
	}  
					
	// ** Now using the doc-boundaries, return only the IDS of the documents with the occurrences.
	mergeOccsIntoDocOccs(candDocs, noccs, wcsa->doc_offsets_sids, wcsa->ndocs);	
	*occs =candDocs;	
	
	return 0;
}
*/


/*************************************************************************************************/	
/*  Public:: only to give the ability to permit measuring AVG document extraction time           */
/*************************************************************************************************/	
/** ------------------------------------------------------------------ 
 * Returns the number of indexed documents *
 *------------------------------------------------------------------ */
int getNumDocs(void *index, uint *ndocs) {
  	twcsa* wcsa = (twcsa *) index;	
	*ndocs = wcsa->ndocs;
	return 0;
}
	
/** ------------------------------------------------------------------ 
 * Given "ndocs" document-ids \in [0..ndocs-1] --> returns the text of such documents *
 * \0 is also added to the end.
 *------------------------------------------------------------------ */
int extractTextOfSelectedDocuments(void *index, uint *docIds, uint nids, uchar ***texts, uint **lens, ulong *tot_chars){
  	twcsa* wcsa = (twcsa *) index;	
	uchar **T = (uchar **) malloc (sizeof(uchar *) * nids);
	uint *l = (uint *) malloc (sizeof(uint) * nids);
	
	*tot_chars=0;
	uint i;
	for (i=0; i< nids; i++) {
	  uint doclen;
	  uchar *doc;
	  extract_doc_representation (wcsa->ct, docIds[i], &doc, &doclen);
	  T[i] = doc;
	  l[i]=doclen;
	  *tot_chars += doclen;
	}
	*texts = T;
	*lens = l; 
}
	



















/*
// ** ------------------------------------------------------------------ 
// * Returns the list of documents containing the nids terms represented by ids[] 
// *------------------------------------------------------------------ 

int index_listDocuments(void *index, uint *ids, uint nids, uint **occs, uint *noccs) {
	twcsa* wcsa = (twcsa *) index;	
	int retval;
	//intersecting lists or extract if only one
	uint *candDocs;

	
	printf("\n index_lists_Docs start"); fflush(stdout);fflush(stderr);
	
	if (nids ==1) {
		retval= extractList_il(wcsa->ils, ids[0], &candDocs, noccs);
	}
	else if (nids ==2) {
		retval = intersect_2_il(wcsa->ils, ids[0],ids[1], noccs, &candDocs);
	}
	else {
		retval = intersect_N_il (wcsa->ils, ids,nids , noccs, &candDocs);
	}  

	printf("\n extract finishes %u",*noccs); fflush(stdout);fflush(stderr);
	printf("\n docbegs[0..k] = ");
	for (int i=0;i<10735; i++) printf("[%u,%u]",i,wcsa->doc_offsets_sids[i]);fflush(stdout);fflush(stderr);
	printf("\n");
	
	printf("\n noccs = %u, occs[0..k] = ", *noccs);
	for (int i=0;i<*noccs; i++) {printf("[%u]",candDocs[i]);}
					
	// ** Now using the doc-boundaries, return only the IDS of the documents with the occurrences.
	mergeOccsIntoDocOccs(candDocs, noccs, wcsa->doc_offsets_sids, wcsa->ndocs);	
	*occs =candDocs;	
	
	printf("\n merge into docs finishes"); fflush(stdout);
	printf("\n noccs = %u, docs[0..k] = ", *noccs);
	for (int i=0;i<*noccs; i++) {printf("[%u]",candDocs[i]);}
	
	exit(0);
	return 0;
}

*/

/*******************************************************************************/

void checkDocMerge(void *index) {
  twcsa *wcsa = (twcsa*) index;
  
  uint nwords = wcsa->nwords;
  uint ndocs = wcsa->ndocs;
  uint i;
  
  uint *docs1, *docs2, *occs;
  uint   len1,   len2, noccs;
  
  for (i=0; i<nwords;i++) {
	printf("\n check occs-2-docs for : word %u of %u",i,nwords);
	index_listDocuments(index,&i,1,&docs1,&len1);
	
	extractList_il(wcsa->ils, i, &occs, &noccs);
	docs2 = mergeOccsIntoDocOccsTrivial(occs,noccs,wcsa->doc_offsets_sids, ndocs, &len2);

	if (len1 != len2) {
	  printf("\n merge is failing for id = %u",i);
	  exit(0);
	}
	else {
	  for (int j=0;j<len1;j++) {
		 if (docs1[j] != docs2 [j]){
		  printf("\n merge is failing for id = %u",i);
		  exit(0);
		  }
	  }
	}
	free(docs2);

	free(docs1);
	free(occs);	
  }
	
	printf("\n MERGE (OF WORDS) WORKED FINE FOR ALL LISTS!!!");
	exit(0); 
}


