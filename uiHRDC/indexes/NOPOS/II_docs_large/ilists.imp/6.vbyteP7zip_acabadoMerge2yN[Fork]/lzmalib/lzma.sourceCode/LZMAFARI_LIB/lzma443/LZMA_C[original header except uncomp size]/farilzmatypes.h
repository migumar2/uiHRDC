#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>



#include "LzmaDecode.h"
#ifndef uchar 
	#define uchar unsigned char
#endif

#define TMP_FILE_TO_COMPRESS_VBYTE "TempFile09871.compressed.vbyte"
#define TMP_FILE_COMPRESSED_LZMA "TempFile09871.compressed.vbyte.lzma"


typedef struct{
	uchar properties[LZMA_PROPERTIES_SIZE]; 	/* uint dictsize; uchar decpropflag; */
	uint sizeunc;  /* size of the uncompressed sequence */
	uchar *compdata;  /* the dictionary + the compressed data */
	uint compdataSize;  /* gives the length in bytes of "compdata" stream */
	uint memusage; /* size of this structure, including the "compdata array" */
}tlzma;


