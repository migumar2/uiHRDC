#include "interfazTextCompressorDocs.h"

int 
build_representation (unsigned char *text, unsigned int length,
	unsigned int *docBeginnings, unsigned int numdocs, 
	char *build_options, void **representation)
{
	RePairDocCompressor *compressor = new RePairDocCompressor();
	*representation = compressor;
	return compressor->build_representation(text, length, docBeginnings, numdocs);
}

int 
save_representation (void *representation, char *filename)
{
	RePairDocCompressor *compressor = (RePairDocCompressor*)representation;
	return compressor->save_representation(filename);
}

int 
load_representation (void **representation, char *filename)
{
	*representation = RePairDocCompressor::load_representation(filename);
	return 0;
}

int 
free_representation (void *representation)
{
	RePairDocCompressor *compressor = (RePairDocCompressor*)representation;
	delete compressor;
}

int 
size_representation (void *representation, uint *size)
{
	RePairDocCompressor *compressor = (RePairDocCompressor*)representation;
	compressor->size_representation(size);
}

int 
extract_doc_representation (void *representation, uint i, unsigned char **doc, unsigned int *len)
{
	RePairDocCompressor *compressor = (RePairDocCompressor*)representation;
	return compressor->extract_doc_representation(i, doc, len);
}



/* Sebas, Miguel, esta funcion no tenéis que implementarla! */
/* La usé sólo para debugging */
void 
printTextInOffset(void *representation, uint offset, uint len) {
	return ;
		
}
