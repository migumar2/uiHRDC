#ifndef LZMA_API_INCLUDED
#define LZMA_API_INCLUDED

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h> //for the getpid();

#ifndef uchar 
	#define uchar unsigned char
#endif
           

#define LZMA_DEC_TO_BC(bc, pos, num)        \
{                                           \
 while (num >= 128) {                       \
 bc[pos++] = (uchar)((num & 127) + 128);     \
 num = (num >> 7);                          \
}                                           \
bc[pos++] = (uchar) num;                     \
}

#define LZMA_SIZE_DEC_TO_BC(num,size)       \
{uint numtmp=num;                           \
  size=0;                                   \
  while (num >= 128) {                      \
   size++;                                  \
   num = (num >> 7);                        \
  }                                         \
size++; num=numtmp;                         \
}			
			
#define LZMA_BC_TO_DEC(bc,pos,num) \
 { \
 	 register uint _pot, _x; \
 	 _pot=1; \
 	 _x = bc[pos++]; \
 	 num=0; \
 	 while (_x >= 128) { \
 	 	num = num + (_x-128)*_pot; \
 	 	_x=bc[pos++]; \
 	 	_pot<<=7; \
 	 } \
 	 num=num + _x*_pot; \
 }




/******************************************************************************/
/* Public functions */
/******************************************************************************/

	/* initializes a tlzma structure */
	int initLzmaStruct(void **lzmadataV);  

	/* frees a tlzma structure */
	int freeLzmaStruct(void *lzmadataV);
	
	/* Compresses the data with system program *lzmaEncoder*
	 * data is first saved into file "TMP_FILE_TO_COMPRESS_VBYTE", then 
	 * lzmaEncoder e <src> <dst> is called and file "TMP_FILE_COMPRESS_LZMA" created.
	 */
	int compressDataExt(uchar *data, uint n, char *path2LzmaEncoder);

	/* Loads the data from file "TMP_FILE_COMPRESS_LZMA" into memory.
	 */
	int loadDataExt(uchar **data, uint *readen);

	/* Given the source stream "inb" containing data compressed with lzmaEncoder,
	 * returns an initialized structure "lzmadata" keeping the compressed data and
	 * the headers (flags, dictSize, uncompressedSize) needed to perform decompression.
	 * @lzmadata structure is not allocated here. 
	 */
	int createLzmaExt(uchar *inb, uint inn, void *lzmadata);

	/* Gives the size (in bytes) of a given tlzmaStructure 
	 * It counts the fields: properties (5), sizeunc (4) and compdata[]  
	 */
	uint sizeBytesLzmaStruct(void *lzmadata);
	
	/* Serializes the structure tlzma.
	 * Writes its data into @outstream[pos...]  
	 * The next @pos (after the output data) is returned.
	 */
	uint writeBuffLzmaStruct(uchar *outstream, uint pos, void *lzmadata);

	/* Deserializes the structure tlzma.
	 * Loads its data (inn bytes) from pointer @inb
	 */
	int  readBuffLzmaStruct(uchar *inb, uint inn, void *lzmadata);


	/* Decodes the lzma data stored in the structure tlzma.
	 * Writes extracted data o an output buffer that is also allocated. 
	 */
	int decodeLzma (void *lzmadata, uchar **outB, uint *outlen);
	
	
/******************************************************************************/
/* Some private functions */
/******************************************************************************/

	/* the size of a file */
	unsigned long getfileSize (const char *filename);
	
	/* the size of the header (props + lenUncomp-text) in bytes of a lzma compressed file */
	uint getHeaderSizeOriginal();

#endif
