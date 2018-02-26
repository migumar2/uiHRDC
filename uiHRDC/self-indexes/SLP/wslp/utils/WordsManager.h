/* WordsManager.h
 * Copyright (C) 2011, Miguel A. Martinez-Prieto
 * all rights reserved.
 *
 * Extracted from the implementation of Antonio Fariña
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 *
 * Contacting the author:
 *   Miguel A Martinez-Prieto:  migumar2@infor.uva.es
 */

#ifndef _WORDSMANAGER_H
#define _WORDSMANAGER_H

#include <fstream>
#include <iostream>
#include <libcdsBasics.h>
#include "words.h"

static int *intVector;

using namespace cds_utils;
using namespace std;

/** WordsManager
 *   Implements a simple words manager to transform texts into integer 
 *   sequences.
 *
 *   @author Miguel A. Martinez-Prieto
 */
class WordsManager
{	
	public:
		/** Generic constructor */
		WordsManager();

		/** Parses a 'text' of 'length' chars returning an integer 
		    'iseq' containing 'nwords' words mapped from the wordData
		    vocabulary */
		void parse(uchar *text, uint length, char *build_options, int **iseq, uint *nwords, uint *docboundaries, uint ndocs);

		/** Given a text pattern translates it into a list of integers
		    (corresponding to the canonical words associated to the 
		    valid words in the text pattern) */
		void parsePattern(uchar *textPattern, uint patLen, uint **integerPattern, uint *sizeIntegers);

		/** Obtains the ID associated with the given word */
		uint getID(uchar *word);

		/** Obtains the word associated with the given id and returns
		    its length. */
		uint getWord(uint id, uchar **word);

		/** Adds the word associated with the given id to the stream
		    and returns its length. The third parameter addresses 
		    spaceless words: it says if the previous symbol is word */
		uint addWord(uint id, uchar *stream, bool *isword);
		
		/** Returns the word average length in this structure */
		uint getAvgLen();

		/** Saves the vocabulary on disk. */
		int save(ofstream & output);
			
		/** Loads the vocabulary from the file named filename. */
		static WordsManager* load(ifstream & input);

		/** Returns the size, in bytes, of the vocabulary. */
		int size();

		/** Static methods for building the word Suffix Array */
		static int suffixCmp(const void *arg1, const void *arg2)
		{
			register int a,b;
			a=*((int *) arg1);
			b=*((int *) arg2);

			while(intVector[a] == intVector[b]) { a++; b++; }
			return (intVector[a] - intVector[b]);
		}

		static void IntSA(int *vector, uint n, int **sa)
		{
			register uint i, j;

			*sa = (int*)malloc(sizeof(int) * n);

			intVector = vector;
			for(i=0; i<n; i++) (*sa)[i]=i;

			qsort(*sa, n, sizeof(int), suffixCmp);
		}

		~WordsManager();

	protected:
		twords wordsData;	// Word vocabulary
		uint zeroNode;		// Number of different words
		uint sourceTextSize;	// Source text size (in bytes)
		uint seSize;		// Number of words in the source text
		uint avglen;		

		void obtainWords(uchar *text, ulong length, char *build_options);
		static int qSortWordsCompareAlpha(const void *arg1, const void *arg2);
		void bitwrite (uint *e, uint p, uint len, uint s);
		uint bitread (uint *e, uint p, uint len);


};
#endif  /* _WORDSMANAGER_H */

