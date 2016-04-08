#include "scdc.h"
////////////////// Dense Codes ////////////////
	uint SS,CC;
	uint TABLABASE[5]; // = {0,128,16512,2113664,270549120};
	
void initializeSC_baseTable (uint s){
	uint c = 256 - s;	
	TABLABASE[0] = 0;
	TABLABASE[1] = s;
	TABLABASE[2] = TABLABASE[1] + s * c;
	TABLABASE[3] = TABLABASE[2] + s * c*c;
	TABLABASE[4] = TABLABASE[3] + s * c*c*c;
}

uint decodeETDC (byte *etdc, register uint pos, uint *num) {
	register uint k=pos;
	register uint j;
	j=0;
	while (etdc[pos] >=128) {
		j = j * 128 + (etdc[pos] - 128);
		pos++;
	}
	j = j * 128 + etdc[pos];
	j += TABLABASE[pos-k]; //the position of the word in the vocabulary
	pos++;
	*num = j;
	return pos;
}


inline uint decodeSC (byte *scdc, register uint pos, uint *num) {
	register uint k=pos;
	register uint j;
	register uint s = SS;
	register uint c = CC;
	j=0;
	while (scdc[pos] >=s) {
		j = j * c + (scdc[pos] - s);
		pos++;
	}	
	j = j * s + scdc[pos];
	j += TABLABASE[pos-k]; //the position of the word in the vocabulary
	pos++;
	*num = j;
	return pos;
}


/* uncompresses a part of a text compressed with scdc
 * previousAlphanumerical should usually be initialized to 0 (so no extra blank is added at the beginning).
 */ 
void uncompressTextSCDC(byte *scdc, uint iniByte, uint endByte, byte **vocabWords, uint *previousAlphanumerical, byte *outBuffer) {
	uint j;
	uint pos;
	byte *dst = outBuffer;
	byte *src;
	//fprintf(stderr,"\n parametros ==>> inibyte = %d, endbyte = %d, prevAlphanum= %d\n",iniByte,endByte, *previousAlphanumerical);

	for (pos=iniByte; pos < endByte; ) {
		pos = decodeSC (scdc, pos, &j);
		src = vocabWords[j];
	
		//checks if a BLANK must be added (spaceless-model)
		if (_Valid[ *src ] ) {
			if (*previousAlphanumerical) *dst++ = ' ';
			*previousAlphanumerical = 1;
		}
		else *previousAlphanumerical = 0;

		while (*src != '\0') *dst++ = *src++;
		//*dst=0;
		//fprintf(stderr,"\n word[%d] = %s:: len = %d, currText = \n %s\n",j,vocabWords[j],strlen((const char *)outBuffer),outBuffer);

	}
}

/* computes the len of the uncompressed text obtained when scdcLen bytes are decoded from *scdc codewords stream.
 * previousAlphanumerical should usually be initialized to 0 (so no extra blank is added at the beginning).
 */ 
uint computeUncompressedSizeSCDC(byte *scdc, uint scdcLen, byte **vocabWords, uint *previousAlphanumerical) {
	uint j;
	uint pos;
	byte *src;
	uint size=0;

	for (pos=0; pos < scdcLen; ) {
		pos = decodeSC (scdc, pos, &j);
		src = vocabWords[j];
			
		//checks if a BLANK must be added (spaceless-model)
		if (_Valid[ *src ] ) {
			if (*previousAlphanumerical) size++;
			*previousAlphanumerical = 1;
		}
		else *previousAlphanumerical = 0;

		//size +=strlen((const char *)src);
		while (*src != '\0') {size++; src++;}
	}
	return size;
}
