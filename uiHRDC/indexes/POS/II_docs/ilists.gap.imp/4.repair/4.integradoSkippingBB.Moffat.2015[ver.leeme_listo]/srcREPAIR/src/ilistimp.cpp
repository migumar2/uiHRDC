#include "ilistimp.h"


/***********************************************************************************/
/*** FUNCTIONS USED FOR PARSING PARAMETERS FROM COMMAND LINE ***********************/
/* Three function to variables to manage parameters */
static bool is_delimeter_il(char *delimiters, char c) {
   int i=0,len_delimiters=strlen(delimiters);
   bool is=false;
   for (i=0;i<len_delimiters;i++)
     if (c == delimiters[i]) is=true;
   return is;
}


static void parse_parameters_il(char *options, int *num_parameters, char ***parameters, char *delimiters) {
  int i=0,j=0,temp=0,num=0, len_options=strlen(options);
  char *options_temp;
  while  (i<len_options) {
    while ((i<len_options) && is_delimeter_il(delimiters,options[i])) i++;
    temp=i;
    while ((i<len_options) && !is_delimeter_il(delimiters,options[i])) i++;
    if (i!=temp) num++;
  }
  (*parameters) = (char **) malloc(num*sizeof(char * ));
  i=0;
  while  (i<len_options) {
    while ((i<len_options) && is_delimeter_il(delimiters,options[i])) i++;
    temp=i;
    while ((i<len_options) && !is_delimeter_il(delimiters,options[i])) i++;
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

static void free_parameters_il(int num_parameters,char ***parameters) {
  int i=0;
  for (i=0; i<num_parameters;i++)
    free((*parameters)[i]);
  free((*parameters));
}

/***********************************************************************************/
/***********************************************************************************/


	/* returns a string with the parameters available for building the method */

char *showParameters_il(){
	static char text1[256];
	static char msg[256];
	sprintf(text1,"\"lenBitmapDiv=%u; maxMemRepair= %u (bytes)",DEFAULT_LEN_BITMAP_DIV,DEFAULT_MAX_MEM_REPAIR);
	sprintf(msg,"%s; replacesPerIter = %u; repairBreak=%2.3f %%\"",text1,DEFAULT_REPAIR_REPLACES_PER_ITER, DEFAULT_REPAIR_BREAK);
		
	return msg;	
}


void writeSampleK(char *file, uint sampleK) {
	
	{  //** reads some constants: maxLenPosting and sizeUncompressed from "convert program".
	  char * fname = new char[strlen(file)+50];
	
	  sprintf(fname,"%s.const.samplek",file);
	  unlink (fname);
	  FILE * fp = fopen64(fname,"w");
	  assert(fp!=NULL);
	  size_t rf;
	  rf=fprintf(fp,"sampleK=%u",sampleK);
	  //rf= fwrite(&sampleK,sizeof(uint),1,fp);    //max len of any posting list. ==> needed for malloc on info[]
	  fclose(fp);
	  delete []fname;
	  cerr << "\n sampleK (writen-to-disk) is " << sampleK ;

	}
}


int build_il (uint *source, uint length, char *build_options, void **ail) {
	//uint lenBitmapDiv= DEFAULT_LEN_BITMAP_DIV; // --> see ilists/ildefconfig.h
	printf("\n CALL TO BUILD_IL !!\n");
	uint lenBitmapDiv    = DEFAULT_LEN_BITMAP_DIV;
	uint maxMemRepair   = DEFAULT_MAX_MEM_REPAIR;
	uint replacesPerIter= DEFAULT_REPAIR_REPLACES_PER_ITER;
	double repairBreak  = DEFAULT_REPAIR_BREAK;
	char filename[256] = "";
	char path2convert[256] = "";
	char path2build_bitmap[256] = "";
	char path2build_index[256] = "";
	char path2irepairgonzalo[256] = "";
	char path2compress_dictionary[256] = "";
	
	t_ilr *il;
	il = (t_ilr *) malloc (sizeof (t_ilr) * 1);
	*ail = il;

	
	/** processing the parameters */
	{
		char delimiters[] = " =;";
		int j,num_parameters;
		char ** parameters;
		
		if (build_options != NULL) {
			parse_parameters_il(build_options,&num_parameters, &parameters, delimiters);
			for (j=0; j<num_parameters;j++) {
				if ((strcmp(parameters[j], "filename") == 0 ) && (j < num_parameters-1) ) {
					strcpy(filename,parameters[j+1]);
					j++;
				}
				else if ((strcmp(parameters[j], "lenBitmapDiv") == 0 ) && (j < num_parameters-1) ) {
					lenBitmapDiv=atoi(parameters[j+1]);	    
					j++;
				}
				else if ((strcmp(parameters[j], "maxMemRepair") == 0 ) && (j < num_parameters-1) ) {
					maxMemRepair=atoi(parameters[j+1]);	    
					j++;
				}
				else if ((strcmp(parameters[j], "replacesPerIter") == 0 ) && (j < num_parameters-1) ) {
					replacesPerIter=atoi(parameters[j+1]);	    
					j++;
				}
				else if ((strcmp(parameters[j], "repairBreak") == 0 ) && (j < num_parameters-1) ) {
					repairBreak=atof(parameters[j+1]);	    
					j++;
				}
				else if ((strcmp(parameters[j], "path2convert") == 0 ) && (j < num_parameters-1) ) {
					strcpy(path2convert,parameters[j+1]);
					j++;
				}else if ((strcmp(parameters[j], "path2build_bitmap") == 0 ) && (j < num_parameters-1) ) {
					strcpy(path2build_bitmap,parameters[j+1]);
					j++;
				}else if ((strcmp(parameters[j], "path2build_index") == 0 ) && (j < num_parameters-1) ) {
					strcpy(path2build_index,parameters[j+1]);
					j++;
				}else if ((strcmp(parameters[j], "path2irepairgonzalo") == 0 ) && (j < num_parameters-1) ) {
					strcpy(path2irepairgonzalo,parameters[j+1]);
					j++;
				}else if ((strcmp(parameters[j], "path2compress_dictionary") == 0 ) && (j < num_parameters-1) ) {
					strcpy(path2compress_dictionary,parameters[j+1]);
					j++;
				}
				
			}
			free_parameters_il(num_parameters, &parameters);
		}	

		char text1[256];
		char msg[256];
		sprintf(text1,"\"lenBitmapDiv=%u; maxMemRepair= %u (bytes)",lenBitmapDiv,maxMemRepair);
		sprintf(msg,"%s; replacesPerIter = %u; repairBreak=%2.8f %%; filename =%s\"",text1,replacesPerIter, repairBreak, filename);
		printf("\n Build parameters of method: %s", msg);
		
		sprintf(text1,"\"path2convert=%s; path2build_bitmap= %s ",path2convert,path2build_bitmap);
		sprintf(msg,"%s; path2build_index = %s; path2irepairgonzalo=%s; path2compress_dictionary=%s \"",text1,path2build_index, path2irepairgonzalo, path2compress_dictionary);
		printf("\n ** build-required PATHS: %s\n", msg);


	}
	
	{ //use external programs to repair compression of the lists + bitmaps.
	  //creating "filename.*" files.

		char command[1000];
	  
		// **call to convert program.**
		if (strcmp(path2convert,"")){
			//copies the posting lists to an external file (temp).
			char filenameTMP[256];
			sprintf(filenameTMP, "%s.%s.%u",filename,".tmp.",getpid());
			unlink(filenameTMP);
			FILE *f = fopen(filenameTMP,"w");
			fwrite(source, sizeof(uint),length,f);
			fclose(f);			
			sprintf(command,"%s %s %s %u", path2convert, filenameTMP, filename, lenBitmapDiv);
			//	sprintf(command,"%s %s %s %u 1>/dev/null",path2convert, filenameTMP, filename, lenBitmapDiv);
			printf("\n=============================");
			printf("\n%s",command);		fflush(stdout);				
			if (system(command) == -1) {
				unlink(filenameTMP);
				fprintf(stderr,"\n call to conversion program failed. Program will now exit!!\n");exit(0);
			}		
			else {unlink(filenameTMP);}
		}
		else {printf("\n Path to **converter program** not provided: I cannot continue\n"); exit(0);}

		// **call to build_bitmap program.**
		if (strcmp(path2build_bitmap,"")){
			//copies the posting lists to an external file (temp).
			char filenameTMP[256];
			sprintf(filenameTMP, "%s.%s.%u",filename,".tmp.",getpid());
			unlink(filenameTMP);
			FILE *f = fopen(filenameTMP,"w");
			fwrite(source, sizeof(uint),length,f);
			fclose(f);			
			sprintf(command,"%s %s %s", path2build_bitmap, filenameTMP, filename);
			//	sprintf(command,"%s %s %s  1>/dev/null",path2build_bitmap, filenameTMP, filename);
			printf("\n=============================");
			printf("\n%s",command);	fflush(stdout);		
			if (system(command) == -1) {
				unlink(filenameTMP);
				fprintf(stderr,"\n call to build_bitmap program failed. Program will now exit!!\n");exit(0);
			}		
			else {unlink(filenameTMP);}
		}
		else {printf("\n Path to **build_bitmap program** not provided: I cannot continue\n"); exit(0);}

		// **call to build_index program.**
		if (strcmp(path2build_index,"") && (strcmp(path2irepairgonzalo,""))){
			printf("\n\n@@@ Uses Gonzalo's Repair for compression\n");
			sprintf(command,"%s %s %u %u %2.15f %s", path2build_index, filename, maxMemRepair, replacesPerIter, repairBreak, path2irepairgonzalo );
			//	sprintf(command,"%s %s %u %u %2.5f 1>/dev/null",path2build_index, filename, maxMemRepair, replacesPerIter, repairBreak );
			printf("\n=============================");
			printf("\n%s",command);	fflush(stdout);			
			if (system(command) == -1) {
				fprintf(stderr,"\n call to build_index program failed. Program will now exit!!\n");exit(0);
			}		
		}
		else if (strcmp(path2irepairgonzalo,"") == 0) {
			printf("\n\n@@@ Uses Claudes's Aprox Repair for compression\n");
			sprintf(command,"%s %s %u %u %2.15f ", path2build_index, filename, maxMemRepair, replacesPerIter, repairBreak );
			//	sprintf(command,"%s %s %u %u %2.5f 1>/dev/null",path2build_index, filename, maxMemRepair, replacesPerIter, repairBreak );
			printf("\n=============================");
			printf("\n%s",command);	fflush(stdout);			
			if (system(command) == -1) {
				fprintf(stderr,"\n call to build_index program failed. Program will now exit!!\n");exit(0);
			}			
		}
		
		 else {printf("\n Path to **build_index program  OR TO Gonzalo's iRepair** not provided: I cannot continue\n"); exit(0);}

		// **call to compress_dictionary program.**
		if (strcmp(path2compress_dictionary,"")){
			sprintf(command,"%s %s ", path2compress_dictionary, filename);
			//	sprintf(command,"%s %s 1>/dev/null",path2compress_dictionary, filename);
			printf("\n=============================");
			printf("\n%s",command);	fflush(stdout);				
			if (system(command) == -1) {
				fprintf(stderr,"\n call to compress_dictionary program failed. Program will now exit!!\n");exit(0);
			}		
		}
		 else {printf("\n Path to **compress_dictionary program** not provided: I cannot continue\n"); exit(0);}
		
	}
	
	writeSampleK(filename, 8);	
	il->rp = new RepairPost(filename);
	printf("\n THE INDEX HAS BEEN BUILT SUCCESFULLY: %s\n",filename);
	
	
	
	//cout << "\n ################ UNCOMPRESSED = " << il->rp->sizeUncompressed << endl;
	
	return 0;
}


    /*  Saves index on disk by using single or multiple files, having 
      proper extensions. */

int save_il (void *ail, char *filebasename){
	//do nothing as all the files were created during build
	return 0;
}


    /*  Loads index from one or more file(s) named filename, possibly 
      adding the proper extensions. */
      
int load_il (char *filebasename, void **ail) {
	t_ilr *il = (t_ilr *) malloc (sizeof(t_ilr) * 1);	
	il->rp = new RepairPost(filebasename);
	*ail = il;
	return 0;
}



    /* Frees the memory occupied by index. */

int free_il (void *ail) {
	t_ilr *il=(t_ilr *) ail;
	if (il->rp) {
		delete il->rp;
		il->rp = NULL;
	}
	free(il);
	return 0;
}
 

/** creates a "fun" matlab file with the ratio of
 *  source len of posting and compressed len
 **/  
int buildBell_il(void *ail, const char *matlabfilename) {

  return 0;
}
	/* Gives the memory occupied by index in bytes. */

int size_il(void *ail, uint *size) {
	t_ilr *il=(t_ilr *) ail;
	*size =  il->rp->size();
	return 0;	
}

    /* Gives the memory occupied by the index assuming an uncompressed
       representation (uint **)*/
       
int size_il_uncompressed(void *ail, uint *size) {
	t_ilr *il=(t_ilr *) ail;
	*size =  il->rp->sizeUncompressed;
	
	cout << "\n ################ UNCOMPRESSED = " << il->rp->sizeUncompressed << endl;
	//*size=0;
	return 0;	
}


    /* Gives the length of the longest posting-list*/
    
int maxSizePosting_il(void *ail, uint *size){
	t_ilr *il=(t_ilr *) ail;
	*size =  il->rp->maxLenPosting;
	return 0;		
}


/**********************************************************************************/
/***** Operations (searching) with the posting lists ****/
/**********************************************************************************/

    /* Extracts the id-th list, having id \in [0..maxId-1] */

int extractList_il (void *ail, uint id, uint **list, uint *len) {
	t_ilr *il=(t_ilr *) ail;
		//** info[0] guarda el número de "elementos" que han sido expandidos = valor actual de pos
	
	id++; // Pues re-pair indexa [1..n] en lugar de [0..n-1] ??
	
	*list =  il->rp->adj_il(id,len);		
	//*len = (*list)[0];
	return 0; 	
}	


    /* Extracts the id-th list, having id \in [0..maxId-1] 
       Assumes list has enough space previously allocated. */
       
int extractListNoMalloc_il (void *ail, uint id, uint *list, uint *len){
	//not implemented for repair indexes (as expand does always malloc).	
	return 99; 			
}	



/**********************************************************************************/
/**********************************************************************************/



    /* Sets options for searches: such as parameters, funtions to use, etc 
       Permits to set different algorithms for intersect2, intersectN and 
       if needed "fsearch-operations needed by svs-type algorithms"     */

int setDefaultSearchOptions_il (void *ail, char *search_options){

	t_ilr *il = (t_ilr *) ail;
	
	il->defs.defaultFsearch  = NULL;
	il->defs.defaultIntersect2 = NULL;
	il->defs.defaultIntersectN = NULL;
	
	/** processing the parameters */
	char delimiters[] = " =;";
	int j,num_parameters;
	char ** parameters;
	
	if (search_options != NULL) {
	parse_parameters_il(search_options,&num_parameters, &parameters, delimiters);

		for (j=0; j<num_parameters;j++) {
			//setting the default function to Intersect-2-lists		
			if ((strcmp(parameters[j], "int2") == 0 ) && (j < num_parameters-1) ) {
				if (strcmp(parameters[j+1], "svs") == 0 )      //just for testing.
					il->defs.defaultIntersect2 = &RepairPost::svs2Exp;
				else if (strcmp(parameters[j+1], "skip") == 0 )
					il->defs.defaultIntersect2 = &RepairPost::int2Skipping;	    
				else if (strcmp(parameters[j+1], "skipE") == 0 )
					il->defs.defaultIntersect2 = &RepairPost::int2SkippingExp;	    
				else if (strcmp(parameters[j+1], "skipS") == 0 )
					il->defs.defaultIntersect2 = &RepairPost::int2SkippingSeq;  //sequential Search in the samples	    
				else if (strcmp(parameters[j+1], "merge") == 0 )
					il->defs.defaultIntersect2 = NULL;	    
				j++;
			}
			
			//setting the default function to Intersect-N-lists		
			else if  ((strcmp(parameters[j], "intn") == 0 ) && (j < num_parameters-1) ) {
				if (strcmp(parameters[j+1], "skip") == 0 ) 
					il->defs.defaultIntersectN = &RepairPost::intNSkipping;			
				if (strcmp(parameters[j+1], "skipE") == 0 ) 
					il->defs.defaultIntersectN = &RepairPost::intNSkippingExp;			
				if (strcmp(parameters[j+1], "skipS") == 0 ) 
					il->defs.defaultIntersectN = &RepairPost::intNSkippingSeq;			
				j++;
			}
		}

	free_parameters_il(num_parameters, &parameters);
	}
	j=0;
	
	fprintf(stderr,"\n ** Call to setDefaultSearchOptions_il: \"%s\"\n", search_options);
		
	return 0;	
}

	/* Returns the len of the id-th posting list */
int lenlist(void *ail, uint id, uint *len){
	t_ilr *il = (t_ilr *) ail;
	*len = il->rp->lenPost[id+1]; 
	return 0;
}


	/* Intersects two lists given by id1 and id2  \in [0..maxId-1].
	   The defaultIntersect2 function must be initialized previously (svs, merge, ...)
	   Returns the resulting list of "intersecting positions"  */

int intersect_2_il (void *ail, uint id1, uint id2,  uint *noccs, uint **occs){
	t_ilr *il = (t_ilr *) ail;
	
	if ( il->defs.defaultIntersect2 == NULL) {
		return 40;	
	}

	// call to svs2, skip, merge2, etc... depending on default settings
	//int (*defaultIntersect2) (void *ail, uint id1, uint id2, uint *noccs, uint **occs ) = il->defs.defaultIntersect2;
	//int reterr = ((*defaultIntersect2) (ail,id1,id2,noccs,occs));
	
	int (RepairPost::*defaultIntersect2)(unsigned int, unsigned int, unsigned int*, unsigned int**) = il->defs.
	defaultIntersect2;
		
	id1++; 	id2++;
	int reterr = (il->rp->*defaultIntersect2)(id1,id2,noccs,occs);

	if ((!reterr) && (!*noccs)){ 
		free (*occs);
		*occs=NULL;
	}
	
	return 	reterr;
}

	/* Intersects $nids$ lists given by ids in ids[], having ids[i] \in [0..maxId-1].
	   Returns the resulting list of "intersecting positions"  */

int intersect_N_il (void *ail, uint *ids, uint nids, uint *noccs, uint **occs ) {
	static uint IDS[MAX_PATTERNS_TO_INTERSECT];
	t_ilr *il = (t_ilr *) ail;
	if ( il->defs.defaultIntersectN == NULL) {
		return 41;	
	}
	
	// call to svsn,, etc... depending on default settings
	//int (*defaultIntersectN) (void *ail, uint *ids, uint nids, uint *noccs, uint **occs ) = il->defs.defaultIntersectN;		
	//int reterr = ((*defaultIntersectN) (ail,ids,nids,noccs,occs));

	int i;
	for (i=0;i<nids;i++) 
		IDS[i] = ids[i]+1;
	
	int (RepairPost::*defaultIntersectN)(unsigned int *, unsigned int, unsigned int*, unsigned int**) = il->defs.
	defaultIntersectN;
	int reterr = (il->rp->*defaultIntersectN)(IDS,nids,noccs,occs);


	if ((!reterr) && (!*noccs)){ 
		free (*occs);
		*occs=NULL;
	}	
	return 	reterr;
}

