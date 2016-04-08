/* RePairSLPIndex.h
 * Copyright (C) 2010, Miguel A. Martinez-Prieto
 * all rights reserved.
 *
 * SLP-based index on Re-Pair compression based on:
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


#ifndef _REPAIRSLPINDEX_H
#define _REPAIRSLPINDEX_H

#include <algorithm>
#include <stdio.h>

#include "RePair/RePair.h"
#include "BinRel/BinaryRelation.h"
#include "BinRel/Point.h"
#include "BinRel/RuleTrav.h"
#include "BinRel/FullQuery.h"
#include "Utils/SuffixArray.h"
#include "Utils/deltacodes.h"
#include "Samplings/sampling.h"

#include <factorization.h>
#include <factorization_var.h>
#include <PermutationBuilder.h>

using namespace cds_utils;

namespace cds_static
{
	/** RePairSLPIndex
	 *   Implements a compressed index on an Straight Line Program (SLP)
	 *   representation of the text. The SLP is obtained with the Re-Pair 
	 *   algorithm by following the work of Claude & Navarro.
	 *
	 *   @author Miguel A. Martinez-Prieto
	 */
	class RePairSLPIndex
	{	
		static const size_t PROOF = 100;
		static const size_t NOTFOUND = 0;
		static const size_t LOCATE_SIZE = 32768;
		static const uint BLOCK_SAMPLING = 16;

		static const size_t DELTA_SAMPLING = 16;
		static const size_t PERM_EPS = 8;

		public:
			/** Generic constructor */
			RePairSLPIndex();

			void build(uchar *text, uint length, uint sampling_length, char *filename);
			int save();
			static int load(char *filename, RePairSLPIndex **rpi);

			unsigned int size();
				
			unsigned int count(uchar *pattern, uint length);
			vector<unsigned int>* locate(uchar *pattern, uint length, uint *numocc);
			unsigned char* extract(unsigned int start, unsigned int end);			
			unsigned int length();

			~RePairSLPIndex();

		protected:
			string filename;

			uint textlength;
			uint seqlength;
			uint terminals;
			Mapper *mapper;

			BitSequence *C;			// Marks the symbols (in Sigma) used in the SLP
			uint *Y;			// Marks terminal rules
			factorization *lenrules;	// Stores length rules
			Permutation *permutation; 	// Stores the permutation from reverse to direct ordering
			LabeledBinaryRelation *slp;	// Binary relation for the SLP

			DeltaCodes *seqB;		// Stores the length (in terminals) of each element in the compressed sequence
			LabeledBinaryRelation *seqR;	// Binary relation for prefixes/suffixes in the compressed sequence

			Sampling *grammarSamp;		// Sampling for the grammar.
			Sampling *reverseSamp;		// Sampling for the reverse grammar.
			Sampling *sequenceSamp;		// Sampling for the compressed sequence.

			bool 
			static sorting(Rule i, Rule j)
			{
				if (strcmp(i.sequence, j.sequence) < 0) return true;
				else return false;
			}

			uint
			static lcp(uchar *str1, uchar *str2, uint len1, uint len2)
			{
				uint equals;
				uint len = len1;
				if (len2 < len1) len = len2;

				for (equals = 1; equals<len; equals++)
					if (str1[equals] != str2[equals]) break;	

				return equals;
			}

			uint
			rankY(uint val)
			{
				// Binary search
				uint l = 0, r = terminals-1, c;

				while (l <= r)
				{
					c = (l+r)/2;	
					if (Y[c] < val) l = c+1;
					else
					{
						if (Y[c] > val) r = c-1;
						else break;
					}
				}

				c++;

				return c;
			}
			
			/** Expands a rule for sortig the Re-Pair grammar.
			    @rule: the rule to be expanded.
			    @expanded: the expanded rule.
			    @grammar: pointer to the Re-Pair grammar.
			*/
			void expandRule(Rule r, char* expanded, vector<Rule> *grammar);

			/** Builds sampling over the index
			    @qsampling: sampling length.
			*/
			void buildSamplings(uint qsampling);

			/** Extracts a sample of chars length from the rule.
			    @rule: the rule to be extracted.
			    @len: length of the 'rule'.
			    @chars: number of chars in the sample.
			    @sample: sample.
			*/
			void extractCharsDirectNoSamples(uint rule, uint len, uint chars, uchar *sample);

			/** Extracts a sample of qsampling length from the reverse rule.
			    @rule: the rule to be extracted.
			    @len: length of the 'rule'.
			    @chars: number of chars in the sample.
			    @sample: sample.
			*/
			void extractCharsReverseNoSamples(uint rule, uint len, uint chars, uchar *sample);

			/** Extracts an snippet of chars length from the given position of the rule.
			    @rule: the rule to be extracted.
			    @len: length of the 'rule'.
			    @position: initial position for extraction
			    @chars: number of chars in the sample.
			    @snippet: rule snippet.
			*/
			void extractCharsDirect(uint rule, uint len, uint position, uint chars, uchar *snippet);

			/** Delimits the ranges of rules expanding (in G) all prefixes in the query.
			    @q: structure containing all query information
			*/
			void getPrefixesInSLP(FullQuery *q);

			/** Delimits the ranges of rules expanding (in G) all suffixes in the query.
			    @q: structure containing all query information
			*/
			void getSuffixesInSLP(FullQuery *q);

			/** Delimits the ranges of rules expanding (in S) all suffixes in the query.
			    @q: structure containing all query information
			*/
			void getSuffixesInSequence(FullQuery *q);

			/** Jumps some characters at the end of the rule in the query q.
			    @rt: RuleTrav structure containing the query information required for jumping.
			*/
			inline void jumpCharsReverse(RuleTrav *rt);

			/** Jumps some characters at the begin of the rule in the query q.
			    @rt: RuleTrav structure containing the query information required for jumping.
			*/
			inline void jumpCharsDirect(RuleTrav *rt);

			/** Performs a process of expansion and comparison with respect to the given prefix.
			    @prefix: the pattern being compared.
			    @q: structure containing all query information. The rule for comparison is
				provided by the RuleTrav structure internal to the query.
			*/
			void expandAndCompareReverse(PSpattern *prefix, FullQuery *q);

			/** Performs a process of expansion (in the grammar G) and comparison with 
			    respect to the given suffix.
			    @suffix: the pattern being compared.
			    @q: structure containing all query information. The rule for comparison is
				provided by the RuleTrav structure internal to the query.
			*/
			void expandAndCompareDirectInG(PSpattern *suffix, FullQuery *q);

			/** Performs a process of expansion (in the sequence S) and comparison with 
			    respect to the given suffix.
			    @suffix: the pattern being compared.
			    @q: structure containing all query information. The rule for comparison is
				provided by the RuleTrav structure internal to the query.
			*/
			void expandAndCompareDirectInS(PSpattern *suffix, FullQuery *q);

			/** Expands the rule in the query q (through RuleTrav) and compares it
			    with the pattern given in prefix.
			    @preffix: the pattern being compared.
			    @q: structure containing all query information. The rule for comparison is
				provided by the RuleTrav structure internal to the query.
			*/
			inline int expandRuleAndCompareReverse(PSpattern *prefix, FullQuery *q);

			/** Expands the rule in the query q (through RuleTrav) and compares it
			    with the pattern given in suffix.
			    @suffix: the pattern being compared.
			    @q: structure containing all query information. The rule for comparison is
				provided by the RuleTrav structure internal to the query.
			*/
			inline int expandRuleAndCompareDirectInG(PSpattern *suffix, FullQuery *q);

			/** Expands the rule in the query q (through RuleTrav) and compares it
			    with the pattern given in suffix. 
			    @suffix: the pattern being compared.
			    @pos: position of the current rule in the compressed sequence.
			    @jumped: number of char previously read (jumped) in the current rule.
			    @q: structure containing all query information. The rule for comparison is
				provided by the RuleTrav structure internal to the query.
			*/
			inline int expandRuleAndCompareDirectInS(PSpattern *suffix, uint pos, uint jumped, FullQuery *q);

			/** Obtains the range of rules: [left, right] delimiting the pattern given in
			    prefix. This operation uses rule as pivot in both binary searches.
			    @rule: the rule used as pivot.
			    @prefix: the pattern being delimited.
			    @q: structure containing all query information. 
			*/
			void obtainRangeReverse(uint rule, PSpattern *prefix, FullQuery *q);

			/** Obtains the range of rules: [left, right] delimiting in G the pattern 
			    given in suffix. This operation uses rule as pivot in both binary searches.
			    @rule: the rule used as pivot.
			    @suffix: the pattern being delimited.
			    @q: structure containing all query information. 
			*/
			void obtainRangeDirectInG(uint rule, PSpattern *suffix, FullQuery *q);


			/** Obtains the range of rules: [left, right] delimiting in S the pattern 
			    given in suffix. This operation uses rule as pivot in both binary searches.
			    @rule: the rule used as pivot.
			    @suffix: the pattern being delimited.
			    @q: structure containing all query information. 
			*/
			void obtainRangeDirectInS(uint rule, PSpattern *suffix, FullQuery *q);
	};
};
#endif  /* _REPAIRSLPINDEX_H */

