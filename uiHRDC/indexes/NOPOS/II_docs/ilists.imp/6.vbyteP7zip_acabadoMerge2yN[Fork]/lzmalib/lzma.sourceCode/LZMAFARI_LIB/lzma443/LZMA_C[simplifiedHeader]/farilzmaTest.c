/* 
farilzmaTest.c
Test application for LZMA Decoder (modification by Antonio Fariña)

Original file written and distributed to public domain by Igor Pavlov.
That file is part of LZMA SDK 4.26 (2005-08-05)
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "farilzma.h"


int main(int argc, const char *argv[])
{
	FILE *inFile = 0;
	uint inFileSize, outFileSize;
	unsigned char *inBuffer, *outBuffer;
	unsigned char  *bcs;
	void *lzmadata, *lzmadata2;

	FILE *outFile = 0;
	int res;

	initLzmaStruct(&lzmadata);
	initLzmaStruct(&lzmadata2);

	printf("\nLZMA Compressor/decompressor test lib \n 4.26 Copyright (c) 1999-2005 Igor Pavlov  2005-08-05, modified by Antonio Fariña (2009-09-09)\n");

	if (argc != 4) {
		printf("\nUsage:  farilzmaTest srcFile filelzma.unc  path2lzmaEncoder \n");
		/**                argv[0]     argv[1]     argv[2]       argv[3]         */
 		return 1;
	}
  
  	bcs=inBuffer=outBuffer=NULL;
  	
	inFileSize = (uint) getfileSize((char *) argv[1]);
	inBuffer = (unsigned char *) malloc (inFileSize * sizeof(unsigned char)); 

	inFile = fopen(argv[1], "rb");
	if (inFile == 0){
		fprintf(stderr, "Can not open input file");
		exit(0);
	}
	
  	/* loads the source file. */
	/*fread(inBuffer,sizeof(unsigned char), inFileSize, inFile); */
	/*if (!MyReadFileAndCheck(inFile, inBuffer, inFileSize))*/
    if (! fread(inBuffer, 1, (size_t)inFileSize, inFile)) {
    	fprintf(stderr,"\nread failed");
	}
	fclose(inFile);	

	/** compresses using a call to external program **/
	res = compressDataExt(inBuffer,inFileSize,(char *) argv[3]);  /* temp file is created */
	free(inBuffer);
	
	/** load the lzma-compressed file into memory **/
		loadDataExt(&inBuffer, &inFileSize);
		createLzmaExt(inBuffer,inFileSize,lzmadata);
		{ uint lzmamemneeds = sizeBytesLzmaStruct(lzmadata);
		  fprintf(stderr,"\n compressed File has %d bytes ==> in memory size = %d bytes", inFileSize, lzmamemneeds); 
		}
		

			
		/** Checks writeBuffLzmaStruct and readBuffLzmaStruct */
	/**/		
		{
			
			uint pos1,pos2;
			bcs = (unsigned char *) malloc (inFileSize *4);
			pos1 = 3;
			
			pos2 = writeBuffLzmaStruct(bcs,pos1, lzmadata);
			fprintf(stderr,"\n space wasted due to writeBuffLzmaStruct = %d bytes . Inipos = %d, endPos=%d\n", pos2-pos1, pos1,pos2);
			fflush(stderr);
			readBuffLzmaStruct(&(bcs[pos1]),pos2-pos1,lzmadata2);
			
		}
		
		/**decompresses the data */
	/*	decodeLzma(lzmadata,&outBuffer,&outFileSize);
	 */		
		decodeLzma(lzmadata2,&outBuffer,&outFileSize);
	/**/
	
	/** opens the output file **/
	outFile = fopen(argv[2], "wb+");
	if (outFile == 0) {
	  fprintf(stderr, "Can not open output file");
	  exit(0);
	}
	
	if (! fwrite(outBuffer, 1, (size_t)outFileSize, outFile)){
		fprintf(stderr,"\n Unable to write data!");
		res = 1;
	}

	if (outFile != 0)
		fclose(outFile);
		
	if (inBuffer) free(inBuffer);
	if (outBuffer) free(outBuffer);
	if (bcs) free(bcs);


	freeLzmaStruct(lzmadata);
	freeLzmaStruct(lzmadata2);
  return res;
}

