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


 #include <sys/types.h>
 #include <sys/stat.h>
 #include <fcntl.h>

#define BUFFERSIZE 100000
   
#include <string.h>
#include <stdlib.h>
#include <math.h>

#include <malloc.h>
#include <stdio.h>

#include "constants.h"
#include "valstring.h"
#include "hashStopWords.h"
#include <time.h>

#ifdef M64
ulong MAX_RAND_VALUE_FARI=RAND_MAX*19;
#endif

#ifdef M32
ulong MAX_RAND_VALUE_FARI=RAND_MAX;
#endif

unsigned char*  repetidas[1000000];
uint countrepetidas =0;

stt_hash stopwordshash;

	uint onlyWords =0;
	
	/*------------------------------------------------------------------
	 * Performs the full parse process.
			- parsing: gather words and frequencies.
			- sorting: words are sorted by frequency.
		* Outputs:
			-modifies: hash, positionInTH, zeroNode.
	------------------------------------------------------------------ */
	void parseFile (char* infile,  uint inFileSize, char *nameoutFile, uint numberOfWords_min, uint numberOfWords_max, uint numberOfPhrases);


	/*------------------------------------------------------------------
	 Parses a phrase containing x valid words from a buffer beloging to a
	 random position obtained from a file
	------------------------------------------------------------------ */
	int getPhrase (unsigned char *bufferIni, unsigned char *bufferEnd,
					uint numberOfWords, unsigned char *phrase,
					uint *cutPos, uint *sizes, uint *cutPosTot);

	/*------------------------------------------------------------------
	 Filters the words in the vocabulary and writes them to the
	 output file.
	------------------------------------------------------------------ */	
	void outputWord(FILE *outFile, unsigned char* phrase, uint len, uint numberOfWords) ;

	/*------------------------------------------------------------------
	 Writes the words that belong to each pattern.
	------------------------------------------------------------------ */
	void outputSeparateWords(FILE *outFile, unsigned char *phrase,
						uint *cutPos, uint *sizes, uint cutPosTot );
	/*------------------------------------------------------------------
	 Writes the integer indicating the number of patterns of the sample file.
	------------------------------------------------------------------ */
	void outputNumPatterns(FILE *outFile, uint num, uint numberOfWords_min, uint numberOfWords_max) ;                 
	
	/*------------------------------------------------------------------
	 Obtains the size of the file to process.
	------------------------------------------------------------------ */                 
	unsigned long fileSize (char *filename);

	/*------------------------------------------------------------------
	 Converts a word to lowercase (used to check if it is a stopword).
	------------------------------------------------------------------ */  	
	void toLow(char *dst, char *src, uint size);
	

