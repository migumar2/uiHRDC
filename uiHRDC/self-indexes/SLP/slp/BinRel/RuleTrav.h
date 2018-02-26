/* RuleTrav.h
 * Copyright (C) 2011, Miguel A. Martinez-Prieto
 * all rights reserved.
 *
 * Structure for Rule Traversing
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

#include "Rule.h"

#ifndef _RULETRAV_H
#define _RULETRAV_H

namespace cds_static
{
	/** RuleTrav
	 *   Describes a specific structure for efficient rule traversing
	 *
	 *   @author Miguel A. Martinez-Prieto
	 */
	class RuleTrav
	{	
		public:
			Rule *r;	 // Parse tree
			uint *p;	 // Rule position in the SLP
			bool *d;	 // Level state: 'true' when full processed
			int level;	 // Current level in the parse tree
			uint read;	 // Current char in the rule
			uint discard;	 // Number of chars to be discarded
			uchar *xseq;	 // Sequence extracted from the rule

			RuleTrav()
			{ 
			}

			RuleTrav(int proof)
			{ 
				r = new Rule[proof];
				p = new uint[proof];
				d = new bool[proof];
				xseq = new uchar[proof+1];
			}

			RuleTrav(int rule, int len, int proof)
			{
				r = new Rule[proof];
				p = new uint[proof];
				d = new bool[proof];
				xseq = new uchar[proof+1];
				level = 0;
				read = 0;
				discard = 0;

				r[0].id = rule;
				r[0].len = len;
				d[0] = false;
			}

			inline void
			ResetRT(int rule, int len)
			{
				level = 0;
				read = 0;
				discard = 0;

				r[0].id = rule;
				r[0].len = len;
			}

			~RuleTrav()
			{
				delete [] r;
				delete [] p;
				delete [] d;
				delete [] xseq;
			};
	};
};

#endif  /* _RULETRAV_H */
