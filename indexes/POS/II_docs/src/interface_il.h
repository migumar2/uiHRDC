
#define IFERRORIL(error) {{if (error) { fprintf(stderr, "%s\n", error_il(error)); exit(1); }}}

/**********************************************************************************/
/*  General interface for using compressed representations of posting lists       *
/*              Any alternative representation must implement all these functions *
/**********************************************************************************/


#ifndef uint
#define uint  unsigned int
#endif


/* Error management */

    /* Returns a string describing the error associated with error number
      e. The string must not be freed, and it will be overwritten with
      subsequent calls. */

char *error_il (int e);

/* Building the inverted list structure */



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

int build_il (uint *source, uint length, char *build_options, void **ail);
	


	/* returns a string with the parameters available for building the method. 
	   So that each index tells the user the available parameters.           */

char *showParameters_il();


    /*  Saves index on disk by using single or multiple files, having 
      proper extensions. */

int save_il (void *ail, char *filebasename);


    /*  Loads index from one or more file(s) named filename, possibly 
      adding the proper extensions. */

int load_il (char *filebasename, void **ail);


    /* Frees the memory occupied by index. */

int free_il (void *ail);


    /* Gives the memory occupied by index in bytes. */

int size_il(void *ail, uint *size);

    /* Gives the memory occupied by the index assuming an uncompressed
       representation (uint **)*/
int size_il_uncompressed(void *ail, uint *size);

    /* Gives the length of the longest posting-list*/
int maxSizePosting_il(void *ail, uint *size);


	/* Generates estatistics about the len of the compressed posting lists */
int buildBell_il(void *ail, const char matlabfilename[]);


/********************************************************/
/***** Operations (searching) with the posting lists ****/
/********************************************************/


    /* Extracts the id-th list, having id \in [0..maxId-1] */

int extractList_il (void *ail, uint id, uint **list, uint *len);

    /* Extracts the id-th list, having id \in [0..maxId-1] 
       Assumes list has enough space previously allocated. */

int extractListNoMalloc_il (void *ail, uint id, uint *list, uint *len);


    /* Sets options for searches: such as parameters, funtions to use, etc 
       Permits to set different algorithms for intersect2, intersectN and 
       if needed "fsearch-operations needed by svs-type algorithms"     */

int setDefaultSearchOptions_il (void *ail, char *search_options);

	/* Intersects two lists given by id1 and id2  \in [0..maxId-1].
	   The defaultIntersect2 function must be initialized previously (svs, merge, ...)
	   Returns the resulting list of "intersecting positions"  */

int intersect_2_il (void *ail, uint id1, uint id2,  uint *noccs, uint **occs);

	/* Intersects $nids$ lists given by ids in ids[], having ids[i] \in [0..maxId-1].
	   Returns the resulting list of "intersecting positions"  */

int intersect_N_il (void *ail, uint *ids, uint nids, uint *noccs, uint **occs );


	/* Returns the len of the id-th posting list */
int lenlist(void *ail, uint id, uint *len);
