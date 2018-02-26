/* wRePairSLPIndex.h
 * Copyright (C) 2011, Miguel A. Martinez-Prieto
 * all rights reserved.
 *
 * SLP-based index on word-based Re-Pair compression. It is based on:
 *    Francisco Claude and Gonzalo Navarro.
 *    Self-Indexed Grammar-Based Compression.
 *    Fundamenta Informaticae
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


#ifndef _WREPAIRSLPINDEX_H
#define _WREPAIRSLPINDEX_H

#include <fstream>
#include <iostream>
#include <libcdsBasics.h>

#include "BinRel/BinaryRelation.h"
#include "BinRel/Point.h"
#include "IRePair/IRePair.h"
#include "utils/deltacodes.h"
#include "utils/WordsManager.h"
#include "utils/SuffixArray.h"
#include "utils/offsets.h"

#include <factorization.h>
#include <factorization_var.h>
#include <PermutationBuilder.h>

using namespace cds_utils;
using namespace cds_static;
using namespace std;

/** wRePairSLPIndex
 *   Implements a compressed word-based index on an Straight Line 
 *   Program (SLP) representation of the text. The SLP is obtained on a
 *   word-based Re-Pair by following the work of Claude & Navarro.
 *
 *   @author Miguel A. Martinez-Prieto
 */
class wRePairSLPIndex
{	
typedef struct
{ uint left;
  uint right;
} Range;

typedef struct
{ Range rows;
  Range columns;
} RangeQuery;

typedef struct
{ uint *pattern; // prefix/suffix pattern
  uint length;	  // Pattern length
  uint left;	  // Left limit
  uint right;	  // Right limit
  uint cleft;	  // Equal symbols in the left limit
  uint cright;	  // Equal symbols in the right limit
  uint lcp;	  // Length of the long common prefix
} PSpattern;

// Esto hay que eliminarlo!! Almacena la profundidad mÃ¡xima de una
// regla en la gramÃ¡tica... (habrÃ­a que obtenerlo con el RePair!)
static const size_t PROOF = 100;
static const size_t NOTFOUND = 0;
static const size_t LOCATE_SIZE = 32768;
static const size_t DELTA_SAMPLING = 16;

public:
	/** Generic constructor */
	wRePairSLPIndex();

	int build(uchar *text, uint length, uint *docs, uint ndocs, char *filename);
	int save();
	static int load(char *filename, wRePairSLPIndex **rpi);

	unsigned int size();
			
	unsigned int count(uchar *pattern, uint length);
	int locate(uchar *pattern, uint length, uint **docs, uint *numocc);
	int extract(uint from, uint to, uchar **snippet);	
	unsigned int length();	

	~wRePairSLPIndex();
protected:
	string filename;

	uint textlength;
	uint seqlength;
	uint terminals;
	Mapper *mapper;

	WordsManager *wm;		// Words manager
	BitSequence *Y;			// Marks terminal rules
	factorization *lenrules;	// Stores length rules
	Permutation *permutation; 	// Stores the permutation from reverse to direct ordering
	LabeledBinaryRelation *slp;	// Binary relation for the SLP

	DeltaCodes *seqB;		// Stores the length (in terminals) of each element in the compressed sequence
	LabeledBinaryRelation *seqR;	// Binary relation for prefixes/suffixes in the compressed sequence

	uint ndocs;			// Number of Documents
	uint *doc_array;		// Doc boundaries

	bool 
	static sorting(Rule i, Rule j)
	{
		uint minlen = i.len;
		uint shortRule = true;	// Is true if the rule 'i' is shortest than 'j'

		if (minlen > j.len) { minlen = j.len; shortRule = false; }
		uint z = 0;

		while (z < minlen)
		{
			if (i.sequence[z] < j.sequence[z]) return true;
			else if (i.sequence[z] > j.sequence[z]) return false;
			z++;
		}

		// The shortest rule is preffix of the largest one
		return shortRule;
	}

	bool 
	static sortingRev(Rule i, Rule j)
	{
		uint minlen = i.len;
		uint shortRule = true;	// Is true if the rule 'i' is shortest than 'j'

		if (minlen > j.len) { minlen = j.len; shortRule = false; }

		uint z = 0;
		uint lenI = i.len-1, lenJ = j.len-1;

		while (z < minlen)
		{
			if (i.sequence[lenI-z] < j.sequence[lenJ-z]) return true;
			else if (i.sequence[lenI-z] > j.sequence[lenJ-z]) return false;
			z++;
		}

		// The shortest rule is preffix of the largest one
		return shortRule;
	}

	bool 
	static sortingP(PSpattern i, PSpattern j)
	{
		uint len = i.length;
		uint shortPatt = true;	// Is true if the pattern 'i' is shortest than 'j'

		if (len > j.length) { len = j.length; shortPatt = false; }
		uint z = 0;

		while (z < len)
		{
			if (i.pattern[z] < j.pattern[z]) return true;
			else if (i.pattern[z] > j.pattern[z]) return false;
			z++;
		}

		// The shortest pattern is preffix of the largest one
		return shortPatt;
	}

	uint
	static lcp(uint *str1, uint *str2, uint len1, uint len2)
	{
		uint equals;
		uint len = len1;
		if (len2 < len1) len = len2;

		for (equals = 1; equals<len; equals++)
			if (str1[equals] != str2[equals]) break;	

		return equals;
	}

	// Transforms integer into word streams returning the length in chars
	uint getWords(uchar **snippet, uint *snippet_uint, uint length);

	// Tracks primary occurrences for 'count' and 'locate'
	void TrackOccsForCount(uint *numocc, uint j, uint *occsL, Rule **labels, uint *rulesize);
	void TrackOccsForLocate(uint *numocc, uint j, uint *occsL, Rule **labels, uint *occsSize, uint **occs, uint *rulesize);
	void LocateOccs(uint prev, uint *occsS, Rule *labels, uint occsSize, uint **occs);

	// Expand the rule from the SLP-based representation
	void expandRule(uint rule, uint len, uint *sequence);

	// Obtains prefix limits (in the SLP) for the given pattern
	int getPrefixesInSLP(uint *pattern, uint length, RangeQuery *q);
	// Obtains suffix limits (in the SLP) for the given pattern
	//  - We discard all suffixes whose complementary prefixes does not exist in th SLP
	int getSuffixesInSLP(uint *pattern, uint length, RangeQuery *q, PSpattern *suffixes);
	// Obtains suffix limits (in the compressed sequence) for the given pattern
	int getSuffixesInSequence(uint *pattern, uint length, RangeQuery *q, PSpattern *suffixes, uint lsuff);

	// Obtains the rule tree by discarding the first 'discard' chars
	void jumpCharsDirect(uint discard, Rule *r, uint *p, bool *d, int *level);
	// Obtains the rule tree by discarding the last 'discard' chars
	void jumpCharsReverse(uint discard, Rule *r, uint *p, bool *d, int *level);

	// Extract 'chars' from the rule discarding the first 'discard' ones
	void extractCharsDirect(uint rule, uint len, uint discard, uint chars, uint *snippet);
	// Extract 'chars' from the rule discarding the last 'discard' ones
	void extractCharsReverse(uint rule, uint len, uint discard, uint chars, uint *snippet);

	// Extract and compare for suffixes
	void extractAndCompareDirect(PSpattern *suffixes, uint size);
	// Extract and compare for prefixes
	void extractAndCompareReverse(PSpattern *prefixes, uint size);
	// Extract and compare for suffixes in the compressed sequence
	void extractAndCompareDirectInSequence(PSpattern *suffixes, uint size);

	// Obtain ranges containing suffixes
	void obtainRangeDirect(uint rule, PSpattern *suffixes, uint size);
	// Obtain ranges containing suffixes in sequence
	void obtainRangeDirectInSequence(uint rule, PSpattern *suffixes, uint size);
	// Obtain ranges containing prefixes
	void obtainRangeReverse(uint rule, PSpattern *prefixes, uint size);

	// Expand the rule comparing with respect to the pattern (suffix)
	// Returns:
	// - '0' if equals
	// - '>0' if the rule is greater than the pattern
	// - '<0' if the rule is less than the pattern
	int expandRuleAndCompareDirect(uint *pattern, uint length, uint *read, Rule *r, uint *p, bool *d, int level);
	// Expand the rule comparing with respect to the pattern (prefix)
	int expandRuleAndCompareReverse(uint *pattern, uint length, uint *read, Rule *r, uint *p, bool *d, int level);
	// Expand the rule comparing with respect to the pattern (suffix); next positions (from 'pos') are extracted if necessary
	int expandRuleAndCompareDirectInSequence(uint *pattern, uint length, uint pos, uint *read, uint cprev, Rule *r, uint *p, bool *d, int level);
};
#endif  /* _WREPAIRSLPINDEX_H */

