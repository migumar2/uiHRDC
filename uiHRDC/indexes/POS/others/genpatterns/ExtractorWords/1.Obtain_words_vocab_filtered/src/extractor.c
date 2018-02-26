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

/*------------------------------------------------------------------
 Function used by qsort to compare two elements a and b
------------------------------------------------------------------*/
int compareFrecListaDesc(const void *a, const void *b)
{
	unsigned long *left,*right;
	left =  (unsigned long *) a;
	right = (unsigned long *) b;
    if ( hash[*left].weight < hash[*right].weight)
                return 1;
    if ( hash[*left].weight > hash[*right].weight)
                return -1;

    return 0;
}

/*------------------------------------------------------------------
 Initilizes the structures used.
 ---------------------------------------------------------------- */
void initialize (unsigned long sizeVoc){
	unsigned long i;

	_memMgr = createMemoryManager();
    initialize_hash(sizeVoc);

    positionInTH = (unsigned long*) malloc (sizeVoc * sizeof(unsigned long));
	zeroNode = 0;
}


/*------------------------------------------------------------------
 * Performs the full parse process.
		- parsing: gather words and frequencies.
		- sorting: words are sorted by frequency.
	* Outputs:
		-modifies: hash, positionInTH, zeroNode.
------------------------------------------------------------------ */
void parseFile (char* infile,  unsigned long N_val) {

	unsigned long codeword,tam;
	BufferRead B;
	unsigned char *text,*top, *nextbuf;
	register unsigned char *pbeg,*pend,*wordstart;
	register unsigned long size;

	register unsigned long k,i,j;

	int f;
	
	unsigned char *aWord;


    StartValid();

	// Initialization of read buffer
	f = open (infile, O_RDONLY);
	if (f == -1) {
		printf("Cannot read file %s",infile);
		exit(0);
	}

	B = bufCreate(MAX_SIZE_OF_WORD+5);//buffer will have at least 5 bytes after a word

	bufSetFile (B,f);

	initialize(N_val); //initializes compressor for up to N_value words

	fprintf(stderr, "\nSTARTING EXTRACTION OF WORDS ...");

	/********************** Beginning of parse process **********************/

	if (bufEmpty(B) ) {size =0; return;}

	size = 0;

	while ( B->size==B->dsize ) {

		bufCurrent (B,(char**)&text,(char**)&top);
		pbeg = text; pend = top;

	 	while ( pbeg < pend )   {

			//parsing a word or separator
			size=0;
			wordstart = pbeg;
			if (_Valid[*pbeg]) {   //alphanumerical data

				while ( (size<MAX_SIZE_OF_WORD) && (  _Valid[*pbeg] )) {
					size++;
					pbeg++;
				}

		    }
			else {
				if (*pbeg != ' ') { //a separator comes starting in ' ' comes, so it is a new word
					//fprintf(stderr,"\n Entra 3 pbeg [%d] pend [%d] valid [%d]",pbeg,pend, _Valid[*pbeg]);
					while ( (size<MAX_SIZE_OF_WORD) && (!_Valid[*pbeg])) {

						size++;
						pbeg++;
					}
				}
				else {  //a  SPACE comes, so we have to test if next character is alphanumerical or not

	                pbeg++;
					if (_Valid [*pbeg] ) {
						wordstart = pbeg;
						while ( (size<MAX_SIZE_OF_WORD) && (_Valid[*pbeg] )) {
							size++;
							pbeg++;
						}

					}
					else {   // a "separator word" is parsed until an alphanumerical character is found

						size++;
						while ( (size<MAX_SIZE_OF_WORD) && (!_Valid[*pbeg] )) {
							size++;
							pbeg++;
						}
					}//else {  // a "separator word"
				}//if (!bufEmpty(B)) {
			}

			aWord=wordstart;  //the word parsed.

	        j = search ((unsigned char *)aWord, size, &addrInTH );

	        if (j==zeroNode) {
				insertElement ((unsigned char *) aWord, size, &addrInTH);
				hash[addrInTH].weight = 0;
				hash[addrInTH].size = 0;
				hash[addrInTH].len=size;
				positionInTH[zeroNode] = addrInTH;
				zeroNode++;
			}

			hash[addrInTH].weight +=1;

    	}/* internal while*/

		nextbuf=pbeg;
		bufLoad (B, (char*)nextbuf);

	}

	//processing the last buffer.

	if (!bufEmpty(B)){
	 	bufCurrent (B,(char**)&text,(char**)&top);
	 	pbeg = text; pend = top;

		while ( pbeg < pend )   {

			size=0;
			wordstart = pbeg;
			if (_Valid[*pbeg]) {   //alphanumerical data
		    	while ( (size<MAX_SIZE_OF_WORD) && (  _Valid[*pbeg] ) &&( pbeg < pend )) {

						size++;
						pbeg++;
				}
		    }
			else {
				if (*pbeg != ' ') { //a separator comes starting in ' ' comes, so it is a new word

					while ( (size<MAX_SIZE_OF_WORD) && (!_Valid[*pbeg])&&( pbeg < pend )) {

						size++;
						pbeg++;
					}
				}
				else {  //a  SPACE comes, so we have to test if next character is alphanumerical or not

				//if (!bufEof(B)) { //not end of file
	                pbeg++;
					if (_Valid [*pbeg] ) {
						wordstart = pbeg;
						while ( (size<MAX_SIZE_OF_WORD) && (_Valid[*pbeg] )&&( pbeg < pend )) {
							size++;
							pbeg++;
						}

					}
					else {   // a "separator word" is parsed until an alphanumerical character is found

						size++;
						while ( (size<MAX_SIZE_OF_WORD) && (!_Valid[*pbeg] )&&( pbeg < pend )) {
							size++;
							pbeg++;
						}
					}//else {  // a "separator word"
				}//if (!bufEmpty(B)) {

			}

			aWord=wordstart;

			j = search ((unsigned char *)aWord, size, &addrInTH );

			if (j==zeroNode) {
				insertElement ((unsigned char *) aWord, size, &addrInTH);
				hash[addrInTH].weight = 0;
				hash[addrInTH].size = 0;
				hash[addrInTH].len=size;
				positionInTH[zeroNode] = addrInTH;
				zeroNode++;
		   	}

			hash[addrInTH].weight +=1;

    	}/* inner while*/

	}//if (!bufEmpty(B){


	fprintf(stderr,"\n Number of words in the vocabulary:  %ld \n",zeroNode);

	/********************** end of parse process **********************/


	// Sorting the vocabulary by frequency.

	{	//Moves all the words with frequency = 1 to the end of the vocabulary.
		register long ii;
		register long kk;

	    kk=zeroNode-1;
	    ii=0;
	    while (ii<kk){
			while ((hash[positionInTH[ii]].weight!=1) && (ii <kk)) { ii++; }
			while ((hash[positionInTH[kk]].weight==1) && (ii <kk)) { kk--; }

			if (ii<kk){
				swap(&positionInTH[ii], &positionInTH[kk]);
				kk--;
				ii++;
			}
	  	}

		k=ii;

		//Aplies qsort to the words with frequency > 1.
		qsort(positionInTH,k,sizeof(unsigned long),compareFrecListaDesc);
	}

	close(f);
	bufDestroy(B);  //freeing resources.

} 


/*------------------------------------------------------------------
 Filters the words in the vocabulary and writes them to the
 output file.
------------------------------------------------------------------ */
void outputWords(char* outfile, unsigned long N_val,
                 unsigned long minFreq, unsigned long maxFreq,
                 unsigned long minLen, unsigned long maxLen, 
                 unsigned long onlyWords) {

	FILE *outFile ;
	
	if ((outFile = fopen(outfile,"w")) == NULL){
		fprintf(stderr,"\nError while opening outputfile: %s\n",outfile);
		fprintf(stderr,"Program will exit now\n");
		return;
	}	
	
	  
	{	unsigned long i;
		unsigned long counter;
		counter=0;
		fprintf(stderr," Extract criteria ....");
		fprintf(stderr,"\n MINFREQ%9d MAXFRE%9d  MINLEN %9d MAXLEN %9d\n",minFreq,maxFreq,minLen,maxLen);
		for (i=0; i< zeroNode;i++) {

			if ( (hash[positionInTH[i]].weight >=minFreq) && (hash[positionInTH[i]].weight <=maxFreq)
			   		&&(hash[positionInTH[i]].len >=minLen) && (hash[positionInTH[i]].len <=maxLen))
			{
				//fprintf(stderr,"%7ld %9d %9d %s\n",i,words[i].size, words[i].occur , words[i].word);

				if (onlyWords == 1) {

					if (_Valid[(unsigned char) (hash[positionInTH[i]].word[0])] ){
						counter++;
					}
				}
				else {
						counter++;
				}
			}
		}


		fprintf(outFile,"%ld\n",counter);
		for (i=0; i< zeroNode;i++) {

			if ( (hash[positionInTH[i]].weight >=minFreq) && (hash[positionInTH[i]].weight <=maxFreq)
			   		&&(hash[positionInTH[i]].len >=minLen) && (hash[positionInTH[i]].len <=maxLen))
			
				//fprintf(stderr,"%7ld %9d %9d %s\n",i,words[i].size, words[i].occur , words[i].word);

					if (onlyWords == 1) {

						if (_Valid[(unsigned char) (hash[positionInTH[i]].word[0])] ){
							fprintf(outFile,"%7ld %9d %9d %s\n",i,hash[positionInTH[i]].len, hash[positionInTH[i]].weight , hash[positionInTH[i]].word);
						}
					}
					else {
						fprintf(outFile,"%7ld %9d %9d %s\n",i,hash[positionInTH[i]].len, hash[positionInTH[i]].weight , hash[positionInTH[i]].word);
					}
			}
	
		fprintf(stderr,"%ld valid words, from the %ld available were extracted\n\n\n",counter,zeroNode);

	}

	fclose(outFile);

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
		fprintf(stderr,"fileSize = %ld",fsize);
	}
	return fsize;
}


/*------------------------------------------------------------------ */
/*
 *  COMPRESSOR PROGRAM OF ETDC.
 */
/*------------------------------------------------------------------ */

int main(int argc, char* argv[])
{
	char fileName[20];
	long sizeNValue;
	unsigned long bytesFile;

	unsigned long minFreq;
	unsigned long maxFreq;
	unsigned long minLen;
	unsigned long maxLen;
	unsigned long onlyWords;

	minFreq = 1;
	maxFreq = 999999999;
	minLen = 1;
	maxLen = 999999999;
	onlyWords = 1;

	fprintf(stderr,"\n ***************************************************************");
	fprintf(stderr,"\n * PROCESSES A TEXT FILE ...                                   *");
	fprintf(stderr,"\n * Extracts words following certain conditions with respect to *");
	fprintf(stderr,"\n *  frequency, lenght of word and (and/or only 'separators').  *");
	fprintf(stderr,"\n *  such as minFreq <= frequency(word) <=maxFreq               *");
	fprintf(stderr,"\n *  such as minLen<= strlen(word) <=maxLen                     *");
	fprintf(stderr,"\n ***************************************************************\n");

	if (argc < 7) {
		fprintf(stderr," \nWrong call to Extract words.\n");
		fprintf(stderr,"Use:  %s  <in file>  <out file> <minFreq> <maxFreq> <minLen> <maxLen> <onlyWords>\n",argv[0]);
		return 0;
	}


	//Estimation (Using Heap's law) of the size of the vocabulary (in number of distinct words).
	//sizeNValue=N_value;
	bytesFile = fileSize(argv[1]);
	if (!bytesFile) {
		fprintf(stderr, "\nFILE EMPTY OR FILE NOT FOUND !!\n Extraction aborted\n");
		exit(0);
	}

	if(bytesFile<5000000) bytesFile = 5000000;
	sizeNValue = (unsigned long) floor(3.9* pow(bytesFile,0.69) );

	minFreq = atol(argv[3]);
	maxFreq = atol(argv[4]);
	minLen = atol(argv[5]);
	maxLen = atol(argv[6]);
	onlyWords = atol(argv[7]);

	printf("\n   Extracting... minFreq=%ld, maxFreq = %ld, minLen=%ld, maxLen = %ld, onlyWords=%d \n",minFreq,maxFreq,minLen,maxLen,onlyWords);

	parseFile(argv[1],sizeNValue);
	outputWords(argv[2],N_value,minFreq,maxFreq, minLen, maxLen, onlyWords);

	free(hash);
	free(positionInTH);
}




