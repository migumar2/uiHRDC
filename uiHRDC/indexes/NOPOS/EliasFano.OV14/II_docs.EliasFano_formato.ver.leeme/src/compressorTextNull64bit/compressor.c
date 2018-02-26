#include "compressor.h"

/***************************************************************************/
/** Creates a compressed representation of a collection of text documents **/
/***************************************************************************/

	/* Creates the [compressed] representation of a collection of text documents
	  whose text is passed as text[0..length-1]. The offset of the document 
	  beginnings are given by docBeginnings[0..numdocs-1], whereas 
	  docBeginnings[numdocs]== length.
	  
	  Note that the *representation* is an 
	  opaque data type. Any build option must be passed in string 
	  build_options, whose syntax depends on the text representation. 
	  The compressed representation must always work with some default 
	  parameters if build_options is NULL. 
	  The returned representation is ready to be decompressed and queried
	  for statistics. */

int build_representation (unsigned char *text, unsigned int length,
	unsigned int *docBeginnings, unsigned int numdocs, 
	char *build_options, void **representation) {
	
	trep *rep = (trep *) malloc (sizeof (trep) * 1);
	*representation = rep;	
		
	rep->length = length;
	
	return 0;
}

	/* Saves the [compressed] representation of the collection on disk by using 
	 * single or multiple files, having proper extensions. */

int save_representation (void *representation, char *basename) {
	return 0;
}

	/*  Loads the [compressed] representation from one or more file(s) named 
	 * filename, possibly adding the proper extensions. */

int load_representation (void **representation, char *basename){
	trep *rep = (trep *) malloc (sizeof (trep) * 1);
	*representation = rep;	

	return 0;
}

	/* Frees the memory occupied by the [compressed] representation. */

int free_representation (void *representation) {
	trep *rep = (trep *) representation;
	free(rep);
	return 0;
}

	/* Gives the memory occupied by the [compressed] representation, in bytes */

int size_representation(void *representation, uint *size){
	//trep *rep = (trep *) representation;
	*size = 0;
	return 0;
}

	/* Recovers the text of i-th document of the collection of documents */

int extract_doc_representation (void *representation, uint i, unsigned char **doc, unsigned int *len) {
	//trep *rep = (trep *) representation;
	*len = 0;
	*doc=NULL;
	
	return 0;
}





/* Sebas, Miguel, esta funcion no tenéis que implementarla! */
/* La usé sólo para debugging */
void printTextInOffset(void *representation, uint offset, uint len) {
	//trep *rep = (trep *) representation;
		
}
