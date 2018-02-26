/* Words extractor -- 

Parses a text file and computes the different words in it.
Then, it applies a filter by frequency of word and/or the lenght
of the words, in such a way that only the "filtered words" are
output to the target file.

This target file is useful since it contains a set of patterns that
can be later searched inside the input text file. 

Copyright (C) 2005 Antonio Fariña.

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

Author's contact: Antonio Fariña, Dept. of Computer Science, University of
A Coruña. Campus de Elviña s/n. Spain  fari@udc.es
*/
   


#include "extractor.h"


void printWord(byte *str, uint len) {
		uint i;
		fflush(stdout);fflush(stderr);
		fprintf(stdout,"<<");
		for (i=0;i<len;i++)
			fprintf(stdout,"%c",str[i]);
		fprintf(stdout,">>");
		fflush(stdout);fflush(stderr);
}

size_t getLongRand() {
	uint bits = 40;
	size_t x = (size_t) rand();
	size_t n = (x<<31L) | ((size_t)rand());
	n = n>>(62L - bits);
	return n;
} 

unsigned long myrand() {
	int i;
	unsigned long total=0;
	for (i=0;i<100; i++)
		total += rand();
		
	return (total % MAX_RAND_VALUE_FARI);
}
#ifdef M64
unsigned long int_rand(unsigned long n){
	return ((unsigned long)  myrand()%n);
}
#endif

/*
unsigned long int_rand_range(unsigned long  a,unsigned long b){
	unsigned long n = (b-a+1);
	return a+ ((long)  (myrand(n)%n) );
}

*/

//ok for 32bit machine
#ifdef M32
unsigned long int_rand(unsigned long n){
	return ((int)  ((1.0*rand()/ (RAND_MAX + 1.0)) * n));
}
#endif

int int_rand_range(unsigned long  a,unsigned long b){
	unsigned long n = (b-a+1);
	return a+ ((int)  ((n*1.0*rand()/ (RAND_MAX + 1.0)) ));
}


int getPhrase (unsigned char *bufferIni, unsigned char *bufferEnd,
					uint numberOfWords, unsigned char *phrase,
					uint *cutPos, uint *sizes, uint *cutPosTot) {
						

	unsigned char *pbeg, *pend, *dst;
	unsigned char *aWord, *wordstart;
	
	uint counter=0, totalSize=0;
	uint size;
	
	uint prevPos;
	
	pbeg=bufferIni;
	pend=bufferEnd;
	dst = phrase;

	//skips till next valid word.
	while ((_Valid[*pbeg]) && (pbeg < pend)) pbeg++;
	//pbeg++;		
	//skips till next valid word.
	while ((!_Valid[*pbeg]) && (pbeg < pend)) pbeg++;
	if (pbeg >= pend) {return -1;}
	
	*cutPosTot =0;			
	//cutPos[*cutPosTot]=0;
	prevPos=0;
	
	//parsing until numberOfWords valid words are parsed
 	while (pbeg < pend )   {
		//parsing a word or separator
		size=0;
		wordstart = pbeg;
		if (_Valid[*pbeg]) {   //alphanumerical data
			while ( (size<MAX_SIZE_OF_WORD) && ( _Valid[*pbeg] )&& (pbeg<=pend)) {size++; *dst++=*pbeg++;}
			counter++;
	    }
      	else { // a separator
			while ( (size<MAX_SIZE_OF_GAP) && (!_Valid[*pbeg]) && (pbeg<=pend)) {size++; *dst++=*pbeg++;}
			if ( (!onlyWords)&& ((size>1) || ( (size==1) && (*(pbeg-1) !=' ') ))  ) counter++;
		}

  		if ( pbeg >= pend ) {  
  			return -1;
  		}

		if ((size ==1) && (*wordstart == ' ')) { // a single space: SPACELESS WORD-MODEL.
			prevPos ++;
		}
		else {
			//cutPos[*cutPosTot]=cutPos[(*cutPosTot)-1]+size;			
			cutPos[*cutPosTot]=prevPos;
			sizes[*cutPosTot]=size;
			prevPos +=size;
			*cutPosTot = *cutPosTot +1;
		}
		
		aWord=wordstart;  //the word parsed.
		totalSize +=size;
		if (stopwordshash) {  //skips stopwords for 
			int i;
			unsigned long addrInTH;
			static char lowWord[MAX_SIZE_OF_GAP+MAX_SIZE_OF_WORD];  //used to lowercase a given word --> comparison if "stopword".
			
			toLow(lowWord,wordstart,size);
			//fprintf(stdout,"\n checking if stopword for "); printWord(wordstart,size); printWord(lowWord,size);
			i = stinHashTable(stopwordshash,lowWord, size, &addrInTH);
			if (i) return -2;
		}

		if (counter >=numberOfWords) break;
		
    	}/* internal while*/

		if (counter >=numberOfWords) {
			*dst='\0';
			//fprintf(stdout,"\n devolveré frase <<%s>> con len %d ", phrase, totalSize);
			return (totalSize); //returns size    	
		}
		return -1;
}



int estaRepetida (unsigned char *phrase) {
	uint i;
	for (i=0;i<countrepetidas;i++) {
		if (strcmp (phrase,repetidas[i]) ==0) return 1;	
	}
	return 0;
	
}
/*------------------------------------------------------------------
 * Performs the full parse process.
		- parsing: gather words and frequencies.
		- sorting: words are sorted by frequency.
	* Outputs:
		-modifies: hash, positionInTH, zeroNode.
------------------------------------------------------------------ */
void parseFile (char* infile,  uint inFileSize, char *nameoutFile, uint numberOfWords_min, uint numberOfWords_max, uint numberOfPhrases) {


	register unsigned long k,i,j;

	FILE *fd;	
	uint len;
	FILE *outFile ;
	unsigned char *phrase,*buffer, *end;
	uint iterations;

	buffer = (unsigned char *) malloc (BUFFERSIZE+1);
	phrase = (unsigned char *) malloc (BUFFERSIZE+1);

	ulong *cutPos;  //positions where a different word starts from the random phrase.
	ulong *sizes;
	ulong cutPosTot;
	cutPos = (uint *) malloc (BUFFERSIZE/2);	
	sizes = (uint *) malloc (BUFFERSIZE/2);	

	ulong randomOffset;
	
	// Inicializes the arrays used to detect if a char is valid or not.	
   	StartValid();
	// Inicializes the arrays used translated a char into lowercase.
	StartToLow();
	
	// Initialization of read buffer
	fd = fopen (infile, "r");
	if (fd == NULL) {
		printf("Cannot read file %s",infile);
		exit(0);
	}

	
	if ((outFile = fopen(nameoutFile,"w")) == NULL){
		fprintf(stdout,"\nError while opening outputfile: %s\n",nameoutFile);
		fprintf(stdout,"Program will exit now\n");
		return;
	}	

	srand(time(NULL));

	fprintf(stdout, "\n    ***  EXTRACTION OF WORDS BEGINS ...\n");

	iterations = 0;
	outputNumPatterns(outFile,numberOfPhrases, numberOfWords_min, numberOfWords_max);
	
	while ( iterations < numberOfPhrases) {
		if (!(iterations %1000)) fprintf(stdout,"#");
	
		randomOffset = int_rand(inFileSize);
		fprintf(stdout,"\n rand_offset in [0..%lu]= %ld",inFileSize,randomOffset);
	
	//	if (lseek(fd, randomOffset, SEEK_SET) == (off_t) -1) {
		if (fseek(fd, randomOffset, SEEK_SET) == (off_t) -1) {
			printf("Error haciendo fseek a la posicion %d\n", randomOffset);
			exit(1);
		}
		
		//printf("\nhaciendo lseek a la posicion %d\n", randomOffset);
			
		//len = read(fd, buffer, BUFFERSIZE);
		len = fread(buffer, sizeof(char), BUFFERSIZE, fd);
		if (len < 0) {
			printf("Error leyendo fichero %s: \n", infile);
			exit(1);
		}
	
		end = (randomOffset+ BUFFERSIZE >inFileSize)? buffer + (inFileSize-randomOffset): buffer+BUFFERSIZE;
		
		uint numberOfWords = int_rand_range(numberOfWords_min, numberOfWords_max);
		fprintf(stdout,"\n random entre [%u - %u] = %u", numberOfWords_min, numberOfWords_max, numberOfWords);
			
		if ((len= getPhrase (buffer,end,numberOfWords, phrase,cutPos,sizes, &cutPosTot)) != -1) {
			
	/*		
			if (stopwordshash) {  //skips stopwords for 
				int i;
				uint addrInTH;
				i = stinHashTable(stopwordshash,phrase, strlen(phrase), &addrInTH);
				if (i) continue;
			}
	*/
			
			if (len ==-2) continue;
			
			if (estaRepetida(phrase)) {
				continue;
			}
			
			repetidas[countrepetidas] = (unsigned char *) malloc (len+1);
			strcpy (repetidas[countrepetidas], phrase);
			countrepetidas++;
			
		//	if (len <5) continue;
			outputWord(outFile,phrase,len, numberOfWords);
			//fprintf(outFile,"%9d %9d %s\n",0,0,phrase);
			//fprintf(stdout,"\nPHRASE: %9d %9d [%s]",0,0,phrase);
			//			{uint kk=0;
			//			fprintf(stdout,"\nPOSITIONS: ");
			//			   for (kk=0;kk<cutPosTot;kk++) {fprintf(stdout,"[%2u]",cutPos[kk]);}
			//			fprintf(stdout,"\n SIZES   : ");
			//			   for (kk=0;kk<cutPosTot;kk++) {fprintf(stdout,"[%2u]",sizes[kk]);}
			//			}
			//outputSeparateWords(outFile, phrase,cutPos, sizes, cutPosTot );			
			
			iterations++;
		}
		*phrase= '\0';
	}
	fprintf(stdout, "... EXTRACTION OF WORDS FINISHED \n");

	/********************** Beginning of parse process **********************/



	//BUFFER;
	free(buffer);
	free(phrase);
	free(cutPos);
	free(sizes);
	for (i=0;i<countrepetidas;i++) free(repetidas[i]);
	
	//close(fd);
	fclose(fd);
	fclose(outFile);
	
} 

/*------------------------------------------------------------------
 Writes the integer indicating the number of patterns of the sample file.
------------------------------------------------------------------ */
void outputNumPatterns(FILE *outFile, uint num, uint numberOfWords_min, uint numberOfWords_max) {
	uint nwords = 000000000 + numberOfWords_min*10000000 + numberOfWords_max*1000;
	fprintf(outFile,"#numberOfFrases::%9d, #NumberOfWordsPerPhrase:: %9d\n",num, nwords);
}
/*------------------------------------------------------------------
 Filters the words in the vocabulary and writes them to the
 output file.
------------------------------------------------------------------ */
void outputWord(FILE *outFile, unsigned char* phrase, uint len, uint numberOfWords) {
	static counter =0;
	fprintf(outFile,"%9d %9d %s\n",counter,len,phrase);
	fprintf(stdout,"%9d %9d %d %s\n",counter,len,numberOfWords,phrase);
	counter++;
}



//reads a phrase (a phrase composed of n words)
unsigned char *inputWord(FILE *infile, ulong *len) {
	ulong tmp; //skips one integer.
	unsigned char *phrase;
	
	fscanf(infile,"%9d %9d ", &tmp, len); 
	phrase = (unsigned char * ) malloc (sizeof(char) * ((*len)+1));
	fread(phrase,sizeof(unsigned char), *len, infile);
	phrase[*len] = '\0';
	fscanf(infile,"\n");
	
	return phrase; 
}

void showCreatedPhrases(FILE *infile) {
	unsigned char *patt;
	ulong i,len=0;

	ulong numPhrase,numOfWords;
	
	fscanf(infile,"#numberOfFrases::%9d, #NumberOfWordsPerPhrase:: %9d\n",&numPhrase, &numOfWords);
	fprintf(stdout," numOfFrase = %9d and numberOfWordsPerPhrase = %9d\n",numPhrase, numOfWords);
		
	for (i=0;i<numPhrase;i++) {
		patt =inputWord(infile, &len);	
		fprintf(stdout,"[%lu]:::  patLen= %3d, patt=<<%s>>\n",i,len,patt);
		free(patt);
		if (i>numPhrase) break;
	}
}

/*------------------------------------------------------------------
 Writes the words that belong to each pattern.
------------------------------------------------------------------ */
//void outputSeparateWords(FILE *outFile, unsigned char *phrase,
//					uint *cutPos, uint *sizes, uint cutPosTot ) {
//
//	uint i=0;
//	unsigned char *str;
//	unsigned char buff[10000];
//	uint len;
//	//fprintf(stdout,"\n<<%s>>\n",phrase);
//	for (i=0; i<cutPosTot;i++) {
//		str = & (phrase[cutPos[i]]);
//		len = sizes[i];
//		strncpy (buff,str,len);
//		buff[len]='\0';
//		fprintf(outFile,"\t\t%9d<<%s>>\n",len, buff);
//		//fprintf(stdout,"\t\t%9d<<%s>>\n",len, buff);
//	}
//}

/*------------------------------------------------------------------
 Writes the words that belong to each pattern.
------------------------------------------------------------------ */
void outputSeparateWords(FILE *outFile, unsigned char *phrase,
					uint *cutPos, uint *sizes, uint cutPosTot ) {

	uint i=0;
	unsigned char *str;
	unsigned char buff[10000];
	uint len;
	//fprintf(stdout,"\n<<%s>>\n",phrase);
	for (i=0; i<cutPosTot;i++) {
		str = & (phrase[cutPos[i]]);
		len = sizes[i];
		strncpy (buff,str,len);
		buff[len]='\0';
		fprintf(outFile,"[%9d][%s]",len, buff);
		//fprintf(stdout,"\t\t%9d<<%s>>\n",len, buff);
	}
	
	
	
	fprintf(outFile,"\n");
}





/*------------------------------------------------------------------
 Obtains the size of the file to process.
------------------------------------------------------------------ */
unsigned long fileSize (char *filename) {
	FILE *fpText;
	unsigned long fsize;
	fpText = fopen(filename,"rb");
	fsize=0;
	if (fpText) {
		fseek(fpText,0,2);
		fsize= ftell(fpText);
		fclose(fpText);
		fprintf(stdout," ** Source fileSize is %ld\n",fsize);
	}
	return fsize;
}


/*------------------------------------------------------------------
 Converts a word to lowercase (used to check if it is a stopword).
------------------------------------------------------------------ */  	
void toLow(char *dst, char *src, uint size) {
	uint i;
	for (i=0;i<size;i++) {*dst++ = _toLow[*src++];} //lowercase
	*dst='\0';
}	


/*------------------------------------------------------------------ */
/*
 *  COMPRESSOR PROGRAM OF ETDC.
 */
/*------------------------------------------------------------------ */

int main(int argc, char* argv[])
{
	char fileName[20];
	unsigned long bytesFile;
	uint numberPhrases, phraseLenght_min, phraseLenght_max;	

	fprintf(stdout,"\n ***************************************************************");
	fprintf(stdout,"\n * PROCESSES A TEXT FILE ...                                   *");
	fprintf(stdout,"\n * Extracts phrases from a given text                          *");
	fprintf(stdout,"\n ***************************************************************\n");
fprintf(stdout,"\n argc = %d",argc);


	if ((argc !=7) && (argc !=8))  {
		fprintf(stdout," \nWrong call to Extract phrases.\n");
		fprintf(stdout,"Use:  %s  <in file>  <out file> <numberOfPhrases> <min phrase len>< max phrase Lenght> <1/0 onlywords ?> [<stopwords file>]\n",argv[0]);
		return 0;
	}
	
	numberPhrases = atoi(argv[3]);
	phraseLenght_min = atoi(argv[4]);
	phraseLenght_max = atoi(argv[5]);
	
	onlyWords = atoi(argv[6]);		
	
	if (argc == 8)
		loadStopWords(&stopwordshash,argv[7]); //initializes the hash table of stopwords [global variable]


	//Estimation (Using Heap's law) of the size of the vocabulary (in number of distinct words).
	bytesFile = fileSize(argv[1]);
	if (!bytesFile) {
		fprintf(stdout, "\nFILE EMPTY OR FILE NOT FOUND !!\n Extraction aborted\n");
		exit(0);
	}

	fprintf(stdout,"\n   Extracting %u random phrases with [%u..%u] valid words each...",numberPhrases,phraseLenght_min,phraseLenght_max);

	parseFile(argv[1],bytesFile, argv[2],phraseLenght_min,phraseLenght_max,numberPhrases);


	if (stopwordshash) stdestroy_hash(stopwordshash);
	
	{
		
	fprintf(stdout, "\n\n SHOWING THE EXTRACTED PHRASES ?? \n\n");	
		FILE *fd;	
		if ((fd = fopen( argv[2],"r")) == NULL){
			fprintf(stdout,"\nError while opening outputfile: %s\n", argv[2]);
			fprintf(stdout,"Program will exit now\n");
			return;
		}	
		
		//showCreatedPhrases(fd);	
		fclose(fd);
	}
	
	
	fprintf(stdout, "\n Look for those phrases in file: %s\n\n", argv[2]);
	
}



