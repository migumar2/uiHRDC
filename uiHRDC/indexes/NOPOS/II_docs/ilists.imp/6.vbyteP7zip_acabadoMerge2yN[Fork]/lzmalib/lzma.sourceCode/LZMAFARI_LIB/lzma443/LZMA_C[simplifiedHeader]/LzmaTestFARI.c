/* 
LzmaTest.c
Test application for LZMA Decoder

This file written and distributed to public domain by Igor Pavlov.
This file is part of LZMA SDK 4.26 (2005-08-05)
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "LzmaDecode.h"

unsigned long getfileSizeUtils (const char *filename){
	FILE *fpText;
	unsigned long fsize;
	fpText = fopen(filename,"rb");
	fsize=0;
	if (fpText) {
		fseek(fpText,0,2);
		fsize= ftell(fpText);
		fclose(fpText);
	}
	return fsize;
}

SizeT getHeaderSize(){
	return (SizeT)(LZMA_PROPERTIES_SIZE + 8);
}









/** Decodes the LZMAdata in the "inputBuffer" and writes it into outFile) */
int main3(unsigned char *inputBuffer, uint inFileSize, FILE *outFile)
{
  /* We use two 32-bit integers to construct 64-bit integer for file size.
     You can remove outSizeHigh, if you don't need >= 4GB supporting,
     or you can use UInt64 outSize, if your compiler supports 64-bit integers*/
  UInt32 outSize = 0;

  SizeT compressedSize;
  unsigned char *inStream;

  SizeT outSizeFull;
  unsigned char *outStream;
  
  CLzmaDecoderState state;  /* it's about 24-80 bytes structure, if int is 32-bit */
  
  unsigned char properties[LZMA_PROPERTIES_SIZE];

  int res;

	/* fari: computes COMPRESSED SIZE (includes the dictionary and the compressed data)*/
  compressedSize = inFileSize - getHeaderSize();  /*  fari <<-- !! length -13 ! */;

  inStream = inputBuffer;

	fprintf(stderr,"\n FARI::header will contain 5+8bytes (properties + uncompressedSize)");
	fprintf(stderr,"\n \tFARI:: COMPRESSED SIZE = %d bytes (infile - (lzmaprperties = %d + 8))", compressedSize,LZMA_PROPERTIES_SIZE);

  /* Read LZMA properties for compressed stream */

	fprintf(stderr,"\n \tFARI:: Reading properties:: sizeof(properties) = %d bytes (tipically 5bytes)", sizeof(properties));

	memcpy((void*) properties, (const void *) inputBuffer, sizeof(properties));
	inStream += (sizeof(properties));


  /* Read uncompressed size */
  {  	
  	/*Aquí sólo carga outsize y outSizeHigh, a partir de 2 uints 32!!!  = 8bytes */	
    int i;
    for (i = 0; i < 8; i++)
    {
      unsigned char b;

		b= *inStream++;
	
		fprintf(stderr,"\n \t\tFARI:: leído==> \"b\" = %d ",  b);
    
      if (i < 4)
        outSize += (UInt32)(b) << (i * 8);
        /* NO QUEREMOS USAR OUTSIZEHIGH, PERO EN LA CABECERA VENÍA ==> SKIP 4 BYTES */
		/* else
			outSizeHigh += (UInt32)(b) << ((i - 4) * 8);
		*/
    }
    outSizeFull = (SizeT)outSize;    
                
   fprintf(stderr,"\n\tFARI::outSize = %d bytes ", outSize);
   
  }

  /* Decode LZMA properties and allocate memory */
    
  if (LzmaDecodeProperties(&state.Properties, properties, LZMA_PROPERTIES_SIZE) != LZMA_RESULT_OK) {
  		fprintf(stderr,"\n Incorrect stream properties");
  		exit(0);	
  }
    
  state.Probs = (CProb *)malloc(LzmaGetNumProbs(&state.Properties) * sizeof(CProb));  

  if (outSizeFull == 0)
    outStream = 0;
  else
    outStream = (unsigned char *)malloc(outSizeFull);


  /** some unnecesary checks in this version */
  if (state.Probs == 0    /* EN ESTE CASO, ABORTA LA DESCOMPRESIÓN !! */
    	|| (outStream == 0 && outSizeFull != 0)
	)
  {
    free(state.Probs);
    free(outStream);
    fprintf(stderr, "unable to allocated memory !");
    exit(0);
  }

  /* Decompress */

  { SizeT inProcessed;
    SizeT outProcessed;
    res = LzmaDecode(&state,
      inStream, compressedSize, &inProcessed,
      outStream, outSizeFull, &outProcessed);
      
    if (res != 0)
    {
      fprintf(stderr, "\nDecoding error = %d\n", res);
      res = 1;
    }
    else if (outFile != 0)
    {
     /* if (!MyWriteFileAndCheck(outFile, outStream, (size_t)outProcessed)) */
      if (! fwrite(outStream, 1, (size_t)outProcessed, outFile))
      {
        fprintf(stderr,"\n Unable to write data!");
        res = 1;
      }
    }

	/* by fari */
		if (outProcessed != outSizeFull) {
			fprintf(stderr,"\n outProcessed != outSizeFull (%d vs %d)\n aborting!.\n", outProcessed, outSizeFull);
			res =1;
			exit(0);
		}    
	/* end by fari */
  }

  free(state.Probs);
  free(outStream);

  return res;
}

int main2(int numArgs, const char *args[])
{
  FILE *inFile = 0;
  uint inFileSize; 
  unsigned char *inBuffer;

  FILE *outFile = 0;
  int res;

  printf("\nLZMA Decoder 4.26 Copyright (c) 1999-2005 Igor Pavlov  2005-08-05, modified by Antonio Fariña (2009-09-09)\n");
  if (numArgs < 2 || numArgs > 3)
  {
    printf("\nUsage:  lzmadecFARI file.lzma [outFile]\n");
    return 1;
  }
  
  inFileSize = (uint) getfileSizeUtils((char *) args[1]);
  inBuffer = (unsigned char *) malloc (inFileSize * sizeof(unsigned char)); 
    
  inFile = fopen(args[1], "rb");
  if (inFile == 0){
    fprintf(stderr, "Can not open input file");
    exit(0);
  }
	
  /* loads the compressed file. */
	/*fread(inBuffer,sizeof(unsigned char), inFileSize, inFile); */
	/*if (!MyReadFileAndCheck(inFile, inBuffer, inFileSize))*/
    if (! fread(inBuffer, 1, (size_t)inFileSize, inFile)) {
    	fprintf(stderr,"\nread failed");
	}

  if (numArgs > 2)
  {
    outFile = fopen(args[2], "wb+");
    if (outFile == 0) {
      fprintf(stderr, "Can not open output file");
      exit(0);
    }
  }

  res = main3(inBuffer,inFileSize, outFile);

  free(inBuffer);

  if (outFile != 0)
    fclose(outFile);
  fclose(inFile);
  return res;
}

int main(int numArgs, const char *args[])
{
  int res = main2(numArgs, args );

  return res;
}
