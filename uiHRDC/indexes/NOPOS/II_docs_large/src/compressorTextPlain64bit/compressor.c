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
		
	unsigned int i;
	rep->docBeginnings = (uint *) malloc (sizeof(uint) * (numdocs+1));
	for (i=0;i<=numdocs;i++) rep->docBeginnings[i] = docBeginnings[i];
	rep->numdocs = numdocs;
	
	rep->text = (unsigned char *) malloc (sizeof(unsigned char) * length);
	for (i=0;i<length;i++) rep->text[i] = text[i];
	rep->length = length;
	
	return 0;
}

	/* Saves the [compressed] representation of the collection on disk by using 
	 * single or multiple files, having proper extensions. */

int save_representation (void *representation, char *basename) {
	trep *rep = (trep *) representation;
	FILE *file;
	char filename[256];
	sprintf(filename,"%s.%s", basename,COMPRESSED_REPRESENTATION_EXT_FILE);
	unlink(filename);
	fprintf(stderr,"\tSaving Compressed text (plain) to disk  [%s]\n", filename);
	
	if( (file = fopen(filename, "w")) == NULL) {
		printf("Cannot open file %s\n", filename);
		exit(0);
	}	
	
	size_t errr;
	errr=fwrite( &rep->length, sizeof(unsigned int),1,file);
	errr=fwrite( rep->text, sizeof(unsigned char),  rep->length,file);		
	errr=fwrite( &rep->numdocs, sizeof(unsigned int),1,file);
	errr=fwrite( rep->docBeginnings, sizeof(uint), (rep->numdocs +1),file);		
	fclose(file);
	
//	if( (file = open(filename, O_WRONLY|O_CREAT,S_IRWXG | S_IRWXU)) < 0) {
//		printf("Cannot open file %s\n", filename);
//		exit(0);
//	}	
//	int errr;	
//	errr=write(file, &rep->length, sizeof(unsigned int));
//	errr=write(file, rep->text, sizeof(unsigned char) * rep->length);		
//	errr=write(file, &rep->numdocs, sizeof(unsigned int));
//	errr=write(file, rep->docBeginnings, sizeof(uint) * (rep->numdocs +1));		
//	close(file);
	return 0;
}

	/*  Loads the [compressed] representation from one or more file(s) named 
	 * filename, possibly adding the proper extensions. */

int load_representation (void **representation, char *basename){
	trep *rep = (trep *) malloc (sizeof (trep) * 1);
	*representation = rep;	
	FILE *file;
	char filename[256];
	sprintf(filename,"%s.%s", basename,COMPRESSED_REPRESENTATION_EXT_FILE);
	fprintf(stderr,"\nLoading Compressed text (plain) to disk  [%s]\n", filename);

	if( (file = fopen(filename, "r")) == NULL) {
		printf("Cannot open file %s\n", filename);
		exit(0);
	}
	
	size_t errr;
	errr= fread(&rep->length, sizeof(unsigned int),1,file);
	rep->text = (unsigned char *) malloc (sizeof(unsigned char) * rep->length);
	errr= fread(rep->text, sizeof(unsigned char), rep->length,file);	
		
	errr= fread(&rep->numdocs, sizeof(unsigned int),1,file);
	rep->docBeginnings = (uint *) malloc (sizeof(uint) * (rep->numdocs+1));
	errr= fread(rep->docBeginnings, sizeof(uint), (rep->numdocs +1),file);		
	fclose(file);	

//	if( (file = open(filename, O_RDONLY)) < 0) {
//		printf("Cannot open file %s\n", filename);
//		exit(0);
//	}	
//	int errr;
//	errr= read(file, &rep->length, sizeof(unsigned int));
//	rep->text = (unsigned char *) malloc (sizeof(unsigned char) * rep->length);
//	errr= read(file, rep->text, sizeof(unsigned char) * rep->length);	
		
//	errr= read(file, &rep->numdocs, sizeof(unsigned int));
//	rep->docBeginnings = (uint *) malloc (sizeof(uint) * (rep->numdocs+1));
//	errr= read(file, rep->docBeginnings, sizeof(uint) * (rep->numdocs +1));		
//	close(file);	

	return 0;
}

	/* Frees the memory occupied by the [compressed] representation. */

int free_representation (void *representation) {
	trep *rep = (trep *) representation;
	free(rep->text);
	free(rep->docBeginnings);
	free(rep);
	return 0;
}

	/* Gives the memory occupied by the [compressed] representation, in bytes */

int size_representation(void *representation, uint *size){
	trep *rep = (trep *) representation;
	*size = (sizeof(unsigned char) * rep->length ) + (sizeof(uint) * (rep->numdocs+1));
	return 0;
}

	/* Recovers the text of i-th document of the collection of documents */

int extract_doc_representation (void *representation, uint i, unsigned char **doc, unsigned int *len) {
	trep *rep = (trep *) representation;
	*len = (rep->docBeginnings[i+1]-rep->docBeginnings[i]);
//	printf("\n I want to alloc %u bytes", *len);fflush(stderr);fflush(stdout);
	unsigned char *text; 
	
//	if (! *doc)
	  text =(unsigned char *) malloc (((*len)+1) * sizeof(unsigned char));
//	else
//	  text = *doc;
	//	if (!text) {
//		printf("\n malloc falla"); exit(0);
//	}
	memcpy(text, rep->docBeginnings[i] + rep->text, *len);
	text[*len]='\0';
	*doc = text;
	return ((int)rep->docBeginnings[i]);
	//return0;
}





/* Sebas, Miguel, esta funcion no tenéis que implementarla! */
/* La usé sólo para debugging */
void printTextInOffset(void *representation, uint offset, uint len) {
	trep *rep = (trep *) representation;
	uint i;
	for (i=offset;i<offset+len; i++)
		fprintf(stderr,"%c",rep->text[i]);
		
}
