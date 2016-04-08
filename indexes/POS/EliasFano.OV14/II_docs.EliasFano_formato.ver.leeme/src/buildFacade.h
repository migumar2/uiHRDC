/* only for getTime() */
#include <sys/time.h>
#include <sys/resource.h>


#include "utils/basics_wi.h"
//#include "utils/errors.c"
#include "interface_il.h"

#include "utils/valstring.h"
#include "utils/defValues.h"
#include "utils/fileInfo.h"
#include "utils/hashWords.h"

//#include "utils/horspool.h"

#include "utils/mergeoccs2docs.h"  //function: mergeOccsIntoDocOccs

#include "interfazTextCompressorDocs.h"

#define FILLERUINT  0000

#ifndef uchar
#define uchar unsigned char
#endif
#ifndef uint
#define uint  unsigned int
#endif
#ifndef ulong
#define ulong unsigned long
#endif


	typedef struct {   //aux structure used to sort vectors (for words of separators)
		//uint slot;       //depending on the values of the associated hash table.
		uint freq;
		byte *word;
	} tauxslots;


	typedef struct SzoneMem {        //a large block of memory to load a file into mem.
		byte *zone;  //block of mem.
		uint size;  //number of bytes	 	 
	} tZoneMem;

	//  words dataStructure.	
	typedef struct {
		uint *words;
		uint elemSize;  //the size (in bits) of each pointer.
		tZoneMem wordsZoneMem; // a block of memory where the canonical words are loaded (from file).

	} twords;


	/**the WCSA index structures... */
	typedef struct {					
		/**vocabulary of words */
		twords wordsData;		/* vocabulary (words) of the index */
		uint nwords;	/** number of words in the vocab. */		

		//#ifdef FREQ_VECTOR_AVAILABLE
		uint *freqs;		/* number of occs of each word */
		//#endif
		uint maxNumOccs;	/* max number Of Ocurrences for a word */
			
		uint sourceTextSize; //the size of the source text.
		
		uint *doc_offsets_sids;	/** beginning of each document, with respect to the rank of words: Sid[] */	
		//uint Nwords;	    	/** number of words in the text. == doc_offsets_sids[ndocs]*/		

		uint ndocs;

		void *ils;  //the compressed posting lists
		void *ct; //compressed representation of the original text.

		/** temporary: used during building phase **/
		byte *text;  		 //the original text (if not set to NULL!!) 
		uint textSize;		 //the size of the compressed source text (in bytes)

	}twcsa;


/** ******************************************************************************
    * Interface (from pizza chili) for using the WCSA index
*********************************************************************************/

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

int build_index (char *sourceFileBaseName, char *build_options, void **index);

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

				/* returns detailled info of the data structures of the index **/
				
int index_info(void *index, char msg[]);

/* Querying the index */

        /* Writes in numocc the number of occurrences of the substring 
          pattern[0..length-1] found in the text indexed by index. */

int count (void *index, uchar *pattern, uint length, uint *numocc);

        /* Writes in numocc the number of occurrences of the substring 
          pattern[0..length-1] in the text indexed by index. It also allocates
          occ (which must be freed by the caller) and writes the locations of 
          the numocc occurrences in occ, in arbitrary order.  */

int locate (void *index, uchar *pattern, uint length, uint **occ, uint *numocc);
int locate1(void *index, uchar *pattern, uint length, uint **occs, uint *noccs);
int locate2(void *index, uchar *pattern, uint length, uint **occs, uint *noccs);
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

 		/*  Initializes the int2 and intn intersection functions to be used */
int setDefaultIntersectionOptions(void *index, char *options);

/** *******************************************************************************************/
/** Building part of the index ****************************************************************/
int build_WordIndex (char *inbasename, char *build_options, void **index);



/** *******************************************************************************************/
/** Search part of the index ******************************************************************/
// Definitions of some PUBLIC function prototipes.

	//***Searching for a TEXT pattern ...
	
		/** returns into word a pointer to wcsa->vocq.zone, for the i-th qgram */
	void getWord(twcsa *wcsa, uint ith, unsigned char **word, uint *lenword) ;

		/** Given a word, retuns the position (Id) in "words[] array" or -1 if not found there. */
	int parseWordIntoId(twcsa *wcsa, byte *aWord, uint len);
	int parseWordIntoIdDebug(twcsa *wcsa, byte *aWord, uint len);

	/** Given a Sequence of words, retuns the position in "words[] array" of each word. */
	/** memory for integerPattern is not allocate, sizeIntegers is usually a constant, and bounded previously*/
	void parseTextIntoIntegers(twcsa *wcsa, byte *textPattern, uint patLen, uint *integerPattern, uint *sizeIntegers); 
 		
 		//shows a snippet with the text around the ocurrences of a pattern.	
	int displayTextOcurrences(void *index, uchar *pattern, ulong length, uint radixDisplay);
	//int displayTextOcurrencesNoShow (void *index, uchar *pattern, ulong length, uint radixDisplay);
	int  displayTextOcurrencesNoShow(void *index, uchar *pattern, ulong length, uint radixDisplay, ulong *numoccs);


/*************************************************************************************************/	
/*************************************************************************************************/	
	/** Given a Sequence of words, retuns the position in "words[] array" of each word. */
	/** memory for integerPattern is not allocated, sizeIntegers is usually a constant, and bounded previously*/
	/** Returns ids of the words, and the number of ids in "integerPattern[]" */
	int index_parseTextIntoIntegers(void *index, byte *textPattern, uint patLen, uint **integerPattern, uint *sizeIntegers);

	/** Returns the list of documents containing the nids terms represented by ids[] **/
//	int index_listDocuments(void *index, uint *ids, uint nids, uint **docs, uint *ndocs);
	
/*************************************************************************************************/	
/*  Public only to give more flexibility to the caller                                           */
/*************************************************************************************************/	

	/** Returns the size of the representation of the inverted lists **/
	int index_getsize_il(void *index, uint *sizeil);
	
	/** Returns the size of the representation of the compressed text **/
	int index_getsize_ct(void *index, uint *sizect);	
	
	
/*************************************************************************************************/	
/*  Public:: only to give the ability to permit measuring AVG document extraction time           */
/*************************************************************************************************/	
	/** Returns the number of indexed documents **/
	int getNumDocs(void *index, uint *ndocs);
	
	/** Given "ndocs" document-ids \in [0..ndocs-1] --> returns the text of such documents **/
	int extractTextOfSelectedDocuments(void *index, uint *docIds, uint nids, uchar ***texts, uint **lens,
																						ulong *tot_chars);
	
	
	

	
	
/* just for debugging:: merging occs into doc-occs[] */	
	void checkDocMerge(void *index);
