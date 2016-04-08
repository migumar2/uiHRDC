/* LabeledPoint.h
 * Copyright (C) 2010, Miguel A. Martinez-Prieto
 * all rights reserved.
 *
 * Labeled Point Description in a Binary Relation
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


#ifndef _RULE_H
#define _RULE_H

#include "Point.h"

namespace cds_static
{
	/** Rule
	 *   Extends the original Point class to support the labeled point
	 *   description used for Rules.
	 *
	 *   @author Miguel A. Martinez-Prieto
	 */
	class Rule : public Point
	{	
		public:
			// The 'left' value is the own symbol and the 'right' is -1 for terminal rules (len=1)
			uint id;  		// Rule ID
			uint len;		// Rule length in terminal symbols (len = 1 implies terminal rules)
			char *sequence;		// Sequence obtained by expanding the rule

			Rule(){};
			Rule(int left, int right, uint id, uint len)
			{
				this->left = left; 
				this->right = right;
				this->id = id;
				this->len = len;
			}
			~Rule(){};
	};
};
#endif  /* _RULE_H */
