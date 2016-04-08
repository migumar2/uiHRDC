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
	sprintf(text, "lzmaEncoderPath=%s; minbcssize=%d","path2lzmaEncoder",MINBCSSIZE);
	return text;	
}


	/* checks if an id should use either bitmap or bytecodes: valid during build_il() */
int shouldUseLZMA(t_il *il, uint id) {  /* 0 <= id < il->nlists */
	return (bitget(il->useslzma,id)) ;
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
	printf("\n CALL TO BUILD_IL !!\n");
	
	t_il *il;
	il = (t_il *) malloc (sizeof (t_il) * 1);
	*ail = il;

	
	/** processing the parameters */
	char delimiters[] = " =;";
	char filename[256] = "unnamed";
	int j,num_parameters;
	char ** parameters;
	
	il->path2lzmaEncoder[0]= '\0';
	il->minbcssize = MINBCSSIZE;
	
	if (build_options != NULL) {
	parse_parameters(build_options,&num_parameters, &parameters, delimiters);
	for (j=0; j<num_parameters;j++) {
	  if ((strcmp(parameters[j], "filename") == 0 ) && (j < num_parameters-1) ) {
	    strcpy(filename,parameters[j+1]);
	    j++;
	  }
	  else if ((strcmp(parameters[j], "minbcssize") == 0 ) && (j < num_parameters-1) ) {
	    il->minbcssize = atoi (parameters[j+1]);
	    j++;
	  }
	  else if ((strcmp(parameters[j], "path2lzmaencoder") == 0 ) && (j < num_parameters-1) ) {
	    strcpy(il->path2lzmaEncoder, (parameters[j+1]));	    
	    j++;
	  }
	}
	/*
	if (il->path2lzmaEncoder[0] == '\0') {
		printf("\n path2lzmaencoder NOT PROVIDED... Cannot continue!!"); exit(0);
	}
	*/
	
	free_parameters(num_parameters, &parameters);
	}
	
	printf("\n parameters of method: filename = %s, path2lzmaencoder=%s, minbcssize = %d\n", filename,il->path2lzmaEncoder, il->minbcssize);

	{	/** 4 ** creates a temporal list of occurrences of each word (block-oriented).
					gives also the len of each list */
		uint i;
		uint **occList;
		uint *lenList;
		uint maxPostValue;
		

	fprintf(stderr,"\n paso 1 ");fflush(stderr);		
		parseAllPostingList(&(il->nlists), &lenList, &occList, &maxPostValue, source,length);				
		free(source);
		
	fprintf(stderr,"\n paso 2 ");fflush(stderr);		
		
		il->lenList = lenList;
		il->maxPostingValue = maxPostValue;
				
		printf("numOflists = %u, maxPost = %u, \n", il->nlists, maxPostValue);
	
		//calculates an stimation of the uncompressed representation of the index (using uints per posting)
		il->sizeUncompressed = sizeof(uint) * (il->nlists) +  //sizeoflenList
		                       sizeof(uint*) * (il->nlists); //pointers to each posting (occs[i])
		                       
		for (i=0; i< il->nlists;i++) { il->sizeUncompressed += (lenList[i] * sizeof(uint));}

		fprintf(stderr,"stimated mem-usage of uncompressed posting-lists = %u bytes\n", il->sizeUncompressed);

		
		/** 5 ** Encoding the occurrences **/
		
		il->occs = (uint *) malloc (sizeof(uint) * (il->nlists+1));
		
			uint *numAbsSamplesperID = (uint *) malloc (sizeof(uint) * (il->nlists));
			
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
				
				uint counterBCcodes =0; 
				uint counterBitmaps=0;
				for (id=0; id<nwords; id++) {
					len= il->lenList[id];					
					prevvalue= 0;					
					il->occs[id] = bcssize;
					for (j=0; j<len; j++){
						gap = occList[id][j]-prevvalue;
						prevvalue = occList[id][j];
						SIZE_DEC_TO_BC(gap,numbytes);
						bcssize += numbytes;
						counterBCcodes++;
					}					
				}
				il->occs[nwords] = bcssize;
				fprintf(stderr,"\n Number of gaps to store is = %d",counterBCcodes);	
				fprintf(stderr,"\n Size of bcs Vector should be  %d bytes",bcssize);
			}
			
		il->bcsSize = bcssize;
		il->bcs = (byte *) malloc (sizeof(byte) * il->bcsSize);		
		
		/** 5.c ** encodes gaps into bcs  */
		{
			uint nwords = il->nlists;
			byte *bcs = il->bcs;
			uint id,j;
			uint prevvalue, gap;
			uint len;

			uint bcpos=0;
							
			for (id=0; id<nwords; id++) {
				len= il->lenList[id];	
				prevvalue=0;
				
				for (j=0; j<len; j++){
					gap = occList[id][j]-prevvalue;
					prevvalue = occList[id][j];
					DEC_TO_BC(bcs, bcpos, gap);  //bcpos is increased.
				}
			}
			
			fprintf(stderr,"\n Gaps were encoded with bc ==> %d bytes",bcpos);
		}		


		/** 6. Now the lists that compresses much better by using vbyte+lzma 
		 *  instead of just vbyte, are compressed with vbyte+lzma
		 */
		{	uint j,id;
			uint nwords = il->nlists;
			byte *newbcs = (byte *) malloc (sizeof(byte) * il->bcsSize);
			uint *newoccs = (uint *) malloc (sizeof (uint) * (nwords+1));
			il->useslzma = (uint *) malloc (sizeof(uint) * ((nwords + W -1)/W));
			initLzmaStruct (&il->lzmadata);
			for (j=0;j< ((nwords + W -1)/W) ; j++) il->useslzma[j]= 0000;
			
			uint bcpos=0;
			int res=0;
			uint lenbcs;
			uint lenlzma;
			
			for (id=0; id<nwords; id++) {
				byte *inb; uint inn;  //pointer to compressed file in RAM and its size.
				newoccs[id]= bcpos;
				lenbcs = il->occs[id+1]-il->occs[id];
				byte *bcsdata = &(il->bcs[il->occs[id]]);   /** ptr to bytecoded data */
				
				if (lenbcs> il->minbcssize)   /** CHECKS THE THRESHOLD FOR lenbcs TO CHECK IF LZMA COULD BE OK **/
				{ 
					byte *bcsdata = &(il->bcs[il->occs[id]]);   /** ptr to bytecoded data */
					res = compressDataExt(bcsdata,lenbcs, il->path2lzmaEncoder);
					if (!res) {
						loadDataExt(&inb,&inn);
						createLzmaExt(inb, inn, il->lzmadata);
						lenlzma = sizeBytesLzmaStruct(il->lzmadata); /* memory need if lzma is used */

						printf("\n [id= %6d/%6d] lenbcs = %8d, lenbcs+lzma = %8d ==> usesLzma = %d (f/t?) (bcpos=%d)",id, nwords,lenbcs,lenlzma, (lenlzma<lenbcs),bcpos);
						if (lenlzma < lenbcs) {
							writeBuffLzmaStruct(newbcs,bcpos,il->lzmadata);
							bcpos +=lenlzma;
							bitset(il->useslzma, id);
						}
						else {
							memcpy( (void *) &(newbcs[bcpos]), (const void *) bcsdata, lenbcs);
							bcpos +=lenbcs;
						}
												
						free(inb);
					}
					else {
						printf("\n LZMA compression failed!. Aborting... \n");exit(0);
					}					
				}	
				else {
					memcpy( (void *) &(newbcs[bcpos]), (const void *) bcsdata, lenbcs);
					bcpos +=lenbcs;
				}				
			}/* for */	
			newoccs[nwords]=bcpos;	
			
			free(il->bcs);
			free(il->occs);
			il->bcs = newbcs;
			il->bcsSize = bcpos;
			il->occs = newoccs;						
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
					if (list[i] != (occList[id][i]-DOCid_ADD) ) {
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
					if (list[i] != (occList[id][i]-DOCid_ADD)) {
						//fprintf(stderr,"\n decoding of lists failed for id = %d: DIFFERENT!!",id);
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

	/** Saving lenList vector */
	{
		uint size    = il->nlists * sizeof(uint);

		write(file, &(il->nlists), sizeof(uint));		
		write(file, il->lenList, size);
	}
	
	/** Saving the postings lists using bytecodes or bytecodes+lzma */
	{
		//fprintf(stderr,"\n saving size ucompressed = %lu!!!!",il->sizeUncompressed);
		write(file, &(il->sizeUncompressed), sizeof(uint));		

		write(file, &(il->bcsSize), sizeof(uint));		

		write(file, il->occs, (il->nlists +1) * sizeof(uint));		
		write(file, il->bcs, il->bcsSize * sizeof(byte));					
	}
	
	/** Saving the bitmap useslzma **/
	{
		if (il->useslzma){	
			write(file, il->useslzma, ((il->nlists + W -1)/W) *  sizeof(uint));
		}				
	}	
				
	
	close(file);				
		
	{ // outputs bcs array to a file
	
	strcpy(filename, basename); strcat(filename, ".");	strcat(filename, "bcs");
	unlink(filename);
	if( (file = open(filename, O_WRONLY|O_CREAT,S_IRWXG | S_IRWXU)) < 0) {
		printf("Cannot open file %s\n", filename);
		exit(0);
	}
		
	write(file, il->bcs, il->bcsSize * sizeof(byte));
	close (file);
	
	}
	
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

	/* loading the posting lists using bytecodes or bytecodes+lzma*/
	{		
		//some integers
		read(file, &(il->sizeUncompressed), sizeof(uint));					
		read(file, &(il->bcsSize), sizeof(uint));		
		
		//vector occs.
		il->occs = (uint *) malloc ((il->nlists+1) * sizeof(uint) );		
		read(file, il->occs, (il->nlists+1) * sizeof(uint)); //wcsa->nwords was loading in "loadVocabulary"

		//vector bcs.
		il->bcs = (byte *) malloc (il->bcsSize * sizeof(byte));		
		read(file, il->bcs, il->bcsSize * sizeof(byte));		
	}		
	
	/** Loading the bitmap useslzma **/
	{
		il->useslzma = (uint *) malloc (sizeof(uint) * ((il->nlists + W -1)/W));
		il->useslzma[((il->nlists + W -1)/W) -1] = 0000;  //valgrind says nothing.
		if (il->useslzma){	
			read(file, il->useslzma, ((il->nlists + W -1)/W) *  sizeof(uint));
		}	
		
		initLzmaStruct (&il->lzmadata);	//structure used during decompression with lzma
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
	if (il->bcs) free(il->bcs);
	if (il->useslzma) free(il->useslzma);
	if (il->lzmadata) freeLzmaStruct(il->lzmadata); 
	free(il);
	return 0;
}

uint  sizetobits(void *ail) {
	uint i; uint maxs, bits_s;
	t_il *il=(t_il *) ail;
	uint total;
	

	// size of lenList
	maxs=0;
	for(uint i=0;i<il->nlists;i++)       //** calcula el m�ximo s�mbolo en symbols_new;
		maxs = max(maxs,il->lenList[i]);
	bits_s = bits(maxs);                   //** numero de bits necesarios para los valores en "symbols_new"
	total = (il->nlists * bits_s / W +1)* (sizeof(uint));   //** valores de symbols_new codificados con "bits_sn" bits.

	//fprintf(stderr,"\n sizeOfLenlist = %d bytes",  (il->nlists * bits_s / W +1));

	// size of occs
	maxs=0;
	for(uint i=0;i<=il->nlists;i++)       //** calcula el m�ximo s�mbolo en symbols_new;
		maxs = max(maxs,il->occs[i]);	
	//maxs = il->occs[il->nlists-1];
	bits_s = bits(maxs);                   //** numero de bits necesarios para los valores en "symbols_new"
	total += ((il->nlists+1) * bits_s / W +1)* (sizeof(uint));   //** valores de symbols_new codificados con "bits_sn" bits.

	//fprintf(stderr,"\n sizeOf occs[] = %d bytes",  (il->nlists * bits_s / W +1));


	return total;
}
 

/** creates a "fun" matlab file with the ratio of
 *  source len of posting and compressed len
 **/  
/** FARI 2011.01.01 --- THIS FUNCTION IS NOT IMPLEMENTED FOR LZMA_IL !!!**/
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
			byte *bcs = il->bcs;
			uint num;
			uint ii;
	
			for (ii=0; ii<lenPost;ii++) {		
				BC_TO_DEC(bcs, bcpos, num);
			}						
			lenPhrase = (bcpos - inibcpos);		 
		}
			
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
	sizeUint += (il->nlists+1) * sizeof(uint) ;   //occs vect

	*size += il->bcsSize ;
	*size += ((il->nlists + W -1)/W ) * sizeof(uint);  //bitmap "usesLZMA"

//	fprintf(stderr,"\n\t size of: lenlist+occs = %d bytes Vs %d bytes (old)",sizetobits(ail),sizeUint);
//	fprintf(stderr,"\n\t size of byteCodes = %d bytes",il->bcsSize);

	fprintf(stderr,"\n\t size of: lenlist+occs = %d bytes; byteCodes = %d bytes;",sizetobits(ail),il->bcsSize);

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

	uint i, bcpos, num, prevvalue;
	bcpos = il->occs[id]; //pos in zoneoccs
	
	uint lenbc = il->occs[id+1]-bcpos;  /**sets "src" and lenbc (for decoding bytecodes)*/
	byte *src = &il->bcs[bcpos];
	
	byte *lzmaDecoderBuffer=NULL;  //lzma decoder will allocated it.
	
	if (shouldUseLZMA(il,id)) {  /**decodes lzma data, and sets "src" and lenbc*/
		readBuffLzmaStruct(src,lenbc, il->lzmadata);
		decodeLzma(il->lzmadata,&lzmaDecoderBuffer,&lenbc);
		src = lzmaDecoderBuffer;
	}
	
	prevvalue=0; bcpos =0; i=0;
	while (bcpos < lenbc) {
		BC_TO_DEC(src, bcpos, num);
		prevvalue +=num;
		alist[i]=prevvalue-DOCid_ADD;
		i++;	
	}
	if (lzmaDecoderBuffer) free(lzmaDecoderBuffer);	
/*	
	for (i=0; i<lenlist;i++) {		
		BC_TO_DEC(bcs, bcpos, num);
		prevvalue +=num;
		alist[i]=prevvalue-DOCid_ADD;			
	}		
*/
	*list =alist;
	*len =i;		
	return 0; 	
}	


    /* Extracts the id-th list, having id \in [0..maxId-1] 
       Assumes list has enough space previously allocated. */

int extractListNoMalloc_il (void *ail, uint id, uint *list, uint *len){

	t_il *il=(t_il *) ail;	

	register uint lenlist  = il->lenList[id];

	uint i, bcpos, num, prevvalue;
	bcpos = il->occs[id]; //pos in zoneoccs
	uint lenbc = il->occs[id+1]-bcpos;

	byte *src = &il->bcs[bcpos];
	byte *lzmaDecoderBuffer=NULL;  //lzma decoder will allocated it.
	
	if (shouldUseLZMA(il,id)) {
		readBuffLzmaStruct(src,lenbc, il->lzmadata);
		decodeLzma(il->lzmadata,&lzmaDecoderBuffer,&lenbc);
		src = lzmaDecoderBuffer;
	}

	prevvalue=0; bcpos =0; i=0;
	while (bcpos < lenbc) {
		BC_TO_DEC(src, bcpos, num);
		prevvalue +=num;
		list[i]=prevvalue-DOCid_ADD;
		i++;	
	}
	if (lzmaDecoderBuffer) free(lzmaDecoderBuffer);		
/*		
	for (i=0; i<lenlist;i++) {		
		BC_TO_DEC(bcs, bcpos, num);
		prevvalue +=num;
		list[i]=prevvalue-DOCid_ADD;			
	}		
*/	
	*len =i;		
	return 0; 			
}	


/*       
int extractListNoMalloc_il (void *ail, uint id, uint *list, uint *len){
	t_il *il=(t_il *) ail;	
	register uint lenlist;
	
	byte *bcs  = il->bcs;// + (il->bcsptr[id]);
	lenlist  = il->lenList[id];
	uint i, bcpos, num, prevvalue;
	bcpos = il->occs[id]; //pos in zoneoccs
	prevvalue=0;
	
	for (i=0; i<lenlist;i++) {		
		BC_TO_DEC(bcs, bcpos, num);
		prevvalue +=num;
		list[i]=prevvalue-DOCid_ADD;			
	}		
	
	*len =i;		
	return 0; 			
}	
*/


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
			else if (strcmp(parameters[j+1], "merge") == 0 )
				il->defs.defaultIntersectN = mergeN;					
			j++;
		}
		//setting the default fSearch function used in svs-based algoritms.		
		else if  ((strcmp(parameters[j], "fsearch") == 0 ) && (j < num_parameters-1) ) {
			if (strcmp(parameters[j+1], "unc") == 0 ) 
				il->defs.defaultFsearch = fsearchUnc;
			//else if (strcmp(parameters[j+1], "dec") == 0 ) 
			//	il->defs.defaultFsearch = fsearchDec;
			//else if (strcmp(parameters[j+1], "seq") == 0 ) 
			//	il->defs.defaultFsearch = fsearchSeq;
			else if (strcmp(parameters[j+1], "seqM") == 0 ) 
				il->defs.defaultFsearch = fsearchSeqMem;							
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

inline int fsearchSeqMem (void *ail, uint id, uint value) {
	//fprintf(stderr,"\n Call to Fsearch-seq-M");
	
	t_il *il=(t_il *) ail;	
	
	value +=DOCid_ADD;
	
	byte *bcs = il->bcs;
	register uint lenlist  = il->lenList[id];
	register uint j;

	uint bcpos, num, val;
	
	bcpos = il->bcpos;	
	j=il->j;
	val=il->val;
	
	for ( ; (j<lenlist) && (val < value); ) {		
		BC_TO_DEC(bcs, bcpos, num);
		val +=num;
		j++;
	}

	il->bcpos = bcpos;
	il->j = j;
	il->val = val;
		
	if ((value == val)) {
		return 1;
	}
	else {		
		return 0;
	}	

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
	//{uint i;
	//fprintf(stderr,"\n Call to svs2:: ");
	//fprintf(stderr,"[%d][%d]",id1,id2);
	//}

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

		n=il->lenList[first];
		extractList_il (ail, first, &firstlist, noccs); //reusing *noccs.
		n=*noccs;
		matches =0;
		
		il->j=0;	 //keeps memory of previous search
		il->bcpos = il->occs[second];
		il->val = 0;				
		
		for (j=0; j<n;j++) {
			result = (*defaultFsearch) (ail, second, firstlist[j]); //call to fsearch;
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

	return merge2ByteCodesLZMA(ail,id1,id2,noccs,occs);	
	
//	uint ids[2];
//	ids[0]=id1; ids[1]=id2;
//	
//	int x= mergeN (ail, ids, 2, noccs, occs);
//	
//	return x;
}


	/*  Intersects 2 posting-lists using merge algorithm (zipper-Sanders)
	    Returns *occs (the result), noccs (number of elements)
	    Memory for *occs is allocated, and must be freed by the caller */


int merge2ByteCodesLZMA (void *ail, uint id1, uint id2, uint *noccs, uint **occs){
	//fprintf(stderr,"\n Call to merge2");

	t_il *il=(t_il *) ail;	

	int GAP1=0;
	int GAP2=1;
	
	register uint  n1, n2;
	uint bcpos1,bcpos2;
	byte *src1, *src2;

	n1= il->lenList[id1]; //reading the first value
	bcpos1 = il->occs[id1]; //pos in zoneoccs
	uint lenbc1 = il->occs[id1+1] -bcpos1;
	src1= &il->bcs[bcpos1];
	
	n2= il->lenList[id2]; //reading the first value
	bcpos2 = il->occs[id2]; //pos in zoneoccs
	uint lenbc2 = il->occs[id2+1] - bcpos2;
	src2= &il->bcs[bcpos2];
	
	bcpos1=bcpos2= 0;

	//uint *list = (uint *) malloc ( n1 * sizeof(uint));
	uint matches=0;
	
	byte *lzmaDecoderBuffer1=NULL;		//lzma decoder will allocate it.
	byte *lzmaDecoderBuffer2=NULL;		//lzma decoder will allocate it.
		
	if (shouldUseLZMA(il,id1)) {  /**decodes lzma data, and sets "src" and lenbc*/				  
		readBuffLzmaStruct(src1,lenbc1, il->lzmadata);
		decodeLzma(il->lzmadata,&lzmaDecoderBuffer1,&lenbc1);
		src1 = lzmaDecoderBuffer1;
	}		
	if (shouldUseLZMA(il,id2)) {  /**decodes lzma data, and sets "src" and lenbc*/		
		readBuffLzmaStruct(src2,lenbc2, il->lzmadata);
		decodeLzma(il->lzmadata,&lzmaDecoderBuffer2,&lenbc2);
		src2 = lzmaDecoderBuffer2;
	}		
				
	merge2uncBCS (id1,n1,src1,id2,n2,GAP1,GAP2, src2,noccs, occs);		
		
	if (lzmaDecoderBuffer1) free(lzmaDecoderBuffer1);
	if (lzmaDecoderBuffer2) free(lzmaDecoderBuffer2);

	return 0;
	
}

/* intersects two streams of byte-codes belonging to term i1, and i2 
 * src1 and src2 point to those streams (from bcs[] of decoded from 
 * lzma_data at bcs[])                                            */ 
int merge2uncBCS (uint id1, uint n1, byte *src1, uint id2, uint n2, int GAP1, int GAP2, byte* src2, uint *noccs, uint **occs){
	uint i1,i2;
	uint bcpos1,bcpos2;
	int num;
	int val1, val2;
	
	uint *list = (uint *) malloc ( n1 * sizeof(uint));
	uint matches=0;

	i1=1;i2=1;
	val1=val2=0;
	bcpos1=bcpos2=0;
	BC_TO_DEC(src1, bcpos1, num);	
	val1 += (int)num-GAP1;
	BC_TO_DEC(src2, bcpos2, num);	
	val2 += (int)num-GAP2;
	
	//fprintf(stderr,"\n call to merge2lzma, id1=%u, id2=%u, n1=%u, n2=%u ", id1,id2,n1,n2);
	
	//do {
	while ((i1 < n1) && (i2<n2)) {	
        /***/
		if (val1 == val2) {
			list[matches++] = val1-DOCid_ADD;

			//next for id1
			{
				BC_TO_DEC(src1, bcpos1, num);		
				val1 +=num;
				i1++;
			}
			
			//next for id2
			{	BC_TO_DEC(src2, bcpos2, num);		
				val2 +=num;
				i2++;
			}	
		}

        /***/
		else if (val1 < val2) {
			//moves i1 
			while ((i1<n1) && (val1 < val2)) {
				BC_TO_DEC(src1, bcpos1, num);		
				val1 +=num;
				i1++;
			}	
		}
        /***/
		else {//if (val1 > val2) {
			//moves i2 
			while ((i2<n2) && (val2 < val1)) {
				BC_TO_DEC(src2, bcpos2, num);		
				val2 +=num;
				i2++;				
			}	
		}				
	}
	//while ((i1 < n1) && (i2<n2));

	//After merge-loop, but an occurrence could still occurr
	//if val1==val2 or 
	//if i_x==n_x, but i_y<n_y  and val_y < val_x ==> we should move on list_y while (i_y<n_y)&& (val_y<val_x)
	if (i1 < n1) { // posting1-not ended.
		while ((i1<n1) && (val1 < val2)) {
			BC_TO_DEC(src1, bcpos1, num);
			val1 +=num;
			i1++;
		}			
	}
	else if (i2 < n2) { // posting1-not ended.
		while ((i2<n2) && (val2 < val1)) {
			BC_TO_DEC(src2, bcpos2, num);		
			val2 +=num;
			i2++;				
		}				
	}

	//finally checking if the last 2-values are identical or not
	if (val1 == val2) {
		list[matches++] = val1 -DOCid_ADD;
	}	
		
	*occs = list;
	*noccs= matches;
	return 0;
}






	/* Intersects 2 posting lists represented with bytecodes, using merge */
/*
int merge2ByteCodes (void *ail, uint id1, uint id2, uint *noccs, uint **occs){
	//fprintf(stderr,"\n Call to merge2 bytecodes %d %d", id1,id2);

	t_il *il=(t_il *) ail;	
	
	byte *bcs = il->bcs;

	//fprintf(stderr,"\nDecoding all the %d occurrences of word[%d]= <<%s>>",lenlist, id, wcsa->words[id]);fflush(stderr);		 		
	register uint  n1, n2;
	uint i1,i2;
	uint bcpos1,bcpos2, val1, val2, num;

	n1= il->lenList[id1]; //reading the first value
	bcpos1 = il->occs[id1]; //pos in zoneoccs
	i1=1; 

	
	n2= il->lenList[id2]; //reading the first value
	bcpos2 = il->occs[id2]; //pos in zoneoccs
	i2=1;

	uint *list = (uint *) malloc ( n1 * sizeof(uint));
	uint matches=0;

	val1=val2=0;
	BC_TO_DEC(bcs, bcpos1, num);	
	val1 +=num;
	BC_TO_DEC(bcs, bcpos2, num);	
	val2 +=num;
	
	//do {
	while ((i1 < n1) && (i2<n2)) {	
        // *** /
		if (val1 == val2) {
			list[matches++] = val1-DOCid_ADD;

			//next for id1
			{
				BC_TO_DEC(bcs, bcpos1, num);		
				val1 +=num;
				i1++;
			}
			
			//next for id2
			{	BC_TO_DEC(bcs, bcpos2, num);		
				val2 +=num;
				i2++;
			}	
		}


        // *** /
		else if (val1 < val2) {
			//moves i1 
			while ((i1<n1) && (val1 < val2)) {
				BC_TO_DEC(bcs, bcpos1, num);		
				val1 +=num;
				i1++;
			}	
		}
        // *** /
		else {//if (val1 > val2) {
			//moves i2 
			while ((i2<n2) && (val2 < val1)) {
				BC_TO_DEC(bcs, bcpos2, num);		
				val2 +=num;
				i2++;				
			}	
		}				
	}
	//while ((i1 < n1) && (i2<n2));

	//After merge-loop, but an occurrence could still occurr
	//if val1==val2 or 
	//if i_x==n_x, but i_y<n_y  and val_y < val_x ==> we should move on list_y while (i_y<n_y)&& (val_y<val_x)
	if (i1 < n1) { // posting1-not ended.
		while ((i1<n1) && (val1 < val2)) {
			BC_TO_DEC(bcs, bcpos1, num);
			val1 +=num;
			i1++;
		}			
	}
	else if (i2 < n2) { // posting1-not ended.
		while ((i2<n2) && (val2 < val1)) {
			BC_TO_DEC(bcs, bcpos2, num);		
			val2 +=num;
			i2++;				
		}				
	}

	//finally checking if the last 2-values are identical or not
	if (val1 == val2) {
		list[matches++] = val1 -DOCid_ADD;
	}

	*occs = list;
	*noccs= matches;
	return 0;	
}
*/
/******************************************************************/
	    
	    
	    
 
	    



/****************************************************************************************/
/********************  intersection of N lists     **************************************/
/****************************************************************************************/
	
	/*  Intersects n posting-lists using set-versus-set
	    Returns *occs (the result), noccs (number of elements)
	    Memory for *occs is allocated, and must be freed by the caller */
int svsn (void *ail, uint *ids, uint nids, uint *noccs, uint **occs) {
	*noccs=0;
	return 99;
}



/* auxiliar function used in mergeN                                    
 * V is an array of integers (uncompressed)
 * merges V with the lists of occurrences of the list "id2".
 * Returns the resulting intersection into V. No extra alloc is done.
 * noccs is set to the lenght of the resulting "intersection" array.
 */
int merge2_UINT_BCS (uint *V, uint n1, uint id2, uint n2, int GAP1, int GAP2, byte* src2, uint *noccs){
	uint i1,i2;
	uint bcpos1,bcpos2;
	
	int num;
	int val1, val2;

	uint *list = V;
	uint matches=0;

	i1=1;i2=1;
	val1=val2=0;
	
	bcpos1=bcpos2=0;	 
	val1 = (int)V[bcpos1]-GAP1 +DOCid_ADD ; bcpos1++;
	
	BC_TO_DEC(src2, bcpos2, num);	
	val2 += (int)num - GAP2;
		
	//do {
	while ((i1 < n1) && (i2<n2)) {	
        /***/
        //printf(" **val1 = %u, val2= %u",val1,val2);
        //exit(0);
		if (val1 == val2) {
			list[matches++] = val1-DOCid_ADD;

			//next for id1
			{
				val1 = (int)V[bcpos1]-GAP1+DOCid_ADD; bcpos1++;						
				i1++;
			}
			
			//next for id2
			{	BC_TO_DEC(src2, bcpos2, num);		
				val2 +=num;
				i2++;
			}	
		}

        /***/
		else if (val1 < val2) {
			//moves i1 
			while ((i1<n1) && (val1 < val2)) {
				val1 = (int)V[bcpos1]-GAP1+DOCid_ADD; bcpos1++;						
				i1++;
			}	
		}
        /***/
		else {//if (val1 > val2) {
			//moves i2 
			while ((i2<n2) && (val2 < val1)) {
				BC_TO_DEC(src2, bcpos2, num);		
				val2 +=num;
				i2++;				
			}	
		}				
	}
	//while ((i1 < n1) && (i2<n2));

	//After merge-loop, but an occurrence could still occurr
	//if val1==val2 or 
	//if i_x==n_x, but i_y<n_y  and val_y < val_x ==> we should move on list_y while (i_y<n_y)&& (val_y<val_x)
	if (i1 < n1) { // posting1-not ended.
		while ((i1<n1) && (val1 < val2)) {
			val1 = (int)V[bcpos1]-GAP1 +DOCid_ADD; bcpos1++;			
			i1++;
		}			
	}
	else if (i2 < n2) { // posting1-not ended.
		while ((i2<n2) && (val2 < val1)) {
			BC_TO_DEC(src2, bcpos2, num);		
			val2 +=num;
			i2++;				
		}				
	}

	//finally checking if the last 2-values are identical or not
	if (val1 == val2) {
		list[matches++] = val1 -DOCid_ADD;
	}	
		
	*noccs= matches;
	return 0;
}


	
/*  Intersects n posting-lists using merge over 2 lists n-1 times
	Returns *occs (the result), noccs (number of elements)
	Memory for *occs is allocated, and must be freed by the caller */
	
int mergeN (void *ail, uint *ids, uint nids, uint *noccs, uint **occs){
	//fprintf(stderr,"\n Call to svsn hybrid: bitmap + bytecodes");	
	t_il *il = (t_il *) ail;
	
	//{uint i;
	//fprintf(stderr,"\n Call to mergeN:: ");
	//for (i=0;i<nids;i++) fprintf(stderr,"[%d]",ids[i]);
	//}

	//sorting ids ==> lists are sorted by frequency !!
	register uint i, numbytecodes;
	tsort *idss = (tsort *) malloc (sizeof(tsort) * nids);

	numbytecodes=0; 
	for (i=0;i<nids;i++) {
			idss[numbytecodes].value=ids[i]; 
			idss[numbytecodes].key = il->lenList[ids[i]];
			idss[numbytecodes].gap = i;
			numbytecodes++;
	}
	


	
	{  //bytecodes (& could also be bytecodes+lzma) --> extract 1st + svsN.

		int GAP1=0;
		int GAP2=0;
		
		int result;
		register uint j, n, matches;
		uint *firstlist;

		if (numbytecodes >1)	//sorting the bytecodes by lenList[]
			simplesortAsc(idss,numbytecodes); 

		extractList_il (ail, idss[0].value, &firstlist, noccs); //reusing *noccs.
		GAP1=idss[0].gap;
		n=*noccs;

		matches =n;	
		for (i=1; i<numbytecodes; i++){ // the "nids-1" remaining posting-lists.
			matches =0;	
			
			uint id2 = idss[i].value;
			    GAP2 = idss[i].gap;
			uint n2= il->lenList[id2]; //reading the first value
			uint bcpos2 = il->occs[id2]; //pos in zoneoccs
			byte *src2= &il->bcs[bcpos2];

			byte *lzmaDecoderBuffer=NULL;		//lzma decoder will allocate it.					
			if (shouldUseLZMA(il,id2)) {  /**decodes lzma data, and sets "src" and lenbc*/	
				uint lenbc2 = il->occs[id2+1] - bcpos2;			  
				readBuffLzmaStruct(src2,lenbc2, il->lzmadata);
				decodeLzma(il->lzmadata,&lzmaDecoderBuffer,&lenbc2);
				src2 = lzmaDecoderBuffer;
			}
						
			merge2_UINT_BCS(firstlist,n,id2,n2,GAP1,GAP2, src2, &matches);
			
			if (lzmaDecoderBuffer) free(lzmaDecoderBuffer);
									
			if (!matches) break;  //search ends.
			n=matches;
			GAP1=0;
		}

		*occs= firstlist;
		*noccs = matches;					
	}
	free(idss);
	
	
//	for (int i=0; i< (*noccs); i++) printf("\n [%u]",(*occs)[i]);
	
	return 0;	
}


