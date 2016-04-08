
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
	char *build_options, void **representation);

	/* Saves the [compressed] representation of the collection on disk by using 
	 * single or multiple files, having proper extensions. */

int save_representation (void *representation, char *filename);

	/*  Loads the [compressed] representation from one or more file(s) named 
	 * filename, possibly adding the proper extensions. */

int load_representation (void **representation, char *filename);

	/* Frees the memory occupied by the [compressed] representation. */

int free_representation (void *representation);

	/* Gives the memory occupied by the [compressed] representation, in bytes */

int size_representation(void *representation, uint *size);

	/* Recovers the text of i-th document of the collection of documents */

int extract_doc_representation (void *representation, uint i, unsigned char **doc, unsigned int *len);




/* Sebas, Miguel, esta funcion no tenéis que implementarla! */
void printTextInOffset(void *representation, uint offset, uint len);
