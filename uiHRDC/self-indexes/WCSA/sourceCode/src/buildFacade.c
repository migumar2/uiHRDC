#include "buildFacade.h"
#include "utils/errors.c"
#include "utils/offsets.h"


/****************************************************************************************/

/** Building the index */

    /* Creates index from text[0..length-1]. Note that the index is an 
      opaque data type. Any build option must be passed in string 
      build_options, whose syntax depends on the index. The index must 
      always work with some default parameters if build_options is NULL. 
      The returned index is ready to be queried. */
int build_index (uchar *text, ulong length, uint *docboundaries, uint ndocs, char *build_options, void **index) {
	int returnvalue;

     printf("\n parameters: \"%s\"\n",build_options); fflush(stderr);
        
    returnvalue = build_WCSA (text, length, docboundaries, ndocs, build_options, index);
 

    if (!returnvalue) 
    	returnvalue = build_iCSA (build_options, *index);
    		   

	if ( ((twcsa*) *index)->ndocs > 2) {
		unsigned char *document;
		uint doclen;
		extractTextOfDocument(*index, 0, &document, &doclen);
		fprintf(stderr,"\n =================== DOC 0 ======================");
		fprintf(stderr,"\n%s",document);
		fprintf(stderr,"\n =================== ***** ======================\n");
		free(document);	
		extractTextOfDocument(*index, 1, &document, &doclen);
		fprintf(stderr,"\n =================== DOC 1 ======================");
		fprintf(stderr,"\n%s",document);
		fprintf(stderr,"\n =================== ***** ======================\n");
		free(document);	
	}		   
	    		   
    return returnvalue;  
}


/**  Saves index on disk by using single or multiple files, having 
	proper extensions. */
int save_index (void *index, char *filename) {

	char *basename = filename;
	twcsa *wcsa=(twcsa *) index;

	uint i,j;
	char *outfilename;
	int file;
	char c;

	printf("\n Saving structures to disk: %s.*",filename);			
	outfilename = (char *)malloc(sizeof(char)*(strlen(basename)+10));

	/**File with some constants (bSize and tohSize); */
	{ uint number;
		strcpy(outfilename, basename);
		strcat(outfilename, ".");
		strcat(outfilename, CONSTANTS_FILE_EXT);
		unlink(outfilename);
		if( (file = open(outfilename, O_WRONLY|O_CREAT,S_IRWXG | S_IRWXU)) < 0) {
			printf("Cannot open file %s\n", outfilename);
			exit(0);
		}	
		write(file, &(wcsa->sourceTextSize), sizeof(ulong));			
		write(file, &(wcsa->seSize), sizeof(ulong));			
		close(file);		
	}	

	saveDocOffsetsSids(wcsa->doc_offsets_sids, wcsa->ndocs, basename);

	/** The Words in the vocabulary of words  (sorted alphabetically)*/
	{	strcpy(outfilename, basename);
		strcat(outfilename, ".");
		strcat(outfilename, VOCABULARY_WORDS_FILE_EXT);
		unlink(outfilename);
		if( (file = open(outfilename, O_WRONLY|O_CREAT,S_IRWXG | S_IRWXU)) < 0) {
			printf("Cannot open file %s\n", outfilename);
			exit(0);
		}

		uint n = wcsa->n;
		uint elemSize = wcsa->wordsData.elemSize;
		write(file, &n, sizeof(uint));
		write(file, &elemSize, sizeof(uint));
		write(file, &(wcsa->wordsData.wordsZoneMem.size), sizeof(uint));
		
		//the number of canonical words
		write(file, (char *)wcsa->wordsData.wordsZoneMem.zone, wcsa->wordsData.wordsZoneMem.size * sizeof(byte));
		write(file, (char *)wcsa->wordsData.words, ((((n+1)* (elemSize))+W-1) /W) * (sizeof(uint)) );

		close(file);
	}	
			
	free(outfilename);

	if (wcsa->myicsa) {
		/******** saves index on integers (bottom) ******/
		//Storing the CSA
		//storeStructsCSA(wcsa->myicsa,basename);
		saveIntIndex((void *) wcsa->myicsa, basename);		
	}	
	
 	
	if (wcsa->se) {
		saveSEfile(basename,wcsa->se, (ulong) wcsa->seSize+1);
		//free(wcsa->se);		
	}


	return 0;
}



    /**  Loads index from one or more file(s) named filename, possibly 
      adding the proper extensions. */
int load_index(char *filename, void **index){
	twcsa *wcsa;
	wcsa = loadWCSA (filename);
	(*index) = (void *) wcsa;
	return 0;
}

	/** Frees the memory occupied by index. */
int free_index(void *index){
	twcsa *wcsa=(twcsa *) index;
	ulong size;
	index_size(index,&size);
	printf("\n[destroying index] ...Freed %lu bytes... RAM", size);	
	
	
	//frees the array SE.
	if (wcsa->se)
		free (wcsa->se);
		
	if (wcsa->doc_offsets_sids)
		free(wcsa->doc_offsets_sids);	

	//the iCSA.
	if (wcsa->myicsa) {
		//destroyStructsCSA(wcsa->myicsa);
		int err = freeIntIndex((void *) wcsa->myicsa);
	}

	//the words.
	free (wcsa->wordsData.wordsZoneMem.zone);
	free (wcsa->wordsData.words); /** huge!! */
	
	//the pointer to wcsa.		
	free(wcsa);
	return 0;
}

	/** Gives the memory occupied by index in bytes. */
int index_size(void *index, ulong *size) {
	ulong totaltmp;
	twcsa *wcsa=(twcsa *)index;
	uint n= wcsa->n;
	*size=0;
	*size += sizeof(twcsa);	

	totaltmp=0;  //words
	totaltmp += ((((n+1)* (wcsa->wordsData.elemSize))+W-1) /W) * (sizeof(uint));  //the pointers
	totaltmp += wcsa->wordsData.wordsZoneMem.size * sizeof(byte); //the characters of the words.
	*size += totaltmp;
	
	if (wcsa->myicsa) {
		uint nbytes;
		int err = sizeIntIndex((void *) wcsa->myicsa, &nbytes);
		*size += nbytes;
		//*size += CSA_size(wcsa->myicsa);		
	}

	*size += sizeof(uint) * (1 + wcsa->ndocs);
	return 0;	
}


/** Querying the index =============================================================*/
	
	/* Writes in numocc the number of occurrences of the substring 
	   pattern[0..length-1] found in the text indexed by index. */
int count (void *index, uchar *pattern, ulong length, ulong *numocc){
	uint integerPatterns[MAX_INTEGER_PATTERN_SIZE];
	uint integerPatternSize;
	ulong l,r;	

	twcsa *wcsa=(twcsa *) index;
	parseTextIntoIntegers(wcsa, pattern, length, integerPatterns, &integerPatternSize);	
	if (!integerPatternSize) {*numocc=0; return 0;} //not found
	
	//*numocc = countCSA(wcsa->myicsa, integerPatterns, integerPatternSize, &l, &r);
	int err = countIntIndex((void *) wcsa->myicsa, integerPatterns, integerPatternSize, numocc,  &l, &r);
	return 0;
}

	/* Writes in numocc the number of occurrences of the substring 
	  pattern[0..length-1] in the text indexed by index. It also allocates
	  occ (which must be freed by the caller) and writes the locations of 
	  the numocc occurrences in occ, in arbitrary order.  */	  
int locate(void *index, uchar *pattern, ulong length, ulong **occ, ulong *numocc){
	return 99;
}

   /* Gives the length of the text indexed */
int get_length(void *index, ulong *length) {
	twcsa *wcsa=(twcsa *) index;
	*length = wcsa->sourceTextSize;
	return 0;
}

    /**  Obtains the length of the text indexed by index. */

int length (void *index, ulong *length) {
	return (get_length(index,length));
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
	return 99;    
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
	return 99;
}



/** ***********************************************************************************
  * WORD-ORIENTED QUERY FUNCTIONS: LocateWord and DisplayWord
  * ***********************************************************************************/  
	/* Writes in numocc the number of occurrences of the substring 
	  pattern[0..length-1] in the text indexed by index. It also allocates
	  occ (which must be freed by the caller) and writes the locations of 
	  the numocc occurrences in occ, in arbitrary order. These occurrences
	  refer to the offsets in TOH where the caller could start a display
	  operation. So locateWord implies synchronization using B.
	  Moreover, positions occ[numocc.. 2*numocc-1] is set with the rank in SE of the
	  words whose codes begin in TOH in the positions in occ[0... numocc-1]
	  ** Parameter kbefore sets locateWord not to obtain the offset in TOH of the
	     searched word, but the offset in TOH of k-before words before.	  
	*/	  
	  
int locateWord(void *index, uchar *pattern, ulong length, ulong **occ, ulong *numocc, uint kbefore){
	uint integerPatterns[MAX_INTEGER_PATTERN_SIZE];
	uint integerPatternSize;
	ulong occurrences,l,r;	
	twcsa *wcsa=(twcsa *) index;
	
	parseTextIntoIntegers(wcsa, pattern, length, integerPatterns, &integerPatternSize);	
	if (!integerPatternSize) {*numocc=0; return 0;} //not found 

	ulong *seOffsets;
  
  	//obtains the indexes in vector SE where the pattern appears.
	//seOffsets = locateCSA(wcsa->myicsa, integerPatterns, integerPatternSize, &occurrences);
	int err = locateIntIndex((void *)wcsa->myicsa, integerPatterns, integerPatternSize, &seOffsets, &occurrences);
		
	*numocc = occurrences;
 	
	if (!occurrences) {(*occ)=NULL;return 0;}

	(*occ) = (ulong *)seOffsets;
	return 0;
}  

/** ***********************************************************************************
  * DOC-ORIENTED QUERY FUNCTIONS: LocateDocumentOffsets
  * ***********************************************************************************/
int LocateDocumentOffsets(void *index, uchar *pattern, uint length, uint **occ, uint *numocc) {
	uint integerPatterns[MAX_INTEGER_PATTERN_SIZE];
	uint integerPatternSize;
	ulong l,r;	
	twcsa *wcsa=(twcsa *) index;
	
	parseTextIntoIntegers(wcsa, pattern, length, integerPatterns, &integerPatternSize);	
	if (!integerPatternSize) {*numocc=0; return 0;} //not found 

	ulong *seOffsets; ulong n;
  
  	//obtains the indexes in vector SE where the pattern appears.
	int err = locateIntIndex((void *)wcsa->myicsa, integerPatterns, integerPatternSize, &seOffsets, &n);
		
	// ** Now using the doc-boundaries, 
	// ** Adds to occs (in positions noccs+0, noccs+1, noccs+2, ... the doc-ids for each occ]
	
	uint *result;
		
	/*  //BY FRANCISCO
	  result = doc_offset_exp_Fari(wcsa->doc_offsets_sids, wcsa->ndocs, seOffsets, (uint) n);
	  *numocc = result[0]/2;
	*/
	
	    //BY FARI
	if (n) {
	  result = addDocIdsToOccs(&seOffsets, (uint) n, wcsa->doc_offsets_sids, wcsa->ndocs);	
	  *numocc = n;
	}
	
	
	*occ =result;	//recall result[0] == *numocc

	if (! (*occ)) {free(result); (*occ)=NULL; return 0;}

	return 0;
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

 int displayWords (void *index, uchar *pattern, ulong length, ulong numc, 
         ulong *numocc, uchar **snippet_text, ulong **snippet_lengths, uint kbefore) {
         	
    /** actually extracts upto length + 2*numc chars, starting extraction kbefore
     *  words before the occurrence **/     	

 	ulong *indexesInSE;
 	ulong occurrences;
 	uint bytesPerSnippet;
 	byte *text_aux;
	twcsa *wcsa=(twcsa *) index;
 	
 	locateWord(index, pattern, length, (ulong **)&indexesInSE, &occurrences, 0); 	
 	(*numocc) = occurrences;   

 	if (!occurrences) {
 		*snippet_text =NULL;
 		*snippet_lengths =NULL;
 		return 0;
 	} 	

	bytesPerSnippet = length+2*numc;
//	bytesPerSnippet = 2*numc;
	*snippet_lengths = (ulong *) malloc((*numocc)*sizeof(ulong));
	if (!(*snippet_lengths)) return 1;
	*snippet_text = (uchar *) malloc((*numocc)*(bytesPerSnippet)*sizeof(uchar) +1) ;  //(the last "1" is for '\0');
	if (!(*snippet_text)) return 1;

 // 	fprintf(stderr,"\n occs found = %7d for pattern %s",*numocc, pattern);
 // 	fflush(stderr);
  	
  	text_aux=*snippet_text;	  	
  	{
  		uint i, j, tmplen;
  		uint ptr, maxptr;
  		byte *src, *dst;
  		uint snippetLen;
  		uint posSEValue,indexSE;
			
  		for (i=0;i<occurrences;i++) {
				uint prevValid=0;
				uint endSnippet =0;
	
				/** decodes words from there */			
				snippetLen=0;							
				indexSE = indexesInSE[i];
				indexSE = (indexSE > kbefore) ? indexSE-kbefore : 0;			 

				dst = text_aux;					
				while ((!endSnippet) && (indexSE < wcsa->seSize) ){ /** extracting words (if not at the end) */

					//posSEValue =displayCSA(wcsa->myicsa,indexSE); 
					int err= displayIntIndex((void *)wcsa->myicsa,indexSE, &posSEValue);
					
					{//obtains pointer to the ith word
						uint offtmp;
						uint ith = posSEValue -1;  // !! 
						tmplen = bitread (wcsa->wordsData.words, (ith +1)* wcsa->wordsData.elemSize, wcsa->wordsData.elemSize);
						offtmp = bitread (wcsa->wordsData.words, ( ith  )* wcsa->wordsData.elemSize, wcsa->wordsData.elemSize);
						tmplen -=offtmp;  //the lenght of the ith word.

						src= (byte *) wcsa->wordsData.wordsZoneMem.zone + offtmp;
					}					

					if (_Valid[*src]) {
						if (prevValid){
							 *dst++ =' ';   
							 snippetLen++;
							 if  (snippetLen==bytesPerSnippet) break;  //end of snippet (ends in BLANK_SPACE)
						}
						prevValid =1;   //for the next iteration
					}
					else prevValid=0;		
																							 		
			 		indexSE++;
			 				 		
					/* at the end ?? */
			 		if  ((tmplen+snippetLen)>=bytesPerSnippet) {
			 			tmplen =(bytesPerSnippet - snippetLen);
			 			endSnippet=1; //so while loop ends;
			 		}	
					
			 		for (j=0;j<tmplen;j++) {*dst++ = *src++;}	  //copies word to the output buffer
			 		snippetLen +=tmplen;		 		
			 	}//while
	
				text_aux += bytesPerSnippet;
				(*snippet_lengths)[i] = snippetLen;		 				
			}	//for
						
			if (occurrences) free(indexesInSE);
		}
		return 0; 	     	
}

/** simulates extration of text process, but do not actually returns anything at all 
   Extracts upto <=2K words from K=wordsbefore words before each occurrence of a pattern.
   Less than 2K words can be extracted if more than numc characters have been already obtained.
   Does nothing else... does not return the text */

int  displayTextOcurrencesNoShow(void *index, uchar *pattern, ulong length, uint wordsbefore, uint maxnumc) {
	
	ulong *indexesInSE;
 	ulong occurrences;
 	byte *text_aux;
 
	twcsa *wcsa=(twcsa *) index;
 	
 	locateWord(index, pattern, length, (ulong **)&indexesInSE, &occurrences, 0); 	
 
 	if (!occurrences) {
 		return 0;
 	} 	
 	
 	ulong maxsnippetLen  = maxnumc;
	ulong extractedbytes = 0;

 	text_aux = (byte *) malloc (maxsnippetLen+1);

  	{
  		uint  j,  tmplen;
  		ulong i;
  		//uint ptr, maxptr;
  		byte *src, *dst;
  		uint snippetLen;
  		ulong indexSE;
  		uint posSEValue;
  		
  		uint numWordsToExtract = 2 * wordsbefore;
  		uint z;
  		//printf("\n occurrences... = %lu",occurrences);
			
  		for (i=0;i<occurrences;i++) {
				uint prevValid=0;
				uint endSnippet =0;

				/** decodes words from there on */			
				snippetLen=0;							
				indexSE = indexesInSE[i];
				indexSE = (indexSE > wordsbefore) ? indexSE-wordsbefore : 0;			 

				dst = text_aux;	
				z=0;				
				while ((z<numWordsToExtract) && (indexSE < wcsa->seSize) ){ /** extracting words (if not at the end) */

					//posSEValue =displayCSA(wcsa->myicsa,indexSE); 
					int err= displayIntIndex((void *)wcsa->myicsa,indexSE, &posSEValue); 
				
					{//obtains pointer to the ith word
						uint offtmp;
						uint ith = posSEValue -1;  // !! 
						tmplen = bitread (wcsa->wordsData.words, (ith +1)* wcsa->wordsData.elemSize, wcsa->wordsData.elemSize);
						offtmp = bitread (wcsa->wordsData.words, ( ith  )* wcsa->wordsData.elemSize, wcsa->wordsData.elemSize);
						tmplen -=offtmp;  //the lenght of the ith word.

						src= (byte *) wcsa->wordsData.wordsZoneMem.zone + offtmp;
					}					
	
					if (_Valid[*src]) {
							if (prevValid){
								 *dst++ =' ';   
								 snippetLen++;
								 if  (snippetLen==maxsnippetLen) break;  //end of snippet (ends in BLANK_SPACE)
								}
							prevValid =1;   //for the next iteration
					}
					else prevValid=0;		
																							 		
			 		indexSE++;
			 				 		
					/* at the end ?? */
			 		if  ((tmplen+snippetLen)>=maxsnippetLen) {
							break;
			 		}	
				
					//fprintf(stderr,"\ntmplen = %d ",tmplen); fflush(stderr); 				 			
			 		for (j=0;j<tmplen;j++) {*dst++ = *src++;}	  //copies word to the output buffer
			 		snippetLen +=tmplen;	
			 		z++;	 		
			 	}//while

				extractedbytes += snippetLen;
	 				
			}	//for
					
			if (occurrences) free(indexesInSE);
		}
		if (text_aux) free (text_aux);
		return extractedbytes; 
}



/**  Allocates text (which must be freed by the caller) and recovers the
  the substring of text starting from the "fromword"-th word up to the
  "toWord"-th words. Returns in text the text, and in "text_lenght" the 
  length of the text  actually extracted. Text is allocated. 
  Actually extracts SE[fromWord .. toWord) ... not the last word.    */

int extractWords (void *index, ulong fromWord, ulong toWord, uchar **text, 
        ulong *text_length){
        	
	twcsa *wcsa=(twcsa *) index;
    uint avgWordLen =7;

	uint j;
	uint prevValid=0;
	byte *src, *dst, *buff;
	uint tmplen =0;

	ulong buffBytes = 10000;
	ulong leng=0; //curr pos in buffer that was occupied.
	
	if (toWord > wcsa->seSize) toWord = wcsa->seSize;
	if (fromWord >= wcsa->seSize) fromWord = wcsa->seSize-1;
	if (buffBytes < ( (toWord-fromWord)* avgWordLen)) buffBytes = ((toWord-fromWord)* avgWordLen);
		
	buff = (uchar *) malloc (buffBytes * sizeof(char));
	if (!buff) return 1; //out of memory.
	dst = buff;
	
	register ulong indexSE=fromWord;
	uint posSEValue=0;
	register uint ith;
		
	while  ( (indexSE < toWord) ){ /** extracting words (if not at the end) */

		int err= displayIntIndex((void *)wcsa->myicsa,indexSE, &posSEValue);  
		
		{//obtains pointer to the ith word
			uint offtmp;
			ith= posSEValue -1;  // !! 
			tmplen = bitread (wcsa->wordsData.words, (ith +1)* wcsa->wordsData.elemSize, wcsa->wordsData.elemSize);
			offtmp = bitread (wcsa->wordsData.words, (ith  )* wcsa->wordsData.elemSize, wcsa->wordsData.elemSize);
			tmplen -=offtmp;  //the lenght of the ith word.
			src= (byte *) wcsa->wordsData.wordsZoneMem.zone + offtmp;
		}					

		if ( buffBytes < (leng + tmplen+1) ) {
			buffBytes *=2;
			while (buffBytes < (leng + tmplen+1)) buffBytes *=2;
			buff = (uchar*) realloc(buff, buffBytes);
			if (!buff) return 1; //out of memory.
			dst = buff + leng;
		}

		if (_Valid[*src]) {
			if (prevValid){
				*dst++ =' ';   					
				leng  += 1;
			}
			prevValid =1;   //for the next iteration
		}
		else prevValid=0;		
																						
		indexSE++;
											
		for (j=0;j<tmplen;j++) {*dst++ = *src++;}	  //copies word to the output buffer
		leng +=tmplen;
	}//while

	*text_length =leng;
	*dst='\0';
	*text = buff;
	return 0;
}





/** ------------------------------------------------------------------
  * Saves: document beginnings within the sequence of words Sids[]
  * ------------------------------------------------------------------*/	  
  int saveDocOffsetsSids(uint *doc_offsets_sids, uint ndocs, char *basename){
	int file; int errw;

	char *filename;	
	filename = (char *)malloc(sizeof(char)*(strlen(basename)+10));
	strcpy(filename, basename); strcat(filename, ".");	strcat(filename, DOCBEGININGS_INDEX_FILE_EXT_PARSER);
	unlink(filename);
	
	{		
		unlink(filename);
		fprintf(stderr,"\t Saving  document-beginnings offsets to disk: file  %s...", filename);		
		if( (file = open(filename, O_WRONLY|O_CREAT,S_IRWXG | S_IRWXU)) < 0) {
			printf("Cannot open file %s\n", filename);
			exit(0);
		}	

		errw = write(file, &(ndocs), sizeof(uint));		
		errw = write(file, doc_offsets_sids, (ndocs +1) *sizeof(uint));
		close(file);		
	}
		
	free(filename);
	return 0;
}


/**------------------------------------------------------------------
  * Loads: document beginnings within the sequence of words Sids[] into doc_offsets_sids[]
  * ----------------------------------------------------------------- */
int loadDocOffsetsSids(uint **docB, uint *nd, char *basename){
	int file,readbytes;

	char *filename;	
	filename = (char *)malloc(sizeof(char)*(strlen(basename)+20));
	strcpy(filename, basename); strcat(filename, ".");	strcat(filename, DOCBEGININGS_INDEX_FILE_EXT_PARSER);

	fprintf(stderr,"\t Loading document-beginnings offsets: from disk %s...", filename);		
	if( (file = open(filename, O_RDONLY)) < 0) {
		printf("Cannot open file %s\n", filename);
		exit(0);
	}	

	uint ndocs;
	readbytes = read(file, &ndocs, sizeof(uint));

	uint *docBeginnings = (uint *) malloc (sizeof(uint) * (ndocs +1));
	readbytes = read(file, docBeginnings, sizeof(uint) * (ndocs+1));		
	close(file);	

	*nd = ndocs;	
	*docB = docBeginnings;
	free(filename);	
	return 0;		
}


/** ***********************************************************************************
	 CONSTRUCTION OF THE INDEX WCSA
    ***********************************************************************************/

	/**------------------------------------------------------------------
	Compares two slots (alphanumericaly). For qsort of canonical words 
	------------------------------------------------------------------ */			   
	int qSortWordsCompareAlpha(const void *arg1, const void *arg2) {
		tposInHT *a1 = (tposInHT *) arg1;
		tposInHT *a2 = (tposInHT *) arg2;
		return strcmp((char*)a1->word, (char *)a2->word);	
	}

/**
  * BUILDS THE WCSA INDEX
  */

int build_WCSA (uchar *text, ulong length, uint *docboundaries, uint ndocs, char *build_options, void **index) {
	
 	unsigned long zeroNode;  //number of different canonical words.
 	
	t_hash hash;   		// the hash table to store both variants and canonical words.				      	
	tposInHT *posInHT;	// structure for canonicals and variants+huffmans 

 	ulong sourceTextSize;  
 
 	ulong seSize=0;  //it's size == "numberOfValidWords". 
 	uint *SE;       //Integers vector. (represents the rank of the valid words in the source text).

	uint totallenWords=0; //The numberOfBytes that occupy canonical words (their ascii version) in memory

	ulong bytesFile,bytesFileReal;
	long sizeNValue;

	/* used during first pass */	
			
	ulong addrInTH;
 	
	byte* inputBuffer = text;	
	bytesFileReal= bytesFile = length;

	sourceTextSize=length;
	
	/** Initializes WCSA structure*/
	twcsa *wcsa;
	wcsa = (twcsa *) malloc (sizeof (twcsa) * 1);
	zeroNode=0;
	/**      */
	/*
	char outfiletmp[2048]="tmp.tmp.File.docs_offsets.dat";

	{ //parsing the parameters of the index....
	
		// processing the parameters 
		char delimiters[] = " =;";
		int j,num_parameters;
		char ** parameters;
		int value;
		
		if (build_options != NULL) {
		parse_parameters(build_options,&num_parameters, &parameters, delimiters);
		for (j=0; j<num_parameters;j++) {
		  
			if  ((strcmp(parameters[j], "outfiletmp") == 0 ) && (j < num_parameters-1) ) {
				strcpy(outfiletmp,parameters[j+1]);
			}
		
			j++;
		}
		free_parameters(num_parameters, &parameters);
		}		
		printf("\n\tparameters of Presentation Layer:doc_offsets_File_tmp = %s",outfiletmp);		
	}
	if (!strcmp(outfiletmp,"")) {
		printf("\n a file with doc-offsets-beginnings must be provided");
		exit(0);
	}
	*/


	/** 2 ** loads the array of document boundaries                           */

	printf("\n ndocs vale = %u",ndocs);
	
	uint *doc_beg_id = (uint *) malloc (sizeof(uint)*(ndocs+1));
//	{int x=0;
//	for (x=0;x<ndocs+1; x++) doc_beg_id[x]=0;
//	}

	wcsa->doc_offsets_sids=doc_beg_id;
	wcsa->ndocs = ndocs;

	//Stimation (Using Heap's law) of the number of different "meaningful" words.
	//sizeNValue=N_value;
	if(bytesFile<5000000) bytesFile = 5000000;
	sizeNValue = (unsigned long) floor(3.9* pow(bytesFile,0.60) );
	

	// Inicializes the arrays used to detect if a char is valid or not.
	StartValid();
	// Inicializes the arrays used translated a char into lowercase.
	StartToLow();
		
	
	// **********************************************************************************
	//STARTING THE FIRST PASS. 
	// **********************************************************************************
	printf("\nSTARTING THE FIRST PASS...");
	
	posInHT = (tposInHT *) malloc(sizeof(tposInHT) * sizeNValue); 
	hash = initialize_hash (sizeNValue); //hash to cointain both the parsed words
	
	//-----------------------------------------------------------------	
	//1st pass (processing the file)
	{ 
		byte *pbeg,*pend,*wordstart,*aWord;
		register ulong size;
		register ulong i;

		byte *T = inputBuffer;
		uint n = bytesFileReal;

		uint j;
		
		for (j=0;j<ndocs;j++) {
			pbeg = T + docboundaries[j];
			pend = T + docboundaries[j+1];
								
			while (pbeg <pend) {  
				
				//parsing either a word or separator.			
				size=0;
				wordstart = pbeg;
				if (_Valid[*pbeg]) {   //alphanumerical data
					while ( (size<MAX_SIZE_OF_WORD) && (pbeg<pend)&& ( _Valid[*pbeg] )) {size++;pbeg++;}
				}		    
				else {
					if (*pbeg != ' ') { //a separator comes starting in ' ' comes, so it is a new word					
						while ( (size<MAX_SIZE_OF_WORD) && (pbeg<pend) &&  (!_Valid[*pbeg] )) {
							size++;pbeg++;}
					}
					else {  //a  SPACE comes, so we have to test if next character is alphanumerical or not
						pbeg++;
						if (pbeg >= pend) {size++;}  // a unique BLANK at the end of the file.
						else {
							if (_Valid [*pbeg] ) {
								wordstart = pbeg;   //So skipping 1 blank character
								while ( (size<MAX_SIZE_OF_WORD) && (pbeg<pend)&& ( _Valid[*pbeg] )) {
									size++;pbeg++;}
							}
							else {   // a "separator word" ...
								size++; //the prev BLANK...
								while ( (size<MAX_SIZE_OF_WORD) && (pbeg<pend) &&  (!_Valid[*pbeg] )) {
									size++;pbeg++;}
							}//else {  // a "separator word"
						}//else ... not a unique BLANK AT THE END.
					}//else ... starting by a BLANK... 
				}

				//The parsed word/separator is  is "wordstart", and its length is "size"...
				aWord=wordstart;				

				//Processement done for each word word			
				i = inHashTable(hash,aWord, size, &addrInTH );				
				if (!i){
					insertElement (hash,aWord, size, &addrInTH);
					posInHT[zeroNode].slot=addrInTH;
					posInHT[zeroNode].word=hash->hash[addrInTH].word;
					hash->hash[addrInTH].posInVoc = zeroNode;
					zeroNode++;
					totallenWords += size +1;			// +1 due to the '\0' char...		
					//fprintf(stderr,"\n Adding word: <<%s>> (size=%d) to the hashTable",hash->hash[addrInTH].word,size);				
				}	
										
				seSize ++;
			}//while pbeg<pend
		}//for docs	

		fprintf(stderr,"\n1st pass ends: TOTAL distinct words: %lu totalWords = %lu",zeroNode,seSize);

	}//1st pass ends


	// **********************************************************************************
	// END OF 1ST PASS
	// **********************************************************************************

	// Sorting the words alphanumerically (over posInHT)
	{	register unsigned long i,j;	
		//sorting canonical words ...
		qsort(posInHT, zeroNode, sizeof(tposInHT), qSortWordsCompareAlpha);		
		
		//setting in hash the new positions of the  words in the hash table
		for (i=0;i<zeroNode;i++) {	
			hash->hash[posInHT[i].slot].posInVoc = i;	
		}
	}	
	
	// INITIALIZING structures for the 2nd pass ......................................
	{
		SE  = (uint *) malloc ((seSize+1+1)*sizeof (uint));
	}


	// **********************************************************************************
	//  STARTING THE SECOND PASS.
	// **********************************************************************************/

	printf("\nSTARTING THE SECOND PASS... ");
	//2nd pass (processing the file)
	{ 
		byte *pbeg,*pend,*wordstart,*aWord;
		register ulong size;
		register uint i;
		register ulong countValidWords = 0;

		byte *T = inputBuffer;
		uint n = bytesFileReal;
		uint j;
	
		for (j=0;j<ndocs;j++) {
			pbeg = T + docboundaries[j];
			pend = T + docboundaries[j+1];
			doc_beg_id[j]=countValidWords;
			
				
			while (pbeg <pend) {  
				
				//parsing either a word or separator.			
				size=0;
				wordstart = pbeg;
				if (_Valid[*pbeg]) {   //alphanumerical data
					while ( (size<MAX_SIZE_OF_WORD) && (pbeg<pend)&& ( _Valid[*pbeg] )) {size++;pbeg++;}
				}		    
				else {
					if (*pbeg != ' ') { //a separator comes starting in ' ' comes, so it is a new word					
						while ( (size<MAX_SIZE_OF_WORD) && (pbeg<pend) &&  (!_Valid[*pbeg] )) {
							size++;pbeg++;}
					}
					else {  //a  SPACE comes, so we have to test if next character is alphanumerical or not
						pbeg++;
						if (pbeg >= pend) {size++;}  // a unique BLANK at the end of the file.
						else {
							if (_Valid [*pbeg] ) {
								wordstart = pbeg;   //So skipping 1 blank character
								while ( (size<MAX_SIZE_OF_WORD) && (pbeg<pend)&& ( _Valid[*pbeg] )) {
									size++;pbeg++;}
							}
							else {   // a "separator word" ...
								size++; //the prev BLANK...
								while ( (size<MAX_SIZE_OF_WORD) && (pbeg<pend) &&  (!_Valid[*pbeg] )) {
									size++;pbeg++;}
							}//else {  // a "separator word"
						}//else ... not a unique BLANK AT THE END.
					}//else ... starting by a BLANK... 
				}

				//The parsed word/separator is  is "wordstart", and its length is "size"...
				aWord=wordstart;					

				//Processement done for each word word			
				i = inHashTable(hash,aWord, size, &addrInTH );				

				SE[countValidWords]=hash->hash[addrInTH].posInVoc+1;  // !!!!
				countValidWords++;		

			}// while pbeg<pend
		}//for docs
		doc_beg_id[j]=countValidWords;
		
		SE[countValidWords] = 0;
		fprintf(stderr,"\n2nd pass ends: TOTAL distinct words: %lu totalWords = %lu",zeroNode,countValidWords);
			
	}//2nd pass ends
	
	// **********************************************************************************
	// END OF 2ND PASS
	// **********************************************************************************
	
	//freeing the source text (it is no longer needed).
	free(inputBuffer); //the text	

	/** Now Setting the data of the index **/
	wcsa->n = zeroNode;
	wcsa->sourceTextSize = sourceTextSize;
	wcsa->seSize = seSize;
	
	// Creating the words of the vocabulary...
	{  
	/** copying the words into WCSA. */
		uint *tmpOffsets = (uint *) malloc (sizeof(uint) * (zeroNode  +1) );  //1 extra uint (to point to the virtual "zeroNode+1" ^th word.
		uint tmpOffset =0;
		 
		byte *zoneMem,*src;
		uint i;
								 
		//Moving data from posInHT to WCSA structure
		//wcsa->wordsData = (twords *) malloc(sizeof(twords) * zeroNode); 
 		wcsa->wordsData.wordsZoneMem.size = totallenWords - zeroNode; //without '\0' bytes (end-tag).
 		wcsa->wordsData.wordsZoneMem.zone = (byte *) malloc ( wcsa->wordsData.wordsZoneMem.size * sizeof(byte) );  
 		zoneMem = wcsa->wordsData.wordsZoneMem.zone;					
		for(i = 0; i < zeroNode; i++) {
			src = posInHT[i].word;				 //copying the canonical word
			//wcsa->wordsData.words[i].word = zoneMem;   //setting the pointer
			tmpOffsets[i]=tmpOffset;  //offset in zoneMem
			while (*src) {*zoneMem++ = *src++; tmpOffset++;}  //moving data until '\0'
			//*zoneMem='\0'; zoneMem++;            //copies also the '\0'
			
		}
		tmpOffsets[zeroNode]=tmpOffset; //setting pointer to the "virtual" word {zeroNode+1}^{th}
		
		//kbit encoding of the offsets
		uint elemSize = bits(tmpOffset);
		wcsa->wordsData.elemSize = elemSize;
		wcsa->wordsData.words = (uint *) malloc (((((zeroNode +1)*elemSize)+W-1) /W) * sizeof(uint));  //with 1 extra slot !.
		wcsa->wordsData.words[((((zeroNode +1)*elemSize)+W-1) /W)   -1 ] =0000;
		//		fprintf(stderr,"\n ElemSize = %d, maxOffset = %d",elemSize,tmpOffset);

		tmpOffset=0;
		for (i=0; i<=zeroNode; i++) {  //setting "zeroNode+1" offsets
				bitwrite(wcsa->wordsData.words, tmpOffset, elemSize, tmpOffsets[i]);
				tmpOffset+=elemSize; 
		}
				
		//////////// CHECKS IT WORKED. old !!!!
	//		{ uint kk;
	//			tmpOffset=0;
	//			for (i=0; i<zeroNode; i++) {  //setting "zeroNode+1" offsets
	//					kk=bitread(wcsa->wordsData.words, i* wcsa->wordsData.elemSize, wcsa->wordsData.elemSize);
	//					tmpOffset+=elemSize; 
	//					if (kk != tmpOffsets[i]) {fprintf(stderr,"\n @@@@@@@@ DISTINTOS OFFSETS "); break;}
	//					else fprintf(stderr,"\n iguales, %d, %d  :: <<%s>>len=%d",kk,i, posInHT[i].word, strlen((char*)posInHT[i].word));
	//			}
	//		}
	//
	//		{ uint len1, len, tmplen, len2;
	//			uint i,p;
	//			byte *wcsaWord, *src;
	//			
	//			for (p=0;p<zeroNode;p++) {
	//				{//preparing for strcompL
	//					len    = bitread (wcsa->wordsData.words, (wcsa->wordsData.elemSize * (p+1)), wcsa->wordsData.elemSize);
	//					tmplen = bitread (wcsa->wordsData.words, (wcsa->wordsData.elemSize * (p))  , wcsa->wordsData.elemSize);
	//			
	//				//fprintf(stderr,"\n  :: off[%d]= %d  -  off [%d] = %d  ==> %d",p+1,len,p,tmplen,len-tmplen);
	//			
	//				len2 =len-tmplen;
	//					wcsaWord= (byte *) wcsa->wordsData.wordsZoneMem.zone + tmplen;
	//				}		
	//				
	//				src = posInHT[p].word;	
	//				len1 = strlen((char *)src);
	//				
	//				if (strcompL(src,wcsaWord,len1,len2) != 0) {
	//					fprintf(stderr,"\n %6d DISTINTOS !! ===len1 %d,len %d===== <<",p,len1,len2);printWord(src,len1); 
	//					fprintf(stderr,">> <<"); printWord(wcsaWord,len2); fprintf(stderr,">>");
	//					exit(0);
	//				}
	//				else {
	//					fprintf(stderr,"\n %6d ======len1 %d,len2 %d===== <<",p,len1,len2);printWord(src,len1); 
	//					fprintf(stderr,">> <<"); printWord(wcsaWord,len2); fprintf(stderr,">>");
	//				}					
	//			}
	//				
	//		}
	//	
	
	/**-----------*/
	//frees memory from hash table and posInHT structures.
		
		free(docboundaries);	
		free(tmpOffsets);
		destroy_hash(hash);
		free(posInHT);
	}
	
	/** ******* creates the self-index on ints (bottom layer) ==> see build_icsa *********/
/**
	#ifdef CSA_ON
	{
		uint total;
		fprintf(stderr,"\n **** CREATING CSA from Edu's Code *****");
		ticsa *myicsa;
		myicsa = createIntegerCSA(&SE,seSize+1,build_options);
		wcsa->myicsa= myicsa;
		total = CSA_size(myicsa);
		
		free(SE);  //SE is no longer needed, (it is indexed by the iCSA)
		printf("\n\t**** [iCSA built on %d words. Size = %ld bytes... RAM",seSize,total);
	}	
	#endif
*/

	//#ifndef CSA_ON
		wcsa->se = SE;
		wcsa->myicsa = NULL;
	//#endif


	printf("\n\t ** Building done! **\n");	
	printf("\n Process finished!\n");

	*index = wcsa;
	return 0;
}


int build_iCSA (char *build_options, void *index)
{
	twcsa *wcsa = (twcsa *) index;
	/********* creates the self-index on ints (bottom layer) *********/
	//creating CSA from Edu's code...
	{
		uint total;
		fprintf(stderr,"\n **** CREATING CSA-bottom-layer *****");
		void *bottomIntIndex;
		int err =  buildIntIndex(wcsa->se,wcsa->seSize+1, build_options,(void **)&bottomIntIndex);				
		wcsa->myicsa = bottomIntIndex;

		//total = CSA_size(wcsa->myicsa);
		err = sizeIntIndex((void *) wcsa->myicsa, &total);

		printf("\n\t**** [iCSA built on %zu words. Size = %u bytes... RAM",(size_t) wcsa->seSize,total);
	}		
	return 0;
}

/** ********************************************************************
  * Loading from disk
  **********************************************************************/ 

/**-----------------------------------------------------------------  
 * LoadWCSA.                                                       
 * Loads all the data structures of WCSA (included the icsa)     
 ----------------------------------------------------------------- */ 

twcsa *loadWCSA(char *filename) {
	twcsa *wcsa;
	// Inicializes the arrays used to detect if a char is valid or not.
	StartValid();
	// Inicializes the arrays used translated a char into lowercase.
	StartToLow();
	
	wcsa = (twcsa *) malloc (sizeof (twcsa) * 1);
	wcsa->n=0;

	int err = loadIntIndex(filename, (void **)&wcsa->myicsa);

	loadStructs(wcsa,filename);   									  

	return wcsa;
}

/** ------------------------------------------------------------------
 * LoadStructs.
 *	Reads files and loads all the data needed for searcherFacade
 ----------------------------------------------------------------- */ 
 void loadStructs(twcsa *wcsa, char *basename) {
	uint i,j;
	char *filename;
	int file;
	uint sizeFile;
	char c;
	uint n;
		
	filename = (char *)malloc(sizeof(char)*(strlen(basename)+10));
	fprintf(stderr,"Loading Index from file %s.*\n", basename);
	
	//** SOME CONSTANTS: sourceTextSize
	{	strcpy(filename, basename);
		strcat(filename, ".");
		strcat(filename, CONSTANTS_FILE_EXT);
		
		if( (file = open(filename, O_RDONLY)) < 0) {
			printf("Cannot open file %s\n", filename);
			exit(0);
		}
				
		read(file, &(wcsa->sourceTextSize), sizeof(ulong));	
		read(file, &(wcsa->seSize), sizeof(ulong));	
		close(file);
	}		
	
	loadDocOffsetsSids(&wcsa->doc_offsets_sids, &wcsa->ndocs, basename);
	
	
	/** File with the words from the vocabulary (sorted alphabetically) */
	{	byte *zoneMem;
		
		strcpy(filename, basename);
		strcat(filename, ".");
		strcat(filename, VOCABULARY_WORDS_FILE_EXT);
		//sizeFile= fileSize(filename)-sizeof(uint);

		if( (file = open(filename, O_RDONLY)) < 0) {
			printf("Cannot open file %s\n", filename);
			exit(0);
		}
		
		//the number of canonical words
		read(file, &n, sizeof(uint));
		wcsa->n = n;
		read(file, &(wcsa->wordsData.elemSize), (sizeof(uint)));
		read(file, &(wcsa->wordsData.wordsZoneMem.size), (sizeof(uint)));
		
		//allocating the memory needed for all words and reading them	//(ascii) << no \0 chars are needed>>.	
		wcsa->wordsData.wordsZoneMem.zone = (byte *) malloc(wcsa->wordsData.wordsZoneMem.size * sizeof(byte));			
	 	read(file, (wcsa->wordsData.wordsZoneMem.zone),    wcsa->wordsData.wordsZoneMem.size * sizeof(byte) );
	
		//reading the offsets of the words (kbitArray that points to offsets in zoneMem of words.
		wcsa->wordsData.words = (uint *) malloc (((((n+1)* (wcsa->wordsData.elemSize))+W-1) /W) * sizeof(uint));	
		wcsa->wordsData.words[ ((((n+1)*(wcsa->wordsData.elemSize))+W-1) /W)   -1 ] =0000;
		read(file, (wcsa->wordsData.words),     ((((n+1)* (wcsa->wordsData.elemSize))+W-1) /W) * (sizeof(uint)));
		
		close(file);
	}	
	wcsa->se= NULL;		
	free(filename);
}




/** ****************************************************************
  * Querying the index WCSA
  * ***************************************************************/
///////////////////////////////////////////////////////////////////////////////////////
//					   FUNCTIONS NEEDED FOR SEARCHING A PATTERN    					 //
///////////////////////////////////////////////////////////////////////////////////////



/*------------------------------------------------------------------
 * Given a text pattern translates it into a list of integers (corresponding to the
 * canonical words associated to the valid words in the text pattern) 
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
				while ( (size<MAX_SIZE_OF_GAP) && (pbeg<pend) &&  (!_Valid[*pbeg] )) {size++;pbeg++;}
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
						while ( (size<MAX_SIZE_OF_GAP) && (pbeg<pend) &&  (!_Valid[*pbeg] )) {size++;pbeg++;}
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
			max = (wcsa->n) - 1;
			while(min < max) {
				p = (min+max)/2;
				
				{//preparing for strcompL
					len    = bitread (wcsa->wordsData.words, (p+1)* wcsa->wordsData.elemSize , wcsa->wordsData.elemSize);
					tmplen = bitread (wcsa->wordsData.words, (p )* wcsa->wordsData.elemSize  , wcsa->wordsData.elemSize);
					len -=tmplen;
					wcsaWord= (byte *) wcsa->wordsData.wordsZoneMem.zone + tmplen;
				}
				
				//if(strncmp((char*)aWord, (char*)wcsa->wordsData[p].word,size) > 0) min = p+1;
				if(strcompL(aWord, wcsaWord, size, len) > 0) min = p+1;
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
			if(!strcompL(aWord, wcsaWord, size, len)) {
				integerPattern[index++] = min +1 ;  //<--
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
	 * Returns the number of occurrences of a given text pattern
	 *------------------------------------------------------------------ */
int countTextOcurrences(twcsa *wcsa, byte *textPattern) {

	return 99;	

}


	/** ------------------------------------------------------------------
	 * locateTextOcurrences: 
	 * Returns the offsets of the source text where a word/phrase appears
	 * Returns also the number of occurrences.
	 *------------------------------------------------------------------ */
uint *locateTextOcurrences(twcsa *wcsa, byte *textPattern, int *numberOccurrences) {
	*numberOccurrences = 0;
	return NULL;  //not implemented: function not available for this index
}


	/** ------------------------------------------------------------------
	 * displayTextOcurrences:
	 * Shows in stdout, the text around the occurrences of a word/phrase
	 * Returns also the number of occurrences.
	 *------------------------------------------------------------------ */
int displayTextOcurrences(twcsa *wcsa, byte *textPattern, uint radixDisplay) {
	return 99;  //not implemented: function not available
}

	/** ------------------------------------------------------------------
	 * Locate Facade: 
	 * For given sePositions, returns the sourceTextPositions
	 * where the those valid-words in se[sePositions[i]] occurr.
	 *------------------------------------------------------------------*/ 
int locateFacade (twcsa *wcsa, uint *sourceTextPositions,uint *sePositions, uint number) {
	return 99;  //not implemented: function not available for this index
}


/** ------------------------------------------------------------------
	* DISPLAYFACADE:
	* Returns the subString from a starting offset to a final offset
	* in the source text. It does not allocate any memory, receives "dstptr"
	* Precondition: offsetIni >=0;
	------------------------------------------------------------------*/
 int displayFacade (twcsa *wcsa, uint offsetIni, uint offsetFin, ulong *length, byte *dstptr) {
	return 99;  //not implemented: function not available for this index		
}


	/**------------------------------------------------------------------
	 * DISPLAYFacadeMalloc:
	 * Returns the subString from a starting offset to a final offset
	 * in the source text. It allocates Memory !!
	 * NOT CURRENTLY USED
	 ------------------------------------------------------------------*/
byte *displayFacadeMalloc (twcsa *wcsa, uint offsetIni, uint offsetFin, ulong *length) {
	byte *dstptr=NULL;	   //not implemented: function not available
	return dstptr;		
}


	/** ------------------------------------------------------------------ 
	 * LOCATEALLandDISPLAY:
	 * Displays the text around an occurrence of the searched word in the source text.
	 * Assuming that $p$ is that position --> shows only chars in [p_radix-1,p_radix]
	 ------------------------------------------------------------------*/ 
int locateAllAndDisplay (twcsa *wcsa, uint *sePositions, uint number, int radix) {
return 99;   //not implemented: function not available for this index

}


	/** ------------------------------------------------------------------
	 * recovers the source text by calling display(0,fileSize);
	 * ------------------------------------------------------------------ */
void recoverSourceText1(twcsa *wcsa, char *basename, char *ext, ulong sourceTextSize) {		

	long start;long end;
	byte *cc;
	char *filename = (char *) malloc (strlen( basename)+ strlen(ext)+1);
	ulong length;
	
	strcpy( filename,  basename);
	strcat( filename, ext);
	filename[strlen( basename)+ strlen(ext)]='\0';
	fprintf(stderr,"\n uncompressing text into file -->%s ",filename);fflush(stderr);

	FILE *salida;
	unlink( filename);
	salida = fopen( filename,"w");
	start=0; end = (long) sourceTextSize-1;
	
	cc = (byte *) malloc (sourceTextSize* sizeof(uchar));

	{
  		uint i, j;//, tmplen;
		uint prevValid;
  		byte *src, *dst;
		ulong leng =0;
		uint tmplen =0;
  		
		ulong indexSE=0;
		uint posSEValue=0;
	
		dst=cc;
		while  ( (indexSE < wcsa->seSize) ){ /** extracting words (if not at the end) */

			int err= displayIntIndex((void *)wcsa->myicsa,indexSE, &posSEValue);
			
			{//obtains pointer to the ith word
				uint offtmp;
				uint ith = posSEValue -1;  // !! 
				tmplen = bitread (wcsa->wordsData.words, (ith +1)* wcsa->wordsData.elemSize, wcsa->wordsData.elemSize);
				offtmp = bitread (wcsa->wordsData.words, ( ith  )* wcsa->wordsData.elemSize, wcsa->wordsData.elemSize);
				tmplen -=offtmp;  //the lenght of the ith word.
				src= (byte *) wcsa->wordsData.wordsZoneMem.zone + offtmp;
			}					

			if (_Valid[*src]) {
				if (prevValid){
					 *dst++ =' ';   					
					leng +=1;
				}
				prevValid =1;   //for the next iteration
			}
			else prevValid=0;		
																							
			indexSE++;
												
			for (j=0;j<tmplen;j++) {*dst++ = *src++;}	  //copies word to the output buffer
			leng +=tmplen;
		}//while

	fprintf(stderr,"\n sourceTextSize = %lu, len = %lu",sourceTextSize,leng);
	fwrite(cc,sizeof(byte),leng,salida);
	fclose(salida);

	free(cc);
	free(filename);
}


}

		//recovers the source text by calling extract Words.
void recoverSourceText2(twcsa *wcsa, char *basename, char *ext, ulong sourceTextSize) {		

	ulong start;ulong end; int error;
	char *filename = (char *) malloc (strlen( basename)+ strlen(ext)+1);
	byte *cc;
	ulong length;
	
	strcpy( filename,  basename);
	strcat( filename, ext);
	filename[strlen( basename)+ strlen(ext)]='\0';
	fprintf(stderr,"\n uncompressing text into file -->%s ",filename);fflush(stderr);

	FILE *salida;
	unlink( filename);
	salida = fopen( filename,"w");
	start=0; end = wcsa->seSize;
	
	error = extractWords((void *) wcsa, start, end, &cc, &length);
	if (error) {fprintf(stderr,"\n error during recoverSourceText2"); exit(0);}

	fprintf(stderr,"\n sourceTextSize = %lu, len = %lu",sourceTextSize,length);
	fwrite(cc,sizeof(byte),length,salida);
	fclose(salida);

	free(cc);
	free(filename);		
}




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

	
	uint PROGRESS = nids /100;
	printf("\n nids to extract = %u PROGRESS= %u",nids,PROGRESS);
	
	*tot_chars=0;
	uint i;
	for (i=0; i< nids; i++) {
		if (!(i%PROGRESS)){
	  		printf("\n [%2.2f%%] to extrat doc = %6u, from pos [%9u to %9u]",(i*100.0/nids), 
	  					docIds[i], wcsa->doc_offsets_sids[docIds[i]], wcsa->doc_offsets_sids[docIds[i]+1]);
		}
		  ulong doclen;
	  uchar *doc;
	  extractWords((void *) wcsa, wcsa->doc_offsets_sids[docIds[i]], wcsa->doc_offsets_sids[docIds[i]+1], &doc, &doclen);
	  
	  if (!(i%PROGRESS)){
	  	printf("--> docLen = %lu",doclen);
	}
	  T[i] = doc;
	  l[i]=doclen;
	  *tot_chars += doclen;
	}
	*texts = T;
	*lens = l; 
}
	

/** ------------------------------------------------------------------ 
 * Given "ndocs" document-ids \in [0..ndocs-1] --> returns the text of such documents *
 * \0 is also added to the end.
 *------------------------------------------------------------------ */
int extractTextOfDocument(void *index, uint docId, uchar **text, uint *len){
  	twcsa* wcsa = (twcsa *) index;	
	
	ulong doclen;
	uchar *doc;
	extractWords((void *) wcsa, wcsa->doc_offsets_sids[docId], wcsa->doc_offsets_sids[docId+1], &doc, &doclen);
	  
	*text = doc;
	*len = (uint)doclen;
}
	












/** *******************************************************************************
  * Showing some statistics and info of the index
  * *******************************************************************************/
void printInfoReduced(twcsa *wcsa) {
	  //not implemented: function not available
}

		/* Shows summary info of the index */
int printInfo(void *index) {
	uint n;

	twcsa *wcsa = (twcsa *) index;
	
	unsigned long indexSize;
	uint intIndexSize, presentationSize;
	int err;
	
	err = index_size(index, &indexSize);
	if (err!=0) return err;
	err = sizeIntIndex(wcsa->myicsa, &intIndexSize); 	
	if (err!=0) return err;
	
	presentationSize = indexSize - intIndexSize;
	
		printf("\n ===================================================:");		
		printf("\n Summary of Presentation layer:");		
		printf("\n   Number of valid words (SEsize) = %zu",(size_t) wcsa->seSize);
		printf("\n   Number of different words = %ld",wcsa->n);
		printf("\n   Number of documents  = %lu (ptr_offsets_beg= %lu bytes)", (ulong) wcsa->ndocs, (ulong)(1+wcsa->ndocs) * sizeof(uint));
		printf("\n   WCSA structure = %zu bytes", (size_t) sizeof(twcsa));
		

		uint totalpointers = ((((wcsa->n+1)* (wcsa->wordsData.elemSize))+W-1) /W) * (sizeof(uint));
		uint totalasciizone = wcsa->wordsData.wordsZoneMem.size * sizeof(byte) ;
		uint totalwords = totalasciizone + totalpointers;

		printf("\n   Size Of words structure (%d bytes):",totalwords);
		printf("\n      [     pointers = %d bytes || AsciiZone = %d bytes", totalpointers, 	totalasciizone);	
				
		printf("\n\n Total = **  %u bytes (in RAM) **",presentationSize);		
		//printf("\n\n @@ Summary of self-index on Integers:");
		err = printInfoIntIndex(wcsa->myicsa, " ");
		if (err!=0) return err;
 			
		printf("\n ===================================================:");		
		printf("\n");
		return 0;
	}

/**------------------------------------------------------------------
 * structsSize.
 *	Counts the memory amount needed by the Facade (Presentation Layer).
 * skipping the stop_words hash table 
 ----------------------------------------------------------------- */
uint structsSizeMem(twcsa *wcsa) {
	return 0;   //not implemented: function not available for this index.
}


/** for debugging **/
void printWord(uchar *str, uint len) {
		uint i;
		for (i=0;i<len;i++)
			fprintf(stderr,"%c",str[i]);
}


	/** saves the content of the file SE (ids of the source words) **/
int saveSEfile (char *basename, uint *v, ulong n) {
	char outfilename[255];
	int file;
	sprintf(outfilename,"%s.%s",basename,SE_FILE_EXT);
	unlink(outfilename);
	if( (file = open(outfilename, O_WRONLY|O_CREAT,S_IRWXG | S_IRWXU)) < 0) {
		printf("Cannot open file %s\n", outfilename);
		exit(0);
	}

	write(file, v, sizeof(uint) * n ); 
	close(file);
}



double getTime2 (void)
{
	double usertime, systime;
	struct rusage usage;

	getrusage (RUSAGE_SELF, &usage);

	usertime = (double) usage.ru_utime.tv_sec +
		(double) usage.ru_utime.tv_usec / 1000000.0;
	systime = (double) usage.ru_stime.tv_sec +
		(double) usage.ru_stime.tv_usec / 1000000.0;

	return (usertime + systime);
}



/**------------------------------------------------------------------ 
  *  MAIN PROGRAM.
  *------------------------------------------------------------------ */
#ifdef FACADEWITHMAIN
	int main(int argc, char* argv[])
	{
		
			
		
		char *infile, *outbasename, *stopwordsfile;	// Name of in/out files
		byte *inputBuffer;
		ulong finsize;
	
	 	int f_in;
		void *Index;

		
		printf("\n*Word-based iCSA: A word-based CSA");
		printf("\n*CopyRight (c) 2008 [LBD & G.N.]\n\n");
	
		// Reads input parameters from command line.
		if(argc < 3) {
			printf("Use: %s <in file> <out basename> \n", argv[0]);
			exit(0);
		}
	
		// Reads params (input file, output basename, and stopwords file)
		infile = argv[1];
		outbasename = argv[2];
		stopwordsfile = argv[3];
		
		finsize= fileSize(infile);
		
		if (! finsize) {
			printf( "\nFILE EMPTY OR FILE NOT FOUND %s !!\nSkipping processement ...\n",infile);
			exit(0);
		}	
	
		// Opening the input text file.
		if( (f_in = open(infile, O_RDONLY)) < 0) {
			printf("Cannot read file %s\n", infile);
			exit(0);
		}	
		inputBuffer = (byte *) malloc(finsize *sizeof(byte));// +1);
		read (f_in,inputBuffer,finsize);	
		close (f_in);	
		
		
	{
		//printf("\n parametros <<%s>>\n\n",stopwordsfile);	
		build_index (inputBuffer, finsize, stopwordsfile, &Index);  /** building the index */

//		/** recovering the source text from the index */
			{
				double start, end;
				start = getTime2();
				ulong size;
				get_length(Index, &size);
				char extension[10]= ".source";
				
				//recoverSourceText1((twcsa*) Index, outbasename,extension, size);
				strcat(extension,"2");
				recoverSourceText2((twcsa*) Index, outbasename,extension,size);
				end = getTime2();	
				fprintf(stderr, "\nRecovering source file time: %.3f secs\n", end-start );	
			}
//		
		// DISPLAYING THE OCCURRENCES OF A TEXT PATTERN (word/phrase).
			{unsigned char textPattern[MAX_TEXT_PATTERN_SIZE];
			 int error = 0;
			ulong numocc,numc, length, i, *snippet_len, tot_numcharext = 0, numpatt;
			uchar *pattern, *snippet_text;
				 
				 pattern = textPattern;
			 printf("\nSEARCH TEST for DISPLAY (pizzachili interface)\n");
				while(1) {	
					printf("Intro string: ");
					fgets((char*)textPattern, MAX_TEXT_PATTERN_SIZE, stdin);
					if (!strcmp((char*)textPattern,"\n") ) break;
					 textPattern[strlen((char*)textPattern)-1] = '\0';
		
					length = strlen( (char*)textPattern);
					numc=50;
         
//					error =	display (Index, textPattern, length, numc, &numocc, 
//							    	 &snippet_text, &snippet_len);
					error =	displayWords (Index, textPattern, length, numc, &numocc, 
							    	 &snippet_text, &snippet_len,1);
					
					if (error){ fprintf(stderr, "%s\n", "Hubo un error durante display");exit(0);}
		
						fprintf(stderr,"\n acabou display");fflush(stderr);			
					{//show the results
						ulong j, len = length + 2*numc;
					    char blank = '\0';
						fprintf(stderr,"\n length = %d",length);
						fprintf(stderr,"\n pattern = %s",pattern);fflush(stderr);
						fprintf(stderr,"\n numocc = %d",numocc);fflush(stderr);
						fprintf(stderr,"\n snippet len = %d",len);fflush(stderr);
						fprintf(stderr,"\n =========");fflush(stderr);		
						for (i = 0; i < numocc; i++){
							fprintf(stderr,"\n[%2d][len=%3d]<<",i+1,snippet_len[i]);fflush(stderr);
							fwrite(snippet_text+len*i,sizeof(uchar),snippet_len[i],stderr);fflush(stderr);
							fprintf(stderr,">>");fflush(stderr);
						}
					}
					numpatt--;
					
					for(i=0; i<numocc; i++) {
						tot_numcharext += snippet_len[i];
					}
						
					if (numocc) {
						free (snippet_len);
						free (snippet_text);
					}
					
					printf("Ocurrences = %d\n", numocc);
					if (!strcmp((char*)textPattern,"\n") ) break;
				}
			}
	
//
//
//	// SEARCHING FOR A TEXT PATTERN (word/phrase).
//	{unsigned char textPattern[MAX_TEXT_PATTERN_SIZE];
//	 int occ;
//	 int len;
//	 uint *occs;
//	 int i;
//	 printf("\nSEARCH TEST for LOCATE\n");
//		while(1) {	
//			printf("Intro string: ");
//			fgets((char*)textPattern, MAX_TEXT_PATTERN_SIZE, stdin);
//			len = strlen((char*)textPattern);
//			if (!strcmp((char*)textPattern,"\n") ) break;
//			textPattern[len-1] = '\0';
//			len --;
//			
//			//occs = locateTextOcurrences(wcsa,textPattern,&occ);
//			// locate(Index, textPattern, len, (ulong **)&occs, (ulong *)&occ);
//			  locateWord(Index, textPattern, len, (ulong **)&occs, (ulong *)&occ, 0);
//			
//			printf("\n*** %s occurs %d times: In the source text in positions:\n\t",textPattern,occ);
//			for (i=0;i<occ;i++) 
//				printf("[%u]",occs[i]);
//			fflush(stderr);	
//			free(occs);		
//			
//			if (!strcmp((char*)textPattern,"\n") ) break;
//		}
//	}	
//	
//

		// COUNTING THE OCCURRENCES OF A TEXT PATTERN (word/phrase).
		/*
		{unsigned char textPattern[MAX_TEXT_PATTERN_SIZE];
		 int occ;
		 int len;
		 printf("\nSEARCH TEST for COUNT.\n");
			while(1) {	
				printf("Intro string: ");
				fgets((char*)textPattern, MAX_TEXT_PATTERN_SIZE, stdin);
				len = strlen((char*)textPattern);
				if (!strcmp((char*)textPattern,"\n") ) break;
				textPattern[len-1] = '\0';
				len --;
				
				count(Index, textPattern, len, (ulong *)&occ);			
				//occ = countTextOcurrences(wcsa,textPattern);
				printf("Ocurrences = %d\n", occ);
			}
		}
		printf("\n END COUNTING OCCURRENCES OF PATTERNS. ...\n");
		//exit(0);
		*/

		/** saving the index to disk*/
		save_index (Index, outbasename);

		/** tells the mem used by the index */
		ulong indexsize;		
		index_size(Index, &indexsize);
		fprintf(stderr,"Index occupied %d bytes, 2 extra mallocs = %d",indexsize,2* sizeof(uint));

		/** freeing the index */		 			
		free_index(Index);	 			
	 		 
	}
}
	
#endif


	
