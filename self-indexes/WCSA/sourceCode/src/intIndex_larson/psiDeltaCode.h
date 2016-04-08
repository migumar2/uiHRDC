#include <stdlib.h>
#include <stdio.h>
#include <malloc.h>
#include <unistd.h>
#include <fcntl.h>




typedef struct {
	unsigned int T;
	unsigned int negativeGap;
	unsigned int deltaCodesSize;	// En palabras
	unsigned int *deltaCodes;					
	unsigned int numberOfSamples;				
	unsigned int *samples;
	unsigned int *pointers;
	unsigned int totalMem;			// the size in bytes used;
} DeltaCompressedPsi;


// PROTOTIPOS DE FUNCI�NS
DeltaCompressedPsi deltaCompressPsi(unsigned int *Psi, unsigned int psiSize, unsigned int T);
int getDeltaPsiValue(DeltaCompressedPsi *cPsi, unsigned int position);
void storeDeltaCompressedPsi(DeltaCompressedPsi *compressedPsi, char *filename);
DeltaCompressedPsi loadDeltaCompressedPsi(char *filename);		
void destroyDeltaCodesCompressedPsi(DeltaCompressedPsi *compressedPsi);
	
// IMPLEMENTACI�N DAS FUNCI�NS

//void destroyDeltaCodesCompressedPsi(DeltaCompressedPsi *compressedPsi) {
//	free(compressedPsi->deltaCodes);
//	free(compressedPsi->samples);
//	free(compressedPsi->pointers);
//}
//
//			
//DeltaCompressedPsi deltaCompressPsi(unsigned int *Psi, unsigned int psiSize, unsigned int T) {
//
//	DeltaCompressedPsi cPsi;
//	
//	int numberOfSamples;
//	register int diff, deltaCodesPos;				 
//	register unsigned int k, p, aux, diffpositive, code, index;
//	unsigned int samplesIndex, codeLenght, currentInput, wordsDeltaCodes, totalSize;
//	unsigned int *deltaCodes;
//	unsigned int *samples;
//	unsigned int *pointers;
//	
//	// Auxiliar para deltaCodes (estimamos como espacio maximo o do array de sufixos)
//	unsigned int *deltaCodesAux;					 
//			 
//	// Calculamos o mellor valor para negativeGap <= 64
//	unsigned int negativeGap;
//	register unsigned int maxNegativeBits = 0;
//	k = psiSize;
//	while(k) {
//		k >>= 1;
//		maxNegativeBits++;		
//	}
//	if(maxNegativeBits<=26) negativeGap = 64;
//	else negativeGap = 1<<(32-maxNegativeBits);				 
//	
//	// Reservamos espacio para as estructuras
//	numberOfSamples = (psiSize + T - 1) / T;
//	samples = (unsigned int *)malloc(sizeof(int)*numberOfSamples);
//	pointers = (unsigned int *)malloc(sizeof(int)*numberOfSamples);
//	
//	deltaCodesAux = (unsigned int *)malloc(sizeof(int)*psiSize);
//	for(index=0; index<psiSize; index++) deltaCodesAux[index] = 0;	 	 
//	
//	samplesIndex = 0;
//	deltaCodesPos = 0;
//	for(index=0; index<psiSize; index++) {
//
//		if(index % T) {
//			
//			diff = Psi[index] - currentInput;
//			currentInput = Psi[index];
//			
//			// Calculamos o codigo correspondente
//			if(diff>0) diffpositive = (negativeGap*diff-1)/(negativeGap-1);
//			else diffpositive = -negativeGap*diff;
//	
//			k = 0;
//			aux = diffpositive;
//			while(aux) {
//				aux >>= 1;
//				k++;
//			}
//			aux = k;
//			p = 0;
//			while(aux) {
//				aux >>= 1;
//				p++;
//			}
//			
//			code = diffpositive & ((1<<(k-1))-1);
//			codeLenght = 2*p+k-2;		
//
//			// Primeiro metemos os p-1 0's iniciais
//			deltaCodesPos += p-1;
//			
//			// Agora metemos os p bits de k
//			if( ((deltaCodesPos%32) + p) > 32 ) {	
//				deltaCodesAux[deltaCodesPos/32] |= (k>>((deltaCodesPos%32)+p-32));
//				deltaCodesAux[deltaCodesPos/32+1] = (k<<(64-(deltaCodesPos%32)-p));	
//			} else {
//				deltaCodesAux[deltaCodesPos/32] |= (k<<(32-p-(deltaCodesPos%32)));				
//			}
//			deltaCodesPos += p;
//			
//			// Por �ltimo metemos os k-1 bits de code (sen o 1 inicial)
//			if( ((deltaCodesPos%32) + (k-1)) > 32 ) {	
//				deltaCodesAux[deltaCodesPos/32] |= (code>>((deltaCodesPos%32)+(k-1)-32));
//				deltaCodesAux[deltaCodesPos/32+1] = (code<<(64-(deltaCodesPos%32)-(k-1)));	
//			} else {
//				deltaCodesAux[deltaCodesPos/32] |= (code<<(32-(k-1)-(deltaCodesPos%32)));				
//			}
//			deltaCodesPos += k-1;
//		
//		} else {
//			samples[samplesIndex] = Psi[index];
//			pointers[samplesIndex++] = deltaCodesPos;			
//			currentInput = Psi[index]; 
//		}	
//
//	}
//	
//	// Ahora que xa sabemos o espacio necesario para os deltaCodes, reservamolo e liberamos a estructura auxiliar
//	wordsDeltaCodes = (deltaCodesPos+31)/32;
//	deltaCodes = (unsigned int *)malloc(sizeof(int)*wordsDeltaCodes);
//	for(index=0;index<wordsDeltaCodes;index++) deltaCodes[index] = deltaCodesAux[index];
//	free(deltaCodesAux);
//	
//	totalSize = sizeof(int)*wordsDeltaCodes + 2*sizeof(int)*numberOfSamples + 4*sizeof(int);
//	printf("Compressed Psi size = %d bytes\n", totalSize);
//	
//	// Asignamos os valores a cPsi e devolvemolo
//	cPsi.T = T;
//	cPsi.negativeGap = negativeGap;
//	cPsi.deltaCodesSize = wordsDeltaCodes;
//	cPsi.deltaCodes = deltaCodes;
//	cPsi.numberOfSamples = numberOfSamples;
//	cPsi.samples = samples;
//	cPsi.pointers = pointers;
//	return cPsi;
//	
//}
//
//
//int getDeltaPsiValue(DeltaCompressedPsi *cPsi, unsigned int position) {
//	
//	int result;
//	register unsigned int code, aux, pointerAux, mask, pointer, toDecode, p, k;
//	
//	// Collemos a mostra inmediatamente inferior, e o punteiro o array de codigos
//	// pointer = punteiro absoluto sobre deltaCodes
//	result = cPsi->samples[position/cPsi->T];
//	pointer = cPsi->pointers[position/cPsi->T];
//	
//	// Calculamos o numero de codigos a decodificar a partir da mostra
//	toDecode = position % cPsi->T;	
//	
//	while(toDecode--) {
//		
//		// Collemos o n�mero ceros iniciais
//		// Po�emos o inicio do c�digo nun enteiro (code) alineado a esquerda
//		// Non importa que non colla todo o c�digo, pero si temos asegurado que
//		// colle p e k (k<=32 (6bits), p<=5bits)
//		code = (cPsi->deltaCodes[pointer/32] << (pointer%32)) |
//			   ((pointer%32 != 0) * (cPsi->deltaCodes[pointer/32+1] >> (32-(pointer%32))));
//		
//		//Ahora contamos o n�mero de ceros (p) que hai nas posicions da esquerda de code
//		p = 1;
//		while(!(code & 0x80000000)) {
//			code <<= 1;
//			p++;
//		}
//		
//		// Ahora calculamos o numero de digitos da representacion binaria do codigo (k)
//		k = code >> (32-p);		
//		
//		// Actualizamos o punteiro global sobre deltaCodes
//		pointer += 2*p-1;
//		
//		// Po�emos a representacion binaria do codigo nun enteiro (code) alineado a esquerda
//		code = (cPsi->deltaCodes[pointer/32] << (pointer%32)) |
//			   ((pointer%32 != 0) * (cPsi->deltaCodes[pointer/32+1] >> (32-(pointer%32))));
//		code = ((code >> 1) | 0x80000000) >> (32-k);
//		pointer += k-1;
//		
//		// Bixecci�n
//		if(code % cPsi->negativeGap) result += (code - (code/cPsi->negativeGap));
//		else result -= code/cPsi->negativeGap;
//			
//	}
//	
//	return result;
//	
//}
//
//
//void storeDeltaCompressedPsi(DeltaCompressedPsi *compressedPsi, char *filename) {
//
//	int file;
//
//	if( (file = open(filename, O_WRONLY|O_CREAT, 0700)) < 0) {
//		printf("Cannot open file %s\n", filename);
//		exit(0);
//	}
//	write(file, &(compressedPsi->T), sizeof(int));
//	write(file, &(compressedPsi->negativeGap), sizeof(int));
//	write(file, &(compressedPsi->deltaCodesSize), sizeof(int));
//	write(file, compressedPsi->deltaCodes, compressedPsi->deltaCodesSize*sizeof(int));
//	write(file, &(compressedPsi->numberOfSamples), sizeof(int));
//	write(file,	compressedPsi->samples, compressedPsi->numberOfSamples*sizeof(int));
//	write(file,	compressedPsi->pointers, compressedPsi->numberOfSamples*sizeof(int));
//	close(file);
//		
//}
//
//
//DeltaCompressedPsi loadDeltaCompressedPsi(char *filename) {
//	
//	DeltaCompressedPsi compressedPsi;
//
//	int file;
//
//	if( (file = open(filename, O_RDONLY)) < 0) {
//		printf("Cannot read file %s\n", filename);
//		exit(0);
//	}
//	read(file, &(compressedPsi.T), sizeof(int)); 
//	read(file, &(compressedPsi.negativeGap), sizeof(int));
//	read(file, &(compressedPsi.deltaCodesSize), sizeof(int));
//	compressedPsi.deltaCodes = (unsigned int *)malloc(compressedPsi.deltaCodesSize*sizeof(int));
//	read(file, compressedPsi.deltaCodes, compressedPsi.deltaCodesSize*sizeof(int));
//	read(file, &(compressedPsi.numberOfSamples), sizeof(int));	
//	compressedPsi.samples = (unsigned int *)malloc(compressedPsi.numberOfSamples*sizeof(int));
//	compressedPsi.pointers = (unsigned int *)malloc(compressedPsi.numberOfSamples*sizeof(int));
//	read(file, compressedPsi.samples, compressedPsi.numberOfSamples*sizeof(int));
//	read(file, compressedPsi.pointers, compressedPsi.numberOfSamples*sizeof(int));		
//	close(file);
//		
//	return compressedPsi;
//
//}
