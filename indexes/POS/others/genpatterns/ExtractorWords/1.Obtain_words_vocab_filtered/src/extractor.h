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
   
#include <string.h>
#include <stdlib.h>
#include <math.h>

#include <malloc.h>
#include <stdio.h>

#include "constants.h"
#include "valstring.h"
#include "BufferedReader.h"
#include "hash.h"


#if 0
static void swap(int *x,
		 int *y)
{
  int temp;

  temp = *x;
  *x = *y;
  *y = temp;
  return;
}
#endif

#define swap( x, y ) { int temp; temp=*(x); *(x)=*(y); *(y)=temp; }

	unsigned long *positionInTH;

    unsigned long addrInTH;		//the value return by calls to Search(hashTable)
	unsigned long zeroNode;		//The number of words that have been added to  the vocabulary.

	/*-----------------------------------------------------------------
	 Function used by qsort to compare two elements a and b
	 ---------------------------------------------------------------- */
	int compareFrecListaDesc(const void *a, const void *b);
	
	/*-----------------------------------------------------------------
	 Initilizes the structures used.
	 ---------------------------------------------------------------- */
	void initialize (unsigned long sizeVoc);
	
	/*------------------------------------------------------------------
	 * Performs the full parse process.
			- parsing: gather words and frequencies.
			- sorting: words are sorted by frequency.
		* Outputs:
			-modifies: hash, positionInTH, zeroNode.
	------------------------------------------------------------------ */
	void parseFile (char* infile,  unsigned long N_val);

	/*------------------------------------------------------------------
	 Filters the words in the vocabulary and writes them to the
	 output file.
	------------------------------------------------------------------ */	
	void outputWords(char* outfile, unsigned long N_val,
                 unsigned long minFreq, unsigned long maxFreq,
                 unsigned long minLen, unsigned long maxLen, 
                 unsigned long onlyWords);
                 
	/*------------------------------------------------------------------
	 Obtains the size of the file to process.
	------------------------------------------------------------------ */                 
	unsigned long fileSize (char *filename);
	

