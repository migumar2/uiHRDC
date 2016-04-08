#include "ilsandersbc.h"


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
		// *[lenPost1][post11][post12]...[post1k碷
		// *[lenPost2][post21][post22]...[post2k创]
		// * ... 
		// *[lenPostN][postN1][postN2]...[postNk创碷		
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
	//static char text[20] = "samplerate=8";
	static char text[200];
	sprintf(text, "samplerate=%d; lenBitmapDiv=%d", DEFAULTB,DEFAULT_LEN_BITMAP_DIV);
	return text;	
}	


	/* checks if an id should use either bitmap or bytecodes: valid during build_il() */
int shouldUseBitmap(t_il *il, uint id) {  /* 0 <= id < il->nlists */
	if (il->lenList[id] > il->lenBitmapThreshold)
		return 1;
	return 0;
}

    /* Creates a structure for the management of inverted lists from a
      given set of posting lists. 
      Array source has the following format...
      [Nwords=N][maxPostvalue][lenPost1][post11][post12]...[post1k碷
                              [lenPost2][post21][post22]...[post2k创]
                               ... 
                              [lenPostN][postN1][postN2]...[postNk创碷
      il is an opaque data type. Any build option must be passed in string 
      build_options, whose syntax depends on the index. The index must 
      always work with some default parameters if build_options is NULL. 
      The returned structure is ready to be queried. */

int build_il (uint *source, uint length, char *build_options, void **ail) {
	uint B=DEFAULTB; // --> see ilists/ildefconfig.h
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
	    B=atoi(parameters[j+1]);
	    
	    j++;

	  }else if  ((strcmp(parameters[j], "filename") == 0 ) && (j < num_parameters-1) ) {
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
	
	printf("\n parameters of method: B=%ld, lenBitmapDiv= %d, filename = %s\n",B, lenBitmapDiv, filename);	

	il->B = B;

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

fprintf(stderr,"\n 1.0.... B= %d",B);fflush(stderr);				
	
		parseAllPostingList(&(il->nlists), &lenList, &occList, &maxPostValue, source,length);				
		free(source);

fprintf(stderr,"\n 2.0....\n");fflush(stderr);
	
			
		il->lenList = lenList;		
		il->maxPostingValue = maxPostValue ;
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
		il->B = B;
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

					
			uint *numBucketsperID = (uint *) malloc (sizeof(uint)* (il->nlists));
		
			/** 5.a ** computes the number of Absolute samples to keep. Depends on B. */
fprintf(stderr,"\n 5.a....");fflush(stderr);
			
			uint numBuckets =0;
			{  uint nwords = il->nlists;
				uint sampleP, k, maxValue;
				maxValue = il->maxPostingValue;
				
				for (i=0;i<nwords; i++){
					if (! shouldUseBitmap(il,i) ) { //** spire **/					
						SAMPLEPERIOD(sampleP,k,B,il->lenList[i], maxValue);  //computes sampleP.					 
						// numBuckets += ( (il->lenList[i] + sampleP -1) /sampleP);
						// numBucketsperID[i]= ( (il->lenList[i] + sampleP -1) /sampleP);
						{
						uint tmpNum;
						NUMBUCKETS (tmpNum,maxValue,k);
						numBuckets += tmpNum;
						numBucketsperID[i]= tmpNum;
						}

/*
						printf("\n tmpnum = %u",numBucketsperID[i]);
						if (!numBucketsperID[i]) {
							printf("\n tmpnum = %u",numBucketsperID[i]);
										{
											uint tmpNum;
											NUMBUCKETS (tmpNum,maxValue,k);
											numBuckets += tmpNum;
											numBucketsperID[i]= tmpNum;
											}
						}
*/
					}
					else numBucketsperID[i]= 0;  //** spire **/
					 //fprintf(stderr,"\n len = %lu, B=%lu, sampleP = %lu", il->lenList[i], B, sampleP);fflush(stderr);	
				}
				fprintf(stderr,"\n Number of buckets (1st level) to store is = %d",numBuckets);	
				fprintf(stderr,"\n numBucketsperID[9] = %d",numBucketsperID[9]);
				
	//		
	//			 uint n,U,numBucks;
	//
	//			 n=8000; U=100000; B=8;
	//			 SAMPLEPERIOD(sampleP,k,B,n,U);  //computes sampleP.
	//			 numBucks = ( (U + sampleP -1) / sampleP);					 
	//			 fprintf(stderr,"\n len = %lu, B=%lu, sampleP = %lu, k=%d, numBucks= %d", n,B,sampleP, k,numBucks);fflush(stderr);	
	//
	//			 n=256; U=100000; B=8;
	//			 SAMPLEPERIOD(sampleP,k,B,n,U);  //computes sampleP.
	//			 numBucks = ( (U + sampleP -1) / sampleP);					 
	//			 fprintf(stderr,"\n len = %lu, B=%lu, sampleP = %lu, k=%d, numBucks= %d", n,B,sampleP, k,numBucks);fflush(stderr);	
	//
	//			 n=25; U=100000; B=8;
	//			 SAMPLEPERIOD(sampleP,k,B,n,U);  //computes sampleP.
	//			 numBucks = ( (U + sampleP -1) / sampleP);					 
	//			 fprintf(stderr,"\n len = %lu, B=%lu, sampleP = %lu, k=%d, numBucks= %d", n,B,sampleP, k,numBucks);fflush(stderr);	
	//
	//			 n=20; U=100000; B=8;
	//			 SAMPLEPERIOD(sampleP,k,B,n,U);  //computes sampleP.
	//			 numBucks = ( (U + sampleP -1) / sampleP);					 
	//			 fprintf(stderr,"\n len = %lu, B=%lu, sampleP = %lu, k=%d, numBucks= %d", n,B,sampleP, k,numBucks);fflush(stderr);	
	//
	//			 n=15; U=100000; B=8;
	//			 SAMPLEPERIOD(sampleP,k,B,n,U);  //computes sampleP.
	//			 numBucks = ( (U + sampleP -1) / sampleP);					 
	//			 fprintf(stderr,"\n len = %lu, B=%lu, sampleP = %lu, k=%d, numBucks= %d", n,B,sampleP, k,numBucks);fflush(stderr);	
	//
	//			 n=10; U=100000; B=8;
	//			 SAMPLEPERIOD(sampleP,k,B,n,U);  //computes sampleP.
	//			 numBucks = ( (U + sampleP -1) / sampleP);					 
	//			 fprintf(stderr,"\n len = %lu, B=%lu, sampleP = %lu, k=%d, numBucks= %d", n,B,sampleP, k,numBucks);fflush(stderr);	
	//
	//			 n=3; U=100000; B=8;
	//			 SAMPLEPERIOD(sampleP,k,B,n,U);  //computes sampleP.
	//			 numBucks = ( (U + sampleP -1) / sampleP);					 
	//			 fprintf(stderr,"\n len = %lu, B=%lu, sampleP = %lu, k=%d, numBucks= %d", n,B,sampleP, k,numBucks);fflush(stderr);	
	//			 					
	//			 n=1; U=100000; B=8;
	//			 SAMPLEPERIOD(sampleP,k,B,n,U);  //computes sampleP.
	//			 numBucks = ( (U + sampleP -1) / sampleP);					 
	//			 fprintf(stderr,"\n len = %lu, B=%lu, sampleP = %lu, k=%d, numBucks= %d", n,B,sampleP, k,numBucks);fflush(stderr);	

			}			

fprintf(stderr,"\n 5.b**** %u numbuckets....",numBuckets);fflush(stderr);		
			uint *zoneOccsTmp = (uint *) malloc (sizeof(uint) * (numBuckets +1));

fprintf(stderr,"\n 5.b....");fflush(stderr);
			
			/** 5.b **
				Computes the size of bc vector by simulating encoding process of gaps.
				Sets also bitmaps for those terms indexed with a bitmap.
				Sets also pointers in bcsptr[] && set pointers into zoneoccsTmp.
				Computes and sets the values for "occs[]".
			*/
						
			uint bcssize=0;
			{
				uint nwords = il->nlists;
				ulong id,j;
				ulong prevvalue,val, gap, numbytes;
				ulong len, sampleP,k, maxValue ;
				maxValue = il->maxPostingValue;

				for (j=0; j<numBuckets;j++) zoneOccsTmp[j]=MAXUINT;	
														
				uint counterAbsPtrs = 0;
				uint counterBCcodes = 0; uint counterBitmaps = 0;	
				
				register int currBucket;						
				for (id=0; id<nwords; id++) {
					len= il->lenList[id];
					
					if (! shouldUseBitmap(il,id) ) { //** spire **/	
						il->bcsptr[id] =bcssize;					
						il->occs[id]=counterAbsPtrs;
					
						SAMPLEPERIOD(sampleP,k,B,len,maxValue);
						currBucket = -1;
						for (j=0; j<len; j++){
							val = occList[id][j];
							if ((val >>k) != currBucket) { //change of bucket --> 1st value of bucket.
								prevvalue = val;
								currBucket = (int)(val >>k);
								//fprintf(stderr,"\n offset in zoneOccsTmp = %lu",counterAbsPtrs + currBucket);fflush(stderr);
								zoneOccsTmp[counterAbsPtrs + currBucket] = bcssize - il->bcsptr[id];
								gap = (val & ((1L<<k) - 1L));  //val = val mod 2^k
								SIZE_DEC_TO_BC(gap,numbytes);
								bcssize += numbytes;								
							}					
							else {  //in the same bucket of the previous value.
								gap = val - prevvalue;
								prevvalue = val;
								SIZE_DEC_TO_BC(gap,numbytes);
								bcssize += numbytes;
							}
							counterBCcodes++;
						}
						
						
						{
						//sets ptrs backwards for non-used buckets.
						//needed for non-used buckets $i$ in the 1st level ==> so t[i+1]-t[i] == 0 and no
						//processing is done in the bottom level.
							uint ptr = bcssize - il->bcsptr[id];
							uint l,r;
							r = counterAbsPtrs + numBucketsperID[id]-1;
							l = counterAbsPtrs;						
							
							for (j=r; j>l; j--) {
															
								if (MAXUINT == zoneOccsTmp[j]) {
									zoneOccsTmp[j]= ptr;
								}
								else {
									ptr=zoneOccsTmp[j];
								}
							}
							if (MAXUINT == zoneOccsTmp[l]) {
								zoneOccsTmp[l]= ptr;
							}
						}
						
						counterAbsPtrs+= numBucketsperID[id];  //numBucks = ( (U + sampleP -1) / sampleP);	
					}
					
					else { //using a bitmap...
						uint iniPosBitmap = counterBitmaps * il->ints_bitmaps_aligned * W;
						il->occs[id] = counterBitmaps;
						for (j=0;j<len;j++)  
							bitset(il->zoneBitmaps, (iniPosBitmap + occList[id][j]));							
						counterBitmaps++;
					}
					
				}
				il->bcsptr[id] = bcssize;	 //last one !!				
				
				//at this point:
				// zoneOccsTmp = pointers inside the buckets for all buckets of all terms.
				// bcsptr is set to point to the beginning of bcs for each id.
				// counterAbsPtrs == numBuckets => sum of buckets for all terms.
				// il->occs = number of buckets until the beginning of the buckets of the i-th term.
				
				fprintf(stderr,"\n\n Active bits for %d bitmaps where already set.",counterBitmaps);						
				fprintf(stderr,"\n Number of ptrs to store in the first level is = %d",counterAbsPtrs);						
				fprintf(stderr,"\n Number of gaps to store is = %d",counterBCcodes);	
				fprintf(stderr,"\n Number of buckets for all words is = %d",numBuckets);						
				fprintf(stderr,"\n Size of bcs Vector should be  %d bytes",bcssize);			
			

				//fprintf(stderr,"\n [SHOULD BE IDENTICAL]: number of words = %d, and sum of occurs is %d",wcsa->Nwords, counterAbsSamples + counterBCcodes);	
				
				
//				//SHOW THE POINTERS THAT ARE STORED IN THE BUCKETS (NON NULL BUCKETS)
//				k=0;
//				for (id=0;id<nwords;id++) {
//					len= numBucketsperID[id];
//					fprintf(stderr,"\n =============numbuckets = %d=====\n id= %d -->",numBucketsperID[id],id);
//					for (j=0;j<len;j++) {
//						val = zoneOccsTmp[k++];			
//						fprintf(stderr,"[%d]",val);
//						//if ( (j!=0) && (val==0)) return 0;
//					}
//				}	
//
			}
			
			
		il->bcsSize = bcssize;
		il->bcs = (byte *) malloc (sizeof(byte) *il->bcsSize);	
		if ( il->bcs != NULL) 							
			fprintf(stderr,"\n bcs bytes allocated = %d bytes ",il->bcsSize);fflush(stderr);

fprintf(stderr,"\n 5.c....");fflush(stderr);
	
		/** 5.c ** encodes gaps into bcs */
		
			{
				uint nwords = il->nlists;
				byte *bcs = il->bcs;
				ulong id,j;
				ulong prevvalue,val, gap, numbytes;
				ulong len, sampleP,k, maxValue ;
				uint bcpos=0;
				maxValue = il->maxPostingValue;				
				register int currBucket;		
								
				for (id=0; id<nwords; id++) {
					len= il->lenList[id];
					if ( ! shouldUseBitmap(il,id) ) {
						//fprintf(stderr,"\n bcpos = %d of %d bytes ",bcpos,bcssize);fflush(stderr);
						SAMPLEPERIOD(sampleP,k,B,len,maxValue);
						currBucket = -1;
						for (j=0; j<len; j++){
							val = occList[id][j];
							if ((val >>k) != currBucket) { //change of bucket --> 1st value.
								prevvalue = val;
								currBucket = (val >>k);
								gap = (val & ((1L<<k) - 1L));  //val = val mod 2^k
								//SIZE_DEC_TO_BC(gap,numbytes);
								//fprintf(stderr,"\n *** bcpos = %d and gap = %d from orig val= %d", bcpos, gap,val);fflush(stderr);							
								DEC_TO_BC(bcs, bcpos, gap);  //bcpos is increased.
							}					
							else {  //in the same bucket of the previous value.
								gap = val - prevvalue;
								prevvalue = val;
								//SIZE_DEC_TO_BC(gap,numbytes);
								//fprintf(stderr,"\n ##### bcpos = %d and gap = %d from orig val = %d ", bcpos, gap,val);fflush(stderr);
								DEC_TO_BC(bcs, bcpos, gap);  //bcpos is increased.
								//bcpos +=numbytes;

							}
						}
						//if (j>0) exit(0);
					}
				}
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
				
			}

		

		il->numBuckets   = numBuckets;
		
		//computes zoneOccsSize;
		{
			uint totalSize=0;
			uint i,obits;
			uint nwords = il->nlists;
			for (i=0;i<nwords;i++) {
				if ( ! shouldUseBitmap(il,i) ) {
					OBITS(obits, il->bcsptr, i);
					//fprintf(stderr,"\n obits = %d ", obits );
					totalSize += (numBucketsperID[i] * obits);
				}
			}
			il->zoneOccsSize = totalSize;
		}	
		fprintf(stderr,"\n zoneoccssize = %d ", il->zoneOccsSize );	
		
		il->zoneOccs = (uint *) malloc (sizeof(uint) * (il->zoneOccsSize + W -1)/W );
		il->zoneOccs[((il->zoneOccsSize +W-1)/W)-1] =0000; //so valgrind is shut up		

	fprintf(stderr,"\n 6.0....");fflush(stderr);

	
		/** 6 ** encode zoneoccurssTmp into zoneoccurss using a "kbit-fashion" 
		         sets also offsets in il->occs[id] \forall id;            */
			{	uint i,j,value;
				uint nwords = il->nlists;
				uint k=0;
				uint obits;	
				uint pos=0;			
				for (i=0;i<nwords;i++) {
					if ( ! shouldUseBitmap(il,i) ){



						OBITS(obits, il->bcsptr, i);
						il->occs[i] = pos;     // setting offsets in il->occs vector
						for (j=0;j<numBucketsperID[i];j++) {  //enconding directory of [ptrs_to buckets]
							value = zoneOccsTmp[k++];
							bitwrite (il->zoneOccs, pos, obits, value);	pos += obits;
							//fprintf(stderr,"\n pos %d ", pos );						
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
				extractList_il (*ail, id, &list, &len);		
				for (i=0;i<len;i++){
					if (list[i] != occList[id][i]) {						
						fprintf(stderr,"\n **decoding of lists failed for id = %d: j=%d DIFFERENT %d!=%d!!",id,i,list[i],occList[id][i]);						
						
						
						{   

						fprintf(stderr,"\n **  len of list = %d, when i= %d, prev = %d, %d", len,i, list[i-1], occList[id][i-1]);

							uint lenlist  = il->lenList[id];
							uint B = il->B;
							uint maxValue= il->maxPostingValue;
							uint sampleP,k;	

							register uint obits;  //size of pointers in 1st level for term "id".
							OBITS(obits, il->bcsptr, id);	

							SAMPLEPERIOD(sampleP, k, B, lenlist, maxValue);	

							register uint numBuckets;
							NUMBUCKETS (numBuckets,maxValue,k);
														
							fprintf(stderr,"\n  ### sampleP = %d, k= %d, B = %d, lenlist = %d, numbuckets = %d, obits = %d",sampleP,k,B,lenlist,numBuckets,obits);
							
							int z,l;
							z= max ( ((int)i)-5, 0);
							fprintf(stderr,"\n");
							for (l=0;l<10;l++) {
								fprintf(stderr,"[%d]",list[z++]);
							}
							exit(0);
							//fprintf(stderr," ** sampleP= %d, k= %d, lenlist= %d, maxVal = %d", sampleP,k,lenlist,maxValue);						
						}
						
						
						//fprintf(stderr,"\n **decoding of lists failed for id = %d: j=%d DIFFERENT %d!=%d!!",id,i,list[i],occList[id][i]);						
//						exit(0);
					}
					else {
						//fprintf(stderr,"\n \t==decoding of lists success for id = %d: j=%d IDENTICAL %d!=%d!!",id,i,list[i],occList[id][i]);						
						
					}
				} 			
				free(list);				
				//exit(0);
			}			
		fprintf(stderr,"\n ! Decoding the list of occurrences worked fine (the same as previous values) ");fflush(stderr);
		}
		
		
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
		free(numBucketsperID);
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
	/** Saving the postings lists */
	{
		//fprintf(stderr,"\n saving size ucompressed = %lu!!!!",il->sizeUncompressed);
		write(file, &(il->sizeUncompressed), sizeof(uint));		
		write(file, &(il->B), sizeof(uint));		
		write(file, &(il->maxPostingValue), sizeof(uint));		
		write(file, &(il->zoneOccsSize), sizeof(uint)); //in # bits
		write(file, &(il->numBuckets), sizeof(uint));		
		write(file, &(il->bcsSize), sizeof(uint));		

		write(file, il->occs, il->nlists * sizeof(uint));		
		write(file, il->zoneOccs, ((il->zoneOccsSize +W-1)/W) * sizeof(uint)); //zoneOccs kbitVec[bucketSize]
		write(file, il->bcs, il->bcsSize * sizeof(byte));					
		write(file, il->bcsptr, (il->nlists +1)* sizeof(uint));				
	}
	
	fprintf(stderr,"\n ########## il->sizeUncompressed = %d ",il->sizeUncompressed   );
	fprintf(stderr,"\n ########## il->B = %d ",il->B   );
	fprintf(stderr,"\n ########## il->maxPostingValue = %d ",il->maxPostingValue   );
	fprintf(stderr,"\n ########## il->zoneOccsSize = %d bits = %d bytes",il->zoneOccsSize, ((il->zoneOccsSize + W -1)/W * sizeof(uint)) );
	fprintf(stderr,"\n ########## il->numBuckets = %d ", il->numBuckets  );
	fprintf(stderr,"\n ########## il->bcsSize = %d ", il->bcsSize  );
	fprintf(stderr,"\n ##########     zoneBitmapsSize = %d bytes", (il->numBitmaps * il->ints_bitmaps_aligned) *  sizeof(uint) );
	fprintf(stderr,"\n ########## il->numBitmaps = %d, bytesPerbitmap = %d ", il->numBitmaps, (il->ints_bitmaps_aligned)* sizeof(uint)   );
	

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


	/* loading the posting lists */
	{		
		//some integers
		read(file, &(il->sizeUncompressed), sizeof(uint));					
		read(file, &(il->B), sizeof(uint));	
		read(file, &(il->maxPostingValue), sizeof(uint));	
		read(file, &(il->zoneOccsSize), sizeof(uint)); //in # bits
		read(file, &(il->numBuckets), sizeof(uint));		
		read(file, &(il->bcsSize), sizeof(uint));		
		
		//vector occs.
		il->occs = (uint *) malloc (il->nlists * sizeof(uint) );		
		read(file, il->occs, il->nlists * sizeof(uint)); //wcsa->nwords was loading in "loadVocabulary"

		//vector zoneOccs (a bistring holding pointers into the bottom layer
		il->zoneOccs = (uint *) malloc (((il->zoneOccsSize +W-1)/W) * sizeof(uint));
		il->zoneOccs[((il->zoneOccsSize +W-1)/W)-1] =0000; //so valgrind is shut up		
		read(file, il->zoneOccs, ((il->zoneOccsSize +W-1)/W) * sizeof(uint)); 

		//vector bcs.
		il->bcs = (byte *) malloc (il->bcsSize * sizeof(byte));		
		read(file, il->bcs, il->bcsSize * sizeof(byte));		
		
		//vectir bcsptr.
		il->bcsptr = (uint *) malloc (sizeof(uint) * (il->nlists +1));
		read(file, il->bcsptr, (il->nlists +1)* sizeof(uint));		
		
	}		
	
	fprintf(stderr,"\n ########## il->sizeUncompressed = %d ",il->sizeUncompressed   );
	fprintf(stderr,"\n ########## il->B = %d ",il->B   );
	fprintf(stderr,"\n ########## il->maxPostingValue = %d ",il->maxPostingValue   );
	fprintf(stderr,"\n ########## il->zoneOccsSize = %d ", il->zoneOccsSize  );
	fprintf(stderr,"\n ########## il->numBuckets = %d ", il->numBuckets  );
	fprintf(stderr,"\n ########## il->bcsSize = %d ", il->bcsSize  );
		
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
	free(il->zoneOccs);
	free(il->zoneBitmaps);	
	free(il);

	return 0;
}


	/* Gives the memory occupied by index in bytes. */

//int size_il(void *ail, uint *size) {
//	t_il *il=(t_il *) ail;
//	*size=0;
//	*size += sizeof(t_il);
//	
//	*size += il->nlists * sizeof(uint) ;   //lenList vect
//	*size += il->nlists * sizeof(uint) ;   //occs vect
//	*size += (il->nlists + 1)* sizeof(uint);  //il->bcsptr 
//
//	*size += (il->zoneOccsSize + W -1)/W * sizeof(uint);
//	*size += il->bcsSize ;
//
//	return 0;	
//}



uint  sizetobits(void *ail) {
	uint i; uint maxs, bits_s;
	t_il *il=(t_il *) ail;
	uint total;
	

	// size of lenList
	maxs=0;
	for(i=0;i<il->nlists;i++)       //** calcula el m醲imo s韒bolo en symbols_new;
		maxs = max(maxs,il->lenList[i]);
	bits_s = bits(maxs);                   //** numero de bits necesarios para los valores en "symbols_new"
	total = (il->nlists * bits_s / W +1)*4;   //** valores de symbols_new codificados con "bits_sn" bits.

	// size of occs
	maxs=0;
	for(uint i=0;i<il->nlists;i++)       //** calcula el maximo simbolo en symbols_new;
		maxs = max(maxs,il->occs[i]);	
	//maxs = il->occs[il->nlists-1];
	bits_s = bits(maxs);                   //** numero de bits necesarios para los valores en "symbols_new"
	total += (il->nlists * bits_s / W +1)*4;   //** valores de symbols_new codificados con "bits_sn" bits.

	// size of bcsptr

	maxs = il->bcsptr[il->nlists];
	bits_s = bits(maxs);                   //** numero de bits necesarios para los valores en "symbols_new"
	total += (il->nlists * bits_s / W +1)*4;   //** valores de symbols_new codificados con "bits_sn" bits.

	return total;
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

	*size += (il->zoneOccsSize + W -1)/W * sizeof(uint);
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


	/* Returns the len of the id-th posting list */
	/* Returns the len of the ids-th posting list */
int lenlist(void *ail, uint id, uint *len){
	t_il *il = (t_il *) ail;
	*len = il->lenList[id];
	return 0;
}
/**********************************************************************************/
/***** Operations (searching) with the posting lists ****/
/**********************************************************************************/

    /* Extracts the id-th list, having id \in [0..maxId-1] */

int extractList_il (void *ail, uint id, uint **list, uint *len) {
	t_il *il=(t_il *) ail;	

	register ulong lenlist  = il->lenList[id];
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
	//else a list encoded with bytecodes follows

	register ulong maxValue = il->maxPostingValue;
	
	register ulong obits;  //size of pointers in 1st level for term "id".
	OBITS(obits, il->bcsptr, id);	
	
	byte *bcs  = il->bcs;
	ulong inibcs_i = il->bcsptr[id];
	ulong endbcs_i = il->bcsptr[id+1];

	uint *zoneoccs = il->zoneOccs;
	//uint lenlist  = il->lenList[id];
	//uint *alist = (uint *) malloc (sizeof(uint) * lenlist);

	ulong B = il->B;
	ulong sampleP,k;	
	SAMPLEPERIOD(sampleP, k, B, lenlist, maxValue);
	//register uint numBuckets = ( (il->lenList[id] + sampleP -1) /sampleP);
		
	register ulong numBuckets;
	NUMBUCKETS (numBuckets,maxValue,k);
	
	//fprintf(stderr,"\n sampleP = %d, k= %d, B = %d, lenlist = %d, numbuckets = %d, obits = %d",sampleP,k,B,lenlist,numBuckets,obits);
	
	register ulong e,s,bcpos,end;
	ulong pos, gap, prevvalue;
	ulong ne,ns;
	

	//looks for the last bucket...	
	pos= il->occs[id] + (obits*(numBuckets -1));
	ne = bitread (zoneoccs, pos, obits); pos-=obits;
	
	pos = il->occs[id]; //pos in zoneoccs
	ns=0; e=0;
	ulong i=0; //position inside alist[]
		
	//while (e < numBuckets) 
	while (i < lenlist) {
		for ( ; e<numBuckets; e++) {  // finds the next pointer to a non-empty bucket.
			ne = bitread (zoneoccs, pos, obits); ;
			pos+=obits;

			if (ne != ns) { s=e-1; break;}
			//if (ne != ns) { break;}
			ns=ne;	
			s=e;
			
		}
		
		if (e == numBuckets) {
			//uncompress values in the last bucket, being ns = ptr to the beginning.
			bcpos = ns + inibcs_i;
			end = endbcs_i; //decoding from bcpos to end.
			prevvalue = s*(1L<<k);
		//	fprintf(stderr,"\n numbucks= %d prevvalue = %d, s= %d, e=%d i=%d",numBuckets,prevvalue,s,e,i);
		//	fprintf(stderr,"\n inipos= %d, bcpos = %d, enpos= %d",inibcs_i, bcpos, endbcs_i);
			while (bcpos < end) {
				BC_TO_DEC(bcs, bcpos, gap);
				prevvalue +=gap;
				alist[i++]=prevvalue;	
			}	
		}
		else {
			//uncompress values in the bucket "s", being ns = ptr to the beginning, ne = ptr to end.
			bcpos = ns + inibcs_i;
			end = ne + inibcs_i; //decoding from bcpos to end.
			prevvalue = s*(1L<<k);
			//fprintf(stderr,"\n s= %d, prevvalue = %d",s, prevvalue); exit(0);
		//	fprintf(stderr,"\n ** numbucks= %d prevvalue = %d, s= %d, e=%d i=%d",numBuckets,prevvalue,s,e,i);
		//	fprintf(stderr,"\n ** inipos= %d, bcpos = %d, enpos= %d",inibcs_i, bcpos, endbcs_i);

			while (bcpos < end) {
				BC_TO_DEC(bcs, bcpos, gap);
				prevvalue +=gap;
				alist[i++]=prevvalue;	
			}	
			
			ns=ne;s=e; /**/		
			e++;
			
		}	
	}
//	fprintf(stderr,"\n len = %d of %d",i,lenlist);
		
	*list =alist;
	*len =i;		
	return 0; 			
}

int LKPextractList_il (void *ail, uint id, uint **list, uint *len, uint offs) {
	t_il *il=(t_il *) ail;	

	register ulong lenlist  = il->lenList[id];
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
				alist[lenlist++] = i-inipos-offs;
		}
		*list =alist;
		*len =lenlist;		
		return 0;				
	}		
	//else a list encoded with bytecodes follows

	register ulong maxValue = il->maxPostingValue;
	
	register ulong obits;  //size of pointers in 1st level for term "id".
	OBITS(obits, il->bcsptr, id);	
	
	byte *bcs  = il->bcs;
	ulong inibcs_i = il->bcsptr[id];
	ulong endbcs_i = il->bcsptr[id+1];

	uint *zoneoccs = il->zoneOccs;
	//uint lenlist  = il->lenList[id];
	//uint *alist = (uint *) malloc (sizeof(uint) * lenlist);

	ulong B = il->B;
	ulong sampleP,k;	
	SAMPLEPERIOD(sampleP, k, B, lenlist, maxValue);
	//register uint numBuckets = ( (il->lenList[id] + sampleP -1) /sampleP);
		
	register ulong numBuckets;
	NUMBUCKETS (numBuckets,maxValue,k);
	
	//fprintf(stderr,"\n sampleP = %d, k= %d, B = %d, lenlist = %d, numbuckets = %d, obits = %d",sampleP,k,B,lenlist,numBuckets,obits);
	
	register ulong e,s,bcpos,end;
	ulong pos, gap, prevvalue;
	ulong ne,ns;
	

	//looks for the last bucket...	
	pos= il->occs[id] + (obits*(numBuckets -1));
	ne = bitread (zoneoccs, pos, obits); pos-=obits;
	
	pos = il->occs[id]; //pos in zoneoccs
	ns=0; e=0;
	ulong i=0; //position inside alist[]
		
	//while (e < numBuckets) 
	while (i < lenlist) {
		for ( ; e<numBuckets; e++) {  // finds the next pointer to a non-empty bucket.
			ne = bitread (zoneoccs, pos, obits); ;
			pos+=obits;

			if (ne != ns) { s=e-1; break;}
			//if (ne != ns) { break;}
			ns=ne;	
			s=e;
			
		}
		
		if (e == numBuckets) {
			//uncompress values in the last bucket, being ns = ptr to the beginning.
			bcpos = ns + inibcs_i;
			end = endbcs_i; //decoding from bcpos to end.
			prevvalue = s*(1L<<k);
		//	fprintf(stderr,"\n numbucks= %d prevvalue = %d, s= %d, e=%d i=%d",numBuckets,prevvalue,s,e,i);
		//	fprintf(stderr,"\n inipos= %d, bcpos = %d, enpos= %d",inibcs_i, bcpos, endbcs_i);
			while (bcpos < end) {
				BC_TO_DEC(bcs, bcpos, gap);
				prevvalue +=gap;
				alist[i++]=prevvalue-offs;	
			}	
		}
		else {
			//uncompress values in the bucket "s", being ns = ptr to the beginning, ne = ptr to end.
			bcpos = ns + inibcs_i;
			end = ne + inibcs_i; //decoding from bcpos to end.
			prevvalue = s*(1L<<k);
			//fprintf(stderr,"\n s= %d, prevvalue = %d",s, prevvalue); exit(0);
		//	fprintf(stderr,"\n ** numbucks= %d prevvalue = %d, s= %d, e=%d i=%d",numBuckets,prevvalue,s,e,i);
		//	fprintf(stderr,"\n ** inipos= %d, bcpos = %d, enpos= %d",inibcs_i, bcpos, endbcs_i);

			while (bcpos < end) {
				BC_TO_DEC(bcs, bcpos, gap);
				prevvalue +=gap;
				alist[i++]=prevvalue-offs;	
			}	
			
			ns=ne;s=e; /**/		
			e++;
			
		}	
	}
//	fprintf(stderr,"\n len = %d of %d",i,lenlist);
		
	*list =alist;
	*len =i;		
	return 0; 			
}	


    /* Extracts the id-th list, having id \in [0..maxId-1] 
       Assumes list has enough space previously allocated. */
       
int extractListNoMalloc_il (void *ail, uint id, uint *list, uint *len){
//	t_il *il=(t_il *) ail;	
	1; 			
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
			else if (strcmp(parameters[j+1], "lookup") == 0 )
				il->defs.defaultIntersect2 = lookup2;	    
			else if (strcmp(parameters[j+1], "merge") == 0 )
				il->defs.defaultIntersect2 = merge2;	    
			j++;
		}
		//setting the default function to Intersect-N-lists		
		else if  ((strcmp(parameters[j], "intn") == 0 ) && (j < num_parameters-1) ) {
			if (strcmp(parameters[j+1], "svs") == 0 ) 
				il->defs.defaultIntersectN = svsn;			
			else if (strcmp(parameters[j+1], "lookup") == 0 ) 
				il->defs.defaultIntersectN = lookupn;			
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
			else if (strcmp(parameters[j+1], "bin") == 0 )
				il->defs.defaultFsearch = fsearchBin;	    
			j++;
		}
	}
	free_parameters(num_parameters, &parameters);
	}
	
	fprintf(stderr,"\n ** Call to setDefaultSearchOptions_il: \"%s\"\n", search_options);
		
	return 0;	
}

	/* Intersects two lists given by id1 and id2  \in [0..maxId-1].
	   The defaultIntersect2 function must be initialized previously (svs, merge, ...)
	   Returns the resulting list of "intersecting positions"  */

/*int intersect_2_il (void *ail, uint id1, uint id2,  uint *noccs, uint **occs){
	t_il *il = (t_il *) ail;
	
	if ( il->defs.defaultIntersect2 == NULL) {
		return 40;	
	}

	// call to svs2, merge2, etc... depending on default settings
	int (*defaultIntersect2) (void *ail, uint id1, uint id2, uint *noccs, uint **occs ) = il->defs.defaultIntersect2;
		
	return ((*defaultIntersect2) (ail,id1,id2,noccs,occs));	
}*/

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
	
		
	int reterr = ((*defaultIntersect2) (ail,id1,id2,noccs,occs));

	if ((!reterr) && (!*noccs)){ 
		free (*occs);
		*occs=NULL;
	}
	
	return 	reterr;
}

	/* Intersects $nids$ lists given by ids in ids[], having ids[i] \in [0..maxId-1].
	   Returns the resulting list of "intersecting positions"  */
	   
/*
int intersect_N_il (void *ail, uint *ids, uint nids, uint *noccs, uint **occs ) {
	t_il *il = (t_il *) ail;
	if ( il->defs.defaultIntersectN == NULL) {
		return 41;	
	}
	
	// call to svsn,, etc... depending on default settings
	int (*defaultIntersectN) (void *ail, uint *ids, uint nids, uint *noccs, uint **occs ) = il->defs.defaultIntersectN;

	return ((*defaultIntersectN) (ail,ids,nids,noccs,occs));	
}
*/

int intersect_N_il (void *ail, uint *ids, uint nids, uint *noccs, uint **occs ) {
	t_il *il = (t_il *) ail;
	if ( il->defs.defaultIntersectN == NULL) {
		return 41;	
	}
	
	// call to svsn,, etc... depending on default settings
	int (*defaultIntersectN) (void *ail, uint *ids, uint nids, uint *noccs, uint **occs ) = il->defs.defaultIntersectN;
		
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

	t_il *il=(t_il *) ail;	
			
	register uint pos = il->occs[id]* il->ints_bitmaps_aligned * W +value;
	if (bitget((il->zoneBitmaps),pos)) return 1;
	
	return 0;	
}

	/* Fsearch, decompressing sequentially up to the searched value */

inline int fsearchDec (void *ail, uint id, uint value) {
	fprintf(stderr,"\n Call to Fsearch-seq");	
//	t_il *il=(t_il *) ail;		
	return 99; 
}

	/* Fsearch, using sequential Search in the samples first*/

inline int fsearchSeq (void *ail, uint id, uint value) {
	fprintf(stderr,"\n Call to Fsearch-seq");
//	t_il *il=(t_il *) ail;	
	return 99;
}

	/* Fsearch, using binary Search in the samples first*/
	
inline int fsearchBin (void *ail, uint id, uint value){
//	fprintf(stderr,"\n Call to Fsearch-bin");
	return 99;	
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
		
		//if (id1 >= id2) 
		if (il->lenList[id1] <= il->lenList[id2]) 
			{ first =id1; second=id2;}
		else  
			{ first =id2; second=id1;}

		//fprintf(stderr," lens --> %d, %d", il->lenList[id1], il->lenList[id2]); 

		n=il->lenList[first];
		extractList_il (ail, first, &firstlist, noccs); //reusing *noccs.
		n=*noccs;
		matches =0;
		
		if (!shouldUseBitmap(il,second)) { //calculates sample period for the second list
			//noOptimizations for keeping state.
		}	
		else {
			defaultFsearch=fsearchBitmap;
		}
		
		for (j=0; j<n;j++) {
			result = (*defaultFsearch) (ail, second, firstlist[j]); //call to fsearch;
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


	/*  Intersects 2 posting-lists using Lookup2 algorithm
	    Returns *occs (the result), noccs (number of elements)
	    Memory for *occs is allocated, and must be freed by the caller 
	    The first list is not uncompressed as a prior step to SVS. Values
	    from the first list are decoded and directly fsearched inside the
	    second list.
	 */
 
int lookup2(void *ail, uint id1, uint id2, uint *noccs, uint **occs){
	t_il *il = (t_il *) ail;
	
	
	if (shouldUseBitmap(il,id1) && shouldUseBitmap(il,id2)) {
		uint nids = 2;
		uint ids[2];
		ids[0]=id1; ids[1]=id2;
		int err =  mergeNBitmap (ail, ids, nids,  noccs, occs);
		return err;
	}

				
	{	
		uint first, second;  //small and longest posting.
		uint *alist; // to hold the intersection result
		uint p1,p2; // GZL

		if (il->lenList[id1] <= il->lenList[id2]) 
			{ first =id1; second=id2; p1=0;p2=1;} // GZL
		else  
			{ first =id2; second=id1; p1=1;p2=0;} // GZL

		LKPextractList_il (ail, first, &alist, noccs, p1); //reusing *noccs. // GZL

		
		if (shouldUseBitmap(il,second)) {
			int (*defaultFsearch) (void *ail, uint id, uint value) = fsearchBitmap;
			register uint j,n,matches;
			n=*noccs; //extract elements in extractList_il.
			matches = 0;		
			int result;
			
			for (j=0; j<n;j++) {
				result = (*defaultFsearch) (ail, second, alist[j]+p2); //call to fsearch; // GZL
				if (result) {
					alist[matches++] = alist[j];
				}
			}
			*occs= alist;
			*noccs = matches;
			return 0;
		}
		
		//else typical lookup search
		{
			
		uint *zoneoccs = il->zoneOccs;
		byte *bcs  = il->bcs;
		ulong maxValue = il->maxPostingValue;
		ulong B = il->B;	

		ulong sampleP,k, lenlist,numBuckets;
		register ulong obits,id,n,matches;
				
		id =second;				
		lenlist  = il->lenList[id];
		SAMPLEPERIOD(sampleP, k, B, lenlist, maxValue);		
		NUMBUCKETS (numBuckets,maxValue,k);            //numBuckets of term id.		
		OBITS(obits, il->bcsptr, id); //size of pointers stored in the 1st level for term "id".
				
		ulong inibcs_i = il->bcsptr[id];    //beginning of encoded gaps for term "id"
		ulong endbcs_i = il->bcsptr[id+1];  //beginning of encoded gaps for term "id+1"
	
		// searches the values of the first list inside the 2nd. -----------------------
	
		register long i; 
		register ulong x,j,e,d,gap;
		ulong h,l,l1,pos,inipos, prevj;
		n = *noccs;
		matches=0;
		
		i=-1;  //currentBucket
		inipos = il->occs[id]; //pos in zoneoccs	
		e=j=0;		
		for (x=0; x < n; x++) {  //processes all the elements in the first list.
			d = alist[x]+p2; // GZL
			h = d>>k;  // d div 2^k
			l = d & ((1L<<k)-1L);  // d mod (2^k);			
			
			if (((long)h)>i) {			 //starting a new bucket
				i=h;
				//j = t_{id}[i];
				pos = inipos + (obits * (i));
				j = bitread (zoneoccs, pos, obits); pos+=obits;
				//e = t_{id}n[i+1];
				if (i == (numBuckets -1)) 
					e = endbcs_i-inibcs_i;
				else 
					e = bitread (zoneoccs, pos, obits); pos+=obits;
				l1=0; 
				j = j + inibcs_i; e = e + inibcs_i;
			}
			while (j<e) {                //bucket not yet exhausted
				prevj=j;
				BC_TO_DEC(bcs, j, gap);  //j is increased.
				l1 += gap;	
				if (l<=l1) {
					if (l==l1){
						alist[matches++] = d-p2; //a match is found // GZL
					}else { 
						j=prevj; l1 -=gap;    //next iteration should read b[j] again
					} 					
					break; //breaks the while loop
				}	
				//j++;	//it was previously increased during decoding.
			}
		}					
	
		*occs= alist;
		*noccs = matches;
		}	
	}
	
	//printf("\n lookup found %u occs",*occs);			
	return 0;	
}




/***********************************************************************/


	/*  Intersects 2 posting-lists using merge algorithm (zipper-Sanders)
	    Returns *occs (the result), noccs (number of elements)
	    Memory for *occs is allocated, and must be freed by the caller */

int merge2 (void *ail, uint id1, uint id2, uint *noccs, uint **occs){
	fprintf(stderr,"\n Call to merge2");
	return 99;
}    

	/*  Intersects n posting-lists using set-versus-set
	    Returns *occs (the result), noccs (number of elements)
	    Memory for *occs is allocated, and must be freed by the caller */



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



int svsn (void *ail, uint *ids, uint nids, uint *noccs, uint **occs){
	fprintf(stderr,"\n Call to svsn");	
	return 99;	
}
	    

	/*  Intersects n posting-lists using lookup technique.
	    Returns *occs (the result), noccs (number of elements)
	    Memory for *occs is allocated, and must be freed by the caller */

int lookupn (void *ail, uint *ids, uint nids, uint *noccs, uint **occs){
	//fprintf(stderr,"\n Call to svsn");	
	t_il *il = (t_il *) ail;
/*	
		{uint i;
		fprintf(stderr,"\n Call to lookupN:: ids=  ");
		for (i=0;i<nids;i++) fprintf(stderr,"[%d]",ids[i]);
		}
*/
	//sorting ids ==> lists are sorted by frequency !!
/*
 	register uint i;
	uint *idss = (uint *)malloc (sizeof(uint) * nids);
	for (i=0;i<nids;i++) idss[i]=ids[i];
	simplesortUint(idss,nids);  // ==> change to sort by lenList !!
*/

	//sorting ids ==> lists are sorted by frequency !! ==> changed to sort by lenList !!
	register uint i, numbytecodes, numbitmaps;
	tsort *idss = (tsort *) malloc (sizeof(tsort) * nids);
	uint *idsBitmaps = (uint *) malloc (sizeof(uint) * nids);

	numbytecodes=0; numbitmaps=0;
	for (i=0;i<nids;i++) {
		if (!shouldUseBitmap(il,ids[i])){
			idss[numbytecodes].value=ids[i]; 
			idss[numbytecodes].p=i;  // GZL
			idss[numbytecodes].key = il->lenList[ids[i]];
			numbytecodes++;
		}
		else {
			//numbitmaps++;
			idsBitmaps[numbitmaps++]=ids[i]; 
			//idss[nids-numbitmaps].key = il->lenList[ids[i]];	//not needed		
		}
	}


	/** Now: idss[0,numbytecodes) keeps ids of terms using bytecodes.
	 *       idsBitmaps[0,numbitmaps) are ids of terms with bitmaps.
	 * */




	if (numbytecodes == 0) { //intersecting only bitmaps --> mergeNbitmap
		int err =  mergeNBitmap (ail, idsBitmaps, numbitmaps,  noccs, occs);
		free(idss);
		free(idsBitmaps);
		return err;
	}
	else {	 //bytecodes (& could also be bitmaps) --> extract 1st + lookupN.
		uint *zoneoccs = il->zoneOccs;
		byte *bcs  = il->bcs;
		ulong maxValue = il->maxPostingValue;
		ulong B = il->B;	

		ulong sampleP,k, lenlist,numBuckets;
		register ulong obits, id;  //size of pointers stored in the 1st level for term "id".
				
		register ulong  n, matches;
		uint *alist;


		if (numbytecodes >1)	//sorting the bytecodes by lenList[]
			simplesortAsc(idss,numbytecodes); 

		LKPextractList_il (ail, idss[0].value, &alist, noccs, idss[0].p); //uncompressing the shortest list (used in a svs approach). // GZL
		n= *noccs;
		matches =n;
		
		{ //regular lookup for bytecodes... finally, fsearch on the bitmaps.
			register long i; 
			register ulong x,z,j,e,d,gap;
			ulong h,l,l1,pos,inipos,bcpos, prevj;
			
			for (z=1; z<numbytecodes;z++){ // the "nids-1" remaining posting-lists.
				matches = 0;	
				id = idss[z].value; //next posting list...
				lenlist  = il->lenList[id];
				SAMPLEPERIOD(sampleP, k, B, lenlist, maxValue);		
				NUMBUCKETS (numBuckets,maxValue,k);            //numBuckets of term id.
				OBITS(obits, il->bcsptr, id);
				
				ulong inibcs_i = il->bcsptr[id];    //beginning of encoded gaps for term "id"
				ulong endbcs_i = il->bcsptr[id+1];  //beginning of encoded gaps for term "id+1"
											
				i=-1;  //currentBucket
				inipos = il->occs[id]; //pos in zoneoccs	
				e=j=0;		
				for (x=0; x < n; x++) {  //processes all the elements in the a list.
					d = alist[x]+idss[z].p; // GZL
					h = d>>k;  // d div 2^k
					l = d & ((1L<<k)-1L);  // d mod (2^k);			
					
					if (((long)h)>i) {			 //starting a new bucket
						i=h;
						//j = t_{id}[i];
						pos = inipos + (obits * (i));
						j = bitread (zoneoccs, pos, obits); pos+=obits;
						//e = t_{id}n[i+1];
						if (i == numBuckets -1) 
							e = endbcs_i-inibcs_i;
						else 
							e = bitread (zoneoccs, pos, obits); pos+=obits;
						l1=0; 
						j = j + inibcs_i; e = e + inibcs_i;
					}
					while (j<e) {                //bucket not yet exhausted
						prevj=j;
						BC_TO_DEC(bcs, j, gap);  //j is increased.
						l1 += gap;	
						if (l<=l1) {
							if (l==l1){
								alist[matches++] = d-idss[z].p; //a match is found // GZL
							}else { 
								j=prevj; l1 -=gap;    //next iteration should read b[j] again
							} 					
							break; //breaks the while loop
						}												
					}
				}/*for*/
				 
				if (!matches) break;  //search ends.
				n=matches;
			} //lookup ends

			if (matches) { //svs on the bitmaps.			
				int (*defaultFsearch) (void *ail, uint id, uint value) = fsearchBitmap; //fsearch bitmap
				int result;
				
				for (i=0; i<numbitmaps; i++){ // the "numbitmaps" remaining posting-lists.
					matches =0;	
							
					for (j=0; j<n;j++) {
						result = (*defaultFsearch) (ail, idsBitmaps[i], alist[j]); //call to fsearch;
						//fprintf(stderr,"\n  Search for value %d -> result = %d",firstlist[j], result);
						if (result) {
							alist[matches++] = alist[j];
						}
					}
					if (!matches) break;  //search ends.
					n=matches;
				}			
			}						
		}
		
		*occs= alist;
		*noccs = matches;					
	}
	
	free(idss);
	free(idsBitmaps);
	return 0;	
}



























/***********************************************************************/

int lookup2_NOBITMAP_sigir(void *ail, uint id1, uint id2, uint *noccs, uint **occs){
	t_il *il = (t_il *) ail;
	
    uint first, second;  //small and longest posting.
	uint *alist; // to hold the intersection result

	if (il->lenList[id1] <= il->lenList[id2]) 
		{ first =id1; second=id2;}
	else  
		{ first =id2; second=id1;}

	extractList_il (ail, first, &alist, noccs); //reusing *noccs.
	
	uint *zoneoccs = il->zoneOccs;
	byte *bcs  = il->bcs;
	uint maxValue = il->maxPostingValue;
	uint B = il->B;	
		
	{
		uint sampleP,k, lenlist,numBuckets;
		register uint obits,id,n,matches;
				
		id =second;				
		lenlist  = il->lenList[id];
		SAMPLEPERIOD(sampleP, k, B, lenlist, maxValue);		
		NUMBUCKETS (numBuckets,maxValue,k);            //numBuckets of term id.		
		OBITS(obits, il->bcsptr, id); //size of pointers stored in the 1st level for term "id".
				
		uint inibcs_i = il->bcsptr[id];    //beginning of encoded gaps for term "id"
		uint endbcs_i = il->bcsptr[id+1];  //beginning of encoded gaps for term "id+1"
	
		// searches the values of the first list inside the 2nd. -----------------------
	
		register int i; 
		register uint x,j,e,d,gap;
		uint h,l,l1,pos,inipos, prevj;
		n = *noccs;
		matches=0;
		
		i=-1;  //currentBucket
		inipos = il->occs[id]; //pos in zoneoccs	
		e=j=0;		
		for (x=0; x < n; x++) {  //processes all the elements in the first list.
			d = alist[x];
			h = d>>k;  // d div 2^k
			l = d & ((1<<k)-1);  // d mod (2^k);			
			
			if (((int)h)>i) {			 //starting a new bucket
				i=h;
				//j = t_{id}[i];
				pos = inipos + (obits * (i));
				j = bitread (zoneoccs, pos, obits); pos+=obits;
				//e = t_{id}n[i+1];
				if (i == (numBuckets -1)) 
					e = endbcs_i-inibcs_i;
				else 
					e = bitread (zoneoccs, pos, obits); pos+=obits;
				l1=0; 
				j = j + inibcs_i; e = e + inibcs_i;
			}
			while (j<e) {                //bucket not yet exhausted
				prevj=j;
				BC_TO_DEC(bcs, j, gap);  //j is increased.
				l1 += gap;	
				if (l<=l1) {
					if (l==l1){
						alist[matches++] = d; //a match is found
					}else { 
						j=prevj; l1 -=gap;    //next iteration should read b[j] again
					} 					
					break; //breaks the while loop
				}	
				//j++;	//it was previously increased during decoding.
			}
		}					
	
		*occs= alist;
		*noccs = matches;	
	}
						
	return 0;	
}




/***********************************************************************/


int lookupn_NOBITMAP_SIGIR (void *ail, uint *ids, uint nids, uint *noccs, uint **occs){
	//fprintf(stderr,"\n Call to svsn");	
	t_il *il = (t_il *) ail;
	
	//	{uint i;
	//	fprintf(stderr,"\n Call to lookupN:: ids=  ");
	//	for (i=0;i<nids;i++) fprintf(stderr,"[%d]",ids[i]);
	//	}

	//sorting ids ==> lists are sorted by frequency !!
	register uint i;
	uint *idss = (uint *)malloc (sizeof(uint) * nids);
	for (i=0;i<nids;i++) idss[i]=ids[i];
	simplesortUint(idss,nids);  // ==> change to sort by lenList !!


	uint *zoneoccs = il->zoneOccs;
	byte *bcs  = il->bcs;
	uint maxValue = il->maxPostingValue;
	uint B = il->B;	

	uint sampleP,k, lenlist,numBuckets;
	register uint obits, id;  //size of pointers stored in the 1st level for term "id".
			
	{	
		register uint  n, matches;
		uint *alist;

		extractList_il (ail, idss[0], &alist, noccs); //uncompressing the shortest list (used in a svs approach).
		n= *noccs;
		
		register int i; 
		register uint x,z,j,e,d,gap;
		uint h,l,l1,pos,inipos,bcpos, prevj;
		
		for (z=1; z<nids;z++){ // the "nids-1" remaining posting-lists.
			matches = 0;	
			id = idss[z]; //next posting list...
			lenlist  = il->lenList[id];
			SAMPLEPERIOD(sampleP, k, B, lenlist, maxValue);		
			NUMBUCKETS (numBuckets,maxValue,k);            //numBuckets of term id.
			OBITS(obits, il->bcsptr, id);
			
			uint inibcs_i = il->bcsptr[id];    //beginning of encoded gaps for term "id"
			uint endbcs_i = il->bcsptr[id+1];  //beginning of encoded gaps for term "id+1"
										
			i=-1;  //currentBucket
			inipos = il->occs[id]; //pos in zoneoccs	
			e=j=0;		
			for (x=0; x < n; x++) {  //processes all the elements in the a list.
				d = alist[x];
				h = d>>k;  // d div 2^k
				l = d & ((1<<k)-1);  // d mod (2^k);			
				
				if (((int)h)>i) {			 //starting a new bucket
					i=h;
					//j = t_{id}[i];
					pos = inipos + (obits * (i));
					j = bitread (zoneoccs, pos, obits); pos+=obits;
					//e = t_{id}n[i+1];
					if (i == numBuckets -1) 
						e = endbcs_i-inibcs_i;
					else 
						e = bitread (zoneoccs, pos, obits); pos+=obits;
					l1=0; 
					j = j + inibcs_i; e = e + inibcs_i;
				}
				while (j<e) {                //bucket not yet exhausted
					prevj=j;
					BC_TO_DEC(bcs, j, gap);  //j is increased.
					l1 += gap;	
					if (l<=l1) {
						if (l==l1){
							alist[matches++] = d; //a match is found
						}else { 
							j=prevj; l1 -=gap;    //next iteration should read b[j] again
						} 					
						break; //breaks the while loop
					}												
				}
			}/*for*/
			 
			if (!matches) break;  //search ends.
			n=matches;
		}
		
		*occs= alist;
		*noccs = matches;					
	}
	free(idss);
	return 0;	
}

