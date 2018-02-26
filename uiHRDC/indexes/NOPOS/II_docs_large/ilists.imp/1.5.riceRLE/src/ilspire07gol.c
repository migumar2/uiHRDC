#include "ilspire07gol.h"


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
                        uint *maxPostValue, uint *source, ulong length){

	uint **occs;
	uint *lens;
	uint n;
	
	register uint i,j,len;
	register ulong sourcepos;
	
	if (length<2) return 1;
	
	n=source[0];
	*maxPostValue = source[1] + DOCid_ADD;
	
	//allocating memory.
	lens = (uint *) malloc (sizeof(uint) * n);
	occs = (uint **) malloc (sizeof(uint *) * n);
	
	sourcepos=2;
	
	uint maxpostL =0;
	for (i=0; i<n; i++) {
		lens[i]= len = source[sourcepos++];	
		occs[i]= (uint *) malloc (sizeof(uint) * len);
		for (j=0; j<len;j++) {
			occs[i][j] = DOCid_ADD + source[sourcepos++];
			if (occs[i][j] > maxpostL) maxpostL = (occs[i][j]);
		}
	}
	
	if (maxpostL > *maxPostValue) *maxPostValue = maxpostL;
	printf("\n **** maxpostSource = %d, maxpostComputed = %d",*maxPostValue,maxpostL);
	
	if (length !=sourcepos) { fprintf(stderr,"\n error in parseAllposting.lists() \n");return 31;}
	
	*nlists =n;
	*lenList = lens;
	*occList = occs;	
	return 0;
}



/**********************************************************************************/
/*  Implementation of the General interface:                                      */
/*              interface for using compressed representations of posting lists   */
/*              Any alternative representation must implement all these functions */
/**********************************************************************************/


	/* returns a string with the parameters available for building the method */

char * showParameters_il(){
	static char text[200];
	sprintf(text, "no parameters needed");
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




int build_il (uint *source, ulong length, char *build_options, void **ail) {
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
	  if  ((strcmp(parameters[j], "filename") == 0 ) && (j < num_parameters-1) ) {
	    strcpy(filename,parameters[j+1]);
	    j++;
	  }
//	  else if ((strcmp(parameters[j], "lenBitmapDiv") == 0 ) && (j < num_parameters-1) ) {
//	    lenBitmapDiv=atoi(parameters[j+1]);	    
//	    j++;
//	  }
	}
	free_parameters(num_parameters, &parameters);
	}
	
	printf("\n parameters of method:  none for building, filename = %s\n", filename);
	
	

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
		
		printf("numOflists = %lu, maxPost = %lu \n", 	
											(ulong)il->nlists, (ulong)maxPostValue);
	
		//calculates an stimation of the uncompressed representation of the index (using uints per posting)
		il->sizeUncompressed = sizeof(uint) * (il->nlists) +  //sizeoflenList
		                       sizeof(uint*) * (il->nlists); //pointers to each posting (occs[i])
		                       
		for (i=0; i< il->nlists;i++) { il->sizeUncompressed += (lenList[i] * sizeof(uint));}

		fprintf(stderr,"stimated mem-usage of uncompressed posting-lists = %lu bytes\n", (ulong)il->sizeUncompressed);
			
		/** 5 ** Encoding the occurrences **/
		
		il->occs = (uint *) malloc (sizeof(uint) * il->nlists);
		
			/** 5.0.1 - parameters for the golombRice coding (added to JASIST) **/
			
			{	
				byte usedB[MAX_GOLOMB_RICE_PARAMETER];
				for (i=0; i< MAX_GOLOMB_RICE_PARAMETER ; i++) usedB[i]=0;

				
				#ifdef REGULAR_GOLOMB_RICE_TYPE	
				il->b = (byte *) malloc (sizeof(byte) * il->nlists);		

				uint *gaps = (uint *)malloc (sizeof(uint) * (il->maxPostingValue +1));				
				uint len,j,n,prevvalue;					
							
				for (i=0; i < il->nlists; i++) {
					/** type 1 **/
					
					//il->b[i] = (byte) getWWW2009GolombRiceParameter_forList(il->maxPostingValue,il->lenList[i]);				
					
					/** type 2 **/
					{	/*
						if (il->lenList[i]>1) {
							1. generate gap_list[] from occList[id][] (removing samples)
							2. n <-- len of gap_list[];
							3. il->b[i] = computeGolombRiceParameter_forList(gap_list, n);
						}
						*/
						
						il->b[i]=0;  //default for uninitialized values.
						
						len = lenList[i];
						
						//UN_OPTIMIZED G-R-PARAM WHEN USING RUN-LENGHT			
						#ifndef OPTIMIZE_RICE_PARAM_FOR_INCLUDING_RUNS
						{	
							n=0;
							prevvalue= 0;	
							for (j=0; j<len; j++) {
								gaps[n++] = occList[i][j]-prevvalue;
								prevvalue = occList[i][j];
							}
								
							il->b[i] = computeGolombRiceParameter_forList(gaps, n);		
						}
						#endif

						//OPTIMIZED PARAMETER FOR G_RIZE WHEN USING RUN-LENGHT
						#ifdef OPTIMIZE_RICE_PARAM_FOR_INCLUDING_RUNS
						{	
							n=0;
							prevvalue= 0;	
							uint runLen=0;
							uint gap;
							for (j=0; j<len; j++) {
							  	gap = occList[i][j]-prevvalue;
								prevvalue = occList[i][j];
								
								if (gap==1){
								  runLen++;
								}
								else {
								  if (runLen) {
									gaps[n++]=+1;
									gaps[n++]=runLen;
									runLen=0;
								  }
								  gaps[n++] = gap;
								}							
							}//for 
							// now processes the 1-run if the posting list ends with a 1-run of size "runLen"
							if (runLen) {
							  gaps[n++]=1;
							  gaps[n++]=runLen;
							  runLen=0;
							}							
				
							il->b[i] = computeGolombRiceParameter_forList(gaps, n);		
						}
						#endif						
						

					} /* end type 2 */					
				}
				
				free(gaps);
				#endif
								
			}
		
			//uint *numAbsSamplesperID = (uint *) malloc (sizeof(uint) * (il->nlists));
		

			/** 5.b **
				Computes the size of bc vector by simulating encoding process of gaps.
				Sets also bitmaps for those terms indexed with a bitmap.
				Computes and sets the values for "occs[]".
			*/
			uint bcssize=0;
			{
				uint nwords = il->nlists;
				uint id,j;
				uint prevvalue, gap, numbits;
				uint len;
				
				uint counterBCcodes=0; 
				for (id=0; id<nwords; id++) {
					len= il->lenList[id];
					uint b_g= B_GOL_RICE(il, id);
/*					
					prevvalue= 0;					
					
					il->occs[id] = bcssize;
					for (j=0; j<len; j++){
						gap = occList[id][j]-prevvalue;
						prevvalue = occList[id][j];
						numbits = rice_size(gap,  b_g);
						bcssize += numbits;
						counterBCcodes++;
					}
*/

					prevvalue= 0;
					uint runLen =0;
					
					il->occs[id] = bcssize;
					for (j=0; j<len; j++){
						gap = occList[id][j]-prevvalue;
						prevvalue = occList[id][j];
												
						if (gap == 1){
						  runLen++;
						}
						else {
						  if (runLen) {
							numbits  = rice_size(+1, b_g);
							numbits += rice_size(runLen,b_g);
					 	    bcssize += numbits;
						    counterBCcodes+=2;
							runLen=0;
						  }
						  
						  //encodes the next gap (which is not a 1-run)
						  numbits = rice_size(gap,  b_g);
						  bcssize += numbits;
						  counterBCcodes++;
						}						 						  
					}//for
					
					// now encodes the 1-run if the posting list ends with a 1-run of size "runLen"
					if (runLen) {
					  numbits  = rice_size(+1, b_g);
					  numbits += rice_size(runLen,b_g);
					  bcssize += numbits;
					  counterBCcodes+=2;
					  runLen=0;
					}					

				/** byte alignment on the bcs goes here **/	
				
				}
				fprintf(stderr,"\n Number of gaps to store is = %d",counterBCcodes);	
				fprintf(stderr,"\n Size of gcs Vector should be  %lu bits (%lu bytes)",(ulong)bcssize,(ulong) (bcssize +W-1)/W*sizeof(uint));
				//fprintf(stderr,"\n [SHOULD BE IDENTICAL]: number of words = %d, and sum of occurs is %d",wcsa->Nwords, counterAbsSamples + counterBCcodes);	
			}
			
		il->bcsSize = bcssize; //in bits
		
		il->gcsSize = (il->bcsSize +W-1)/W;
		il->gcs = (uint *) malloc (sizeof(uint) * il->gcsSize);
		if (il->gcs) {
			il->gcs[il->gcsSize -1] = 0000; //valgrind gives no uninitialized-byte warnings.
		}
		
		fprintf(stderr,"\n *** BCSSIZE = %u, GCSSIZE= %u **\n\n",il->bcsSize, il->gcsSize);
		fflush(stderr);fflush(stdout);
		
		/** 5.c ** encodes gaps into gcs && set pointers into zoneoccurssTmp */
		{
			uint nwords = il->nlists;
			//byte *bcs = il->bcs;
			uint *gcs = il->gcs;
			uint id,j;
			uint prevvalue, gap;
			uint len ;

			uint bcpos=0;
			
			for (id=0; id<nwords; id++) {
				len= il->lenList[id];	
				uint b_g=  B_GOL_RICE(il, id);
/*					
				prevvalue =0;
				for (j=0; j<len; j++){
					gap = occList[id][j]-prevvalue;
					prevvalue = occList[id][j];
					bcpos = rice_encode(gcs, bcpos, gap, b_g);
				}
*/

				prevvalue =0;
				uint runLen=0;
				for (j=0; j<len; j++){
					gap = occList[id][j]-prevvalue;
					prevvalue = occList[id][j];
					//bcpos = rice_encode(gcs, bcpos, gap, b_g);
					
					if (gap == 1){
					  runLen++;
					}
					else {
					  if (runLen) {
						bcpos = rice_encode(gcs, bcpos,     +1, b_g);
						bcpos = rice_encode(gcs, bcpos, runLen, b_g);
						runLen=0;
					  }
					  
					  //encodes the next gap (which is not a 1-run)
					  bcpos = rice_encode(gcs, bcpos, gap, b_g);
					}					
					
				}//for
				// now encodes the 1-run if the posting list ends with a 1-run of size "runLen"
				if (runLen) {
				  bcpos = rice_encode(gcs, bcpos,     +1, b_g);
				  bcpos = rice_encode(gcs, bcpos, runLen, b_g);
				  runLen=0;
				}	
				
			}				
			
			fprintf(stderr,"\n Gaps were encoded with bc ==> %lu bits (%lu bytes)",(ulong)bcpos,(ulong) (bcpos+W-1)/W *sizeof(uint));			
		}		
			

		/** checking that the lists are decoded successfully *************/

		fprintf(stderr,"\n ! CHECKING that the decoded lists are identical to the original ones. ");
		{ //checking:: decoding a list
			uint i, id ,len;
			uint *list;
			
			for (id=0;id < il->nlists;id++) {
				extractList_il (*ail, id, &list, &len);		
				for (i=0;i<len;i++){
					if (list[i] != (occList[id][i]-DOCid_ADD)) {
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
				extractListNoMalloc_il(*ail,id,list,&len);	
				for (i=0;i<len;i++){
					if (list[i] != (occList[id][i]-DOCid_ADD)) {
						fprintf(stderr,"\n decoding of lists failed for id = %d: DIFFERENT!!: (%d,%d)",id, list[i],occList[id][i]);
						exit(0);
					}
				} 
			}			
			free(list);				

		}
		fprintf(stderr,"\n ! Decoding the list of occurrences worked fine (the same as previous values) ");
	
		
		/******************************************************/

		//frees memory
		for (i=0;i<il->nlists;i++) free(occList[i]);
		free(occList);
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

	/** Saving the maxPostingValue **/
	write (file,&(il->maxPostingValue), sizeof(uint));
	
	/** Saving lenList vector */
	{
		uint size    = il->nlists * sizeof(uint);

		write(file, &(il->nlists), sizeof(uint));		
		write(file, il->lenList, size);
	}
	

	/** Saving the postings lists using golomb-Rice codes. */
	{
		//fprintf(stderr,"\n saving size ucompressed = %lu!!!!",il->sizeUncompressed);
		write(file, &(il->sizeUncompressed), sizeof(ulong));		
		write(file, &(il->bcsSize), sizeof(uint));		
		write(file, &(il->gcsSize), sizeof(uint));		
				

		//fprintf(stderr,"\n *sampleK = %d, zoneOccsSize = %d, numbucks= %d",wcsa->occs.sampleK, wcsa->occs.zoneOccsSize, wcsa->occs.numBuckets); 
		//fprintf(stderr,"\n *absoffsetSize = %d, bcOffsetSize = %d, occsBucketSize= %d",wcsa->occs.absOffsetSize, wcsa->occs.bcOffsetSize, wcsa->occs.occsBucketSize); 
		//fprintf(stderr,"\n *bcsSize = %d",wcsa->occs.bcsSize);

		write(file, il->occs, il->nlists * sizeof(uint));		
						
		#ifdef REGULAR_GOLOMB_RICE_TYPE
			write(file, il->b, il->nlists * sizeof(byte));	
		#endif
			
		write(file, il->gcs, il->gcsSize * sizeof(uint));							
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


	/* Saving the maxPostingValue */
	read (file,&(il->maxPostingValue), sizeof(uint));
	
	/** loading nlists variable and lenList vector */
	{	
		read(file, &(il->nlists), sizeof(uint)); // number of posting lists 
	
		il->lenList = (uint *) malloc ( (il->nlists) * sizeof(uint));  // the lenList array
		read(file, il->lenList, ((il->nlists) * sizeof(uint)) );
	}


	/** loading the posting lists using variable-length codes */
	{		
		//some integers
		read(file, &(il->sizeUncompressed), sizeof(ulong));					
		read(file, &(il->bcsSize), sizeof(uint)); //size (in bits used) of the compressed sequence (gcs)
		read(file, &(il->gcsSize), sizeof(uint)); //size (in uints) of gcs.		
				
		//vector occs.
		il->occs = (uint *) malloc (il->nlists * sizeof(uint) );		
		read(file, il->occs, il->nlists * sizeof(uint)); //wcsa->nwords was loading in "loadVocabulary"
		
		#ifdef REGULAR_GOLOMB_RICE_TYPE
			//vector b. //value of Golomb_Rice parameter.
			il->b = (byte *) malloc (il->nlists * sizeof(uint));				
			read(file, il->b, il->nlists * sizeof(byte));	
		#endif
			
					
		//vector gcs.
		//il->gcs = (uint *) malloc ((il->gcsSize+1) * sizeof(uint));	
		//il->gcs[il->gcsSize]=0000; /***** FARI CHECK !! *****/	
		il->gcs = (uint *) malloc ((il->gcsSize) * sizeof(uint));	
		if (il->gcs) {
			il->gcs[il->gcsSize-1]=0000; /***** FARI CHECK !! *****/	
		}
		read(file, il->gcs, il->gcsSize * sizeof(uint));		
						
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
	if (il->gcs) free(il->gcs);
		
	#ifdef REGULAR_GOLOMB_RICE_TYPE
		free(il->b);
	#endif
		
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



ulong  sizetobits(void *ail) {
	uint maxs, bits_s;
	t_il *il=(t_il *) ail;
	ulong total;
	

	// size of lenList
	maxs=0;
	for(uint i=0;i<il->nlists;i++)       //** calcula el m�ximo s�mbolo en symbols_new;
		maxs = max(maxs,il->lenList[i]);
	bits_s = bits(maxs);                   //** numero de bits necesarios para los valores en "symbols_new"
	total = (il->nlists * bits_s / W +1)* sizeof(uint);   //** valores de symbols_new codificados con "bits_sn" bits.

	//fprintf(stderr,"\n sizeOfLenlist = %d bytes",  (il->nlists * bits_s / W +1));

	// size of occs
	maxs=0;
	for(uint i=0;i<il->nlists;i++)       //** calcula el m�ximo s�mbolo en symbols_new;
		maxs = max(maxs,il->occs[i]);	
	//maxs = il->occs[il->nlists-1];
	bits_s = bits(maxs);                   //** numero de bits necesarios para los valores en "symbols_new"
	total += (il->nlists * bits_s / W +1)* sizeof(uint);   //** valores de symbols_new codificados con "bits_sn" bits.

	//fprintf(stderr,"\n sizeOf occs[] = %d bytes",  (il->nlists * bits_s / W +1));

	
	#ifdef REGULAR_GOLOMB_RICE_TYPE
		maxs=0;
		for(uint i=0;i<il->nlists;i++)       //** calcula el m�ximo s�mbolo en symbols_new;
			maxs = max(maxs,il->b[i]);	
		bits_s = bits(maxs);                   //** numero de bits necesarios para los valores en "symbols_new"
		total += (il->nlists * bits_s / W +1)* sizeof(uint);   //** valores de symbols_new codificados con "bits_sn" bits.
		//fprintf(stderr,"\n  sizeOf b_golom[] = %d bytes, bits_s = %d, max_b= %d", (il->nlists * bits_s / W +1)* sizeof(uint), bits_s, maxs);
	#endif

	return total;
}
 

/** creates a "fun" matlab file with the ratio of
 *  source len of posting and compressed len
 **/  
int buildBell_il(void *ail, const char *matlabfilename) {
	uint maxs;
	uint bits_lenlist,bits_occs;
	t_il *il=(t_il *) ail;
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
		
		{
			uint inibcpos = il->occs[i];
			uint bcpos=inibcpos;
			uint *gcs = il->gcs;
			uint num;
			uint ii;
			uint b_g=  B_GOL_RICE(il, i);	
	
			for (ii=0; ii<lenPost;ii++) {		
				num=rice_decode(gcs, &bcpos, b_g);  //BC_TO_DEC(bcs, bcpos, num);
			}		
				
			lenPhrase = (bcpos - inibcpos);		 
		}
			
		M[lenPost][count[lenPost]] = lenPhrase;   //len of the posting list (en bits!!!!)
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
			avgs[i] = (total*1.0 / count[i] )/8.0;	 //DIVIDIR ENTRE 8 para pasar a bytes!
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
	fprintf(f,"\nfigure(5);");
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

int size_il(void *ail, ulong *size) {
	t_il *il=(t_il *) ail;
	*size=0;
	*size += sizeof(t_il);
	
//	*size += il->nlists * sizeof(uint) ;   //lenList vect
//	*size += il->nlists * sizeof(uint) ;   //occs vect
//	*size += (il->nlists + 1)* sizeof(uint);  //il->bcsptr 

	*size += sizetobits(ail);

	ulong sizeUint=0;
	sizeUint += il->nlists * sizeof(uint) ;   //lenList vect
	sizeUint += il->nlists * sizeof(uint) ;   //occs vect

	#ifdef REGULAR_GOLOMB_RICE_TYPE
		sizeUint += il->nlists * sizeof(byte) ;   //lenList b
	#endif


	fprintf(stderr,"\n size of lenlist+occs+b_golomb[] = %lu bytes Vs %lu bytes (old)",sizetobits(ail),sizeUint);
	fprintf(stderr,"\n size of compressed sequence = %lu bits (%lu bytes) \n",(ulong)il->bcsSize,(ulong) il->gcsSize*sizeof(uint));


	*size += il->gcsSize * sizeof(uint);
	
	return 0;	
}


    /* Gives the memory occupied by the index assuming an uncompressed
       representation (uint **)*/
       
int size_il_uncompressed(void *ail, ulong *size) {
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

	uint *gcs = il->gcs;
	

	uint i, bcpos, num, prevvalue;
	bcpos = il->occs[id]; //pos in zoneoccs

	uint b_g=  B_GOL_RICE(il, id);
	prevvalue=0;
/*	
	for (i=0; i<lenlist;i++) {		
		num=rice_decode(gcs, &bcpos, b_g);  //BC_TO_DEC(bcs, bcpos, num);
		prevvalue +=num;
		alist[i]=prevvalue-DOCid_ADD;			
	}		
*/

	for (i=0; i<lenlist; ) {		
		num=rice_decode(gcs, &bcpos, b_g); 
		
		if (num ==1) {
		  num=rice_decode(gcs, &bcpos, b_g);

		  register uint z;
		  alist[i]= prevvalue-DOCid_ADD +1;
		  i++;
		  for (z=1; z< num; z++){
			alist[i]=alist[i-1] +1;
			i++;
		  }
		  prevvalue +=num;
		}
		else {
		  prevvalue +=num;
		  alist[i]=prevvalue-DOCid_ADD;			
		  i++;
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

	
	uint *gcs  = il->gcs;
	lenlist  = il->lenList[id];

	//fprintf(stderr,"\nDecoding all the %d occurrences of word[%d]= <<%s>>",lenlist, id, wcsa->words[id]);fflush(stderr);		 		
	uint i, bcpos, num, prevvalue;
	bcpos = il->occs[id]; //pos in zoneoccs
	uint b_g= B_GOL_RICE(il, id);
	
	prevvalue=0;
	for (i=0; i<lenlist; ) {		
		num=rice_decode(gcs, &bcpos, b_g); 
		
		if (num ==1) {
		  num=rice_decode(gcs, &bcpos, b_g);

		  register uint z;
		  list[i]= prevvalue-DOCid_ADD +1;
		  i++;
		  for (z=1; z< num; z++){
			list[i]=list[i-1] +1;
			i++;
		  }
		  prevvalue +=num;
		}
		else {
		  prevvalue +=num;
		  list[i]=prevvalue-DOCid_ADD;			
		  i++;
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
//			if (strcmp(parameters[j+1], "svs") == 0 ) 
//				il->defs.defaultIntersect2 = svs2;
//			else 
			if (strcmp(parameters[j+1], "merge") == 0 )
				il->defs.defaultIntersect2 = merge_2_uncompressFst;	    
			j++;
		}
		//setting the default function to Intersect-N-lists		
		else if  ((strcmp(parameters[j], "intn") == 0 ) && (j < num_parameters-1) ) {
			if (strcmp(parameters[j+1], "merge") == 0 ) 
				il->defs.defaultIntersectN = merge_N_uncompressFst;			
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
		*noccs=0;
		*occs=NULL;
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

int intersect_N_il (void *ail, uint *ids, uint nids, uint *noccs, uint **occs ) {
	t_il *il = (t_il *) ail;
	if ( il->defs.defaultIntersectN == NULL) {
		*noccs=0;
		*occs=NULL;		
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

	/* Intersects two lists given by id1 and id2  \in [0..maxId-1].
	   The defaultIntersect2 function must be initialized previously (svs, merge, ...)
	   Returns the resulting list of "intersecting positions"  */

uint* intersect_aux(uint* arr1, uint len1, uint* arr2, uint len2, uint* len){
    //uint alen = len1<len2?len1:len2;
    //uint* intersect = (uint*)malloc(sizeof(uint)*alen);
    uint* intersect = arr1;


    uint a=0;
    uint b=0;
    uint count=0;
    while( (a<len1) &&(b<len2)){
        if(arr1[a]==arr2[b]){
            intersect[count] = arr1[a];
            count += 1;
			a++;b++;
        }else if(arr1[a]<arr2[b]){
            a++;
        }else{
            b++;
        }
    }	
	if (!count) {free(intersect); intersect=NULL;}
    *len = count;
    return intersect;
}
/***********************************************************************/
int merge_2_uncompressFst (void *ail, uint id1, uint id2,  uint *noccs, uint **occs){
    uint len1;
    uint* arr1;
    extractList_il(ail, id1, &arr1, &len1);
    uint len2;
    uint* arr2;
    extractList_il(ail, id2, &arr2, &len2);
    *occs = intersect_aux(arr1,len1,arr2,len2,noccs);
    //free(arr1);
    free(arr2);
	//if (!(*noccs)) {free(*occs); *occs=NULL;}
    return 0;
}



/******************************************************************/
typedef struct {
    uint pos;
    uint len;
} lpair;

int lpair_compare(const void *a, const void *b) {
        return ((int)(((lpair*)a)->len) > (int)(((lpair*)b)->len));
}
	/* Intersects $nids$ lists given by ids in ids[], having ids[i] \in [0..maxId-1].
	   Returns the resulting list of "intersecting positions"  */

	/*  Intersects n posting-lists using set-versus-set
	    Returns *occs (the result), noccs (number of elements)
	    Memory for *occs is allocated, and must be freed by the caller */
	    
int merge_N_uncompressFst (void *ail, uint *ids, uint nids, uint *noccs, uint **occs){
//	fprintf(stderr,"\n call to mergeN")
//	{uint i;
//	fprintf(stderr,"\n Call to svsN:: ");
//	for (i=0;i<nids;i++) fprintf(stderr,"[%d]",ids[i]);
//	}


    lpair* lengths = (lpair*)malloc(sizeof(lpair)*nids);
    for(unsigned int i=0;i<nids;i++){
        lengths[i].pos = ids[i];
        lenlist(ail, ids[i], &(lengths[i].len));
    }
    qsort(lengths, nids, sizeof(lpair), &lpair_compare);
    uint* auxArr;
    uint auxLen;
    merge_2_uncompressFst(ail, lengths[0].pos, lengths[1].pos, &auxLen, &auxArr);
    //printf("\n initial merge = %lu occs (len1=%u, len2=%u)",auxLen,lengths[0].len, lengths[1].len);
    for(unsigned int i=2;(i<nids)&&(auxLen);i++){
        uint len;
        uint* arr;
        extractList_il(ail, lengths[i].pos, &arr, &len);
        auxArr = intersect_aux(auxArr,auxLen,arr,len,&auxLen);
    	//printf("\n next merge = %lu occs (len1=%u, len2=%u)",auxLen, auxLen, len);
    	free(arr);
    }
    free(lengths);
	//if (!auxLen) {free(auxArr); auxArr=NULL;}
    *occs = auxArr;
    *noccs = auxLen;
    return 0;	
}




/***********************************************************************/


	/*  Intersects 2 posting-lists using merge algorithm (zipper-Sanders)
	    Returns *occs (the result), noccs (number of elements)
	    Memory for *occs is allocated, and must be freed by the caller */


int merge2 (void *ail, uint id1, uint id2, uint *noccs, uint **occs){
	//fprintf(stderr,"\n Call to merge2");
		
	return merge_2_uncompressFst(ail,id1,id2,noccs,occs);
	
}

/******************************************************************/
/******************************************************************/



	/*  Intersects n posting-lists using set-versus-set
	    Returns *occs (the result), noccs (number of elements)
	    Memory for *occs is allocated, and must be freed by the caller */

int mergeN (void *ail, uint *ids, uint nids, uint *noccs, uint **occs){
	return merge_N_uncompressFst(ail,ids,nids,noccs,occs);
}






#ifdef FACADEWITHMAIN 

int main (int argc, char *argv[]) {
//	check_GOLOMB_works_show(0,34);


	check_GOLOMB_works2(0,50,3);
	check_GOLOMB_works2(0,50,4);
	check_GOLOMB_works2(0,50000,5);
	
	show_Optimal_GOLOMB_works(0,50);
	
	return 0;
	
}

#endif
