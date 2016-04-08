


///** ------------------------------------------------------------------
//  * loads the compressed text (ETDC) into memory
//  * ------------------------------------------------------------------*/
//int loadTextInMem (byte **text, uint *size, char *basename) {
//	int file;
//	uint sizeFile;
//	byte *buffer;
//
//	char *filename;	
//	filename = (char *)malloc(sizeof(char)*(strlen(basename)+10));
//	strcpy(filename, basename); strcat(filename, ".");	strcat(filename, ETDCDATA);
//
//	sizeFile= fileSize(filename);
//	buffer = (byte *) malloc (sizeof(byte) * sizeFile);
//
//	fprintf(stderr,"\tLoading Compressed Text into memory from %s\n", filename);
//	if( (file = open(filename, O_RDONLY)) < 0) {
//		printf("Cannot open file %s\n", filename);
//		exit(0);
//	}
//	read(file, buffer, sizeFile);
//	close(file);			
//	*text = buffer;
//	*size = sizeFile;
//	
//	free(filename);
//	return 0;
//}
//
//
///** ------------------------------------------------------------------
//  * saves the compressed text (ETDC) to disk
//  * ------------------------------------------------------------------*/
//int saveTextInMem (byte *text, uint size, char *basename) {
//	int file;
//	uint sizeFile;
//	byte *buffer;
//
//	char *filename;	
//	filename = (char *)malloc(sizeof(char)*(strlen(basename)+10));
//	strcpy(filename, basename); strcat(filename, ".");	strcat(filename, ETDCDATA);
//	unlink(filename);
//	fprintf(stderr,"\tSaving Compressed Text (ETDC) into disk  %s\n", filename);
//	{		
//		if( (file = open(filename, O_WRONLY|O_CREAT,S_IRWXG | S_IRWXU)) < 0) {
//			printf("Cannot open file %s\n", filename);
//			exit(0);
//		}	
//		write(file, text, sizeof(byte) * size);		
//		close(file);		
//	}	
//	
//	free(filename);
//	return 0;
//}


///**------------------------------------------------------------------
// Loads the VOCABULARY of ETDC into the hash table, and sets also the
// pointers from byte *words vector.
// ----------------------------------------------------------------- */
//int loadVocabulary (void *index, char* basename){
//	twcsa *wcsa=(twcsa *) index;	
//	uint nwords;
//	byte aword[MAX_SIZE_OF_WORD];
//	uint awordSize;
//	byte c;
//
//	char *filename;	
//	uint sizeFile;
//	int file;
//	filename = (char *)malloc(sizeof(char)*(strlen(basename)+10));
//	
//	{	byte *zoneMem;
//		strcpy(filename, basename);
//		strcat(filename, ".");
//		strcat(filename, ETDCVOC);
//		sizeFile= fileSize(filename)-sizeof(uint);
//
//		fprintf(stderr,"\tLoading vocabulary of index %s\n", filename);
//
//		if( (file = open(filename, O_RDONLY)) < 0) {
//			printf("Cannot open file %s\n", filename);
//			exit(0);
//		}		
//		
//		//the number of canonical words
//		read(file, &nwords, sizeof(uint));
//		wcsa->nwords = nwords;
//		wcsa->words = (byte **) malloc(sizeof(byte *) * (wcsa->nwords)); 
//
//		{
//			unsigned int counter=0;	
//			unsigned long addrInTH,i;
//			Wt_hash wordshash;
//			wordshash = wtinitialize_hash(wcsa->nwords, sizeFile); //creates the hash table & zoneMem
//			wcsa->hash = wordshash;
//					
//			//reads each word
//			awordSize = 0;
//			while(read(file, &c, sizeof(char))) {		
//				if(c == '\0') {
//					aword[awordSize] = '\0';
//					i = wtsearch (wordshash, aword, awordSize, &addrInTH );
//					wtinsertElement (wordshash, aword, awordSize, &addrInTH);
//					//fprintf(stderr,"\n Word[%d] = <<%s>> ",counter,wordshash->hash[addrInTH].word);	
//					counter++;
//					awordSize = 0;
//				} else {
//					aword[awordSize] = c;
//					awordSize++;	
//				}	
//			}
//
//			//			fprintf(stderr,"\nHash Table initilized with: %lu slots. Zone.size = %d\n",wcsa->hash->SIZE_HASH, wcsa->hash->zone.size);
//			//			fprintf(stderr,"\nHash Table handles  %lu words.\n",wcsa->hash->NumElem);
//
//			//setting the pointers in byte **words vector
//			zoneMem = wcsa->hash->zone.zone;
//			for(i = 0; i < (wcsa->nwords); i++) {
//				wcsa->words[i]=zoneMem;
//				while (1) {read(file, zoneMem, sizeof(byte)); if (!(*zoneMem++)) break;} //reads word
//			}
//			close(file);
//		}
//	}		
//	free(filename);
//	/** showing all the loaded data: wcsa->hash:: (zoneWords, hash->words, hash->id,  hash->len)
//										   && pointers wcsa->words */
//	/*
//	{	uint i,j; ulong addrInTH;
//		for(i = 0; i <(wcsa->nwords); i++) {
//			fprintf(stderr,"\n word[%d]= <<%40s>>", i, wcsa->words[i]);			
//			j = wtinHashTable(wcsa->hash, wcsa->words[i], strlen((char *)wcsa->words[i]), &addrInTH);
//			if (j) {fprintf(stderr," id = %d",  wcsa->hash->hash[addrInTH].id);}
//		}
//	}
//	*/
//	
//	return (0);
//} 
//
//
///** ------------------------------------------------------------------
//  * Saves the VOCABULARY of ETDC  to disk
//  * ------------------------------------------------------------------*/
//
//int saveVocabulary (void *index, char* basename) {
//	int file;
//
//	char *filename;	
//	filename = (char *)malloc(sizeof(char)*(strlen(basename)+10));
//	strcpy(filename, basename); strcat(filename, ".");	strcat(filename, ETDCVOC);
//	unlink(filename);
//	
//	twcsa *wcsa=(twcsa *) index;	
//	uint nwords;
//	{		
//		unlink(filename);
//		fprintf(stderr,"\tSaving Vocabulary (ETDC) into disk %s\n", filename);		
//		if( (file = open(filename, O_WRONLY|O_CREAT,S_IRWXG | S_IRWXU)) < 0) {
//			printf("Cannot open file %s\n", filename);
//			exit(0);
//		}	
//
//		write(file, &(wcsa->nwords), sizeof(uint));		
//		write(file, wcsa->hash->zone.zone, wcsa->hash->zone.size* sizeof(byte));		
//		close(file);		
//	}	
//	
//	free(filename);
//	return 0;
//}

//
//	// ** Outputs the list of each posting list and its values.
//	// *[Nwords=N]
//	// *[maxPostvalue]
//	// *[lenPost1][post11][post12]...[post1k´]
//	// *[lenPost2][post21][post22]...[post2k´´]
//	// * ... 
//	// *[lenPostN][postN1][postN2]...[postNk´´´]		
//int saveAllPostingList(uint Nwords, uint *lenList, uint **occList, uint maxPost, char *basename){
//	int file;
//
//	char *filename;	
//	filename = (char *)malloc(sizeof(char)*(strlen(basename)+10));
//	strcpy(filename, basename); strcat(filename, ".");	strcat(filename, POSTINGS);
//	unlink(filename);
//	
//	uint nwords;
//	{		
//		unlink(filename);
//		fprintf(stderr,"\tSaving Posting lists (as intergers) to disk %s\n", filename);		
//		if( (file = open(filename, O_WRONLY|O_CREAT,S_IRWXG | S_IRWXU)) < 0) {
//			printf("Cannot open file %s\n", filename);
//			exit(0);
//		}	
//
//	
//
//		write(file, &(Nwords), sizeof(uint));
//		write(file, &(maxPost), sizeof(uint));
//		uint i,j;
//		for (i=0;i<Nwords;i++) {
//			write(file, &(lenList[i]),sizeof(uint));		
//			write(file, occList[i], lenList[i] * sizeof(uint));		
//		}
//		close(file);		
//	}	
//	
//	free(filename);
//	return 0;
//	
//}
	
/** **********************************************************************/
/** Some functions used during index construction                        */
/** **********************************************************************/
//
///** ------------------------------------------------------------------
//  * Computes the frequency of each id--word
//  *          && sets the number of words in the source text: N
//  * ------------------------------------------------------------------*/
//int computeFreqs (void *index) {
//	uint i;	
//	twcsa *wcsa=(twcsa *) index;	
//	byte *text = wcsa->text;
//	uint N = wcsa->textSize;
//	uint n = wcsa->nwords;
//	uint totalfreq=0;
//
//	fprintf(stderr,"Computing frequencies of each word");	
//	for (i=0;i<n;i++) wcsa->freqs[i]=0;
//	
//	//decode && increase freqs
//	i=0;
//	uint num=0;
//	while (i<N) {
//		//BC_TO_DEC(text,i,num);
//		i =decode (text, i, &num) ;  //decoding etdc...
//		wcsa->freqs[num]++;
//		//fprintf(stderr,"\n decoded word with id = %d is ",num);fflush(stderr);
//		//fprintf(stderr,"<<%s>>",wcsa->words[num]);fflush(stderr);	
//		totalfreq++;	
//	}		
//	wcsa->Nwords = totalfreq;
//	fprintf(stderr,"--> freqs[0] = %d, freqs[%d] = %d",wcsa->freqs[0], n-1, wcsa->freqs[n-1]);	
//	fprintf(stderr,"\n Number of words in the source text is: %d",wcsa->Nwords);	
//	return 0;
//}



//int	parseAllPostingList(uint *nlists, uint **lenList, uint ***occList, 
//                        uint *maxPostValue, uint *source, uint length) {
//
//	uint **occs;
//	uint *lens;
//	uint n;
//	
//	register uint i,j,sourcepos,len;
//	
//	if (length<2) return 1;
//	
//	n=source[0];
//	*maxPostValue = source[1];
//	
//	//allocating memory.
//	lens = (uint *) malloc (sizeof(uint) * n);
//	occs = (uint **) malloc (sizeof(uint *) * n);
//	
//	sourcepos=2;
//	
//	for (i=0; i<n; i++) {
//		lens[i]= len = source[sourcepos++];	
//		occs[i]= (uint *) malloc (sizeof(uint) * len);
//		for (j=0; j<len;j++) occs[i][j] = source[sourcepos++];
//	}
//	
//	if (length !=sourcepos) return 31;
//	
//	*nlists =n;
//	*lenList = lens;
//	*occList = occs;	
//	return 0;
//}

///** ------------------------------------------------------------------
//  * Creates the lists of occurrences of all the words
//  * Block-oriented: Only 1 occurrence per block is marked for each word.
//  * A word belongs to a block if its associated codeword starts in that block.
//  * ------------------------------------------------------------------*/
//int createListsOfOccurrences(void *index, uint ***occList, uint **lenList) {
//	uint i;	
//	twcsa *wcsa=(twcsa *) index;	
//	byte *text = wcsa->text;
//	uint N = wcsa->textSize;
//	uint n = wcsa->nwords;
//	uint offset;
//	uint blocksize = wcsa->blockSize;
//	
//	uint currblock = 0;
//	uint posinblock=0;
//
//	//allocating memory.
//	uint **occs;
//	occs = (uint **) malloc (sizeof(uint *) * n);
//	for (i=0;i<n;i++) occs[i]= (uint *) malloc (sizeof(uint) * wcsa->freqs[i]);
//	
//	//temporal array to keep the last index in the list of the i-th word
//	uint *currpos = (uint *) malloc (sizeof(uint)*n);
//	//temporal array to keep the block of the last occurrence of the i-th word
//	int *lastBlock = (int *) malloc (sizeof(int)*n);
//	for (i=0;i<n;i++) {currpos[i]=0; lastBlock[i] = -1;}
//		
//	//decode && increase freqs
//	i=0;
//	uint num=0;
//	while (i<N) {
//		offset =i/blocksize;  //optimizable is a power of 2 is used as block-size
//		i =decode (text, i, &num);
//		if (offset != lastBlock[num]) {  //only 1 occurrence per block is marked.
//			occs[num][currpos[num]] = offset;  //word "num" occurrs in block "offset"
//			currpos[num]++;			
//			lastBlock[num] = offset;
//		}	
//	}		
//	free(lastBlock);	
//
//	*occList = occs;	
//	*lenList = currpos;
//	return 0;
//}



/** **********************************************************************/
/** Saving/Loading the index to/from disk                               **/
/** **********************************************************************/

///** Saving freq vector */
//int saveFreqVector(uint *V, uint size, char *basename) {
//	int file;
//	uint sizeFile = size*sizeof(uint);
//	uint *buffer = V;
//
//	char *filename;	
//	filename = (char *)malloc(sizeof(char)*(strlen(basename)+10));
//	strcpy(filename, basename); strcat(filename, ".");	strcat(filename, FREQVECTOR);
//
//	fprintf(stderr,"\tSaving Vector of frequencies %s\n", filename);
//	unlink(filename);
//	if( (file = open(filename, O_WRONLY|O_CREAT,S_IRWXG | S_IRWXU)) < 0) {
//		printf("Cannot open file %s\n", filename);
//		exit(0);
//	}
//	write(file, buffer, sizeFile);
//	close(file);				
//	free(filename);
//	return 0;
//}
//
///** Loading  freq vector */
//int loadFreqVector(uint **V, uint *size, char *basename) {
//	int file;
//	uint sizeFile;
//	uint *buffer;
//
//	char *filename;	
//	filename = (char *)malloc(sizeof(char)*(strlen(basename)+10));
//	strcpy(filename, basename); strcat(filename, ".");	strcat(filename, FREQVECTOR);
//
//	sizeFile= fileSize(filename);
//	buffer = (uint *) malloc (sizeFile);
//
//	fprintf(stderr,"\tLoading Vector of frequencies %s\n", filename);
//	if( (file = open(filename, O_RDONLY)) < 0) {
//		printf("Cannot open file %s\n", filename);
//		exit(0);
//	}
//	read(file, buffer, sizeFile);
//	close(file);			
//	free(filename);
//
//	*V = buffer;
//	*size = sizeFile/4;	
//	return 0;
//}


///** Saving lenList vector */
//int savelenListVector(uint *V, uint size, char *basename) {
//	int file;
//	uint sizeFile = size*sizeof(uint);
//	uint *buffer = V;
//
//	char *filename;	
//	filename = (char *)malloc(sizeof(char)*(strlen(basename)+10));
//	strcpy(filename, basename); strcat(filename, ".");	strcat(filename, LENLISTS);
//
//	fprintf(stderr,"\tSaving Vector with lens for the list of each id: %s\n", filename);
//	unlink(filename);
//	if( (file = open(filename, O_WRONLY|O_CREAT,S_IRWXG | S_IRWXU)) < 0) {
//		printf("Cannot open file %s\n", filename);
//		exit(0);
//	}
//	write(file, buffer, sizeFile);
//	close(file);				
//	free(filename);
//	return 0;
//}
//
//
///** Loading  lenList vector */
//int loadlenListVector(uint **V, uint *size, char *basename) {
//	int file;
//	uint sizeFile;
//	uint *buffer;
//
//	char *filename;	
//	filename = (char *)malloc(sizeof(char)*(strlen(basename)+10));
//	strcpy(filename, basename); strcat(filename, ".");	strcat(filename, LENLISTS);
//
//	sizeFile= fileSize(filename);
//	buffer = (uint *) malloc (sizeFile);
//
//	fprintf(stderr,"\tLoading Vector with lens for the list of each id: %s\n", filename);
//	if( (file = open(filename, O_RDONLY)) < 0) {
//		printf("Cannot open file %s\n", filename);
//		exit(0);
//	}
//	read(file, buffer, sizeFile);
//	close(file);			
//	free(filename);
//
//	*V = buffer;
//	*size = sizeFile/4;	
//	return 0;
//}
//
///** Saving the lists of occurrences of all the words */
//int saveOccLists (void *ail, char *basename) {
//	int file;
//	char *filename;	
//	filename = (char *)malloc(sizeof(char)*(strlen(basename)+10));
//	strcpy(filename, basename); strcat(filename, ".");	strcat(filename, LISTSOCC);
//	fprintf(stderr,"\tSaving set of posting lists to disk %s\n", filename);
//	unlink(filename);
//			
//	t_il *il=(t_il *) ail;	
//	{		
//		if( (file = open(filename, O_WRONLY|O_CREAT,S_IRWXG | S_IRWXU)) < 0) {
//			printf("Cannot open file %s\n", filename);
//			exit(0);
//		}	
//		//fprintf(stderr,"\n saving size ucompressed = %lu!!!!",il->sizeUncompressed);
//		write(file, &(il->nlists), sizeof(uint));		
//		write(file, &(il->sizeUncompressed), sizeof(uint));		
//		write(file, &(il->sampleK), sizeof(uint));		
//		write(file, &(il->zoneOccsSize), sizeof(uint)); //in # bits
//		write(file, &(il->numBuckets), sizeof(uint));		
//		write(file, &(il->absOffsetSize), sizeof(uint));		
//		write(file, &(il->bcOffsetSize), sizeof(uint));		
//		write(file, &(il->bcsSize), sizeof(uint));		
//
//		//fprintf(stderr,"\n *sampleK = %d, zoneOccsSize = %d, numbucks= %d",wcsa->occs.sampleK, wcsa->occs.zoneOccsSize, wcsa->occs.numBuckets); 
//		//fprintf(stderr,"\n *absoffsetSize = %d, bcOffsetSize = %d, occsBucketSize= %d",wcsa->occs.absOffsetSize, wcsa->occs.bcOffsetSize, wcsa->occs.occsBucketSize); 
//		//fprintf(stderr,"\n *bcsSize = %d",wcsa->occs.bcsSize);
//
//		write(file, il->occs, il->nlists * sizeof(uint));		
//		write(file, il->zoneOccs, ((il->zoneOccsSize +W-1)/W) * sizeof(uint)); //zoneOccs kbitVec[bucketSize]
//		write(file, il->bcs, il->bcsSize * sizeof(byte));		
//		close(file);		
//	}		
//	free(filename);
//	return 0;
//}
//
///** Loading the lists of occurrences of all the words into "index" */
//int loadOccLists (void *ail, char *basename)  {
//	int file;
//	char *filename;	
//	t_il *il=(t_il *) ail;	
//
//	filename = (char *)malloc(sizeof(char)*(strlen(basename)+10));
//	strcpy(filename, basename); strcat(filename, ".");	strcat(filename, LISTSOCC);
//	fprintf(stderr,"\tLoading sets of posting lists %s\n", filename);
//	{		
//		if( (file = open(filename, O_RDONLY)) < 0) {
//			printf("Cannot open file %s\n", filename);
//			exit(0);
//		}	
//
//		//some integers
//		uint number;
//		read(file, &(il->nlists), sizeof(uint));	
//		read(file, &(il->sizeUncompressed), sizeof(uint));					
//		read(file, &(il->sampleK), sizeof(uint));		
//		read(file, &(il->zoneOccsSize), sizeof(uint)); //in # bits
//		read(file, &(il->numBuckets), sizeof(uint));		
//		read(file, &(il->absOffsetSize), sizeof(uint));		
//		read(file, &(il->bcOffsetSize), sizeof(uint));	
//		il->occsBucketSize = il->bcOffsetSize + il->absOffsetSize;
//		read(file, &(il->bcsSize), sizeof(uint));		
//
//		//fprintf(stderr,"\n *sampleK = %d, zoneOccsSize = %d, numbucks= %d",wcsa->occs.sampleK, wcsa->occs.zoneOccsSize, wcsa->occs.numBuckets); 
//		//fprintf(stderr,"\n *absoffsetSize = %d, bcOffsetSize = %d, occsBucketSize= %d",wcsa->occs.absOffsetSize, wcsa->occs.bcOffsetSize, wcsa->occs.occsBucketSize); 
//		//fprintf(stderr,"\n *bcsSize = %d",wcsa->occs.bcsSize);
//		
//		//vector occs.
//		il->occs = (uint *) malloc (il->nlists * sizeof(uint) );		
//		read(file, il->occs, il->nlists * sizeof(uint)); //wcsa->nwords was loading in "loadVocabulary"
//
//		//vector zoneOccs (the kbitVector, with numBuckets of size (occsBucketSize) bits each
//		il->zoneOccs = (uint *) malloc (((il->zoneOccsSize +W-1)/W) * sizeof(uint));
//		il->zoneOccs[((il->zoneOccsSize +W-1)/W)-1] =0000; //so valgrind is shut up		
//		read(file, il->zoneOccs, ((il->zoneOccsSize +W-1)/W) * sizeof(uint)); //zoneOccs kbitVec[bucketSize]
//
//		//vector bcs.
//		il->bcs = (byte *) malloc (il->bcsSize * sizeof(byte));		
//		read(file, il->bcs, il->bcsSize * sizeof(byte));		
//		close(file);		
//	}		
//	
//	free(filename);
//	return 0;
//}



/** **********************************************************************/
/** Functions needed for searches                                       **/
/** **********************************************************************/

/** Uncompresses the list of occurrences of a given id-word **/
int uncompressList (void *index, uint id, uint **list, uint *len) {
	twcsa *wcsa=(twcsa *) index;	
	register uint absBits = wcsa->occs.absOffsetSize;
	register uint bcBits = wcsa->occs.bcOffsetSize;
	byte *bcs  = wcsa->occs.bcs;
	uint *zoneoccs = wcsa->occs.zoneOccs;
	uint lenlist  = wcsa->lenList[id];
	uint sampleK = wcsa->occs.sampleK;
	uint *alist = (uint *) malloc (sizeof(uint) * lenlist);

	//fprintf(stderr,"\nDecoding all the %d occurrences of word[%d]= <<%s>>",lenlist, id, wcsa->words[id]);fflush(stderr);		 		
	uint i, pos, bcpos, num, prevvalue;
	pos = wcsa->occs.occs[id]; //pos in zoneoccs
	for (i=0; i<lenlist;i++) {		
		if (! (i%sampleK)) {	//an abs sample
			prevvalue = bitread (zoneoccs, pos, absBits);
			alist[i]=prevvalue;
			pos+=absBits;
			bcpos = bitread (zoneoccs, pos, bcBits);
			pos +=bcBits;			
		}
		else { //decoding from bcs
			BC_TO_DEC(bcs, bcpos, num);
			prevvalue +=num;
			alist[i]=prevvalue;			
		}
	}		
	*list =alist;
	*len =i;	
//		fprintf(stderr,"\n[%d] decoded word at position text[%d] ",i,pos);
//		pos = decode (text, pos, &num) ;	
//		fprintf(stderr," and id = %d is ",num);fflush(stderr);
//		fprintf(stderr,"<<%s>>",wcsa->words[num]);fflush(stderr);
//		break;		 			
}	



	#include "iisvsETDCBlock.c"   //for the full text index (word-oriented with ETDC).



