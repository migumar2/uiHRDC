
////////////////// Dense Codes ////////////////



#ifndef byte
#define byte unsigned char
#endif
#ifndef uint
#define uint  unsigned int
#endif
#ifndef ulong
#define ulong unsigned long
#endif

#include "valstring.h"


	void initializeSC_baseTable (uint s);

	uint decodeETDC (byte *etdc, register uint pos, uint *num);
	uint decodeSC (byte *scdc, register uint pos, uint *num);

	/* uncompresses a part of a text compressed with scdc
	 * previousAlphanumerical should usually be initialized to 0 (so no extra blank is added at the beginning).
	 */ 
	void uncompressTextSCDC(byte *scdc, uint iniByte, uint endByte, byte **vocabWords, uint *previousAlphanumerical, byte *outBuffer) ;

	/* computes the len of the uncompressed text obtained when scdcLen bytes are decoded from *scdc codewords stream.
	 * previousAlphanumerical should usually be initialized to 0 (so no extra blank is added at the beginning).
	 */ 
	uint computeUncompressedSizeSCDC(byte *scdc, uint scdcLen, byte **vocabWords, uint *previousAlphanumerical) ;


