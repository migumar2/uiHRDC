
#include "farilzmatypes.h"
#include "farilzma.h"



/******************************************************************************/
/* Public functions */
/******************************************************************************/


/* initializes a tlzma structure */
int initLzmaStruct(void **lzmadataV) {
	tlzma *str = (tlzma *) malloc (1 * sizeof(tlzma));
	*lzmadataV = str;
	return 1;
}

/* frees a tlzma structure */
int freeLzmaStruct(void *lzmadataV) {
	tlzma *ptr = (tlzma *) lzmadataV;
	free(ptr);
	//unlink(TMP_FILE_COMPRESSED_LZMA);
	
	static char filevbytelzma[1000];
	sprintf(filevbytelzma,"%s_%d", TMP_FILE_COMPRESSED_LZMA, getpid());	
	unlink(filevbytelzma);
	return 1;
}


/* Compresses the data with system program *lzmaEncoder*
 * data is first saved into file "TMP_FILE_TO_COMPRESS_VBYTE", then 
 * lzmaEncoder e <src> <dst> is called and file "TMP_FILE_COMPRESS_LZMA" created.
 */
int compressDataExt(uchar *data, uint n, char *path2LzmaEncoder) {
	static char command[1000];
	static char filevbyte[1000];
	static char filevbytelzma[1000];
	sprintf(filevbyte,"%s_%d", TMP_FILE_TO_COMPRESS_VBYTE, getpid());
	sprintf(filevbytelzma,"%s_%d", TMP_FILE_COMPRESSED_LZMA, getpid());
	FILE *f;
	command[0]='\0';
	/** writes data to a temp file **/
	unlink(filevbyte);
	if (! (f = fopen(filevbyte, "wb+"))) {
		fprintf(stderr, "Cannot open temp files for compression"); exit(0);
	}
	fwrite(data,sizeof(char), n, f);
	fclose(f);
	
	/** now compressing the file */
	sprintf(command,"%s e %s %s",path2LzmaEncoder, filevbyte, filevbytelzma);
//	sprintf(command,"%s e %s %s 1>/dev/null",path2LzmaEncoder, TMP_FILE_TO_COMPRESS_VBYTE, TMP_FILE_COMPRESSED_LZMA);
	
	if (system(command) == -1) {
		fprintf(stderr,"\n compression with lzma algorithm failed. Program will now exit!!\n");exit(0);
	}
	return 0;		
}
	
	
/* Loads the data from file "TMP_FILE_COMPRESS_LZMA" into memory.
 */
int loadDataExt(uchar **data, uint *readen) {
	static char filevbyte[1000];
	static char filevbytelzma[1000];
	sprintf(filevbyte,"%s_%d", TMP_FILE_TO_COMPRESS_VBYTE, getpid());
	sprintf(filevbytelzma,"%s_%d", TMP_FILE_COMPRESSED_LZMA, getpid());	
	
	FILE *f; uint lenFile;
	uchar *buffer;
	unlink(filevbyte);
	if (! (f = fopen(filevbytelzma, "rb"))){ 
		fprintf(stderr, "Can ot open temp files for compression. Exitting...!\n"); exit(0);
	}	

	lenFile = getfileSize(filevbytelzma);
	buffer = (uchar *) malloc (sizeof(uchar) * lenFile);
	if (lenFile != fread(buffer,sizeof(char), lenFile, f)){
		fprintf(stderr, "Cannot read temp files \"loadDataExt()\" ... exitting...!\n "); exit(0);
	}
	fclose(f);	
	*data=buffer;
	*readen = lenFile; 
	return 0;
}
	

/* Given the source stream "inb" containing data compressed with lzmaEncoder,
 * returns an initialized structure "lzmadata" keeping the compressed data and
 * the headers (flags, dictSize, uncompressedSize) needed to perform decompression.
 * @lzmadata structure is not allocated here.
 */
int createLzmaExt(uchar *inb, uint inn, void *lzmadataV) {
	uint uncompressedSize, compressedSize;
	tlzma *lzmadata = (tlzma *) lzmadataV;
	uint headerSize = getHeaderSizeOriginal();
	uncompressedSize = 0;
	
	memcpy((void*) lzmadata->properties, (const void *) inb, sizeof(lzmadata->properties));

	{ /** Read uncompressed size (inb[5..8] && skipping inb[9..12] )*/
		/** Aquí sólo carga outsize y outSizeHigh, a partir de 2 uints 32!!!  = 8bytes */
		int i; unsigned char b; uchar *inStream;
		inStream = &(inb[ LZMA_PROPERTIES_SIZE ]); /** position &inb[5] */
		for (i = 0; i < 4; i++) { 
			b= *inStream++;
			uncompressedSize += (UInt32)(b) << (i * 8);
		}
	}

	/* size of the compressed data + the dictionary *
	 */	
	compressedSize = inn - headerSize; /**  fari <<-- !! length -13 ! */;;  
	
	lzmadata->sizeunc  = uncompressedSize;
	lzmadata->compdata = &(inb[headerSize]); /* &(inb[13]) */
	lzmadata->compdataSize = compressedSize; /* 32-bit used for uncompressedSize instead of 8bytes */
	
	lzmadata->memusage = inn - 4 ;   /* 32-bit used for uncompressedSize instead of 8bytes*/
	lzmadata->memusage -= 4; /*DICT size (in properties field) not used */	
	
	{ uint numb; //size of uncompressedSize encoded with bytecodes instead of a 32-bit uint.
	  LZMA_SIZE_DEC_TO_BC(uncompressedSize,numb);
	  lzmadata->memusage -= 4;
	  lzmadata->memusage += numb;  /*the size of the bytecode */
	  fprintf(stderr,"\n uncompSize is encoded with %d bytes",numb);
    }
    
	return 0;
}

/* Gives the size (in bytes) of a given tlzmaStructure 
 * It counts the fields: properties (5), sizeunc (4) and compdata[]  
 */
uint sizeBytesLzmaStruct(void *lzmadataV) {
	tlzma *lzmadata = (tlzma *) lzmadataV;
	return lzmadata->memusage;
}

/* Serializes the structure tlzma.
 * Writes its data into @outstream[pos...]  
 * The next @pos (after the output data) is returned.
 */
uint writeBuffLzmaStruct(uchar *outstream, uint pos, void *lzmadataV) {
	uchar *dst; 
	tlzma *lzmadata = (tlzma *) lzmadataV;
	uint numb;
	/** writes properties data : prop-flags && dictionary size */
	dst = &(outstream[pos]);
	memcpy ((void *) dst , (const void *) lzmadata->properties, sizeof(lzmadata->properties) );  //just 1 byte
	pos += sizeof(lzmadata->properties);
	
	/** writes size uncompressed */
	dst = &(outstream[pos]);
	//memcpy ((void *) dst , (const void *) &lzmadata->sizeunc, sizeof(uint) );
	//pos += sizeof(uint);
	
	numb=0;
	LZMA_DEC_TO_BC(dst,numb,lzmadata->sizeunc);
	pos +=numb;

	/** writes the compressed data (dict + comp stream) */

	dst = &(outstream[pos]);
	memcpy ((void *) dst , (const void *) lzmadata->compdata, sizeof(uchar) * lzmadata->compdataSize );
	pos += (sizeof(uchar) * lzmadata->compdataSize);	
	
	return pos;
}
	
/* Deserializes the structure tlzma.
 * Loads its data (inn bytes) from pointer @inb
 */
int  readBuffLzmaStruct(uchar *inb, uint inn, void *lzmadataV) {
	uchar *dst, *src; 
	uint remainbytes;
	uint numb;
	tlzma *lzmadata = (tlzma *) lzmadataV;

	lzmadata->memusage = inn; /* 32-bit used for uncompressedSize instead of 8bytes */	
	src= inb; remainbytes = inn;
	/** writes read data : prop-flags && dictionary size */
	dst = (uchar *) &(lzmadata->properties);
	memcpy ((void *) dst , (const void *) src, sizeof(lzmadata->properties) );
	src += sizeof(lzmadata->properties);
	remainbytes -= sizeof(lzmadata->properties);
	
	/** read size uncompressed */
	//dst = (uchar *) &(lzmadata->sizeunc);
	//memcpy ((void *) dst , (const void *) src, sizeof(uint) );
	//src += sizeof(uint);
	//remainbytes -= sizeof(uint);
	
	numb=0;
	LZMA_BC_TO_DEC(src, numb, lzmadata->sizeunc);
	src +=numb;
	remainbytes -=numb;

	/** read the compressed data (dict + comp stream) */

	/*dst = (uchar *) &(lzmadata->compdata);*/
	/*memcpy ((void *) dst , (const void *) src, sizeof(uchar) * remainbytes ); */
	lzmadata->compdata = src;
	lzmadata->compdataSize = remainbytes; 
	return 0;
}	
	

/* Decodes the lzma data stored in the structure tlzma.
 * Writes extracted data o an output buffer that is also allocated. 
 */
int decodeLzma (void *lzmadataV, uchar **outB, uint *outlen) {

	SizeT compressedSize;
	UInt32 outSize = 0;
	unsigned char *inStream;
	SizeT outSizeFull;
	unsigned char *outStream;

	CLzmaDecoderState state;  /** it's about 24-80 bytes structure, if int is 32-bit */

	unsigned char properties[FARI_LZMA_PROPERTIES_SIZE];
	int res;
	tlzma *lzmadata = (tlzma *) lzmadataV;

	/** Read LZMA properties for compressed stream */

	memcpy((void*) properties, (const void *) lzmadata->properties, sizeof(properties));

	/** Read uncompressed size */
 	outSize = lzmadata->sizeunc;
    outSizeFull = (SizeT)outSize;    

	/** Decode LZMA properties and allocate memory */
	if (LzmaDecodeProperties(&state.Properties, properties, LZMA_PROPERTIES_SIZE) != LZMA_RESULT_OK) {  /** FARI: MANDATORY TO USE HERE: LZMA_PROPERTIES_SIZE INSTEAD OF FARI_LZMA_PROPERTIES_SIZE (due to check inside the function) */
		fprintf(stderr,"\n Incorrect stream properties");
		exit(0);	
	}
   	state.Probs = (CProb *)malloc(LzmaGetNumProbs(&state.Properties) * sizeof(CProb));  

	/** Preparing for decompression */
	if (outSizeFull == 0)
		outStream = NULL;
	else
		outStream = (unsigned char *)malloc(outSizeFull);

	inStream = lzmadata->compdata;
	compressedSize = lzmadata->compdataSize;

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

  	/** Decompress */
	{ 
	  	SizeT inProcessed, outProcessed;
		res = LzmaDecode(&state,
						inStream, compressedSize, &inProcessed,
						outStream, outSizeFull, &outProcessed);
		  
		if (res != 0) {
			fprintf(stderr, "\nDecoding error = %d\n", res);
			res = 1;
		}
		
		if (outProcessed != outSizeFull) {
			fprintf(stderr,"\n outProcessed != outSizeFull (%d vs %d)\n aborting!.\n", outProcessed, outSizeFull);
			res =1;
		}
	}

	free(state.Probs);
	*outB = outStream;
	*outlen = outSizeFull;
	return res;
}


/******************************************************************************/
/* Private functions */
/******************************************************************************/

unsigned long getfileSize (const char *filename){
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

uint getHeaderSizeOriginal(){
	return (uint)(LZMA_PROPERTIES_SIZE + 8);
}
