#include "ii.h"

/** ------------------------------------------------------------------
  * loads the source plain text into memory
  * ------------------------------------------------------------------*/
int loadTextInMem (byte **text, ulong *size, char *basename) {
	FILE *file;
	size_t sizeFile;
	byte *buffer;

	char *filename;	
	filename = (char *)malloc(sizeof(char)*(strlen(basename)+10));
	strcpy(filename, basename); //strcat(filename, ".");	strcat(filename, SOURCETEXT);

	sizeFile= fileSize(filename);
	//fprintf(stderr,"\n need to allocate %lu bytes", (ulong) sizeFile); fflush(stderr);

	buffer = (byte *) malloc (sizeof(byte) * sizeFile);
	//fprintf(stderr,"\n After allocate %lu bytes", (ulong) sizeFile); fflush(stderr);

	fprintf(stderr,"\n\tLoading Source Text into memory from %s", filename);
	if( (file = fopen(filename, "r")) == NULL) {
		printf("Cannot open file %s\n", filename);
		exit(0);
	}
	
	size_t err;
	err = fread  (buffer, sizeof(byte), sizeFile, file);
	if (sizeFile != err) {
	  printf("\n ii.c: loadTextInMem:: leidos %zu bytes but required %lu ", err,sizeFile);
	  exit(0);
	}
	
	//fprintf(stderr,"\n first letter = %c, last letter = %c",buffer[0],buffer[sizeFile -1]);
	
	fclose(file);			
	*text = buffer;
	*size = sizeFile;
	
	free(filename);
	return 0;
}


/**------------------------------------------------------------------
Compares two slots (alphanumericaly). For qsort of q-grams
------------------------------------------------------------------ */			   
int qsortCompareAlpha(const void *arg1, const void *arg2) {
	tauxslots *a1 = (tauxslots *) arg1;
	tauxslots *a2 = (tauxslots *) arg2;
	return strcmp((char*)a1->word, (char *)a2->word);	
}

/**------------------------------------------------------------------
 Processes the source sequence:
 Gathers the different words, its frequency and sets also the 
 pointers for the byte *words array (sorted alphanumerically).
 Sets also nwords = number of diff words.
 * parses the text, gathers the vocabulary, freqs[], and maxNumOccs
 ----------------------------------------------------------------- */
int CreateVocabularyOfWords (void *index, ulong *docboundaries, uint ndocs){
	twcsa *wcsa=(twcsa *) index;	
	byte *T = wcsa->text;
	ulong n = wcsa->sourceTextSize;

	uint nwords;
	//byte **words;

	Wt_hash hw;  //Hash of words
	//uint maxN= MAX_NUM_OF_DIFFERENT_Q_GRAMS;  // !!!! ENORMOUS !!!
	//Stimation (Using Heap's law) of the number of different words.
	
	ulong maxN;
	if(n<5000000) n = 5000000;
	maxN = (unsigned int) floor(3.9* pow(n , 0.60) );
	if (maxN > 100000000)   //100Million-words!
	  maxN=100000000;

	hw = wtinitialize_hash(maxN); //creates the hash table & zoneMem
	
	fprintf(stderr, "\n max vocab size allowed (maxN) = %lu textSize=%lu",maxN,n); 
	ulong i;
	ulong addrInTH;
	ulong totallenWords;

	//1st pass (processing the file)   -----------------------------------------	
	{ 
		byte *pbeg=NULL;
		byte *pend,*wordstart,*aWord;
		register ulong size;
		ulong seSize=0;
		register ulong i;

		
		uint progress_cut = ndocs/20;	
		
		ulong j;
		for (j=0;j<ndocs;j++) {
			if (!(j%progress_cut)){
				printf("\n# 1st pass %2.2f%% docs processed", 100.0 * ((j*1.0)/(ndocs)) );
				fflush(stdout);
			}		

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

				//fprintf(stderr,"\n parsed word:");
				//printWord(aWord,size);
				
				//Processement done for each word word			
				i = wtinHashTable(hw,aWord, size, &addrInTH );				
				if (!i){
					wtinsertElement (hw,aWord, size, &addrInTH);	
					//fprintf(stderr,"\n Adding word: <<%s>> (size=%d) to the hashTable",hw->hash[addrInTH].word,size);
				}	
				else {	
					hw->hash[addrInTH].freq ++;					
				}
				seSize ++;
			}//while pbeg<pend
		}//for docs
	nwords = hw->NumElem;
	totallenWords = hw->zone_size;

		fprintf(stderr,"\n1st pass ends: TOTAL distinct words: %u totalWords = %lu",nwords,seSize);
		if (pbeg)
		  fprintf(stderr,"\n1st last offset processed = %lu", pbeg -T);
	}//1st pass ends ----------------------------------------


	 
	tauxslots *aux = (tauxslots *) malloc (sizeof(tauxslots) * nwords);
	{/**Sorting **/
		//copying to auxiliar structure for sorting
		ulong curr;

		curr = 0;
		for (i=0;i<hw->SIZE_HASH; i++) {
				if (hw->hash[i].word != NULL){
					aux[curr].word = hw->hash[i].word;
					//aux[curr].slot = i;  //not needed
					aux[curr].freq = hw->hash[i].freq;
					curr++;
				}
		}

		//sorting
		qsort(aux, nwords, sizeof(tauxslots), qsortCompareAlpha);	
									
		wcsa->nwords = nwords;
		wcsa->freqs = (uint *)  malloc(sizeof(uint) * wcsa->nwords); 

		ulong sumfreqs=0; uint maxF =0;
		for(i = 0; i < (wcsa->nwords); i++) {
			//memcpy(zoneMem, (const void *) aux[i].word, (q));
			//zoneMem += (q);
			wcsa->freqs[i] = aux[i].freq;
			if (maxF < aux[i].freq) maxF = aux[i].freq;
			sumfreqs += aux[i].freq;
		//	fprintf(stderr,"\n sorted word [%d] = %s",i,aux[i].word);
		}

		wcsa->maxNumOccs = maxF;
		fprintf(stderr,"\n\n sumfreqs = %lu, maxFreq of word = %u",sumfreqs,maxF);
		if (sumfreqs > (4000000000L)) {
			printf("\n sumfreq=%lu!!!.\n I will exceed ilist-representation number of postings. We have a problem for CIKM\n", sumfreqs);
			//exit(0);
		}
		  
	}

	// Creating the words of the vocabulary...
	{  
	/** copying the words into the twcsa structure. */
		uint *tmpOffsets = (uint *) malloc (sizeof(uint) * (nwords +1) );  //1 extra uint (points to virtual "n+1"-th word.
		uint tmpOffset =0;
		 
		byte *zoneMem,*src;
		uint i;
								 
		//Moving data from posInHT to WCSA structure
 		wcsa->wordsData.wordsZoneMem.size = totallenWords - nwords; //without '\0' bytes (end-tag).
 		wcsa->wordsData.wordsZoneMem.zone = (byte *) malloc ( wcsa->wordsData.wordsZoneMem.size * sizeof(byte) );  
 		zoneMem = wcsa->wordsData.wordsZoneMem.zone;					
		for(i = 0; i < nwords; i++) {
			src = aux[i].word;		  //copying the word
			tmpOffsets[i]=tmpOffset;  //offset in zoneMem
			while (*src) {*zoneMem++ = *src++; tmpOffset++;}  //moving data until '\0'
			//*zoneMem='\0'; zoneMem++; tmpOffset++;            //copies also the '\0'			
		}
		tmpOffsets[nwords]=tmpOffset; //setting pointer to the "virtual" word {nwords+1}^{th}
		
		//kbit encoding of the offsets
		uint elemSize = bits(tmpOffset);
		wcsa->wordsData.elemSize = elemSize;
		wcsa->wordsData.words = (uint *) malloc (((((nwords +1)*elemSize)+W-1) /W) * sizeof(uint));  //with 1 extra slot !.
		wcsa->wordsData.words[((((nwords +1)*elemSize)+W-1) /W)   -1 ] =0000;
		//		fprintf(stderr,"\n ElemSize = %d, maxOffset = %d",elemSize,tmpOffset);

		tmpOffset=0;
		for (i=0; i<=nwords; i++) {  //setting "zeroNode+1" offsets
				bitwrite(wcsa->wordsData.words, tmpOffset, elemSize, tmpOffsets[i]);
				tmpOffset+=elemSize; 
		}


/*
		unsigned char * str; uint len2;
		for (i = 0; i<wcsa->nwords; i++) {
			
				getWord(wcsa,i,&str,&len2);				
				fprintf(stderr,"\n freq[%6d]=%6u ",i,  wcsa->freqs[i]);
				fprintf(stderr,", words[%6d] = ",i);
				printWord(str,len2);
				//fprintf(stderr,"%s",(byte *)(tmpOffsets[i]+ wcsa->wordsData.wordsZoneMem.zone));
				
				//fprintf(stderr,"\n sorted word [%d] = %s",i,aux[i].word);
			//}
		}
*/
		
		free(tmpOffsets);		
	}
		
	free(aux);	
	wtdestroy_hash(hw);

	return 0;
}








/**------------------------------------------------------------------
 Loads the VOCABULARY of SCDC, SS and CC values and sets also the 
 pointers for the byte *words array.
 ----------------------------------------------------------------- */
int loadVocabulary (void *index, char* basename){
	twcsa *wcsa=(twcsa *) index;	
	uint nwords;
	//byte *zoneMem;
	char *filename;	
	uint sizeFile;
	int file;
	//ulong i; 	
	int errr;
	filename = (char *)malloc(sizeof(char)*(strlen(basename)+10));
	
	{	
		strcpy(filename, basename);
		strcat(filename, ".");
		strcat(filename, VOCABULARY_WORDS_FILE_EXT);
		sizeFile= fileSize(filename)-sizeof(uint);

		fprintf(stderr,"\n\tLoading vocabulary of words from index %s", filename);

		if( (file = open(filename, O_RDONLY)) < 0) {
			printf("Cannot open file %s\n", filename);
			exit(0);
		}		
		
		errr= read(file, &nwords, sizeof(uint));   //number of words in the vocab
		wcsa->nwords = nwords;				
	
		errr= read(file, &(wcsa->wordsData.elemSize), sizeof(uint));  //number of bits (to represent v);	        	
		errr= read(file, &(wcsa->wordsData.wordsZoneMem.size), (sizeof(uint)));	//number of words	

		//allocating the memory needed for all words and reading them	//(ascii) << no \0 chars are needed>>.	
		wcsa->wordsData.wordsZoneMem.zone = (byte *) malloc(wcsa->wordsData.wordsZoneMem.size * sizeof(byte));			
	 	errr = read(file, (wcsa->wordsData.wordsZoneMem.zone), wcsa->wordsData.wordsZoneMem.size * sizeof(byte) );

	
		//reading the offsets of the words (kbitArray that points to offsets in zoneMem of words.
		wcsa->wordsData.words = (uint *) malloc (((((nwords+1)* (wcsa->wordsData.elemSize))+W-1) /W) * sizeof(uint));	
		wcsa->wordsData.words[ ((((nwords+1)*(wcsa->wordsData.elemSize))+W-1) /W)   -1 ] =0000;
		errr = read(file, (wcsa->wordsData.words), ((((nwords+1)* (wcsa->wordsData.elemSize))+W-1) /W) * (sizeof(uint)));
		
				
	}		
	free(filename);

	return (0);
} 


/** ------------------------------------------------------------------
  * Saves the VOCABULARY of SCDC to disk
  * ------------------------------------------------------------------*/
int saveVocabulary (void *index, char* basename) {
	int file; int errw;
	
	char *filename;	
	filename = (char *)malloc(sizeof(char)*(strlen(basename)+10));
	strcpy(filename, basename); strcat(filename, ".");	strcat(filename, VOCABULARY_WORDS_FILE_EXT);
	unlink(filename);
	
	twcsa *wcsa=(twcsa *) index;	
	uint nwords= wcsa->nwords;
	{		
		unlink(filename);
		fprintf(stderr,"\tSaving Vocabulary (list of sorted words) into disk %s\n", filename);		
		if( (file = open(filename, O_WRONLY|O_CREAT,S_IRWXG | S_IRWXU)) < 0) {
			printf("Cannot open file %s\n", filename);
			exit(0);
		}	

		uint elemSize = wcsa->wordsData.elemSize;
		errw = write(file, &(wcsa->nwords), sizeof(uint));	        //number of words			
		errw = write(file, &elemSize, sizeof(uint));                //pointer bits 
		errw = write(file, &(wcsa->wordsData.wordsZoneMem.size), sizeof(uint)); //characters.
		
		//the number of canonical words
		errw = write(file, (char *)wcsa->wordsData.wordsZoneMem.zone, wcsa->wordsData.wordsZoneMem.size * sizeof(byte));
		errw = write(file, (char *)wcsa->wordsData.words, ((((nwords+1)* (elemSize))+W-1) /W) * (sizeof(uint)) );
	
		close(file);		
	}	
		
	free(filename);
	return 0;
}





/** **********************************************************************/
/** Some functions used during index construction                        */
/** **********************************************************************/

	/* document beginnings within the source text are marked in offsets */
int loadDocBeginngins(ulong **offsets, uint *docs, char *basename) {
	int file;
	ssize_t readbytes;

	char *filename;	
	filename = (char *)malloc(sizeof(char)*(strlen(basename)+20));
	strcpy(filename, basename); strcat(filename, ".");	strcat(filename, DOCBOUNDARIES_FILE_EXT);

	fprintf(stderr,"\t Loading document boundaries (as integers) from disk %s...", filename);		
	if( (file = open(filename, O_RDONLY)) < 0) {
		printf("Cannot open file %s\n", filename);
		exit(0);
	}	

	uint ndocs = (uint) (fileSize(filename)/sizeof(ulong) -1);
	
	ulong *docBeginnings = (ulong *) malloc (sizeof(ulong) * (ndocs +1));

	readbytes = read(file, docBeginnings, sizeof(ulong) * (ndocs+1));		
	close(file);	
	*docs = ndocs;
	*offsets = docBeginnings;

	fprintf(stderr,"%u docs loaded \n", ndocs);		
	free(filename);	
	return 0;		
}


/** ******************************************************************************************/

/** ------------------------------------------------------------------
  * Creates the lists of occurrences of all the words
  * Doc-oriented: Only 1 occurrence per doc is marked for each word.
  * A word belongs to a doc if its associated codeword starts in that doc.
  * ------------------------------------------------------------------*/



/** ------------------------------------------------------------------
  * Creates the lists of occurrences of all the words
  * Doc-oriented: Only 1 occurrence per doc is marked for each word.
  * A word belongs to a doc if its associated codeword starts in that doc.
  * ------------------------------------------------------------------*/
int createListsOfOccurrences(void *index, uint ***occList, uint ***termfreqdoc, uint **idsindoc, uint **lenList, ulong *docboundaries, uint ndocs) {
	uint i;	
	twcsa *wcsa=(twcsa *) index;	
	byte *text = wcsa->text;
	uint n = wcsa->nwords;
	uint offset;  //document being processed
	
	fprintf(stderr,"\n Entering 'create list of occurrences - Doc-oriented'\n.");fflush(stderr);

	//allocating memory for the postings
	uint **occs;
	uint **tfd;
	occs = (uint **) malloc (sizeof(uint *) * n);
	tfd = (uint **) malloc (sizeof(uint *) * n);
	for (i=0;i<n;i++) occs[i]= (uint *) malloc (sizeof(uint) * wcsa->freqs[i]);
	for (i=0;i<n;i++)  tfd[i]= (uint *) malloc (sizeof(uint) * wcsa->freqs[i]);
	
	//temporal array to keep the last index in the list of the i-th word
	uint *currpos = (uint *) malloc (sizeof(uint)*n);
	//temporal array to keep the doc of the last occurrence of the i-th word
	int *lastDoc = (int *) malloc (sizeof(int)*n);
	for (i=0;i<n;i++) {currpos[i]=0; lastDoc[i] = -1;}
		
	//Loads back all the words into a hashtable (for speed).
	
	Wt_hash hw;  //Hash of words
	hw = wtinitialize_hash(wcsa->nwords); //creates the hash table & zoneMem
	ulong addrInTH;
	{
	  //randomizing word-ids (since words are sorted!!!
	  uint *wordIDs;	
	  uint nids = wcsa->nwords;
	  wordIDs = (uint *) malloc (sizeof(uint) * nids);
	  for (i=0;i<nids;i++)
		  wordIDs[i] = i;
	  
	  makeRandom(wordIDs,nids);	  
	  uint id;
	  for (id = 0; id < wcsa->nwords; id++) {
		unsigned char *str; uint len;
		i = wordIDs[id];
		getWord(wcsa,i,&str,&len);
		ulong iht = wtinHashTable(hw,str, len, &addrInTH);
		if (!iht) {
			wtinsertElement_id(hw,str, len, i,  &addrInTH);//ids are assigned from 0 on.
		}
		else {
			printf("error during  ii.c::createListsOfOccurrences(), duplicated word found: size = %u word==>",len);
			printWord(str,len); 			
			fflush(stderr);fflush(stdout);				
			parseWordIntoIdDebug(wcsa,str,len);
			fflush(stderr);fflush(stdout);							
			exit(0);			  					
		}
	  }
	  free(wordIDs);
	}
	

	//parses back the text and creates the occurrences.
	uint progress_cut = ndocs/20;	

	uint *idsperdoc = (uint *) malloc (sizeof(uint) * ndocs);
	for (i=0; i< ndocs;i++) idsperdoc[i]=0;
	
	int num=0;
	for (i=0;i<ndocs;i++) {
		
		
		if (!(i%progress_cut)){
			printf("\n2nd pass # %2.2f%% docs processed", 100.0 * ((i*1.0)/(ndocs)) );
			fflush(stdout);
		}
		
		offset = i;  //current document
		text = docboundaries[i]+ wcsa->text;
		uint bytesDoc = docboundaries[i+1] -docboundaries[i];
		uint posindoc=0;
		
		byte *pbeg,*pend,*wordstart,*aWord;
		register uint size;
		pbeg = text;
		pend = text +bytesDoc;
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
			posindoc=wordstart-text;  //position within the current document.
			
			//fprintf(stderr,"\n Doc %u, byte = %lu, parsed word... size = %u bytes, word = ",offset,wordstart-wcsa->text,size);
			//printWord(aWord,size); 
		
			/*
			if ( (num = parseWordIntoId(wcsa,aWord,size)) == -1) {
				printf("error during  ii.c::createListsOfOccurrences(), word not found: size = %u word==>",size);
				printWord(aWord,size); 
				printf("<==");
				printf("\nposition in doc %u = %lu\n", i, (ulong) (posindoc));
				printf("\nposition in text = %lu\n", (ulong) docboundaries[i]+(posindoc));
				
				fflush(stderr);fflush(stdout);				
				parseWordIntoIdDebug(wcsa,aWord,size);
				fflush(stderr);fflush(stdout);				
				
				exit(0);
			}	
			*/
			ulong iht = wtinHashTable(hw,aWord, size, &addrInTH );
			if (!iht) {
				printf("error during  ii.c::createListsOfOccurrences(), word not found: size = %u word==>",size);
				printWord(aWord,size); 
				printf("<==");
				printf("\nposition in doc %u = %lu\n", i, (ulong) (posindoc));
				printf("\nposition in text = %lu\n", (ulong) docboundaries[i]+(posindoc));
				
				fflush(stderr);fflush(stdout);				
				parseWordIntoIdDebug(wcsa,aWord,size);
				fflush(stderr);fflush(stdout);				
				
				exit(0);			  
			}
			num = hw->hash[addrInTH].id;
			idsperdoc[i]++;
			
			//fprintf(stderr,"\n word %s appeared at document %s",wcsa->words[num],&text[i-1]);
			if (offset != lastDoc[num]) {  //only 1 occurrence per block is marked.
				occs[num][currpos[num]] = offset;  //word "num" occurrs in block "offset"
				tfd[num][currpos[num]]=1;
				currpos[num]++;			
				lastDoc[num] = offset;
			}	
			else {
				tfd[num][currpos[num]-1] +=1;	   // num of occs of term num into doc [currpos[num] -1]
			}			
		}
		
	}
		
	free(lastDoc);	
	wtdestroy_hash(hw);

	*termfreqdoc=tfd;
	*occList = occs;	
	*idsindoc = idsperdoc;
	*lenList = currpos;
	
	return 0;
}


/** *******************************************************************************************/





/*
int createListsOfOccurrencesNOHASHTABLE(void *index, uint ***occList, uint **lenList, ulong *docboundaries, uint ndocs) {
	uint i;	
	twcsa *wcsa=(twcsa *) index;	
	byte *text = wcsa->text;
	uint n = wcsa->nwords;
	uint offset;  //document being processed
	
	fprintf(stderr,"\n Entering 'create list of occurrences - Doc-oriented'\n.");fflush(stderr);

	//allocating memory for the postings
	uint **occs;
	occs = (uint **) malloc (sizeof(uint *) * n);
	for (i=0;i<n;i++) occs[i]= (uint *) malloc (sizeof(uint) * wcsa->freqs[i]);
	
	//temporal array to keep the last index in the list of the i-th word
	uint *currpos = (uint *) malloc (sizeof(uint)*n);
	//temporal array to keep the doc of the last occurrence of the i-th word
	int *lastDoc = (int *) malloc (sizeof(int)*n);
	for (i=0;i<n;i++) {currpos[i]=0; lastDoc[i] = -1;}
		
	//decode && increase freqs
	i=0;
	
	
uint progress_cut = ndocs/20;	

	int num=0;
	for (i=0;i<ndocs;i++) {
		
		if (!(i%progress_cut)){
			printf("\n2nd pass # %2.2f%% docs processed", 100.0 * ((i*1.0)/(ndocs)) );
			fflush(stdout);
		}
		
		offset = i;  //current document
		text = docboundaries[i]+ wcsa->text;
		uint bytesDoc = docboundaries[i+1] -docboundaries[i];
		uint posindoc=0;
		
		byte *pbeg,*pend,*wordstart,*aWord;
		register uint size;
		pbeg = text;
		pend = text +bytesDoc;
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
			posindoc=wordstart-text;  //position within the current document.
			
			//fprintf(stderr,"\n Doc %u, byte = %lu, parsed word... size = %u bytes, word = ",offset,wordstart-wcsa->text,size);
			//printWord(aWord,size); 
			
			//if ( (num = parseWordIntoId(wcsa,aWord,size)) == -1) {
			//	printf("error during  ii.c::createListsOfOccurrences(), word not found: size = %u word=>",size);
			//	printWord(aWord,size); 
			//	printf("\nposition in file = %lu\n", (ulong) (posindoc));
			//	fflush(stderr);fflush(stdout);
			//	exit(0);
			//}
			
			if ( (num = parseWordIntoId(wcsa,aWord,size)) == -1) {
				printf("error during  ii.c::createListsOfOccurrences(), word not found: size = %u word==>",size);
				printWord(aWord,size); 
				printf("<==");
				printf("\nposition in doc %u = %lu\n", i, (ulong) (posindoc));
				printf("\nposition in text = %lu\n", (ulong) docboundaries[i]+(posindoc));
				
				fflush(stderr);fflush(stdout);				
				parseWordIntoIdDebug(wcsa,aWord,size);
				fflush(stderr);fflush(stdout);				
				
				exit(0);
			}			

			//fprintf(stderr,"\n word %s appeared at document %s",wcsa->words[num],&text[i-1]);
			if (offset != lastDoc[num]) {  //only 1 occurrence per block is marked.
				occs[num][currpos[num]] = offset;  //word "num" occurrs in block "offset"
				currpos[num]++;			
				lastDoc[num] = offset;
			}					
		}
		
	}
		
	free(lastDoc);	

	*occList = occs;	
	*lenList = currpos;
	return 0;
}
*/













/** ------------------------------------------------------------------
  * Prepares the source uint array that should be passed to build_il()
  * Inputs: lenList[], occList[][], nwords and maxPost
  * Output: formatedList[] and formatedLen
  * Format of the created source array.
  * 	// [Nwords=N]
		// [maxPostvalue]
		// [lenPost1][post11][post12]...[post1k]
		// [lenPost2][post21][post22]...[post2k]
		//  ... 
		// [lenPostN][postN1][postN2]...[postNk]
  * ------------------------------------------------------------------*/
int prepareSourceFormatForIListBuilder (uint nwords, uint maxPost, uint *lenList, 
										uint **occList, uint **formatedList, ulong *formatedLen){
	ulong i,j;
	ulong sourcelen=1+1;  //nwords and maxPost
	for (i=0;i<nwords;i++) 	sourcelen += 1 + lenList[i];
	
	uint *source = (uint *) malloc (sourcelen * sizeof(uint));
	if (!source) {
		fprintf(stderr,"\n could not allocate %lu bytes... [PrepareSourceFormatForIlistBuilder]\nexitting!!\n",(ulong) (sourcelen * sizeof(uint)) ); exit(0);
	}
	
	ulong z=0; 
	source[z++]= nwords; 	source[z++]= maxPost;  //z=2;
	for (i=0;i<nwords;i++) {
		source[z++] = lenList[i];
		for (j=0;j<lenList[i];j++) {
			source[z++] = occList[i][j];
		}
	}
	*formatedLen  = sourcelen;
	*formatedList = source;
	return 0;
}



int output_posting_lists_concatenated_DEBUGGING_ONLY (uint nwords, 
					uint maxPost, uint *lenList, uint **occList, char *filename){
	ulong i,j;
	ulong len=0;  //nwords and maxPost
	for (i=0;i<nwords;i++) 	len += lenList[i];	
	uint *dst = (uint *) malloc (len * sizeof(uint));
	
	if (!dst) {
		fprintf(stderr,"\n could not allocate %lu bytes... [output_posting_lists_concatenated]\nexitting!!\n",
						  (ulong) (len * sizeof(uint)) ); exit(0);
	}


	ulong z=0; 
	for (i=0;i<nwords;i++) {
		  for (j=0;j<lenList[i];j++) {
			dst[z++] = occList[i][j];
		}
	}
	
   FILE *f = fopen(filename, "w");
   fwrite(dst,sizeof(uint),len,f);  
   fclose(f);

   free(dst);
	return 0;
}


/**** load those (preprocessed) lists from a  source file **********************/

/** ------------------------------------------------------------------
  * Loads the source uint array that should be passed to build_il()
  * Input: basename[.post is added]
  * Output: formatedList[] and formatedLen
  * Format of the created source array.
  * 	// [Nwords=N]
		// [maxPostvalue]
		// [lenPost1][post11][post12]...[post1k]
		// [lenPost2][post21][post22]...[post2k]
		//  ... 
		// [lenPostN][postN1][postN2]...[postNk]
  * ------------------------------------------------------------------*/

int load_posting_lists_from_file(uint *maxPost, ulong *n_il, uint **source_il, char *basename) {
	
	FILE *file; long errr;
	char filename[256];
	sprintf(filename, "%s.%s",basename, PREPROCESED_POSTINGS);
	fprintf(stderr,"\n\tLoading vocabulary of words from index %s", filename);

	if( (file = fopen(filename, "r")) < 0) {
		printf("Cannot open file %s\n", filename);
		exit(0);
	}		
	
	size_t sizeFile= fileSize(filename);
	uint *buffer = (uint *) malloc (sizeFile);
	//errr= fread(buffer, sizeof(uint), sizeFile,file);   //number of words in the vocab	
	
	//** cambiado 2013.06.03...
	errr= fread(buffer, sizeof(uint), sizeFile/sizeof(uint),file);   //number of words in the vocab	
	//**
	
	fclose(file);
	printf("\n I have read %lu uints = %lu bytes of %lu", errr, errr*sizeof(uint), sizeFile);
	fflush(stdout);fflush(stderr);
	
	
	*n_il = (ulong) (sizeFile / sizeof(uint));
	*source_il = buffer;
	*maxPost = buffer[1];
	
	return 0;
}





/** **********************************************************************/
/** Saving/Loading the index to/from disk                               **/
/** **********************************************************************/

/** Saving some configuration constants: blockSize, sourceTextSize, maxNumOccs */
int saveIndexConstants(void *index, char *basename) {
	int file; int errw ;
	char *filename;	
	filename = (char *)malloc(sizeof(char)*(strlen(basename)+10));
	strcpy(filename, basename); strcat(filename, ".");	strcat(filename, CONSTANTS_FILE);
	fprintf(stderr,"\tSaving the configuration constants to file %s\n", filename);
	unlink(filename);
			
	twcsa *wcsa=(twcsa *) index;

	if( (file = open(filename, O_WRONLY|O_CREAT,S_IRWXG | S_IRWXU)) < 0) {
		printf("Cannot open file %s\n", filename);
		exit(0);
	}

	errw = write(file, &wcsa->sourceTextSize, sizeof(ulong));
	errw = write(file, &wcsa->maxNumOccs, sizeof(uint));
	errw = write(file, &wcsa->ndocs, sizeof(uint));
	
	
	close(file);				
	free(filename);
	return 0;
}

/** Loading some configuration constants: blockSize, sourceTextSize, maxNumOccs */
int loadIndexConstants(void *index, char *basename) {
	int file; int errr;
	char *filename;	
	filename = (char *)malloc(sizeof(char)*(strlen(basename)+10));
	strcpy(filename, basename); strcat(filename, ".");	strcat(filename, CONSTANTS_FILE);
	fprintf(stderr,"\t Loading the configuration constants from file %s\n", filename);
		
	twcsa *wcsa=(twcsa *) index;
	
	if( (file = open(filename, O_RDONLY)) < 0) {
		printf("Cannot open file %s\n", filename);
		exit(0);
	}
	errr = read(file, &wcsa->sourceTextSize, sizeof(ulong));
	errr = read(file, &wcsa->maxNumOccs, sizeof(uint));
	errr = read(file, &wcsa->ndocs, sizeof(uint));


	close(file);			
	free(filename);
	return 0;
}



/** Saving freq vector if used. */
int saveFreqVector(uint *V, uint size, char *basename) {
	int file;
	uint sizeFile = size*sizeof(uint);
	uint *buffer = V;

	char *filename;	
	filename = (char *)malloc(sizeof(char)*(strlen(basename)+10));
	strcpy(filename, basename); strcat(filename, ".");	strcat(filename, FREQVECTOR);

	fprintf(stderr,"\tSaving Vector of frequencies %s\n", filename);
	unlink(filename);
	if( (file = open(filename, O_WRONLY|O_CREAT,S_IRWXG | S_IRWXU)) < 0) {
		printf("Cannot open file %s\n", filename);
		exit(0);
	}
	int errw = write(file, buffer, sizeFile);
	errw=0;
	
	close(file);				
	free(filename);
	return 0;
}

/** Loading  freq vector if used */
int loadFreqVector(uint **V, uint *size, char *basename) {
	int file;
	uint sizeFile;
	uint *buffer;

	char *filename;	
	filename = (char *)malloc(sizeof(char)*(strlen(basename)+10));
	strcpy(filename, basename); strcat(filename, ".");	strcat(filename, FREQVECTOR);

	sizeFile= fileSize(filename);
	buffer = (uint *) malloc (sizeFile);

	fprintf(stderr,"\tLoading Vector of frequencies %s\n", filename);
	if( (file = open(filename, O_RDONLY)) < 0) {
		printf("Cannot open file %s\n", filename);
		exit(0);
	}
	int errr = read(file, buffer, sizeFile);
	errr=0;
	close(file);			
	free(filename);

	*V = buffer;
	*size = sizeFile/(sizeof(uint));	
	return 0;
}






/***************************************************************************************/


/** for debugging **/
void printWord(uchar *str, uint len) {
		uint i;
		for (i=0;i<len;i++)
			fprintf(stderr,"%c",str[i]);
}





/*********************************************************/
int int_rand_range_IDS(uint  a,uint b){
	uint n = (b-a+1);
	return a+ ((int)  ((n*1.0*rand()/ (RAND_MAX + 1.0)) ));
}	

/* Reorders randomly all the elments of V */
void makeRandom (uint *V, uint n) {
  int r=n-1;
  uint i;
  uint rdnpos;
  
  for (i=0;i<n-1; i++) {
	rdnpos = int_rand_range_IDS(0,r);
	
	uint tmp;
	tmp = V[r];
	V[r]= V[rdnpos];
	V[rdnpos] = tmp;
	r--;
  }
  
 // for (i=0;i<n;i++)
 //	  printf("[%u]",V[i]);
  
}
