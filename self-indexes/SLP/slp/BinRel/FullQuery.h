/* FullQuery.h
 * Copyright (C) 2011, Miguel A. Martinez-Prieto
 * all rights reserved.
 *
 * Structure for Querying the RePair SLP Index
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

#ifndef _FULLQUERY_H
#define _FULLQUERY_H

#include "UtilsBinRel.h"

namespace cds_static
{
	/** FullQuery
	 *   Describes a specific structure for full querying the RePair SLP Index
	 *
	 *   @author Miguel A. Martinez-Prieto
	 */
	class FullQuery
	{	
		public:
			uchar *pattern;		// Query pattern
			uchar *revpattern;	// Query pattern (reverse)
			uint length;		// Pattern length

			uint nprefixes;		// Number of 'valid' prefixes
			uint nsuffixesG;	// Number of 'valid' suffixes in the Grammar (G)
			uint nsuffixesS;	// Number of 'valid' prefixes in the Sequence (S)

			PSpattern *prefixes;	// Structure for prefix searching
			PSpattern *suffixesG;	// Structure for suffix searching in the Grammar (G)
			PSpattern *suffixesS;	// Structure for suffix searching in the Sequence (S)

			RangeQuery *rq;		// Structure containing the final range query
			RuleTrav *rt;		// Structure (auxiliar) for rule traversing

			FullQuery(){};

			FullQuery(uchar *pattern, uint length, uint proof)
			{
				this->pattern = pattern;
				this->length =length;

				nprefixes = length-1;
				nsuffixesG = 0;
				nsuffixesS = 0;

				this->prefixes = new PSpattern[length-1]; FillPrefixes();
				this->suffixesG = new PSpattern[length-1];

				this->rq = new RangeQuery[length-1];
				this->rt = new RuleTrav(0, 0, proof);
			};

			~FullQuery()
			{
				delete [] prefixes;
				delete [] suffixesG;
				delete [] suffixesS;

				delete [] revpattern;
				delete [] rq;
				delete rt;		
			};

			bool 
			static sorting(PSpattern i, PSpattern j)
			{
				if (strcmp((char*)i.spattern, (char*)j.spattern) < 0) return true;
				else return false;
			}

			void
			FillPrefixes()
			{
				revpattern = new uchar[length+1];
				for (uint i=0; i<length; i++) revpattern[i] = pattern[length-i-1];
				revpattern[length] = '\0';
		
				for (uint i=0; i<length-1; i++)
				{
					prefixes[i].spattern = revpattern+i+1;
					prefixes[i].length = length-i-1;
				}

				sort (prefixes, prefixes+length-1, sorting);
			}

			void
			FillSuffixesG()
			{
				for (uint i=0; i<length-1; i++)
				{
					if (prefixes[i].limits.l != NOTFOUND)
					{
						// The prefix exists in G, so its corresponding
						// suffix must be searched. Its range is also 
						// stored for the final range query.
						uint lp = prefixes[i].length;

						suffixesG[nsuffixesG].spattern = pattern+lp;
						suffixesG[nsuffixesG].length = length-lp;
						nsuffixesG++;
	
						rq[prefixes[i].length-1].rows.l = prefixes[i].limits.l;
						rq[prefixes[i].length-1].rows.r = prefixes[i].limits.r;
					}
					else
					{
						rq[prefixes[i].length-1].rows.l = NOTFOUND;
						rq[prefixes[i].length-1].rows.r = NOTFOUND;
						nprefixes--;
					}
				}

				sort(suffixesG, suffixesG+nsuffixesG, sorting);
			}

			void
			FillRangesInSLP()
			{
				for (uint i=0; i<nsuffixesG; i++)
				{
					if (suffixesG[i].limits.l != NOTFOUND)
					{	
						rq[length-suffixesG[i].length-1].columns.l = suffixesG[i].limits.l;
						rq[length-suffixesG[i].length-1].columns.r = suffixesG[i].limits.r;
					}
					else
					{
						rq[length-suffixesG[i].length-1].columns.l = NOTFOUND;
						rq[length-suffixesG[i].length-1].columns.r = NOTFOUND;
					}
				}
			}

			void
			FillRangesInSequence()
			{
				for (uint i=0; i<nsuffixesS; i++)
				{
					if (suffixesS[i].limits.l != NOTFOUND)
					{	
						rq[length-suffixesS[i].length-1].columns.l = suffixesS[i].limits.l;
						rq[length-suffixesS[i].length-1].columns.r = suffixesS[i].limits.r;
					}
					else
					{
						rq[length-suffixesS[i].length-1].columns.l = NOTFOUND;
						rq[length-suffixesS[i].length-1].columns.r = NOTFOUND;
					}
				}
			}

			void
			FillSuffixesS()
			{
				nsuffixesS = nsuffixesG;
				suffixesS = new PSpattern[nsuffixesS];				
				memcpy(suffixesS, suffixesG, nsuffixesG*sizeof(PSpattern));
			}
	};
};

#endif  /* _FULLQUERY_H */
