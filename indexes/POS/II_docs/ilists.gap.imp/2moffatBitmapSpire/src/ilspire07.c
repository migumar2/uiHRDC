#include "ilspire07.h"


/***********************************************************************************/
/*** FUNCTIONS USED FOR PARSING PARAMETERS FROM COMMAND LINE ***********************/
/* Three function to variables to manage parameters */
static bool is_delimeter(char *delimiters, char c) {
   int i=0,len_delimiters=strlen(delimiters);
   bool is=false;
   for (i=0;i<len_delimiters;i++)
     if (c == delimiters[i]) is=true;
   return is;
}


static void parse_parameters(char *options, int *num_parameters, char ***parameters, char *delimiters) {
  int i=0,j=0,temp=0,num=0, len_options=strlen(options);
  char *options_temp;
  while  (i<len_options) {
    while ((i<len_options) && is_delimeter(delimiters,options[i])) i++;
    temp=i;
    while ((i<len_options) && !is_delimeter(delimiters,options[i])) i++;
    if (i!=temp) num++;
  }
  (*parameters) = (char **) malloc(num*sizeof(char * ));
  i=0;
  while  (i<len_options) {
    while ((i<len_options) && is_delimeter(delimiters,options[i])) i++;
    temp=i;
    while ((i<len_options) && !is_delimeter(delimiters,options[i])) i++;
    if (i!=temp) {
      (*parameters)[j]=(char *) malloc((i-temp+1)*sizeof(char));
      options_temp = options+temp;
      strncpy((*parameters)[j], options_temp, i-temp);
      ((*parameters)[j])[i-temp] = '\0';
      j++;
    }
  }
  *num_parameters = num;
}

static void free_parameters(int num_parameters,char ***parameters) {
  int i=0;
  for (i=0; i<num_parameters;i++)
    free((*parameters)[i]);
  free((*parameters));
}

/***********************************************************************************/
/***********************************************************************************/

/** *******************************************************************************
    * function used to parse the source posting lists that are loaded in build_il.*
    * Refer to "build_il() comments below" for a more detailed description        *
    * Basically, it reads the len of each posting list and its values.
		// *[Nwords=N]
		// *[maxPostvalue]
		// *[lenPost1][post11][post12]...[post1k�]
		// *[lenPost2][post21][post22]...[post2k��]
		// * ... 
		// *[lenPostN][postN1][postN2]...[postNk���]		
		//
	* It recreates array lenList, and uint ** occList and variables maxPostValue and nlists. */
	
int	parseAllPostingList(uint *nlists, uint **lenList, uint ***occList, 
                        uint *maxPostValue, uint *source, uint length){

	uint **occs;
	uint *lens;
	uint n;
	
	register uint i,j,sourcepos,len;
	
	if (length<2) return 1;
	
	n=source[0];
	*maxPostValue = source[1];
	
	//allocating memory.
	lens = (uint *) malloc (sizeof(uint) * n);
	occs = (uint **) malloc (sizeof(uint *) * n);
	
	sourcepos=2;
	
	for (i=0; i<n; i++) {
		lens[i]= len = source[sourcepos++];	
		occs[i]= (uint *) malloc (sizeof(uint) * len);
		for (j=0; j<len;j++) occs[i][j] = source[sourcepos++];
	}
	
	if (length !=sourcepos) return 31;
	
	*nlists =n;
	*lenList = lens;
	*occList = occs;	
	return 0;
}



/**********************************************************************************/
/*  Implementation of the General interface:                                      *
/*              interface for using compressed representations of posting lists   *
/*              Any alternative representation must implement all these functions *
/**********************************************************************************/


	/* returns a string with the parameters available for building the method */

char *showParameters_il(){
	static char text[200];
	sprintf(text, "samplerate=%d; lenBitmapDiv=%d", DEFAULTKSAMPLE,DEFAULT_LEN_BITMAP_DIV);
	return text;	
}



	/* checks if an id should use either bitmap or bytecodes: valid during build_il() */
/*	
int shouldUseBitmap(t_il *il, uint **occList, uint id, uint *valBitmap, uint *valBCs) {  // 0 <= id < il->nlists 
	uint j,len;
	uint prevvalue, gap, numbytes;
	uint bcssize=0;
	
	len= il->lenList[id];		
	prevvalue=0;				
	for (j=0; j<len; j++){
		gap = occList[id][j]-prevvalue;
		prevvalue = occList[id][j];
		SIZE_DEC_TO_BC(gap,numbytes);
		bcssize += numbytes;
	}
	
	*valBitmap = il->maxPostingValue / 8;
	*valBCs =bcssize;
	
	if (*valBitmap <= bcssize)
		return 1;
	return 0;
}
*/	

	/* checks if an id should use either bitmap or bytecodes: valid during build_il() */
int shouldUseBitmap(t_il *il, uint id) {  /* 0 <= id < il->nlists */
	if (il->lenList[id] > il->lenBitmapThreshold)
		return 1;
	return 0;
}

    /* Creates a structure for the management of inverted lists from a
      given set of posting lists. 
      Array source has the following format...
      [Nwords=N][maxPostvalue][lenPost1][post11][post12]...[post1k�]
                              [lenPost2][post21][post22]...[post2k��]
                               ... 
                              [lenPostN][postN1][postN2]...[postNk���]
      il is an opaque data type. Any build option must be passed in string 
      build_options, whose syntax depends on the index. The index must 
      always work with some default parameters if build_options is NULL. 
      The returned structure is ready to be queried. */


int build_il (uint *source, uint length, char *build_options, void **ail) {
	uint sampleK=DEFAULTKSAMPLE; // --> see ilists/ildefconfig.h
	uint lenBitmapDiv= DEFAULT_LEN_BITMAP_DIV; // --> see ilists/ildefconfig.h
	printf("\n CALL TO BUILD_IL !!\n");
	
	t_il *il;
	il = (t_il *) malloc (sizeof (t_il) * 1);
	*ail = il;

	
	/** processing the parameters */
	char delimiters[] = " =;";
	char filename[256] = "unnamed";
	int j,num_parameters;
	char ** parameters;

	
	if (build_options != NULL) {
	parse_parameters(build_options,&num_parameters, &parameters, delimiters);
	for (j=0; j<num_parameters;j++) {
	  if ((strcmp(parameters[j], "samplerate") == 0 ) && (j < num_parameters-1) ) {
	    sampleK=atoi(parameters[j+1]);	    
	    j++;
	  } 
	  else if  ((strcmp(parameters[j], "filename") == 0 ) && (j < num_parameters-1) ) {
	    strcpy(filename,parameters[j+1]);
	    j++;
	  }
	  else if ((strcmp(parameters[j], "lenBitmapDiv") == 0 ) && (j < num_parameters-1) ) {
	    lenBitmapDiv=atoi(parameters[j+1]);	    
	    j++;

	  }
	}
	free_parameters(num_parameters, &parameters);
	}
	
	printf("\n parameters of method: sampleK=%ld, lenBitmapDiv= %d, filename = %s\n",sampleK, lenBitmapDiv, filename);
	
	il->sampleK = sampleK;



	{	/** 4 ** creates a temporal list of occurrences of each word (block-oriented).
					gives also the len of each list */
		uint i;
		uint **occList;
		uint *lenList;
		uint maxPostValue;
		
		//	//allocating memory.
		//	uint **occs;
		//	occs = (uint **) malloc (sizeof(uint *) * n);
		//	for (i=0;i<n;i++) occs[i]= (uint *) malloc (sizeof(uint) * wcsa->freqs[i]);

fprintf(stderr,"\n paso 1 ");fflush(stderr);		
		parseAllPostingList(&(il->nlists), &lenList, &occList, &maxPostValue, source,length);				
		free(source);
		
	fprintf(stderr,"\n paso 2 ");fflush(stderr);		
		
		il->lenList = lenList;
		il->maxPostingValue = maxPostValue;
		//il->lenBitmapThreshold = maxPostValue / lenBitmapDiv;
		il->lenBitmapThreshold = (maxPostValue+1) / lenBitmapDiv;  //+1 due to ids in [0.. maxPostValue]
		
		printf("numOflists = %ld, maxPost = %ld, lenBitmapThreshold = %ld \n", 	
											il->nlists, maxPostValue, il->lenBitmapThreshold);
	
		//calculates an stimation of the uncompressed representation of the index (using uints per posting)
		il->sizeUncompressed = sizeof(uint) * (il->nlists) +  //sizeoflenList
		                       sizeof(uint*) * (il->nlists); //pointers to each posting (occs[i])
		                       
		for (i=0; i< il->nlists;i++) { il->sizeUncompressed += (lenList[i] * sizeof(uint));}

		fprintf(stderr,"stimated mem-usage of uncompressed posting-lists = %ld bytes\n", il->sizeUncompressed);


			
		/** 5 ** Encoding the occurrences 
					temporal zoneOccsTmp is created [uint M | uint O][uint M | uint O]...
					bc is set.
					O's are definitive offsets into bc ==> sizeBcs is computed
					M's are definitive offsets into text.
					*/
		il->sampleK = sampleK;
		il->occs = (uint *) malloc (sizeof(uint) * il->nlists);
		il->bcsptr = (uint *) malloc (sizeof(uint) * (il->nlists +1));
		
			/** 5.0 ** Sets zoneBitmaps[] and numBitmaps; sets also occs for the lists using bitmap. **/
			
			{  //counts the number of terms indexed with bitmaps and reserves memory for them.
				
			
			  	uint numBitmaps=0;
			  	uint nwords = il->nlists;
			  	 
				for (i=0;i<nwords; i++){
			  		uint option;	
					if (option = shouldUseBitmap(il, i)) numBitmaps++;
					
					//if (i<2000) //printfs only
					//	fprintf(stderr,"\n id=%6d, with lenlist=%6d, shouldUseBitmap=%d, [thresholdLen = %7d]",i,lenList[i],option, il->lenBitmapThreshold);
				}  	
				fprintf(stderr,"\n Number of lists = %d, %d using bitmaps, %d using bcs",il->nlists, numBitmaps, il->nlists - numBitmaps);
			  					
				il->numBitmaps = numBitmaps;
				il->bits_bitmaps = il->maxPostingValue+1;  //also docId = [0.. maxPostingValue]
				il->ints_bitmaps_aligned =  (il->bits_bitmaps +W-1) /W;
				il->zoneBitmaps = (uint *) malloc (sizeof(uint) *  (il->numBitmaps * il->ints_bitmaps_aligned));
				for (i=0;i< ( il->numBitmaps * il->ints_bitmaps_aligned); i++)
					il->zoneBitmaps[i] = 0000;   //so no further "set to zero" is needed for each bitmap.
					
			}
/*			
			fprintf(stderr,"\n num antes = %x, %x",il->zoneBitmaps[0],il->zoneBitmaps[1]);
			bitset(il->zoneBitmaps,31);
			fprintf(stderr,"\n num antes = %x, %x",il->zoneBitmaps[0],il->zoneBitmaps[1]);
			bitset(il->zoneBitmaps,32);
			fprintf(stderr,"\n num antes = %x, %x
			* ",il->zoneBitmaps[0],il->zoneBitmaps[1]);
			bitset(il->zoneBitmaps,33);
			fprintf(stderr,"\n num antes = %x, %x",il->zoneBitmaps[0],il->zoneBitmaps[1]);
*/						
		
		
			uint *numAbsSamplesperID = (uint *) malloc (sizeof(uint) * (il->nlists));
		
			/** 5.a ** 
				Computes the number of Absolute samples to keep for bytecodes. Depends on sampleK. 
				Sets that value to "zero" if the inv list belongs to a term using a bitmap.			
			*/
			uint absCounter =0;
			{  uint nwords = il->nlists;
				uint sampleP;
				
				for (i=0;i<nwords; i++){
					
					if (! shouldUseBitmap(il,i) ) { //** spire **/					
						SAMPLEPERIOD(sampleP,sampleK,il->lenList[i]);
						absCounter += ( (il->lenList[i] + sampleP -1) /sampleP);
						numAbsSamplesperID[i]= ( (il->lenList[i] + sampleP -1) /sampleP);
						//fprintf(stderr,"\n len = %lu, samplek=%lu, sampleP = %lu", il->lenList[i], sampleK, sampleP);fflush(stderr);	
					}
					else numAbsSamplesperID[i]= 0;  //** spire **/
				}
				
				fprintf(stderr,"\n Number of absolute samples to store is = %d",absCounter);		 					
			}	
								
				//return 0;
		
			uint *zoneOccsTmp = (uint *) malloc (sizeof(uint) * 2 * absCounter);

			/** 5.b **
				Computes the size of bc vector by simulating encoding process of gaps.
				Sets also bitmaps for those terms indexed with a bitmap.
				Computes and sets the values for "occs[]".
			*/
			uint bcssize=0;
			{
				uint nwords = il->nlists;
				uint id,j;
				uint prevvalue, gap, numbytes;
				uint len, sampleP;
				
				uint counterBCcodes =0; uint counterAbsSamples=0;
				uint counterBitmaps=0;
				for (id=0; id<nwords; id++) {
					len= il->lenList[id];
					
					if (! shouldUseBitmap(il,id) ) { //** spire **/					
						il->occs[id]=counterAbsSamples;   /** points to a number of bucket --> changes in step **6** */
						counterAbsSamples++;
						SAMPLEPERIOD(sampleP,sampleK,len);
						prevvalue= occList[id][0];

						for (j=1; j<len; j++){
							if (j %sampleP) { //a gap;
								gap = occList[id][j]-prevvalue;
								prevvalue = occList[id][j];
								SIZE_DEC_TO_BC(gap,numbytes);
								bcssize += numbytes;
								counterBCcodes++;
							}
							else {
								prevvalue = occList[id][j];
								counterAbsSamples++;
							}
						}
					}
					else { //using a bitmap...
						uint iniPosBitmap = counterBitmaps * il->ints_bitmaps_aligned * W;
						il->occs[id] = counterBitmaps;
						for (j=0;j<len;j++)  
							bitset(il->zoneBitmaps, (iniPosBitmap + occList[id][j]));							
						counterBitmaps++;
					}
				}
				fprintf(stderr,"\n\n Active bits for %d bitmaps where already set.",counterBitmaps);						
				fprintf(stderr,"\n Number of absolute samples to store is = %d",counterAbsSamples);						
				fprintf(stderr,"\n Number of gaps to store is = %d",counterBCcodes);	
				fprintf(stderr,"\n Size of bcs Vector should be  %d bytes",bcssize);
				//fprintf(stderr,"\n [SHOULD BE IDENTICAL]: number of words = %d, and sum of occurs is %d",wcsa->Nwords, counterAbsSamples + counterBCcodes);	
			}
			
		il->bcsSize = bcssize;
		il->bcs = (byte *) malloc (sizeof(byte) * il->bcsSize);		
		
		/** 5.c ** encodes gaps into bcs && set pointers into zoneoccurssTmp */
			{
				uint nwords = il->nlists;
				byte *bcs = il->bcs;
				uint id,j;
				uint prevvalue, gap;
				uint len, sampleP ;

				uint bcpos=0;
				uint zoneoccspos=0;
				uint inibcs_i;
				
				for (id=0; id<nwords; id++) {
					il->bcsptr[id] = bcpos;
					inibcs_i = bcpos;
					len= il->lenList[id];	
					
					if ( ! shouldUseBitmap(il,id) ) {											
						for (j=0; j<len; j++){
							SAMPLEPERIOD(sampleP,sampleK,len);
							if (j %sampleP) { //a gap;
								gap = occList[id][j]-prevvalue;
								prevvalue = occList[id][j];
								DEC_TO_BC(bcs, bcpos, gap);  //bcpos is increased.
							}
							else {
								prevvalue = occList[id][j];
								zoneOccsTmp[zoneoccspos++]=prevvalue;
								if (len ==(j+1)) zoneOccsTmp[zoneoccspos++] = FILLERUINT;  //fill next int (last occurr in abs sample).
								else zoneOccsTmp[zoneoccspos++]= bcpos-inibcs_i;	
								//	else zoneOccsTmp[zoneoccspos++]= bcpos;		
							}
						}
					}
				}
				il->bcsptr[nwords] = bcpos;
				printf("\n bcpos = %d, il->bcssize= %d",bcpos,il->bcsSize);
				
				
				//sets bcsptr[i] <-- bcsptr[j], if i uses bitmap, j uses bytecodesand j is the next term using bytecodes after i.
				//this will permit using Obits(id) = bcsptr[id+1] - bcsptr[id];
				{
					int idd;
					uint nextptr = il->bcsptr[nwords];

					for (idd=nwords-1;idd>=0;idd--) {
						if (shouldUseBitmap(il,idd))
							il->bcsptr[idd]=nextptr;
						else nextptr = il->bcsptr[idd];
					} 
				}
				
				fprintf(stderr,"\n Gaps were encoded with bc ==> %d bytes",bcpos);
				//fprintf(stderr,"\n zoneoccspos = %d ",zoneoccspos);				
			}		

		il->absOffsetSize = bits(il->maxPostingValue);    //in bits    //antes wcsa->textSize;!!!!!!!
		il->bcOffsetSize = bits(il->bcsSize); //in bits

		il->occsBucketSize = il->absOffsetSize + il->bcOffsetSize ; //in bits
		il->numBuckets   = absCounter;
		
		//computes zoneOccsSize;
		{uint totalSize=0;
		 uint i,obits;
		 uint nwords = il->nlists;
		 for (i=0;i<nwords;i++) {
		 	if ( ! shouldUseBitmap(il,i) ) {
				//tmp = il->bcsptr[i+1] - il->bcsptr[i]; CEILLOG_2(len,obits);
				OBITS(obits, il->bcsptr, i);
				//fprintf(stderr,"\n obits = %d ", obits );
				totalSize += (numAbsSamplesperID[i] * (il->absOffsetSize + obits));
			}
		 }
		 il->zoneOccsSize = totalSize;
		}	
		fprintf(stderr,"\n zoneoccssize = %d ", il->zoneOccsSize );	
		
		//il->zoneOccsSize = il->numBuckets * il->occsBucketSize; //in bits
		il->zoneOccs = (uint *) malloc (sizeof(uint) * (il->zoneOccsSize + W -1)/W );
		if (il->zoneOccs) {
			il->zoneOccs[((il->zoneOccsSize +W-1)/W)-1] =0000; //so valgrind is shut up		
		}


		/** 6 ** encode zoneoccurssTmp into zoneoccurss using a "kbit-fashion" 
		         sets also offsets in il->occs[id] \forall id;            */
			{	uint i,j,value;
				uint nwords = il->nlists;
				uint absBits = il->absOffsetSize;
				uint k=0;
				uint obits;	
				uint pos=0;			
				for (i=0;i<nwords;i++) {
					if ( ! shouldUseBitmap(il,i) ){
						OBITS(obits, il->bcsptr, i);
						il->occs[i] = pos;     // setting offsets in il->occs vector for non-bitmap-type terms
						for (j=0;j<numAbsSamplesperID[i];j++) {  //enconding directory of [Abs|bc] buckets 
							value = zoneOccsTmp[k++];
							bitwrite (il->zoneOccs, pos, absBits, value);	pos += absBits;
							value = zoneOccsTmp[k++];
							bitwrite (il->zoneOccs, pos, obits, value);	pos += obits;
						}	
					}
				}
			}

		/** checking that the lists are decoded successfully *************/

		fprintf(stderr,"\n ! CHECKING that the decoded lists are identical to the original ones. ");
		{ //checking:: decoding a list
			uint i, id ,len;
			uint *list;
			
			for (id=0;id < il->nlists;id++) {
				//if ( shouldUseBitmap(il,id) ) continue;
				extractList_il (*ail, id, &list, &len);		
				//extractList (void *ail, uint id, uint **list, uint *len)	
				for (i=0;i<len;i++){
					if (list[i] != occList[id][i]) {
						fprintf(stderr,"\n decoding of lists failed for id = %d: DIFFERENT!!: (%d,%d)",id, list[i],occList[id][i]);
						exit(0);
					}
				} 
				
				free(list);				
			}			
		}
		fprintf(stderr,"\n ! Decoding the list of occurrences worked fine (the same as previous values) ");

		fprintf(stderr,"\n ! CHECKING that the decoded lists are identical to the original ones (extract_no_malloc). ");
		{ //checking:: decoding a list
			uint i, id ,len;
			uint *list = (uint *) malloc(sizeof(uint) * (il->maxPostingValue+1));
			
			for (id=0;id < il->nlists;id++) {
				//if ( shouldUseBitmap(il,id) ) continue;
				//extractList_il (*ail, id, &list, &len);	
				extractListNoMalloc_il(*ail,id,list,&len);	
				//extractList (void *ail, uint id, uint **list, uint *len)	
				for (i=0;i<len;i++){
					if (list[i] != occList[id][i]) {
						//fprintf(stderr,"\n decoding of lists failed for id = %d: DIFFERENT!!",id);
						fprintf(stderr,"\n decoding of lists failed for id = %d: DIFFERENT!!: (%d,%d)",id, list[i],occList[id][i]);
						exit(0);
					}
				} 
			}			
			free(list);				

		}
		fprintf(stderr,"\n ! Decoding the list of occurrences worked fine (the same as previous values) ");


		
//		{//counting the number of words that do not have any d-gapg (freq = 1).
//			uint i; uint count=0;
//			uint obits;
//			for (i=0;i<il->nlists;i++) { 
//				OBITS(obits, il->bcsptr, i);
//				if (il->bcsptr[i] == il->bcsptr[i+1]) {
//					fprintf(stderr,"\n id = %d has no bcs (occs=1)--> bcsBITS = %d",i,obits);
//					count++;
//				}				
//			}		
//			fprintf(stderr,"\n There are %lu IDs with no bytecodes", count);	
//		}
		/******************************************************/

		//frees memory
		for (i=0;i<il->nlists;i++) free(occList[i]);
		free(occList);
		free(zoneOccsTmp);
		free(numAbsSamplesperID);
	}

	fprintf(stderr,"\n The index has already been built!!\n");
	
	return 0;
}


    /*  Saves index on disk by using single or multiple files, having 
      proper extensions. */

int save_il (void *ail, char *filebasename){
	char *basename = filebasename;
	t_il *il=(t_il *) ail;

	fprintf(stderr,"\nSaving structures to disk: %s.*\n",basename);			
	
	int file;
	char *filename;	
	filename = (char *)malloc(sizeof(char)*(strlen(basename)+20));
	strcpy(filename, basename); strcat(filename, ".");	strcat(filename, POSTINGS);
	fprintf(stderr,"\tSaving Vector with lens for the list of each id: %s\n", filename);
	unlink(filename);
	if( (file = open(filename, O_WRONLY|O_CREAT,S_IRWXG | S_IRWXU)) < 0) {
		printf("Cannot open file %s\n", filename);
		exit(0);
	}

	/** Saving lenList vector */
	{
		uint size    = il->nlists * sizeof(uint);

		write(file, &(il->nlists), sizeof(uint));		
		write(file, il->lenList, size);
	}
	
	/** Saving the postings lists using bitmaps.*/
	{
		write(file, &(il->lenBitmapThreshold), sizeof(uint));		
		write(file, &(il->bits_bitmaps), sizeof(uint));		
		write(file, &(il->ints_bitmaps_aligned), sizeof(uint));	
		write(file, &(il->numBitmaps), sizeof(uint));		
		//write(file, il->zoneBitmaps, (il->numBitmaps * il->ints_bitmaps_aligned) *  sizeof(uint));
		if (il->numBitmaps){	
			write(file, il->zoneBitmaps, (il->numBitmaps * il->ints_bitmaps_aligned) *  sizeof(uint));
		}		
	}
	/** Saving the postings lists using bytecodes. */
	{
		//fprintf(stderr,"\n saving size ucompressed = %lu!!!!",il->sizeUncompressed);
		write(file, &(il->sizeUncompressed), sizeof(uint));		
		write(file, &(il->sampleK), sizeof(uint));		
		write(file, &(il->lenBitmapThreshold), sizeof(uint));		
		write(file, &(il->zoneOccsSize), sizeof(uint)); //in # bits
		write(file, &(il->numBuckets), sizeof(uint));		
		write(file, &(il->absOffsetSize), sizeof(uint));		
		write(file, &(il->bcOffsetSize), sizeof(uint));		
		write(file, &(il->bcsSize), sizeof(uint));		
				

		//fprintf(stderr,"\n *sampleK = %d, zoneOccsSize = %d, numbucks= %d",wcsa->occs.sampleK, wcsa->occs.zoneOccsSize, wcsa->occs.numBuckets); 
		//fprintf(stderr,"\n *absoffsetSize = %d, bcOffsetSize = %d, occsBucketSize= %d",wcsa->occs.absOffsetSize, wcsa->occs.bcOffsetSize, wcsa->occs.occsBucketSize); 
		//fprintf(stderr,"\n *bcsSize = %d",wcsa->occs.bcsSize);

		write(file, il->occs, il->nlists * sizeof(uint));		
		write(file, il->zoneOccs, ((il->zoneOccsSize +W-1)/W) * sizeof(uint)); //zoneOccs kbitVec[bucketSize]
		write(file, il->bcs, il->bcsSize * sizeof(byte));					
		write(file, il->bcsptr, (il->nlists +1)* sizeof(uint));				
	}
	
	close(file);				
	free(filename);
	return 0;
}


    /*  Loads index from one or more file(s) named filename, possibly 
      adding the proper extensions. */
      
int load_il (char *filebasename, void **ail) {
	char *basename = filebasename;
	t_il *il = (t_il *) malloc (sizeof(t_il) * 1);		
	
	int file;

	char *filename;	
	filename = (char *)malloc(sizeof(char)*(strlen(basename)+20));
	strcpy(filename, basename); strcat(filename, ".");	strcat(filename, POSTINGS);
	fprintf(stderr,"\tLoading posting lists: %s\n", filename);
	
	if( (file = open(filename, O_RDONLY)) < 0) {
		printf("Cannot open file %s\n", filename);
		exit(0);
	}

	/* loading nlists variable and lenList vector */
	{	
		read(file, &(il->nlists), sizeof(uint)); // number of posting lists 
	
		il->lenList = (uint *) malloc ( (il->nlists) * sizeof(uint));  // the lenList array
		read(file, il->lenList, ((il->nlists) * sizeof(uint)) );
	}

	/** Loading the postings lists using bitmaps.*/
	{
		read(file, &(il->lenBitmapThreshold), sizeof(uint));		
		read(file, &(il->bits_bitmaps), sizeof(uint));		
		read(file, &(il->ints_bitmaps_aligned), sizeof(uint));	
		
		read(file, &(il->numBitmaps), sizeof(uint));		

		//il->zoneBitmaps = (uint *) malloc (sizeof(uint) * ( il->numBitmaps * il->ints_bitmaps_aligned));
		//il->zoneBitmaps[( il->numBitmaps * il->ints_bitmaps_aligned) -1] =0000; //so valgrind is shut up		
		//read(file, il->zoneBitmaps, ( il->numBitmaps * il->ints_bitmaps_aligned) *  sizeof(uint));

		if (il->numBitmaps){
			il->zoneBitmaps = (uint *) malloc (sizeof(uint) * ( il->numBitmaps * il->ints_bitmaps_aligned));
			il->zoneBitmaps[( il->numBitmaps * il->ints_bitmaps_aligned) -1] =0000; //so valgrind is shut up		
			read(file, il->zoneBitmaps, ( il->numBitmaps * il->ints_bitmaps_aligned) *  sizeof(uint));
		}
		else il->zoneBitmaps = NULL;		
	}

	/* loading the posting lists using bytecodes*/
	{		
		//some integers
		read(file, &(il->sizeUncompressed), sizeof(uint));					
		read(file, &(il->sampleK), sizeof(uint));
		read(file, &(il->lenBitmapThreshold), sizeof(uint));				
		read(file, &(il->zoneOccsSize), sizeof(uint)); //in # bits
		read(file, &(il->numBuckets), sizeof(uint));		
		read(file, &(il->absOffsetSize), sizeof(uint));		
		read(file, &(il->bcOffsetSize), sizeof(uint));	
		il->occsBucketSize = il->bcOffsetSize + il->absOffsetSize;
		read(file, &(il->bcsSize), sizeof(uint));		
		
		//vector occs.
		il->occs = (uint *) malloc (il->nlists * sizeof(uint) );		
		read(file, il->occs, il->nlists * sizeof(uint)); //wcsa->nwords was loading in "loadVocabulary"

		//vector zoneOccs (the kbitVector, with numBuckets of size (occsBucketSize) bits each
		il->zoneOccs = (uint *) malloc (((il->zoneOccsSize +W-1)/W) * sizeof(uint));
		if (il->zoneOccs) {
			il->zoneOccs[((il->zoneOccsSize +W-1)/W)-1] =0000; //so valgrind is shut up		
		}
		read(file, il->zoneOccs, ((il->zoneOccsSize +W-1)/W) * sizeof(uint)); //zoneOccs kbitVec[bucketSize]

		//vector bcs.
		il->bcs = (byte *) malloc (il->bcsSize * sizeof(byte));		
		read(file, il->bcs, il->bcsSize * sizeof(byte));		
		
		//vectir bcsptr.
		il->bcsptr = (uint *) malloc (sizeof(uint) * (il->nlists +1));
		read(file, il->bcsptr, (il->nlists +1)* sizeof(uint));		
		
	}		
	
	close(file);			
	free(filename);
						    
	*ail = il;
	return 0;
}



    /* Frees the memory occupied by index. */

int free_il (void *ail) {
	t_il *il=(t_il *) ail;

	free(il->lenList);
	free(il->occs);
	free(il->bcs);
	free(il->bcsptr);
	if (il->zoneOccs) free(il->zoneOccs);
	free(il->zoneBitmaps);
	free(il);

	return 0;
}


//	/* Gives the memory occupied by index in bytes. */
//
//int size_il(void *ail, uint *size) {
//	t_il *il=(t_il *) ail;
//	*size=0;
//	*size += sizeof(t_il);
//	
//	*size += il->nlists * sizeof(uint) ;   //lenList vect
//	*size += il->nlists * sizeof(uint) ;   //occs vect
//	*size += (il->nlists + 1)* sizeof(uint);  //il->bcsptr 
//	*size += (il->zoneOccsSize + W -1)/W * sizeof(uint);
//	*size += il->bcsSize ;
//
//	return 0;	
//}
//



uint  sizetobits(void *ail) {
	uint i; uint maxs, bits_s;
	t_il *il=(t_il *) ail;
	uint total;
	

	// size of lenList
	maxs=0;
	for(uint i=0;i<il->nlists;i++)       //** calcula el m�ximo s�mbolo en symbols_new;
		maxs = max(maxs,il->lenList[i]);
	bits_s = bits(maxs);                   //** numero de bits necesarios para los valores en "symbols_new"
	total = (il->nlists * bits_s / W +1)*4;   //** valores de symbols_new codificados con "bits_sn" bits.

	//fprintf(stderr,"\n sizeOfLenlist = %d bytes",  (il->nlists * bits_s / W +1));

	// size of occs
	maxs=0;
	for(uint i=0;i<il->nlists;i++)       //** calcula el m�ximo s�mbolo en symbols_new;
		maxs = max(maxs,il->occs[i]);	
	//maxs = il->occs[il->nlists-1];
	bits_s = bits(maxs);                   //** numero de bits necesarios para los valores en "symbols_new"
	total += (il->nlists * bits_s / W +1)*4;   //** valores de symbols_new codificados con "bits_sn" bits.

	//fprintf(stderr,"\n sizeOf occs[] = %d bytes",  (il->nlists * bits_s / W +1));

	// size of bcsptr

	maxs = il->bcsptr[il->nlists];
	bits_s = bits(maxs);                   //** numero de bits necesarios para los valores en "symbols_new"
	total += (il->nlists * bits_s / W +1)*4;   //** valores de symbols_new codificados con "bits_sn" bits.

	//fprintf(stderr,"\n  sizeOf bcsptr[] = %d bytes", 4* (il->nlists * bits_s / W +1) );

	return total;
}
 

/** creates a "fun" matlab file with the ratio of
 *  source len of posting and compressed len
 **/  
int buildBell_il(void *ail, const char *matlabfilename) {
	uint i; uint maxs, bits_s;
	uint bits_lenlist,bits_occs,bits_bcsptr;
	t_il *il=(t_il *) ail;
	uint total;
	uint maxLen;

	{

	// size of lenList
	maxs=0;
	for(uint i=0;i<il->nlists;i++)       		//** calcula el m�ximo s�mbolo en lenlist;
		maxs = max(maxs,il->lenList[i]);
	bits_lenlist = bits(maxs);                   //** numero de bits necesarios para los valores en "len_list"
	maxLen=maxs;

	// size of occs
	//maxs = il->occs[il->nlists-1];
	maxs=0;
	for(uint i=0;i<il->nlists;i++)       		//** calcula el m�ximo s�mbolo en lenlist;
		maxs = max(maxs,il->occs[i]);	
	bits_occs = bits(maxs);                   	//** numero de bits necesarios para los valores en "occs"

	// size of bcsptr

	maxs = il->bcsptr[il->nlists];
	bits_s = bits(maxs);                   //** numero de bits necesarios para los valores en "bcs_ptr"
	bits_bcsptr = bits_s;
	
	}	

  {
  	
  	uint nodes = il->nlists;
	uint i,lenPost,lenPhrase;
	uint *count = (uint *) malloc ( sizeof(uint) * (maxLen+1));
	for (i=0; i<= maxLen; i++) count[i]=0;

	for (i=0; i< nodes; i++) {
		lenPost = il->lenList[i];
		count[lenPost]++;
	}
	
	uint **M = (uint **) malloc (sizeof(uint *) * (maxLen+1));    
	for (i=1; i<=maxLen;i++) {
		if (count[i] >0)
			M[i] = (uint *) malloc (sizeof(uint) * (count[i]));
	} 
		
	for (i=1;i<=maxLen;i++) count[i]=0;

	for (i=0; i< nodes; i++) {
		lenPost = il->lenList[i];                    //len of original list
		
		if (!shouldUseBitmap(il,i))
			lenPhrase = il->bcsptr[i+1] - il->bcsptr[i]; //len of the posting list
		else 
			lenPhrase = (il->ints_bitmaps_aligned) * sizeof(uint);
			
		M[lenPost][count[lenPost]] = lenPhrase;
		count[lenPost]++;
	}
	//Ahora en cada lenPostOriginal tenemos almacenadas las longitudes de frase actuales de aquellos postings

	double *avgs = (double *) malloc (sizeof(double) * (maxLen+1));	

	uint j;
	uint total;
	for (i=1; i<=maxLen;i++) {
		if (count[i] >0) {
			total = 0;
			for (j=0;j<count[i];j++) {	
				total += M[i][j];
			}
			avgs[i] = total*1.0 / count[i];	
	//		printf("\n valores medios: pos= %d, count = %d, avg = %f",i, count[i],avgs[i]);
		}			
	} 
	
	//FILE *f = fopen ("grafica.m","w");  
	FILE *f = fopen (matlabfilename,"w");  
	uint curr=1;
	for (i=1;i<=maxLen;i++) {
		if (count[i]>0) {
			fprintf(f,"\ngx(%d)=%d;",curr,i);
			fprintf(f,"\ngy(%d)=%f;",curr,(avgs[i]));  // * (g->bits_sn/8.0)));
			curr++;
		}
	}
	//	fprintf(stderr,"\n ... bits_sn vale = %d",g->bits_sn);
	fprintf(f,"\nfigure(4);");
	fprintf(f,"\nplot(gx,gy);");
	fprintf(f,"xlabel('len of original list');");
	fprintf(f,"ylabel('len of compressed list (bytes)');");
	fclose(f);

	for (i=1; i<=maxLen;i++) {
		if (count[i] >0) free(M[i]);
	}
	free(M);
	free(count);
	free(avgs);

  }
	
  return 0;
}







	/* Gives the memory occupied by index in bytes. */

int size_il(void *ail, uint *size) {
	t_il *il=(t_il *) ail;
	*size=0;
	*size += sizeof(t_il);
	
//	*size += il->nlists * sizeof(uint) ;   //lenList vect
//	*size += il->nlists * sizeof(uint) ;   //occs vect
//	*size += (il->nlists + 1)* sizeof(uint);  //il->bcsptr 

	*size += sizetobits(ail);

	uint sizeUint=0;
	sizeUint += il->nlists * sizeof(uint) ;   //lenList vect
	sizeUint += il->nlists * sizeof(uint) ;   //occs vect
	sizeUint += (il->nlists + 1)* sizeof(uint);  //il->bcsptr 

	fprintf(stderr,"\n size of lenlist+occs+bcsptr = %d bytes Vs %d bytes (old)",sizetobits(ail),sizeUint);
	fprintf(stderr,"\n size of byteCodes = %d bytes \n",il->bcsSize);

	*size += (il->zoneOccsSize + W -1)/W * sizeof(uint);

	fprintf(stderr,"\n size of zoneOccs = %d bytes \n",((il->zoneOccsSize + W -1)/W * sizeof(uint)));

	*size += il->bcsSize ;

	fprintf(stderr,"\n size of bitmaps = %d bytes \n",(( il->numBitmaps * il->ints_bitmaps_aligned) * sizeof(uint) ));
	*size += (( il->numBitmaps * il->ints_bitmaps_aligned) * sizeof(uint) );

	return 0;	
}


    /* Gives the memory occupied by the index assuming an uncompressed
       representation (uint **)*/
       
int size_il_uncompressed(void *ail, uint *size) {
	t_il *il=(t_il *) ail;
	*size=il->sizeUncompressed;	
	
	return 0;	
}


    /* Gives the length of the longest posting-list*/
    
int maxSizePosting_il(void *ail, uint *size){
	t_il *il=(t_il *) ail;
	uint maxlen=0;
	register uint i, end;
	end = il->nlists;
	for (i=0; i< end; i++) {
		if (il->lenList[i] > maxlen) maxlen = il->lenList[i];
	}
	
	*size=maxlen;	
	return 0;	
}



/**********************************************************************************/
/***** Operations (searching) with the posting lists ****/
/**********************************************************************************/

    /* Extracts the id-th list, having id \in [0..maxId-1] */

int extractList_il (void *ail, uint id, uint **list, uint *len) {
	t_il *il=(t_il *) ail;	

	register uint lenlist  = il->lenList[id];
	uint *alist = (uint *) malloc (sizeof(uint) * lenlist);	

	//a list encoded with a bitmap.
	if ( shouldUseBitmap(il,id) ) {
		
		uint inipos = il->occs[id]*il->ints_bitmaps_aligned*W; //pos in zoneBitmaps.
		uint endpos = inipos + il->bits_bitmaps;
		uint *zoneBitmaps = il->zoneBitmaps;
		uint i;
		
		lenlist=0;		
		for (i=inipos; i<endpos; i++) {
			if (bitget(zoneBitmaps,i)) 
				alist[lenlist++] = i-inipos;
		}
		*list =alist;
		*len =lenlist;		
		return 0;				
	}

	//else ga list encoded with bytecodes
	register uint absBits = il->absOffsetSize;
	uint obits;
	OBITS(obits, il->bcsptr, id);	
	register uint bcBits = obits; //il->bcOffsetSize;
	
	byte *bcs  = il->bcs;// + (il->bcsptr[id]);
	uint inibcs_i = il->bcsptr[id];

	uint *zoneoccs = il->zoneOccs;
	uint sampleK = il->sampleK;
	uint sampleP;
	SAMPLEPERIOD(sampleP,sampleK,lenlist);
	
	//fprintf(stderr,"\nDecoding all the %d occurrences of word[%d]= <<%s>>",lenlist, id, wcsa->words[id]);fflush(stderr);		 		
	uint i, pos, bcpos, num, prevvalue;
	pos = il->occs[id]; //pos in zoneoccs
	for (i=0; i<lenlist;i++) {		
		if (! (i%sampleP)) {	//an abs sample
			prevvalue = bitread (zoneoccs, pos, absBits);
			alist[i]=prevvalue;
			pos+=absBits;
			bcpos = bitread (zoneoccs, pos, bcBits);
			bcpos +=inibcs_i;
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
	return 0; 			
}	

int SVNextractList_il (void *ail, uint id, uint **list, uint *len, uint offs) { // GZL
	t_il *il=(t_il *) ail;	

	register uint lenlist  = il->lenList[id];
	uint *alist = (uint *) malloc (sizeof(uint) * lenlist);	

	//a list encoded with a bitmap.
	if ( shouldUseBitmap(il,id) ) {
		
		uint inipos = il->occs[id]*il->ints_bitmaps_aligned*W; //pos in zoneBitmaps.
		uint endpos = inipos + il->bits_bitmaps;
		uint *zoneBitmaps = il->zoneBitmaps;
		uint i;
		
		lenlist=0;		
		for (i=inipos; i<endpos; i++) {
			if (bitget(zoneBitmaps,i)) 
				alist[lenlist++] = i-inipos-offs; // GZL
		}
		*list =alist;
		*len =lenlist;		
		return 0;				
	}

	//else ga list encoded with bytecodes
	register uint absBits = il->absOffsetSize;
	uint obits;
	OBITS(obits, il->bcsptr, id);	
	register uint bcBits = obits; //il->bcOffsetSize;
	
	byte *bcs  = il->bcs;// + (il->bcsptr[id]);
	uint inibcs_i = il->bcsptr[id];

	uint *zoneoccs = il->zoneOccs;
	uint sampleK = il->sampleK;
	uint sampleP;
	SAMPLEPERIOD(sampleP,sampleK,lenlist);
	
	//fprintf(stderr,"\nDecoding all the %d occurrences of word[%d]= <<%s>>",lenlist, id, wcsa->words[id]);fflush(stderr);		 		
	uint i, pos, bcpos, num, prevvalue;
	pos = il->occs[id]; //pos in zoneoccs
	for (i=0; i<lenlist;i++) {		
		if (! (i%sampleP)) {	//an abs sample
			prevvalue = bitread (zoneoccs, pos, absBits);
			alist[i]=prevvalue-offs; // GZL
			pos+=absBits;
			bcpos = bitread (zoneoccs, pos, bcBits);
			bcpos +=inibcs_i;
			pos +=bcBits;			
		}
		else { //decoding from bcs
			BC_TO_DEC(bcs, bcpos, num);
			prevvalue +=num;
			alist[i]=prevvalue-offs; // GZL
		}
	}		
	*list =alist;
	*len =i;		
	return 0; 			
}	


    /* Extracts the id-th list, having id \in [0..maxId-1] 
       Assumes list has enough space previously allocated. */
       
int extractListNoMalloc_il (void *ail, uint id, uint *list, uint *len){
	t_il *il=(t_il *) ail;	
	register uint lenlist;

	//a list encoded with a bitmap.
	if ( shouldUseBitmap(il,id) ) {
		
		uint inipos = il->occs[id]*il->ints_bitmaps_aligned*W; //pos in zoneBitmaps.
		uint endpos = inipos+il->bits_bitmaps;
		uint *zoneBitmaps = il->zoneBitmaps;
		uint i;
		
		lenlist=0;		
		for (i=inipos; i<endpos; i++) {
			if (bitget(zoneBitmaps,i)) 
				list[lenlist++] = i-inipos;
		}
		*len =lenlist;		
		return 0;				
	}
	
	register uint absBits = il->absOffsetSize;
	uint obits;
	OBITS(obits, il->bcsptr, id);	
	register uint bcBits = obits; //il->bcOffsetSize;
	byte *bcs  = il->bcs;// + (il->bcsptr[id]);
	uint inibcs_i = il->bcsptr[id];		
	uint *zoneoccs = il->zoneOccs;
	lenlist  = il->lenList[id];
	uint sampleK = il->sampleK;
	uint sampleP;
	SAMPLEPERIOD(sampleP,sampleK,lenlist);

	//fprintf(stderr,"\nDecoding all the %d occurrences of word[%d]= <<%s>>",lenlist, id, wcsa->words[id]);fflush(stderr);		 		
	uint i, pos, bcpos, num, prevvalue;
	pos = il->occs[id]; //pos in zoneoccs
	for (i=0; i<lenlist;i++) {		
		if (! (i%sampleP)) {	//an abs sample
			prevvalue = bitread (zoneoccs, pos, absBits);
			list[i]=prevvalue;
			pos+=absBits;
			bcpos = bitread (zoneoccs, pos, bcBits);
			bcpos +=inibcs_i;
			pos +=bcBits;			
		}
		else { //decoding from bcs
			BC_TO_DEC(bcs, bcpos, num);
			prevvalue +=num;
			list[i]=prevvalue;			
		}
	}		
	
	*len =i;		
	return 0; 			
}	



/**********************************************************************************/
/**********************************************************************************/



    /* Sets options for searches: such as parameters, funtions to use, etc 
       Permits to set different algorithms for intersect2, intersectN and 
       if needed "fsearch-operations needed by svs-type algorithms"     */

int setDefaultSearchOptions_il (void *ail, char *search_options){

	t_il *il = (t_il *) ail;
	
	il->defs.defaultFsearch  = NULL;
	il->defs.defaultIntersect2 = NULL;
	il->defs.defaultIntersectN = NULL;
	
	/** processing the parameters */
	char delimiters[] = " =;";
	int j,num_parameters;
	char ** parameters;
	
	if (search_options != NULL) {
	parse_parameters(search_options,&num_parameters, &parameters, delimiters);
	for (j=0; j<num_parameters;j++) {
		//setting the default function to Intersect-2-lists		
		if ((strcmp(parameters[j], "int2") == 0 ) && (j < num_parameters-1) ) {
			if (strcmp(parameters[j+1], "svs") == 0 ) 
				il->defs.defaultIntersect2 = svs2;
			else if (strcmp(parameters[j+1], "merge") == 0 )
				il->defs.defaultIntersect2 = merge2;	    
			j++;
		}
		//setting the default function to Intersect-N-lists		
		else if  ((strcmp(parameters[j], "intn") == 0 ) && (j < num_parameters-1) ) {
			if (strcmp(parameters[j+1], "svs") == 0 ) 
				il->defs.defaultIntersectN = svsn;			
			j++;
		}
		//setting the default fSearch function used in svs-based algoritms.		
		else if  ((strcmp(parameters[j], "fsearch") == 0 ) && (j < num_parameters-1) ) {
			if (strcmp(parameters[j+1], "unc") == 0 ) 
				il->defs.defaultFsearch = fsearchUnc;
			else if (strcmp(parameters[j+1], "dec") == 0 ) 
				il->defs.defaultFsearch = fsearchDec;
			else if (strcmp(parameters[j+1], "seq") == 0 ) 
				il->defs.defaultFsearch = fsearchSeq;
			else if (strcmp(parameters[j+1], "seqM") == 0 ) 
				il->defs.defaultFsearch = fsearchSeqMem;
			else if (strcmp(parameters[j+1], "bin") == 0 )
				il->defs.defaultFsearch = fsearchBin;	    
			else if (strcmp(parameters[j+1], "binM") == 0 ) 
				il->defs.defaultFsearch = fsearchBinMem;
			else if (strcmp(parameters[j+1], "exp") == 0 )
				il->defs.defaultFsearch = fsearchExp;	    
			else if (strcmp(parameters[j+1], "expM") == 0 ) 
				il->defs.defaultFsearch = fsearchExpMem;
							
			j++;
		}
	}
	free_parameters(num_parameters, &parameters);
	}
	
	fprintf(stderr,"\n ** Call to setDefaultSearchOptions_il: \"%s\"\n", search_options);
		
	return 0;	
}

	/* Returns the len of the ids-th posting list */
int lenlist(void *ail, uint id, uint *len){
	t_il *il = (t_il *) ail;
	*len = il->lenList[id];
	return 0;
}


	/* Intersects two lists given by id1 and id2  \in [0..maxId-1].
	   The defaultIntersect2 function must be initialized previously (svs, merge, ...)
	   Returns the resulting list of "intersecting positions"  */

int intersect_2_il (void *ail, uint id1, uint id2,  uint *noccs, uint **occs){
	t_il *il = (t_il *) ail;
	
	if ( il->defs.defaultIntersect2 == NULL) {
		return 40;	
	}

	// call to svs2, merge2, etc... depending on default settings
	int (*defaultIntersect2) (void *ail, uint id1, uint id2, uint *noccs, uint **occs ) = il->defs.defaultIntersect2;
//	(*defaultIntersect2) (ail,id1,id2,noccs,occs);	
//	return 0;	
		
	int reterr = ((*defaultIntersect2) (ail,id1,id2,noccs,occs));

	if ((!reterr) && (!*noccs)){ 
		free (*occs);
		*occs=NULL;
	}
	
	return 	reterr;
}

	/* Intersects $nids$ lists given by ids in ids[], having ids[i] \in [0..maxId-1].
	   Returns the resulting list of "intersecting positions"  */

int intersect_N_il (void *ail, uint *ids, uint nids, uint *noccs, uint **occs ) {
	t_il *il = (t_il *) ail;
	if ( il->defs.defaultIntersectN == NULL) {
		return 41;	
	}
	
	// call to svsn,, etc... depending on default settings
	int (*defaultIntersectN) (void *ail, uint *ids, uint nids, uint *noccs, uint **occs ) = il->defs.defaultIntersectN;
	//(*defaultIntersectN) (ail,ids,nids,noccs,occs);
	//return 0;	
	
	//return ((*defaultIntersectN) (ail,ids,nids,noccs,occs));
		
	int reterr = ((*defaultIntersectN) (ail,ids,nids,noccs,occs));

	if ((!reterr) && (!*noccs)){ 
		free (*occs);
		*occs=NULL;
	}
	
	return 	reterr;
}




/*********************************************************************************/
/*********************************************************************************/

/*******************************************************************************/
/* Implementation of the different algoritms needed during searches            */
/*******************************************************************************/


	/* Fsearch, uncompressing the whole posting-list before seq. search  */
	   
inline int fsearchUnc (void *ail, uint id, uint value) {
	//fprintf(stderr,"\n Call to FsearchUnc");

		/** searches for "value" inside list idList 
			 returns 0 if not found,
			        >0 if found
			 !!very slow if a very frequent id is needed.
		 */
	uint *list;
	register uint i;
	uint len;
	extractList_il (ail, id, &list, &len); 
	for (i=0;i<len;i++){
		if (list[i] == value) {
			free(list);
			return 1;			
		}
	} 
	free(list);				
	return 0;	
}



	/* Fsearch, uncompressing the whole posting-list before seq. search  */
	   
inline int fsearchBitmap (void *ail, uint id, uint value) {
	//fprintf(stderr,"\n Call to FsearchBitmap");

		/** searches for "value" inside list idList 
			 returns 0 if not found,
			        >0 if found
		 */
	t_il *il=(t_il *) ail;	
			
	register uint pos = il->occs[id]* il->ints_bitmaps_aligned * W +value;
	if (bitget((il->zoneBitmaps),pos)) return 1;
	
	return 0;	
}



	/* Fsearch, decompressing sequentially up to the searched value */

inline int fsearchDec (void *ail, uint id, uint value) {
	//fprintf(stderr,"\n Call to Fsearch-seq");
	
	t_il *il=(t_il *) ail;	
	
	register uint absBits = il->absOffsetSize;
	//register uint bcBits = il->bcOffsetSize;	
	uint obits;
	OBITS(obits, il->bcsptr, id);	
	register uint bcBits = obits; //il->bcOffsetSize;
		
	//byte *bcs  = il->bcs + (il->bcsptr[id]);
	byte *bcs  = il->bcs;// + (il->bcsptr[id]);
	uint inibcs_i = il->bcsptr[id];
	
	uint *zoneoccs = il->zoneOccs;
	register uint lenlist  = il->lenList[id];
		
	register uint sampleP = il->sampleP;	


	//fprintf(stderr,"\nDecoding all the %d occurrences of word[%d]= <<%s>>",lenlist, id, wcsa->words[id]);fflush(stderr);		 		
	register uint i;
	uint pos, bcpos, num, val;
	pos = il->occs[id]; //pos in zoneoccs
	for (i=0; i<lenlist;i++) {		
		if (! (i%sampleP)) {	//an abs sample
			val = bitread (zoneoccs, pos, absBits);
			if (value == val) {return 1;}
			pos+=absBits;
			bcpos = bitread (zoneoccs, pos, bcBits);
			bcpos +=inibcs_i;
			pos +=bcBits;			
		}
		else { //decoding from bcs
			BC_TO_DEC(bcs, bcpos, num);
			val +=num;
			if (value == val) {return 1;}
		}
	}		
	return 0; 
}

	/* Fsearch, using sequential Search in the samples first*/

inline int fsearchSeq (void *ail, uint id, uint value) {
	//fprintf(stderr,"\n Call to Fsearch-seq");
	
	t_il *il=(t_il *) ail;	
	
	register uint absBits = il->absOffsetSize;

	//register uint bcBits = il->bcOffsetSize;	
	uint obits;
	OBITS(obits, il->bcsptr, id);	
	register uint bcBits = obits; //il->bcOffsetSize;

	register uint bucketBits = absBits + bcBits;

	byte *bcs  = il->bcs;// + (il->bcsptr[id]);
	register uint inibcs_i = il->bcsptr[id];
		
	uint *zoneoccs = il->zoneOccs;
	register uint lenlist  = il->lenList[id];
		
	register uint sampleP = il->sampleP;	
	
	register uint i,l,n; //,r
	uint pos, bcpos, num, val;
	pos = il->occs[id]; //pos in zoneoccs
	
	n = (lenlist +sampleP -1)/sampleP; //number of samples to seq search.			
	l = pos; //r= l +	(lenList +sampleP -1)/sampleP;
	
	/** Searching in the samples */	
	val = bitread (zoneoccs, pos, absBits);
	
	if (n==1) {
		if (val == value) return 1;
		else if (val > value) return 0;
		// else decoding is needed;	
		n = lenlist - 1 ;  //number of values to decode
	}
	else { //if (n > 1) {	
		//Seq search in the samples.		
		//i=0;
		if (val == value) return 1;
		else if (val > value) return 0;
		pos += bucketBits; //next pos			
		
		for (i=1; i< n; i++) {   
			val = bitread (zoneoccs, pos, absBits);		
			if (val >= value) {
				if (val == value) return 1;
				break;	 //val > value;
			}
			pos += bucketBits; //next pos			
		}
		//fprintf(stderr,"\n i= %d",i);

		//preparing to decode if needed
		pos-=bucketBits;
		val = bitread (zoneoccs, pos, absBits);
		if (i==n)
			n=(lenlist -1)%sampleP;  //number of values to decode
		else 
			n = sampleP -1;		     //number of values to decode
	}
	
	//value not found in a sampled-position ==> decoding is needed.
	//pos is the position in previous sample
	//val is the value in previous sample
	//n is the max number of bytecodes that should be decoded.
	
	pos+=absBits;
	bcpos = bitread (zoneoccs, pos, bcBits); //ptr to bcs array.
	bcpos +=inibcs_i;
	for (i=0; i<n;i++) {		
		BC_TO_DEC(bcs, bcpos, num);
		val +=num;
		if (value == val) return 1;
		else if (val > value) return 0; //not found
	}		
	return 0; 
}


	/* Fsearch, using sequential Search in the samples first*/

  //** VERSI�N ALENEX **/
inline int fsearchSeqMem2 (void *ail, uint id, uint value) {
	fprintf(stderr,"\n Call to Fsearch-seq: not implemented");
	

	return 0; 
}

  //** VERSI�N MEJORADA SIGIR **/
inline int fsearchSeqMem (void *ail, uint id, uint value){
	//fprintf(stderr,"\n Call to Fsearch-bin");
	t_il *il=(t_il *) ail;	
	
	register uint absBits = il->absOffsetSize;

	uint obits;
	OBITS(obits, il->bcsptr, id);	
	register uint bcBits = obits; //il->bcOffsetSize;

	register uint bucketBits = absBits + bcBits;

	byte *bcs  = il->bcs;
	uint inibcs_i = il->bcsptr[id];
	
	
	uint *zoneoccs = il->zoneOccs;
	register uint lenlist  = il->lenList[id];
			
	register uint sampleP = il->sampleP;
	
	uint R = (lenlist +sampleP -1)/sampleP -1;  //the last sample
	
	register uint i,l,r,m,val; 
	register uint pos, num ;
	
	pos = il->occs[id]; //pos in zoneoccs

	uint inipos=pos;	
	
	
	register uint bcpos,j;
	
	l = 0; r= R;

	l = il->i;
	/** Searching in the samples */	
    /** exp search **/
	
	i=l;  //** keeps prev i value **/

	if (l<R) {
		pos = inipos + (l+1) * bucketBits;
		val = bitread (zoneoccs, pos, absBits);	
				
		/**seq search**/
		while (((1+l) <R) && (val < value)){
			l++;
			pos = inipos + (l+1) * bucketBits;
			val = bitread (zoneoccs, pos, absBits);	
		}
		r=l+1;
	}
	
	
	pos = inipos + (r * bucketBits);
	val = bitread (zoneoccs, pos, absBits);	

    if (val <value) {
        l=r;
    }
    else if (val == value) {il->i=r; il->j=0;return 1;}
	else {
		pos = inipos + l * bucketBits;
		val = bitread (zoneoccs, pos, absBits);	
		if (value == val) {il->i=l; il->j=0; return 1;}
		else if (val > value) {il->i=l; il->j=0; return 0; }//no decoding is needed.
	}
	
	il->i=l;	

	if (i==l) 
		j=il->j;
	else j=0;
		
	//value not found in a sampled-position ==> decoding is needed.
	//pos = l*bucketBits is the position of the starting absolute sample.
	//val is the value in previous sample
	//n is the max number of bytecodes that should be decoded.
	
	//decoding from l .......................

	if (j==0) { 
		bcpos = bitread (zoneoccs, pos+absBits, bcBits); //ptr to bcs array.
		bcpos +=inibcs_i;
	}
	else {
		bcpos = il->bcpos;	
		val = il->val;
	}
	

	register uint n;
		
	//if after the last sampled value
	if (l== R)
		n = (lenlist -1)%sampleP;
	else n = sampleP - 1;		
			
	uint prevbcpos;
	for (; j<n; j++) {	//checks in the last "n" values...	
		prevbcpos = bcpos;
		BC_TO_DEC(bcs, bcpos, num);
		val +=num;
		
		if (value == val) {
			il->bcpos = bcpos;
			il->val = val;
			il->j=j+1;
			if (j+1==n) {il->i=l+1; il->j=0;}
			return 1;
		}
		else if (val > value) {
			il->bcpos = prevbcpos;
			il->val = val - num;
			il->j= j;
			return 0; //not found
		}		
	}		

	il->i = l+1;
	//il->bcpos = bcpos;
	il->j=0;

	return 0; 	
}



inline int fsearchBin (void *ail, uint id, uint value){
	//fprintf(stderr,"\n Call to Fsearch-bin");
	t_il *il=(t_il *) ail;	
	
	register uint absBits = il->absOffsetSize;

	//register uint bcBits = il->bcOffsetSize;	
	uint obits;
	OBITS(obits, il->bcsptr, id);	
	register uint bcBits = obits; //il->bcOffsetSize;

	register uint bucketBits = absBits + bcBits;

	byte *bcs  = il->bcs;// + (il->bcsptr[id]);
	uint inibcs_i = il->bcsptr[id];
	
	uint *zoneoccs = il->zoneOccs;
	register uint lenlist  = il->lenList[id];
		
	register uint sampleP = il->sampleP;	
	
	register uint i,l,r,m,val; 
	register uint pos, num ;
	
	pos = il->occs[id]; //pos in zoneoccs
	
	l = 0; r= l+ (lenlist +sampleP -1)/sampleP -1;
	
	/** Searching in the samples */	
	val = bitread (zoneoccs, pos, absBits);
	
	//   ** first case**
	//   After the last sampled absolute pos: ==> sequential decoding from there on.
	val = bitread (zoneoccs, (pos + r*bucketBits), absBits);  //the last Abs sample
	if (val  <= value) {
		if (val == value) return 1;
		pos = bitread (zoneoccs, pos + r*bucketBits + absBits , bcBits);
		pos +=inibcs_i;	
		l = 0;
		//r = lenlist%sampleP;  //number of codewords to decode (at most)
		r=(lenlist -1)%sampleP; 
		while (l<r) {  //decodes sequentially
			BC_TO_DEC(bcs, pos, num);
			val += num;
			if (val >  value) return 0; //not found
			else if (val == value) return 1; // found!!.
			l++;	
		}
		return 0;
	}

	//   ** second case**
	//Before the last sampled absolute pos ==> binary search. 
	//printf("\nTO BIN SEARCH:: l= %d, r=%d, val = %d, value =%d",l,r,val,value);fflush(stdout);
	uint inipos=pos;
	while (r>(l+1)) {
		m = (l + r) / 2;	
		pos = inipos + (m * bucketBits);
		val = bitread (zoneoccs, pos, absBits);
		if (val < value) l = m;
		else if (val == value) return 1;
		else r = m;
	}
	
	//value not found in a sampled-position ==> decoding is needed.
	//pos = l*bucketBits is the position of the starting absolute sample.
	//val is the value in previous sample
	//n is the max number of bytecodes that should be decoded.

	//decoding from 
	pos = inipos + l * bucketBits;
	val = bitread (zoneoccs, pos, absBits);
	register uint n;
	n=sampleP - 1;
	
	if (value == val) return 1;
			
	pos+=absBits;	
	pos = bitread (zoneoccs, pos, bcBits); //ptr to bcs array.
	pos +=inibcs_i;	
	for (i=0; i<n; i++) {	//checks in the last "n" values...	
		BC_TO_DEC(bcs, pos, num);
		val +=num;
		if (value == val) return 1;
		else if (val > value) return 0; //not found
	}		
	return 0; 	
}



inline int fsearchBinMem (void *ail, uint id, uint value){
	//fprintf(stderr,"\n Call to Fsearch-bin");
	t_il *il=(t_il *) ail;	
	
	register uint absBits = il->absOffsetSize;

	uint obits;
	OBITS(obits, il->bcsptr, id);	
	register uint bcBits = obits; //il->bcOffsetSize;

	register uint bucketBits = absBits + bcBits;

	byte *bcs  = il->bcs;
	uint inibcs_i = il->bcsptr[id];
	
	
	uint *zoneoccs = il->zoneOccs;
	register uint lenlist  = il->lenList[id];
			
	register uint sampleP = il->sampleP;
	
	uint R = (lenlist +sampleP -1)/sampleP -1;  //the last sample
	
	register uint i,l,r,m,val; 
	register uint pos, num ;
	
	pos = il->occs[id]; //pos in zoneoccs

	uint inipos=pos;	
	
	
	register uint bcpos,j;
	
	l = 0; r= R;

	l = il->i;
	//l=0;
	//j = il->j;
	/** Searching in the samples */	
    /** bin search **/
	
	register uint probe = 1;
	i=l;

	if (l<R) {
		pos = inipos + (l+probe) * bucketBits;
		val = bitread (zoneoccs, pos, absBits);			

		//fprintf(stderr,"\n [out] exp:: l= %d, r= %d, firstval= %d, value searched = %d", l, r, val,value);
		/** bin search **/	
		while (r>(l+1)) {
			m = (l + r) / 2;	
			pos = inipos + (m * bucketBits);
			val = bitread (zoneoccs, pos, absBits);
			if (val < value) l = m;
			else r = m;
		}
	}
	
	pos = inipos + (r * bucketBits);
	val = bitread (zoneoccs, pos, absBits);	

    if (val <value) {
        l=r;
    }
    else if (val == value) {il->i=r; il->j=0;return 1;}
	else {
		pos = inipos + l * bucketBits;
		val = bitread (zoneoccs, pos, absBits);	
		if (value == val) {il->i=l; il->j=0; return 1;}
		else if (val > value) {il->i=l; il->j=0; return 0; }//no decoding is needed.
	}
	
	il->i=l;	

	if (i==l) 
		j=il->j;
	else j=0;

	//value not found in a sampled-position ==> decoding is needed.
	//pos = l*bucketBits is the position of the starting absolute sample.
	//val is the value in previous sample
	//n is the max number of bytecodes that should be decoded.
	
	//decoding from l .......................

	if (j==0) { 
		bcpos = bitread (zoneoccs, pos+absBits, bcBits); //ptr to bcs array.
		bcpos +=inibcs_i;
	}
	else {
		bcpos = il->bcpos;	
		val = il->val;
	}
	

	register uint n;
		
	//if after the last sampled value
	if (l== R)
		n = (lenlist -1)%sampleP;
	else n = sampleP - 1;		
			
	uint prevbcpos;
	for (; j<n; j++) {	//checks in the last "n" values...	
		prevbcpos = bcpos;
		BC_TO_DEC(bcs, bcpos, num);
		val +=num;
		
		if (value == val) {
			il->bcpos = bcpos;
			il->val = val;
			il->j=j+1;
			if (j+1==n) {il->i=l+1; il->j=0;}
			return 1;
		}
		else if (val > value) {
			il->bcpos = prevbcpos;
			il->val = val - num;
			il->j= j;
			return 0; //not found
		}		
	}		

	il->i = l+1;
	//il->bcpos = bcpos;
	il->j=0;

	return 0; 	
}


inline int fsearchExp (void *ail, uint id, uint value){
	//fprintf(stderr,"\n Call to Fsearch-bin");
	t_il *il=(t_il *) ail;	
	
	register uint absBits = il->absOffsetSize;

	uint obits;
	OBITS(obits, il->bcsptr, id);	
	register uint bcBits = obits; //il->bcOffsetSize;

	register uint bucketBits = absBits + bcBits;

	byte *bcs  = il->bcs;// + (il->bcsptr[id]);
	uint inibcs_i = il->bcsptr[id];
	
	uint *zoneoccs = il->zoneOccs;
	register uint lenlist  = il->lenList[id];

	register uint sampleP = il->sampleP;	
	
	uint R = (lenlist +sampleP -1)/sampleP -1;
	register uint i,l,r,m,val; 
	register uint pos, num ;
	
	pos = il->occs[id]; //pos in zoneoccs
	uint inipos=pos;	
	
	l = 0; r= R;

	l=il->i;
	

	/** Searching in the samples */	
	
//	pos = inipos + (l * bucketBits);
//	val = bitread (zoneoccs, pos, absBits);
//	if (val == value) return 1;
//	else if (val > value) return 0;
//	else l++;
//
//	/** seq search **/
//	while (l<r) {
//		m=l+1;
//		pos = inipos + (m * bucketBits);
//		val = bitread (zoneoccs, pos, absBits);
//		if (val < value) l = m;
//		else if (val == value) return 1;
//		else break;		
//	}
//	l--;
//	pos = inipos + l * bucketBits;
//	val = bitread (zoneoccs, pos, absBits);	
//  if (value == val) return 1;
//  /** exp search **/

	
	register uint probe = 1;

	if (l<R) {
		pos = inipos + (l+probe) * bucketBits;
		val = bitread (zoneoccs, pos, absBits);	
		
		//fprintf(stderr,"\n [IN ] exp:: l= %d, r= %d, firstval= %d, value searched= %d", l, r, val,value);
		
		/**exp search**/
		while (((probe+l) <R) && (val < value)){
			l=l+probe;
			probe *=2;
			if ((l+probe) > R) probe=R-l;
			pos = inipos + (l+probe) * bucketBits;
			val = bitread (zoneoccs, pos, absBits);	
		}
			
		r=l+probe;
		//fprintf(stderr,"\n [out] exp:: l= %d, r= %d, firstval= %d, value searched = %d", l, r, val,value);
		/** bin search **/	
		while (r>(l+1)) {
			m = (l + r) / 2;	
			pos = inipos + (m * bucketBits);
			val = bitread (zoneoccs, pos, absBits);
			if (val < value) l = m;
			else r = m;
		}
	}
	
	pos = inipos + (r * bucketBits);
	val = bitread (zoneoccs, pos, absBits);	

    if (val <value)
        l=r;
    else if (val == value) return 1;
	else {
		pos = inipos + l * bucketBits;
		val = bitread (zoneoccs, pos, absBits);	
		if (value == val) return 1;	
		else if (val > value) return 0; //no decoding is needed.
	}
	
	il->i=l;

		
	//value not found in a sampled-position ==> decoding is needed.
	//pos = l*bucketBits is the position of the starting absolute sample.
	//val is the value in previous sample
	//n is the max number of bytecodes that should be decoded.

	
	//decoding from l .......................

	register uint n;
		
	//if after the last sampled value
	if (l== R)
		n = (lenlist -1)%sampleP;
	else n = sampleP - 1;		
			
	pos+=absBits;	
	pos = bitread (zoneoccs, pos, bcBits); //ptr to bcs array.
	pos +=inibcs_i;	
	for (i=0; i<n; i++) {	//checks in the last "n" values...	
		BC_TO_DEC(bcs, pos, num);
		val +=num;
		if (value == val) return 1;
		else if (val > value) return 0; //not found
	}		
	return 0; 	
}


inline int fsearchExpMem (void *ail, uint id, uint value){
	//fprintf(stderr,"\n Call to Fsearch-bin");
	t_il *il=(t_il *) ail;	
	
	register uint absBits = il->absOffsetSize;

	uint obits;
	OBITS(obits, il->bcsptr, id);	
	register uint bcBits = obits; //il->bcOffsetSize;

	register uint bucketBits = absBits + bcBits;

	byte *bcs  = il->bcs;
	uint inibcs_i = il->bcsptr[id];
	
	
	uint *zoneoccs = il->zoneOccs;
	register uint lenlist  = il->lenList[id];
			
	register uint sampleP = il->sampleP;
	
	uint R = (lenlist +sampleP -1)/sampleP -1;  //the last sample
	
	register uint i,l,r,m,val; 
	register uint pos, num ;
	
	pos = il->occs[id]; //pos in zoneoccs

	uint inipos=pos;	
	
	
	register uint bcpos,j;
	
	l = 0; r= R;

	l = il->i;
	//j = il->j;
	/** Searching in the samples */	
    /** exp search **/
	
	register uint probe = 1;
	i=l;

	if (l<R) {
		pos = inipos + (l+probe) * bucketBits;
		val = bitread (zoneoccs, pos, absBits);	
		
		//fprintf(stderr,"\n [IN ] exp:: l= %d, r= %d, firstval= %d, value searched= %d", l, r, val,value);
		
		/**exp search**/
		while (((probe+l) <R) && (val < value)){
			l=l+probe;
			probe *=2;
			if ((l+probe) > R) probe=R-l;
			pos = inipos + (l+probe) * bucketBits;
			val = bitread (zoneoccs, pos, absBits);	
		}
			
		r=l+probe;
		//fprintf(stderr,"\n [out] exp:: l= %d, r= %d, firstval= %d, value searched = %d", l, r, val,value);
		/** bin search **/	
		while (r>(l+1)) {
			m = (l + r) / 2;	
			pos = inipos + (m * bucketBits);
			val = bitread (zoneoccs, pos, absBits);
			if (val < value) l = m;
			else r = m;
		}
	}
	
	pos = inipos + (r * bucketBits);
	val = bitread (zoneoccs, pos, absBits);	

    if (val <value) {
        l=r;
    }
    else if (val == value) {il->i=r; il->j=0;return 1;}
	else {
		pos = inipos + l * bucketBits;
		val = bitread (zoneoccs, pos, absBits);	
		if (value == val) {il->i=l; il->j=0; return 1;}
		else if (val > value) {il->i=l; il->j=0; return 0; }//no decoding is needed.
	}
	
	il->i=l;	

	if (i==l) 
		j=il->j;
	else j=0;
		
	//value not found in a sampled-position ==> decoding is needed.
	//pos = l*bucketBits is the position of the starting absolute sample.
	//val is the value in previous sample
	//n is the max number of bytecodes that should be decoded.

	
	//decoding from l .......................


	if (j==0) { 
		bcpos = bitread (zoneoccs, pos+absBits, bcBits); //ptr to bcs array.
		bcpos +=inibcs_i;
	}
	else {
		bcpos = il->bcpos;	
		val = il->val;
	}
	

	register uint n;
		
	//if after the last sampled value
	if (l== R)
		n = (lenlist -1)%sampleP;
	else n = sampleP - 1;		
			
	uint prevbcpos;
	for (; j<n; j++) {	//checks in the last "n" values...	
		prevbcpos = bcpos;
		BC_TO_DEC(bcs, bcpos, num);
		val +=num;
		
		if (value == val) {
			il->bcpos = bcpos;
			il->val = val;
			il->j=j+1;
			if (j+1==n) {il->i=l+1; il->j=0;}
			return 1;
		}
		else if (val > value) {
			il->bcpos = prevbcpos;
			il->val = val - num;
			il->j= j;
			return 0; //not found
		}		
	}		

	il->i = l+1;
	//il->bcpos = bcpos;
	il->j=0;

	return 0; 	
}



	/*  Intersects 2 posting-lists using set-versus-set
	    Returns *occs (the result), noccs (number of elements)
	    Memory for *occs is allocated, and must be freed by the caller */
	    
int svs2 (void *ail, uint id1, uint id2, uint *noccs, uint **occs){
	t_il *il = (t_il *) ail;
	
	if ( il->defs.defaultFsearch == NULL) {
		return 42;	
	}

	//fprintf(stderr,"\n Call to svs2 %d %d", id1,id2);


	if (shouldUseBitmap(il,id1) && shouldUseBitmap(il,id2)) {
		uint nids = 2;
		uint ids[2];
		ids[0]=id1; ids[1]=id2;
		int err =  mergeNBitmap (ail, ids, nids,  noccs, occs);
		return err;
	}

	// initializing ptr to function "fsearch"
	int (*defaultFsearch) (void *ail, uint id, uint value) = il->defs.defaultFsearch;	
	//(*defaultFsearch) (ail,id,value); //call to fsearch;
	
	{	int result;
		register uint j, n, matches;
		register uint first, second;  //small and longest posting.
		uint *firstlist;
    	        uint p1,p2; // GZL
		
		//if (id1 >= id2) 
		if (il->lenList[id1] <= il->lenList[id2]) 
			{ first =id1; second=id2; p1=0;p2=1;} //GZL
		else  
			{ first =id2; second=id1; p1=1;p2=0;} //GZL

		//fprintf(stderr," lens --> %d, %d", il->lenList[id1], il->lenList[id2]); 

		n=il->lenList[first];
		SVNextractList_il (ail, first, &firstlist, noccs, p1); //reusing *noccs. // GZL
		n=*noccs;
		matches =0;
		
		if (!shouldUseBitmap(il,second)) { //calculates sample period for the second list
			register uint lenlist  = il->lenList[second];			
			uint sampleK = il->sampleK;
			register uint sampleP;
			SAMPLEPERIOD(sampleP,sampleK,lenlist);
			il->sampleP = sampleP;
			il->i=0;	
			il->j=0;
		}	
		else {
{ fprintf(stderr,"ERROR GZL\n"); exit(1); } // GZL
			defaultFsearch=fsearchBitmap;
		}
		
		for (j=0; j<n;j++) {
			result = (*defaultFsearch) (ail, second, firstlist[j] +p2); //call to fsearch; // GZL
			//fprintf(stderr,"\n  Search for value %d -> result = %d",firstlist[j], result);
			if (result) {
				firstlist[matches++] = firstlist[j];
			}
		}
		
		//if (matches == 0) {free(firstlist); firstlist = NULL;}
		
		*occs= firstlist;
		*noccs = matches;			
		
	}
	
	return 0;	
}




/***********************************************************************/


	/*  Intersects 2 posting-lists using merge algorithm (zipper-Sanders)
	    Returns *occs (the result), noccs (number of elements)
	    Memory for *occs is allocated, and must be freed by the caller */


int merge2 (void *ail, uint id1, uint id2, uint *noccs, uint **occs){
	//fprintf(stderr,"\n Call to merge2");

	t_il *il=(t_il *) ail;	
		
	if (shouldUseBitmap(il,id1) && shouldUseBitmap(il,id2)) {
		uint nids = 2;
		uint *ids = (uint *) malloc (nids * sizeof(uint));
		ids[0]=id1; ids[1]=id2;
		int err =  mergeNBitmap (ail, ids, nids,  noccs, occs);
		free(ids);
		return err;
	}
	else if ((!shouldUseBitmap(il,id1)) && (!shouldUseBitmap(il,id2)) ) {
		return merge2ByteCodes(ail,id1,id2,noccs,occs);
	}
	else if (shouldUseBitmap(il,id1)) {
		return merge2Hybrid(ail,id1,id2,noccs,occs);
	}
	else return merge2Hybrid(ail,id2,id1,noccs,occs);
	
}

/* Intersects 2 posting lists simulating a merge-type, one using bytecodes, the other using a bitmap
 * The bytecoded one is uncompressed and directly fsearched into the bitmap. No previous decompression
 * is done (as in svs-2 approach) */
int merge2Hybrid(void *ail, uint idbitmap, uint idbytecode, uint *noccs, uint **occs){
	//fprintf(stderr,"\n Call to mergeHybrid %d %d", idbitmap,idbytecode);
	t_il *il=(t_il *) ail;	

	register uint id = idbytecode;	
	register uint lenlist  = il->lenList[id];
	uint *alist = (uint *) malloc (sizeof(uint) * lenlist);	

	//else ga list encoded with bytecodes
	register uint absBits = il->absOffsetSize;
	uint obits;
	OBITS(obits, il->bcsptr, id);	
	register uint bcBits = obits; //il->bcOffsetSize;
	
	byte *bcs  = il->bcs;// + (il->bcsptr[id]);
	uint inibcs_i = il->bcsptr[id];

	uint *zoneoccs = il->zoneOccs;
	uint sampleK = il->sampleK;
	uint sampleP;
	SAMPLEPERIOD(sampleP,sampleK,lenlist);
	
	//fprintf(stderr,"\nDecoding all the %d occurrences of word[%d]= <<%s>>",lenlist, id, wcsa->words[id]);fflush(stderr);		 		
	uint i, pos, bcpos, num, prevvalue;
	pos = il->occs[id]; //pos in zoneoccs
	uint matches =0;
	for (i=0; i<lenlist;i++) {		
		if (! (i%sampleP)) {	//an abs sample
			prevvalue = bitread (zoneoccs, pos, absBits);
			
			if (fsearchBitmap (ail, idbitmap, prevvalue))
				alist[matches++] = prevvalue;
			pos+=absBits;
			bcpos = bitread (zoneoccs, pos, bcBits);
			bcpos +=inibcs_i;
			pos +=bcBits;			
		}
		else { //decoding from bcs
			BC_TO_DEC(bcs, bcpos, num);
			prevvalue +=num;
			if (fsearchBitmap (ail, idbitmap, prevvalue))
				alist[matches++]=prevvalue;
		}
	}		

	*occs = alist;
	*noccs= matches;
	return 0;	
}





	/* Intersects 2 posting lists represented with bytecodes, using merge */
int merge2ByteCodes (void *ail, uint id1, uint id2, uint *noccs, uint **occs){
	//fprintf(stderr,"\n Call to mergeBytecodes %d %d", id1,id2);

	t_il *il=(t_il *) ail;	
	

	register uint absBits = il->absOffsetSize;
	//register uint bcBits = il->bcOffsetSize;
	//byte *bcs  = il->bcs;
	uint *zoneoccs = il->zoneOccs;
	uint sampleK = il->sampleK;
	byte *bcs = il->bcs;
	register uint sampleP1,sampleP2;


	//fprintf(stderr,"\nDecoding all the %d occurrences of word[%d]= <<%s>>",lenlist, id, wcsa->words[id]);fflush(stderr);		 		
	register uint  n1, n2;
	uint i1,i2;
	uint pos1,pos2, bcpos1,bcpos2, val1, val2, num;
	//byte *bcs1, *bcs2;
	register uint bcBits1, bcBits2;
	uint inibcs_i1, inibcs_i2;

//register uint bcBits = il->bcOffsetSize;	
	{uint obits;
	 OBITS(obits, il->bcsptr, id1);	
 	 bcBits1 = obits; //il->bcOffsetSize;
	 //bcs1  = il->bcs + (il->bcsptr[id1]);
	  inibcs_i1 = il->bcsptr[id1];
	 
	 
	 OBITS(obits, il->bcsptr, id2);	
 	 bcBits2 = obits; //il->bcOffsetSize;
	 //bcs2  = il->bcs + (il->bcsptr[id2]);
	  inibcs_i2 = il->bcsptr[id2];
	}

	n1= il->lenList[id1]; //reading the first value
	pos1 = il->occs[id1]; //pos in zoneoccs
	val1 = bitread (zoneoccs, pos1, absBits);
	pos1+=absBits;
	bcpos1 = bitread (zoneoccs, pos1, bcBits1);
	bcpos1+=inibcs_i1;
	pos1 +=bcBits1;	
	i1=1; 
	
	n2= il->lenList[id2]; //reading the first value
	pos2 = il->occs[id2]; //pos in zoneoccs
	val2 = bitread (zoneoccs, pos2, absBits);
	pos2+=absBits;
	bcpos2 = bitread (zoneoccs, pos2, bcBits2);
	bcpos2+=inibcs_i2;
	pos2 +=bcBits2;		
	i2=1;

		
	SAMPLEPERIOD(sampleP1,sampleK,n1);
	SAMPLEPERIOD(sampleP2,sampleK,n2);

	uint *list = (uint *) malloc ( n1 * sizeof(uint));
	uint matches=0;

	do {
        /***/
		if (val1 == val2) {
			list[matches++] = val1;

			//next for id1
			{
				if (! (i1%sampleP1)) {	//an abs sample
					val1 = bitread (zoneoccs, pos1, absBits);
					pos1+=absBits;
					bcpos1 = bitread (zoneoccs, pos1, bcBits1);
					bcpos1+=inibcs_i1;
					pos1 +=bcBits1;			
				}
				else { //decoding from bcs
					BC_TO_DEC(bcs, bcpos1, num);
					val1 +=num;
				}		
				i1++;
			}
			
			//next for id2
			{
				if (! (i2%sampleP2)) {	//an abs sample
					val2 = bitread (zoneoccs, pos2, absBits);
					pos2+=absBits;
					bcpos2 = bitread (zoneoccs, pos2, bcBits2);
					bcpos2 +=inibcs_i2;					
					pos2 +=bcBits2;			
				}
				else { //decoding from bcs
					BC_TO_DEC(bcs, bcpos2, num);
					val2 +=num;
				}		
				i2++;
			}	
		}

        /***/
		else if (val1 < val2) {
			//moves i1 
			while ((i1<n1) && (val1 < val2)) {
				if (! (i1%sampleP1)) {	//an abs sample
					val1 = bitread (zoneoccs, pos1, absBits);
					pos1+=absBits;
					bcpos1 = bitread (zoneoccs, pos1, bcBits1);
					bcpos1+=inibcs_i1;
					pos1 +=bcBits1;			
				}
				else { //decoding from bcs
					BC_TO_DEC(bcs, bcpos1, num);
					val1 +=num;
				}					
				i1++;
			}	
		}
        /***/
		else {//if (val1 > val2) {
			//moves i2 
			while ((i2<n2) && (val2 < val1)) {
				if (! (i2%sampleP2)) {	//an abs sample
					val2 = bitread (zoneoccs, pos2, absBits);
					pos2+=absBits;
					bcpos2 = bitread (zoneoccs, pos2, bcBits2);
					bcpos2+=inibcs_i2;
					pos2 +=bcBits2;			
				}
				else { //decoding from bcs
					BC_TO_DEC(bcs, bcpos2, num);
					val2 +=num;
				}	
				i2++;				
			}	
		}				
	}
	while ((i1 < n1) && (i2<n2));

	//After merge-loop, but an occurrence could still occurr
	//if val1==val2 or 
	//if i_x==n_x, but i_y<n_y  and val_y < val_x ==> we should move on list_y while (i_y<n_y)&& (val_y<val_x)
	if (i1 < n1) { // posting1-not ended.
		while ((i1<n1) && (val1 < val2)) {
			if (! (i1%sampleP1)) {	//an abs sample
				val1 = bitread (zoneoccs, pos1, absBits);
				pos1+=absBits;
				bcpos1 = bitread (zoneoccs, pos1, bcBits1);
				bcpos1+=inibcs_i1;
				pos1 +=bcBits1;			
			}
			else { //decoding from bcs
				BC_TO_DEC(bcs, bcpos1, num);
				val1 +=num;
			}					
			i1++;
		}			
	}
	else if (i2 < n2) { // posting1-not ended.
		while ((i2<n2) && (val2 < val1)) {
			if (! (i2%sampleP2)) {	//an abs sample
				val2 = bitread (zoneoccs, pos2, absBits);
				pos2+=absBits;
				bcpos2 = bitread (zoneoccs, pos2, bcBits2);
				bcpos2+=inibcs_i2;
				pos2 +=bcBits2;			
			}
			else { //decoding from bcs
				BC_TO_DEC(bcs, bcpos2, num);
				val2 +=num;
			}	
			i2++;				
		}				
	}

	//finally checking if the last 2-values are identical or not
	if (val1 == val2) {
		list[matches++] = val1;
	}

	*occs = list;
	*noccs= matches;
	return 0;	
}
	    

	/* Intersects "nids" bitmap-represented posting lists whose ids are given by ids[].
	    Returns *occs (the result), noccs (number of elements)
	    Memory for *occs is allocated, and must be freed by the caller 
	    note: nids >=2
	*/

int mergeNBitmap (void *ail, uint *ids, register uint nids,  uint *noccs, uint **occs){
	//fprintf(stderr,"\n Call to mergeNBitmap");
	t_il *il=(t_il *) ail;	
	
	register uint nints = il->ints_bitmaps_aligned;
	register uint i,j; 

	uint *bitmapAND = (uint *) malloc (nints * sizeof(uint));  //uints for the resulting bitmap.
	bitmapAND[nints-1] = 0000;

	//intesects the first 2 bitmaps (bit-parallel)
	{
		uint *bitmap1 = (uint *) il->zoneBitmaps + il->occs[ids[0]] * il->ints_bitmaps_aligned;
		uint *bitmap2 = (uint *) il->zoneBitmaps + il->occs[ids[1]] * il->ints_bitmaps_aligned;
		for (i=0;i< nints; i++) {
			bitmapAND[i] = bitmap1[i] & bitmap2[i];			
		}
	}
	
	//intersects the remainder (nids-2) bitmaps.
	{
		uint *bitmap;
		for (j=2;j<nids;j++) {
			bitmap = (uint *) il->zoneBitmaps + il->occs[ids[j]] * il->ints_bitmaps_aligned;
			for (i=0;i< nints; i++) {
				bitmapAND[i] &= bitmap[i];
			}
		}
	}
	
	//generates the resulting list of integers.
	{
		register uint counter =0;
			//reserves memory for "at most" the smallest? list.
		uint *resultList = (uint *) malloc (sizeof(uint) * il->lenList[ids[0]]); 
		
		j=il->bits_bitmaps;
		
		for (i=0;i<j;i++) {
			if (bitget(bitmapAND, i)) 
				resultList[counter++]=i;
		}
		
	/*	if (!counter){                     //intersect_2() and intersect_N() would free memory.
			free (resultList);
			resultList=NULL;
		}
	*/
		*occs = resultList;
		*noccs= counter;
	}
		
	free(bitmapAND);
	return 0;
}




	/*  Intersects n posting-lists using set-versus-set
	    Returns *occs (the result), noccs (number of elements)
	    Memory for *occs is allocated, and must be freed by the caller */

int svsn (void *ail, uint *ids, uint nids, uint *noccs, uint **occs){
	//fprintf(stderr,"\n Call to svsn hybrid: bitmap + bytecodes");	
	t_il *il = (t_il *) ail;
	
	if ( il->defs.defaultFsearch == NULL) {
		return 42;	
	}

	//	{uint i;
	//	fprintf(stderr,"\n Call to svsN:: ");
	//	for (i=0;i<nids;i++) fprintf(stderr,"[%d]",ids[i]);
	//	}

/*
		{uint i;
		fprintf(stderr,"\n Call to svsN:: ");
		for (i=0;i<nids;i++) fprintf(stderr,"[%d]",ids[i]);
		}
*/

	// initializing ptr to function "fsearch"
	int (*defaultFsearch) (void *ail, uint id, uint value) = il->defs.defaultFsearch;	

	//sorting ids ==> lists are sorted by frequency !!
	register uint i, numbytecodes, numbitmaps;
	tsort *idss = (tsort *) malloc (sizeof(tsort) * nids);
	uint *idsBitmaps = (uint *) malloc (sizeof(uint) * nids);

	numbytecodes=0; numbitmaps=0;
	for (i=0;i<nids;i++) {
		if (!shouldUseBitmap(il,ids[i])){
			idss[numbytecodes].value=ids[i]; 
		        idss[numbytecodes].p = i; // GZL
			idss[numbytecodes].key = il->lenList[ids[i]];
			numbytecodes++;
		}
		else {
			//numbitmaps++;
{ fprintf(stderr,"ERROR GZL\n"); exit(1); } // GZL
			idsBitmaps[numbitmaps++]=ids[i]; 
			//idss[nids-numbitmaps].key = il->lenList[ids[i]];	//not needed		
		}
	}

/*
		fprintf(stderr,"\n UNSORTED ?? ==> \n");
		for (i=0;i<numbytecodes;i++) {
			fprintf(stderr,"\t[%d], freq=[%d], useBitmap vale %d\n",idss[i].value, idss[i].key, shouldUseBitmap(il,idss[i].value));
		}
		for (i=0;i<numbitmaps;i++) {
			fprintf(stderr,"\t[%d], useBitmap vale %d\n",idsBitmaps[i],shouldUseBitmap(il,idsBitmaps[i]));
		}
*/

	/** Now: idss[0,numbytecodes) keeps ids of terms using bytecodes.
	 *       idsBitmaps[0,numbitmaps) are ids of terms with bitmaps.
	 * */

	//	fprintf(stderr,"\n UNSORTED ?? ==> ");
	//	for (i=0;i<nids;i++) {
	//		fprintf(stderr,"[%d]",idss[i].key);
	//	}

	//simplesortAsc(idss,nids);  
//	if (numbytecodes >1)
//		simplesortAsc(idss,numbytecodes); 
	
/*
		fprintf(stderr,"\n SORTED ?? ==> \n");
		for (i=0;i<numbytecodes;i++) {
			fprintf(stderr,"\t[%d], freq=[%d], useBitmap vale %d\n",idss[i].value, idss[i].key, shouldUseBitmap(il,idss[i].value));
		}
		for (i=0;i<numbitmaps;i++) {
			fprintf(stderr,"\t[%d], useBitmap vale %d\n",idsBitmaps[i],shouldUseBitmap(il,idsBitmaps[i]));
		}
*/	

	
	//	fprintf(stderr,"\nSORTED ?? ==> ");
	//	for (i=0;i<nids;i++) {
	//		fprintf(stderr,"[%d]",idss[i].key);
	//	}
			
	if (numbytecodes == 0) { //intersecting only bitmaps --> mergeNbitmap
		int err =  mergeNBitmap (ail, idsBitmaps, numbitmaps,  noccs, occs);
		free(idss);
		free(idsBitmaps);
		return err;
	}
	else {  //bytecodes (& could also be bitmaps) --> extract 1st + svsN.
	
		int result;
		register uint j, n, matches;
		uint *firstlist;

		if (numbytecodes >1)	//sorting the bytecodes by lenList[]
			simplesortAsc(idss,numbytecodes); 

		SVNextractList_il (ail, idss[0].value, &firstlist, noccs, idss[0].p); //reusing *noccs. // GZL
		n=*noccs;

		matches =n;	
		for (i=1; i<numbytecodes; i++){ // the "nids-1" remaining posting-lists.
			matches =0;	

			{ //calculates sample period for the next list
			register uint lenlist  = il->lenList[idss[i].value];			
			uint sampleK = il->sampleK;
			register uint sampleP;
			SAMPLEPERIOD(sampleP,sampleK,lenlist);
			il->sampleP = sampleP;
			il->i=0;	
			il->j=0;
			}		
									
			for (j=0; j<n;j++) {
				result = (*defaultFsearch) (ail, idss[i].value, firstlist[j] +idss[i].p); //call to fsearch; // GZL
				//fprintf(stderr,"\n  Search for value %d -> result = %d",firstlist[j], result);
				if (result) {
					firstlist[matches++] = firstlist[j];
				}
			}
			if (!matches) break;  //search ends.
			n=matches;
		}
		
		if (matches) { //svs on the bitmaps.
			defaultFsearch=fsearchBitmap;
			for (i=0; i<numbitmaps; i++){ // the "numbitmaps" remaining posting-lists.
				matches =0;	
						
				for (j=0; j<n;j++) {
					result = (*defaultFsearch) (ail, idsBitmaps[i], firstlist[j]); //call to fsearch;
					//fprintf(stderr,"\n  Search for value %d -> result = %d",firstlist[j], result);
					if (result) {
						firstlist[matches++] = firstlist[j];
					}
				}
				if (!matches) break;  //search ends.
				n=matches;
			}			
		}

		
		*occs= firstlist;
		*noccs = matches;					
	}
	free(idss);
	free(idsBitmaps);
	return 0;	
}





/****************************************************************************************/


	/*  Intersects n posting-lists using set-versus-set
	    Returns *occs (the result), noccs (number of elements)
	    Memory for *occs is allocated, and must be freed by the caller */

int svsn_sigir_antes_bitmap (void *ail, uint *ids, uint nids, uint *noccs, uint **occs){
	//fprintf(stderr,"\n Call to svsn");	
	t_il *il = (t_il *) ail;
	
	if ( il->defs.defaultFsearch == NULL) {
		return 42;	
	}

	//	{uint i;
	//	fprintf(stderr,"\n Call to svsN:: ");
	//	for (i=0;i<nids;i++) fprintf(stderr,"[%d]",ids[i]);
	//	}

	// initializing ptr to function "fsearch"
	int (*defaultFsearch) (void *ail, uint id, uint value) = il->defs.defaultFsearch;	

	//sorting ids ==> lists are sorted by frequency !!
	register uint i;
	tsort *idss = (tsort *)malloc (sizeof(tsort) * nids);
	for (i=0;i<nids;i++) {idss[i].value=ids[i]; idss[i].key = il->lenList[ids[i]];}


	//	fprintf(stderr,"\n UNSORTED ?? ==> ");
	//	for (i=0;i<nids;i++) {
	//		fprintf(stderr,"[%d]",idss[i].key);
	//	}

	simplesortAsc(idss,nids);  // ==> change to sort by lenList !!
	
	//	fprintf(stderr,"\nSORTED ?? ==> ");
	//	for (i=0;i<nids;i++) {
	//		fprintf(stderr,"[%d]",idss[i].key);
	//	}
			
	{	int result;
		register uint j, n, matches;
		uint *firstlist;

		extractList_il (ail, idss[0].value, &firstlist, noccs); //reusing *noccs.
		n=*noccs;

		for (i=1; i<nids;i++){ // the "nids-1" remaining posting-lists.
			matches =0;	

			{ //calculates sample period for the next list
			register uint lenlist  = il->lenList[idss[i].value];			
			uint sampleK = il->sampleK;
			register uint sampleP;
			SAMPLEPERIOD(sampleP,sampleK,lenlist);
			il->sampleP = sampleP;
			il->i=0;	
			il->j=0;
			}		
									
			for (j=0; j<n;j++) {
				result = (*defaultFsearch) (ail, idss[i].value, firstlist[j]); //call to fsearch;
				//fprintf(stderr,"\n  Search for value %d -> result = %d",firstlist[j], result);
				if (result) {
					firstlist[matches++] = firstlist[j];
				}
			}
			if (!matches) break;  //search ends.
			n=matches;
		}

		
		*occs= firstlist;
		*noccs = matches;					
	}
	free(idss);
	return 0;	
}

	    
	    
	    
	    
	    
//int svsn_old (void *ail, uint *ids, uint nids, uint *noccs, uint **occs){
//	//fprintf(stderr,"\n Call to svsn");	
//	t_il *il = (t_il *) ail;
//	
//	if ( il->defs.defaultFsearch == NULL) {
//		return 42;	
//	}
//
//	{uint i;
//	fprintf(stderr,"\n Call to svsN:: ");
//	for (i=0;i<nids;i++) fprintf(stderr,"[%d]",ids[i]);
//	}
//
//	// initializing ptr to function "fsearch"
//	int (*defaultFsearch) (void *ail, uint id, uint value) = il->defs.defaultFsearch;	
//
//	//sorting ids ==> lists are sorted by frequency !!
//	register uint i;
//	uint *idss = (uint *)malloc (sizeof(uint) * nids);
//	for (i=0;i<nids;i++) idss[i]=ids[i];
//	simplesortUint(idss,nids);  // ==> change to sort by lenList !!
//			
//	{	int result;
//		register uint j, n, matches;
//		uint *firstlist;
//
//		extractList_il (ail, idss[0], &firstlist, noccs); //reusing *noccs.
//		n=*noccs;
//
//		for (i=1; i<nids;i++){ // the "nids-1" remaining posting-lists.
//			matches =0;				
//			for (j=0; j<n;j++) {
//				result = (*defaultFsearch) (ail, idss[i], firstlist[j]); //call to fsearch;
//				//fprintf(stderr,"\n  Search for value %d -> result = %d",firstlist[j], result);
//				if (result) {
//					firstlist[matches++] = firstlist[j];
//				}
//			}
//		}
//		//if (matches == 0) {free(firstlist); firstlist = NULL;}
//		
//		*occs= firstlist;
//		*noccs = matches;					
//	}
//	free(idss);
//	return 0;	
//}
