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
	sprintf(text, "lenBitmapDiv=%d",DEFAULT_LEN_BITMAP_DIV);
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
      [Nwords=N][maxPostvalue][lenPost1][post11][post12]...[post1k�]
                              [lenPost2][post21][post22]...[post2k��]
                               ... 
                              [lenPostN][postN1][postN2]...[postNk���]
      il is an opaque data type. Any build option must be passed in string 
      build_options, whose syntax depends on the index. The index must 
      always work with some default parameters if build_options is NULL. 
      The returned structure is ready to be queried. */


int build_il (uint *source, uint length, char *build_options, void **ail) {
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
	  if ((strcmp(parameters[j], "filename") == 0 ) && (j < num_parameters-1) ) {
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
	
	printf("\n parameters of method:  lenBitmapDiv= %d, filename = %s\n", lenBitmapDiv, filename);


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
		il->lenBitmapThreshold = (maxPostValue+1) / lenBitmapDiv;  //+1 due to ids in [0.. maxPostValue]
				
		printf("numOflists = %ld, maxPost = %ld, lenBitmapThreshold = %ld \n", 	
											il->nlists, maxPostValue, il->lenBitmapThreshold);
	
		//calculates an stimation of the uncompressed representation of the index (using uints per posting)
		il->sizeUncompressed = sizeof(uint) * (il->nlists) +  //sizeoflenList
		                       sizeof(uint*) * (il->nlists); //pointers to each posting (occs[i])
		                       
		for (i=0; i< il->nlists;i++) { il->sizeUncompressed += (lenList[i] * sizeof(uint));}

		fprintf(stderr,"stimated mem-usage of uncompressed posting-lists = %ld bytes\n", il->sizeUncompressed);

		
		/** 5 ** Encoding the occurrences **/
		
		il->occs = (uint *) malloc (sizeof(uint) * il->nlists);
		
			/** 5.0 ** Sets zoneBitmaps[] and numBitmaps; sets also occs for the lists using bitmap. **/
			
			{  //counts the number of terms indexed with bitmaps and reserves memory for them.
							
			  	uint numBitmaps=0;
			  	uint nwords = il->nlists;
			  	 
				for (i=0;i<nwords; i++){
			  		uint option;	
					if (option = shouldUseBitmap(il, i)) numBitmaps++;
				}  	
				fprintf(stderr,"\n Number of lists = %d, %d using bitmaps, %d using variableLenght codes",il->nlists, numBitmaps, il->nlists - numBitmaps);
			  					
				il->numBitmaps = numBitmaps;
				il->bits_bitmaps = il->maxPostingValue+1-DOCid_ADD;  //also docId = [0.. maxPostingValue]
				il->ints_bitmaps_aligned =  (il->bits_bitmaps +W-1) /W;
				
				//if (il->numBitmaps) {
					il->zoneBitmaps = (uint *) malloc (sizeof(uint) *  (il->numBitmaps * il->ints_bitmaps_aligned));
					for (i=0;i< ( il->numBitmaps * il->ints_bitmaps_aligned); i++)
						il->zoneBitmaps[i] = 0000;   //so no further "set to zero" is needed for each bitmap.
				//}
				//else  il->zoneBitmaps = NULL;						
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
				uint prevvalue, gap, numbytes;
				uint len, sampleP;
				
				uint counterBCcodes =0; 
				uint counterBitmaps=0;
				for (id=0; id<nwords; id++) {
					len= il->lenList[id];
					
					if (! shouldUseBitmap(il,id) ) { //** spire **/					
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
					else { //using a bitmap...
						uint iniPosBitmap = counterBitmaps * il->ints_bitmaps_aligned * W;
						il->occs[id] = counterBitmaps;
						for (j=0;j<len;j++)  
							bitset(il->zoneBitmaps, (iniPosBitmap + occList[id][j] - DOCid_ADD));							
						counterBitmaps++;
					}
				}
				fprintf(stderr,"\n\n Active bits for %d bitmaps where already set.",counterBitmaps);						
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
				uint len;

				uint bcpos=0;
								
				for (id=0; id<nwords; id++) {
					len= il->lenList[id];	
					prevvalue=0;
					
					if ( ! shouldUseBitmap(il,id) ) {											
						for (j=0; j<len; j++){
							gap = occList[id][j]-prevvalue;
							prevvalue = occList[id][j];
							DEC_TO_BC(bcs, bcpos, gap);  //bcpos is increased.
						}
					}
				}
				
				fprintf(stderr,"\n Gaps were encoded with bc ==> %d bytes",bcpos);
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
	
	/** Saving the postings lists using bitmaps.*/
	{
		write(file, &(il->lenBitmapThreshold), sizeof(uint));		
		write(file, &(il->bits_bitmaps), sizeof(uint));		
		write(file, &(il->ints_bitmaps_aligned), sizeof(uint));	
		write(file, &(il->numBitmaps), sizeof(uint));		

		if (il->numBitmaps){	
			write(file, il->zoneBitmaps, (il->numBitmaps * il->ints_bitmaps_aligned) *  sizeof(uint));
		}		
	}
	/** Saving the postings lists using bytecodes. */
	{
		//fprintf(stderr,"\n saving size ucompressed = %lu!!!!",il->sizeUncompressed);
		write(file, &(il->sizeUncompressed), sizeof(uint));		
		write(file, &(il->lenBitmapThreshold), sizeof(uint));		

		write(file, &(il->bcsSize), sizeof(uint));		

		write(file, il->occs, il->nlists * sizeof(uint));		
		write(file, il->bcs, il->bcsSize * sizeof(byte));					
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
		read(file, &(il->lenBitmapThreshold), sizeof(uint));				

		read(file, &(il->bcsSize), sizeof(uint));		
		
		//vector occs.
		il->occs = (uint *) malloc (il->nlists * sizeof(uint) );		
		read(file, il->occs, il->nlists * sizeof(uint)); //wcsa->nwords was loading in "loadVocabulary"

		//vector bcs.
		il->bcs = (byte *) malloc (il->bcsSize * sizeof(byte));		
		read(file, il->bcs, il->bcsSize * sizeof(byte));		
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
	if (il->zoneBitmaps) free(il->zoneBitmaps);
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
	for(uint i=0;i<il->nlists;i++)       //** calcula el m�ximo s�mbolo en symbols_new;
		maxs = max(maxs,il->occs[i]);	
	//maxs = il->occs[il->nlists-1];
	bits_s = bits(maxs);                   //** numero de bits necesarios para los valores en "symbols_new"
	total += (il->nlists * bits_s / W +1)* (sizeof(uint));   //** valores de symbols_new codificados con "bits_sn" bits.

	//fprintf(stderr,"\n sizeOf occs[] = %d bytes",  (il->nlists * bits_s / W +1));


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
		
		if (!shouldUseBitmap(il,i)){
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

	*size += il->bcsSize ;

//	fprintf(stderr,"\n\t size of: lenlist+occs = %d bytes Vs %d bytes (old)",sizetobits(ail),sizeUint);
//	fprintf(stderr,"\n\t size of byteCodes = %d bytes",il->bcsSize);
//	fprintf(stderr,"\n\t size of bitmaps = %d bytes \n",(( il->numBitmaps * il->ints_bitmaps_aligned) * sizeof(uint) ));
	*size += (( il->numBitmaps * il->ints_bitmaps_aligned) * sizeof(uint) );

	fprintf(stderr,"\n\t size of: lenlist+occs = %d bytes; byteCodes = %d bytes; bitmaps = %d bytes",sizetobits(ail),il->bcsSize,(( il->numBitmaps * il->ints_bitmaps_aligned) * sizeof(uint) ));

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

	byte *bcs  = il->bcs;// + (il->bcsptr[id]);

	uint i, bcpos, num, prevvalue;
	bcpos = il->occs[id]; //pos in zoneoccs
	prevvalue=0;
	
	for (i=0; i<lenlist;i++) {		
		BC_TO_DEC(bcs, bcpos, num);
		prevvalue +=num;
		alist[i]=prevvalue-DOCid_ADD;			
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

		n=il->lenList[first];
		extractList_il (ail, first, &firstlist, noccs); //reusing *noccs.
		n=*noccs;
		matches =0;
		
		if (!shouldUseBitmap(il,second)) { //calculates sample period for the second list
			il->j=0;	 //keeps memory of previous search
			il->bcpos = il->occs[second];
			il->val = 0;				
		}	
		else {
			defaultFsearch=fsearchBitmap;
			//printf("\n *** SHOULD USE BITMAP %d ****",second);
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




/***********************************************************************/


	/*  Intersects 2 posting-lists using merge algorithm (zipper-Sanders)
	    Returns *occs (the result), noccs (number of elements)
	    Memory for *occs is allocated, and must be freed by the caller */


int merge2 (void *ail, uint id1, uint id2, uint *noccs, uint **occs){
	//fprintf(stderr,"\n Call to merge2");

	t_il *il=(t_il *) ail;	
		
	//fprintf(stderr,"\n call to merge2, id1=%u, id2=%u ", id1,id2);
		
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
	//fprintf(stderr,"\n Call to mergeHybrid** %d %d", idbitmap,idbytecode);
	//fflush(stderr);fflush(stdout);
	
	t_il *il=(t_il *) ail;	

	register uint id = idbytecode;	
	register uint lenlist  = il->lenList[id];
	uint *alist = (uint *) malloc (sizeof(uint) * lenlist);	
		
	byte *bcs  = il->bcs;// + (il->bcsptr[id]);

	//fprintf(stderr,"\nDecoding all the %d occurrences of word[%d]= <<%s>>",lenlist, id, wcsa->words[id]);fflush(stderr);		 		
	uint i, bcpos, num, prevvalue;
	bcpos = il->occs[id]; //pos in zoneoccs
	uint matches =0;
	prevvalue =0;
	for (i=0; i<lenlist;i++) {		
		BC_TO_DEC(bcs, bcpos, num);	
		prevvalue +=num;
		if (fsearchBitmap (ail, idbitmap, prevvalue-DOCid_ADD))
			alist[matches++]=prevvalue-DOCid_ADD;
	}		

	*occs = alist;
	*noccs= matches;
	return 0;	
}





	/* Intersects 2 posting lists represented with bytecodes, using merge */
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
	
	do {
        /***/
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


        /***/
		else if (val1 < val2) {
			//moves i1 
			while ((i1<n1) && (val1 < val2)) {
				BC_TO_DEC(bcs, bcpos1, num);		
				val1 +=num;
				i1++;
			}	
		}
        /***/
		else {//if (val1 > val2) {
			//moves i2 
			while ((i2<n2) && (val2 < val1)) {
				BC_TO_DEC(bcs, bcpos2, num);		
				val2 +=num;
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

/******************************************************************/
	    

	    
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
			idss[numbytecodes].key = il->lenList[ids[i]];
			numbytecodes++;
		}
		else {
			//numbitmaps++;
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

		extractList_il (ail, idss[0].value, &firstlist, noccs); //reusing *noccs.
		n=*noccs;

		matches =n;	
		for (i=1; i<numbytecodes; i++){ // the "nids-1" remaining posting-lists.
			matches =0;	

			{ //calculates sample period for the next list
			register uint lenlist  = il->lenList[idss[i].value];			

			il->j=0;	 //keeps memory of previous search
			il->bcpos = il->occs[idss[i].value];
			il->val = 0;	
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
