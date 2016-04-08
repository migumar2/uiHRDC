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




/* ********************************************************************************/
/*  Implementation of the General interface:                                      */
/*              interface for using compressed representations of posting lists   */
/*              Any alternative representation must implement all these functions */
/**********************************************************************************/


	/* returns a string with the parameters available for building the method */

char *showParameters_il(){
	static char text[200];
	sprintf(text, "no parameters needed");
	return text;	
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

	
	if (build_options != NULL) {
	parse_parameters(build_options,&num_parameters, &parameters, delimiters);
	for (j=0; j<num_parameters;j++) {
	  if ((strcmp(parameters[j], "filename") == 0 ) && (j < num_parameters-1) ) {
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
				
		printf("numOflists = %ld, maxPost = %ld,  \n", 	
											(long)il->nlists, (long)maxPostValue);
	
		//calculates an stimation of the uncompressed representation of the index (using uints per posting)
		il->sizeUncompressed = sizeof(uint) * (il->nlists) +  //sizeoflenList
		                       sizeof(uint*) * (il->nlists); //pointers to each posting (occs[i])
		                       
		for (i=0; i< il->nlists;i++) { il->sizeUncompressed += (lenList[i] * sizeof(uint));}

		fprintf(stderr,"estimated mem-usage of uncompressed posting-lists = %ld bytes\n", (long)il->sizeUncompressed);

		
		/** 5 ** Encoding the occurrences **/
		
		il->occs = (uint *) malloc (sizeof(uint) * il->nlists);
		
		uint maxlenlist=0;
		for (i=0; i< il->nlists;i++) { if (maxlenlist<lenList[i]) maxlenlist=lenList[i];}
		fprintf(stderr,"\n maxlenlist = %lu",(ulong)maxlenlist);

		uint *DIFF= (uint *) malloc (sizeof(uint)*(maxlenlist+28));  //stores diferences.
		uint *C= (uint *) malloc (sizeof(uint)*(maxlenlist+28));            //compressed data. 
		
	
			/** 5.b **
				Computes the size of bc vector by simulating encoding process of gaps.
				Sets also bitmaps for those terms indexed with a bitmap.
				Computes and sets the values for "occs[]".
			*/
			uint bcssize=0;
			{
				uint nwords = il->nlists;
				uint id,j;
				uint prevvalue, gap;
				uint len;
				
				uint counterValues =0; 
				for (id=0; id<nwords; id++) {
					len= il->lenList[id];
					
					//Calculates DIFF.
						prevvalue= 0;	
						il->occs[id] = bcssize;
						for (j=0; j<len; j++){
							gap = occList[id][j]-prevvalue;
							DIFF[j]=gap; 
							prevvalue = occList[id][j];
							//SIZE_DEC_TO_BC(gap,numbytes);
							//bcssize += numbytes;
							counterValues++;
						}

					int N = s9_Compression(DIFF, C, (int) len);
					bcssize+= N;

				}
				fprintf(stderr,"\n Number of integers to store is = %lu", (ulong)counterValues);	
				fprintf(stderr,"\n Number of integers(s9) required by simple-9 is= %lu (%lu bytes)",(ulong) bcssize,(ulong) (bcssize*sizeof(uint)));  //integers for s9-compressed data.
			}
		
		il->cdataSize = bcssize;
		il->cdata = (uint *)malloc (sizeof(uint) * il->cdataSize);
		il->cdata[il->cdataSize -1] = 0000;
			
		//il->bcsSize = bcssize;
		//il->bcs = (byte *) malloc (sizeof(byte) * il->bcsSize);		
		
		/** 5.c ** encodes gaps into cdata */
			{
				uint nwords = il->nlists;
				uint *cdata = il->cdata;
				uint id,j;
				uint prevvalue, gap;
				uint len;

				for (id=0; id<nwords; id++) {
					len= il->lenList[id];
					
					//Calculates DIFF.
						prevvalue= 0;	
						for (j=0; j<len; j++){
							gap = occList[id][j]-prevvalue;
							DIFF[j]=gap; 
							prevvalue = occList[id][j];
						}

					int N = s9_Compression(DIFF, cdata, (int) len);
					cdata = cdata + N;
				}
				
				fprintf(stderr,"\n Gaps were encoded with s9 ==> %lu uints (%lu)",(ulong) il->cdataSize, (ulong) (cdata - il->cdata));
			}		

		free(DIFF);
		free(C);


		/** checking that the lists are decoded successfully *************/

		fprintf(stderr,"\n ! CHECKING that the decoded lists are identical to the original ones. ");
		{ //checking:: decoding a list
			uint i, id ,len;
			uint *list;
			uint counterr=0;
			
			for (id=0;id < il->nlists;id++) {
				extractList_il (*ail, id, &list, &len);		
				for (i=0;i<len;i++){
					if (list[i] != (occList[id][i]-DOCid_ADD) ) {
						fprintf(stderr,"\n decoding of lists failed for id = %d: DIFFERENT!!: (%d,%d)",id, list[i],occList[id][i]);
						
						counterr++; if (counterr>20) exit(0);
					}
				} 
				
				free(list);				
			}			
		}
		fprintf(stderr,"\n ! Decoding the list of occurrences worked fine (the same as previous values) ");

		fprintf(stderr,"\n ! CHECKING that the decoded lists are identical to the original ones (extract_no_malloc). ");
		{ //checking:: decoding a list
			uint i, id ,len;
			uint *list = (uint *) malloc(sizeof(uint) * (il->maxPostingValue+1+28));
			
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

	/** Saving lenList vector */
	{
		uint size  = il->nlists * sizeof(uint);

		write(file, &(il->nlists), sizeof(uint));		
		write(file, il->lenList, size);
	}
	
	/** Saving the postings lists using bytecodes. */
	{
		//fprintf(stderr,"\n saving size ucompressed = %lu!!!!",il->sizeUncompressed);
		write(file, &(il->sizeUncompressed), sizeof(uint));		
		write(file, &(il->cdataSize), sizeof(uint));		

		write(file, il->occs, il->nlists * sizeof(uint));		
		write(file, il->cdata, il->cdataSize * sizeof(uint));					
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

	/* loading the posting lists using bytecodes*/
	{		
		//some integers
		read(file, &(il->sizeUncompressed), sizeof(uint));					

		read(file, &(il->cdataSize), sizeof(uint));		
		//vector occs.
		il->occs = (uint *) malloc (il->nlists * sizeof(uint) );		
		read(file, il->occs, il->nlists * sizeof(uint)); //wcsa->nwords was loaded in "loadVocabulary"

		//vector cdata.
		il->cdata = (uint *) malloc (il->cdataSize * sizeof(uint));		
		il->cdata[il->cdataSize -1] = 0000;
		read(file, il->cdata, il->cdataSize * sizeof(uint));		
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
	if (il->cdata) free(il->cdata);
	free(il);
	return 0;
}



/** creates a "fun" matlab file with the ratio of
 *  source len of posting and compressed len
 **/  
int buildBell_il(void *ail, const char *matlabfilename) {
	
  return 99;
}



uint  sizetobits(void *ail) {
	uint maxs; uint bits_s;
	t_il *il=(t_il *) ail;
	uint total;
	
	// size of lenList
	maxs=0;
	for(uint i=0;i<il->nlists;i++)      
		maxs = max(maxs,il->lenList[i]);
		
	bits_s = bits(maxs);                   //** numero de bits necesarios para los valores en "symbols_new"
	total = (il->nlists * bits_s / W +1)* (sizeof(uint));   //** valores de symbols_new codificados con "bits_sn" bits.

	// size of occs
	maxs=0;
	for(uint i=0;i<il->nlists;i++)       //** calcula el m�ximo s�mbolo en symbols_new;
		maxs = max(maxs,il->occs[i]);	
	//maxs = il->occs[il->nlists-1];
	bits_s = bits(maxs);                   //** numero de bits necesarios para los valores en "symbols_new"
	total += (il->nlists * bits_s / W +1)* (sizeof(uint));   //** valores de symbols_new codificados con "bits_sn" bits.

	return total;
}
 

	/* Gives the memory occupied by index in bytes. */

int size_il(void *ail, uint *size) {
	t_il *il=(t_il *) ail;
	*size=0;
	*size += sizeof(t_il);
	
//	*size += (il->nlists + 1)* sizeof(uint);  //il->bcsptr 

	*size += sizetobits(ail);

	uint sizeUint=0;
	sizeUint += il->nlists * sizeof(uint) ;   //lenList vect
	sizeUint += il->nlists * sizeof(uint) ;   //occs vect

	*size += il->cdataSize*sizeof(uint) ;

	fprintf(stderr,"\n\t size of: lenlist+occs = %lu bytes; s9-data = %lu bytes",(ulong)sizetobits(ail), (ulong) il->cdataSize*sizeof(uint));

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
	register uint i;
		
	uint *alist = (uint *) malloc (sizeof(uint) * (lenlist + 28));	
	uint *cdata  = il->cdata + (il->occs[id]);

	s9_Decompression(cdata, alist, (int) lenlist);
	
	alist[0] -= DOCid_ADD;	
	for (i=1; i<lenlist;i++) {		
		alist[i]=alist[i]+alist[i-1];//-DOCid_ADD;			
	}		
	
	*list =alist;
	*len =lenlist;		
	return 0; 	
}	


    /* Extracts the id-th list, having id \in [0..maxId-1] 
       Assumes list has enough space previously allocated. */
       
int extractListNoMalloc_il (void *ail, uint id, uint *list, uint *len){
	t_il *il=(t_il *) ail;	

	register uint lenlist  = il->lenList[id];
	register uint i;
		
	uint *cdata  = il->cdata + (il->occs[id]);	
	
	s9_Decompression(cdata, list, (int) lenlist);

	list[0] -= DOCid_ADD;
	for (i=1; i<lenlist;i++) {		
		list[i]= list[i]+list[i-1];//-DOCid_ADD;			
	}		
		
	*len =lenlist;		
	return 0; 			
}	



/**********************************************************************************/
/**********************************************************************************/



    /* Sets options for searches: such as parameters, funtions to use, etc 
       Permits to set different algorithms for intersect2, intersectN and 
       if needed "fsearch-operations needed by svs-type algorithms"     */

int setDefaultSearchOptions_il (void *ail, char *search_options){

	t_il *il = (t_il *) ail;
	
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
			if (strcmp(parameters[j+1], "merge") == 0 )
				il->defs.defaultIntersect2 = merge_2_uncompressFst;	  				
			else if (strcmp(parameters[j+1], "mergeS9") == 0 ) 
				il->defs.defaultIntersect2 = merge_2_unc_s9;	
						  
			j++;
		}
		//setting the default function to Intersect-N-lists		
		else if  ((strcmp(parameters[j], "intn") == 0 ) && (j < num_parameters-1) ) {
			if (strcmp(parameters[j+1], "merge") == 0 ) 
				il->defs.defaultIntersectN = merge_N_uncompressFst;		
			if (strcmp(parameters[j+1], "mergeS9") == 0 ) 
				il->defs.defaultIntersectN = merge_N_unc_s9;
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
	
//	{uint i;
//	for (i=0;(i<10) && (i<*noccs);i++) printf("\n occs[%u] = %u",i,(*occs)[i]);
//	}

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



/****************************************************************************************/
/** Optimized merge2 and mergeN for s9 **************************************************/
/****************************************************************************************/
/* The first list is extracted, and the others are decompressed block-wise 
 * (1 compressed s9-uint at a time), and the decoded integers intersected with
 * the first list.
 * This avoids decompression + iterating over the decompressed secuence twice
 */

/******************************************************************************/
int merge_2_unc_s9 (void *ail, uint id1, uint id2,  uint *noccs, uint **occs){
	t_il *il=(t_il *) ail;
	
	uint len1;
    uint* arr1;
    extractList_il(ail, id1, &arr1, &len1);
    uint len2;
    //uint* arr2;
    //extractList_il(ail, id2, &arr2, &len2);
    //*occs = intersect_aux(arr1,len1,arr2,len2,noccs);
    
    lenlist(ail,id2,&len2);
    //arr2 = (uint *) malloc (sizeof(uint) * (28+ len2));
	uint *cdata  = il->cdata + (il->occs[id2]);
	uint *ptr=arr1;
	uint *ptrend = arr1 +len1;
    s9_DecompressionAndIntersect(cdata,(int) len2, ptr, ptrend, noccs);
    //free(arr2);

    *occs = arr1;
    
    //free(arr1);
	//if (!(*noccs)) {free(*occs); *occs=NULL;}
    return 0;

}

int merge_N_unc_s9 (void *ail, uint *ids, uint nids, uint *noccs, uint **occs){
	t_il *il=(t_il *) ail;
	
    lpair* lengths = (lpair*)malloc(sizeof(lpair)*nids);
    for(unsigned int i=0;i<nids;i++){
        lengths[i].pos = ids[i];
        lenlist(ail, ids[i], &(lengths[i].len));
    }
    qsort(lengths, nids, sizeof(lpair), &lpair_compare);
    uint* auxArr;
    uint auxLen;

    extractList_il(ail, lengths[0].pos, &auxArr, &auxLen);
    for(unsigned int i=1;(i<nids)&&(auxLen);i++){
        uint len;
        uint id=lengths[i].pos;
        
		lenlist(ail,id,&len);
		uint *cdata  = il->cdata + (il->occs[id]);
		uint *ptr=auxArr;
		uint *ptrend = auxArr +auxLen;
		s9_DecompressionAndIntersect(cdata,(int) len, ptr, ptrend, &auxLen);
    	//printf("\n next merge = %lu occs (len1=%u, len2=%u)",auxLen, auxLen, len);
    }
    free(lengths);
    *occs = auxArr;
    *noccs = auxLen;
    return 0;	
}

/*
 * Proceses the $size$ compressed-uints in the compressed sequence $input$ (1 uint at a time)
 * For each iteration, up to 28 integers ($num$) are decoded into $output$.
 * 	 In addition, intersection of those $num$ elements is done against the array 
 *   ptri[] (ptrend -ptri = size of ptri).
 * The resulting intersection is returned into ptri[] and $matches$ is set to 
 * mark the new size of ptri[]
 */
void s9_DecompressionAndIntersect(unsigned int* input, int size, uint *ptri, uint *ptrend, uint *matches) {
	unsigned int output[28];	
	unsigned int *_p = output;
	int num;
	unsigned* tmp = input;

	uint value =0 + (DOCid_ADD);
	uint *ptr_prev;
	uint *I = ptri;
	int j;

	uint match=0;

	int left = size;
	while (left > 0) {
		_p = ptr_prev = output;
		num = s9_decode(tmp,&_p);    
		tmp++;  // 1 integer that has been decoded (and there were "num" values encoded within it).
		left -= num;

		/* num gaps were decompressed */
		for (j=0; (j<num)&&(ptri<ptrend) ;j++) {
			value += *ptr_prev++;
			if (value == *ptri){
				ptri++;
				I[match++]=value;
			}
			else if (value > *ptri){
				while ((ptri<ptrend) && (*ptri < value)) {
					ptri++;
				}
				if (ptri<ptrend) {
					if (value ==*ptri) {
						ptri++;
						I[match++]=value;
					}
				}
			}
		//	if (ptri == ptrend) break;
		}    
	}
	*matches = match;
//	return tmp - input;
}
/******************************************************************************/

