
/* General interface for using the compressed index libraries */

#ifndef uchar
#define uchar unsigned char
#endif
#ifndef uint
#define uint  unsigned int
#endif
#ifndef ulong
#define ulong unsigned long
#endif

/* Error management */

        /* Returns a string describing the error associated with error number
          e. The string must not be freed, and it will be overwritten with
          subsequent calls. */

char *error_index (int e);

/* Building the index */

        /* Creates index from text[0..length-1]. Note that the index is an 
          opaque data type. Any build option must be passed in string 
          build_options, whose syntax depends on the index. The index must 
          always work with some default parameters if build_options is NULL. 
          The returned index is ready to be queried. */

//int build_index (uchar *text, ulong length, char *build_options, void **index);
int build_index (uchar *text, ulong length, uint *docboundaries, uint ndocs, char *build_options, void **index);

        /*  Saves index on disk by using single or multiple files, having 
          proper extensions. */

int save_index (void *index, char *filename);

        /*  Loads index from one or more file(s) named filename, possibly 
          adding the proper extensions. */

int load_index (char *filename, void **index);

        /* Frees the memory occupied by index. */

int free_index (void *index);

        /* Gives the memory occupied by index in bytes. */

int index_size(void *index, ulong *size);

/* Querying the index */

        /* Writes in numocc the number of occurrences of the substring 
          pattern[0..length-1] found in the text indexed by index. */

int count (void *index, uchar *pattern, ulong length, ulong *numocc);

        /* Writes in numocc the number of occurrences of the substring 
          pattern[0..length-1] in the text indexed by index. It also allocates
          occ (which must be freed by the caller) and writes the locations of 
          the numocc occurrences in occ, in arbitrary order.  */

int locate (void *index, uchar *pattern, ulong length, ulong **occ, 
        ulong *numocc);

        /* Gives the length of the text indexed */

int get_length(void *index, ulong *length);

/* Accessing the indexed text  */

        /*  Allocates snippet (which must be freed by the caller) and writes 
          the substring text[from..to] into it. Returns in snippet_length the 
          length of the text snippet actually extracted (that could be less 
          than to-from+1 if to is larger than the text size). */

int extract (void *index, ulong from, ulong to, uchar **snippet, 
        ulong *snippet_length);

        /* Displays the text (snippet) surrounding any occurrence of the 
          substring pattern[0..length-1] within the text indexed by index. 
          The snippet must include numc characters before and after the 
          pattern occurrence, totalizing length+2*numc characters, or less if 
          the text boundaries are reached. Writes in numocc the number of 
          occurrences, and allocates the arrays snippet_text and 
          snippet_lengths (which must be freed by the caller). The first is a 
          character array of numocc*(length+2*numc) characters, with a new 
          snippet starting at every multiple of length+2*numc. The second 
          gives the real length of each of the numocc snippets. */

int display (void *index, uchar *pattern, ulong length, ulong numc, 
        ulong *numocc, uchar **snippet_text, ulong **snippet_lengths);

        /*  Obtains the length of the text indexed by index. */

int length (void *index, ulong *length);

		/* Shows summary info of the index */
		
int printInfo(void *index);



/** ***********************************************************************************
  * WORD-ORIENTED QUERY FUNCTIONS: LocateWord and DisplayWord
  * ***********************************************************************************/  
	/** Writes in numocc the number of occurrences of the substring 
	  pattern[0..length-1] in the text indexed by index. It also allocates
	  occ (which must be freed by the caller) and writes the locations of 
	  the numocc occurrences in occ, in arbitrary order. These occurrences
	  refer to the offsets in TOH where the caller could start a display
	  operation. So locateWord implies synchronization using B.
	  ** Parameter kbefore sets locateWord not to obtain the offset in TOH of the
	     searched word, but the offset in TOH of k-before words before.	  
	*/	  
	  
int locateWord(void *index, uchar *pattern, ulong length, ulong **occ, ulong *numocc, uint kbefore);

  /** Displays the text (snippet) surrounding any occurrence of the 
    substring pattern[0..length-1] within the text indexed by index. 
    The snippet must include numc characters before and after the 
    pattern occurrence, totalizing length+2*numc characters, or less if 
    the text boundaries are reached. Writes in numocc the number of 
    occurrences, and allocates the arrays snippet_text and 
    snippet_lengths (which must be freed by the caller). The first is a 
    character array of numocc*(length+2*numc) characters, with a new 
    snippet starting at every multiple of length+2*numc. The second 
    gives the real length of each of the numocc snippets. */

 int displayWords (void *index, uchar *pattern, ulong length, ulong numc, 
         ulong *numocc, uchar **snippet_text, ulong **snippet_lengths, uint kbefore);


/** simulates extration of text process, but do not actually returns anything at all 
   Extracts upto <=2K words from K=wordsbefore words before each occurrence of a pattern.
   Less than 2K words can be extracted if more than numc characters have been already obtained.
   Do nothing else... do not return the text */

int  displayTextOcurrencesNoShow(void *index, uchar *pattern, ulong length, uint wordsbefore, uint maxnumc);



/**  Allocates text (which must be freed by the caller) and recovers the
  the substring of text starting from the "fromword"-th word up to the
  "toWord"-th words. Returns in text the text, and in "text_lenght" the 
  length of the text  actually extracted. Text is allocated. 
  Actually extracts SE[fromWord .. toWord) ... not the last word.    */

int extractWords (void *index, ulong fromWord, ulong toWord, uchar **text, 
       ulong *text_length);



/** ***********************************************************************************
  * DOC-ORIENTED QUERY FUNCTIONS: LocateDocumentOffsets
  * ***********************************************************************************/
int LocateDocumentOffsets(void *index, uchar *pattern, uint length, uint **occs, uint *noccs);
int getNumDocs(void *index, uint *ndocs) ;
int extractTextOfSelectedDocuments(void *index, uint *docIds, uint nids, uchar ***texts, uint **lens, ulong *tot_chars);

