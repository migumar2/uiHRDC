/* UtilsBinRel.h
 * Copyright (C) 2010, Miguel A. Martinez-Prieto
 * all rights reserved.
 *
 * Utilities for managing the Binary Relation
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


#ifndef _UTILSBINREL_H
#define _UTILSBINREL_H

namespace cds_static
{
	typedef struct
	{
	  uint l;	// Left component
	  uint r;	// Right component
	} Pair;

	typedef struct
	{ Pair rows;	  // Range of rows
	  Pair columns;   // Range of columns
	  uint results;	  // Number of results
	} RangeQuery;

	typedef struct
	{ uchar *spattern; // Prefix / Suffix sub-pattern
	  uint length;	   // Sub-pattern length
	  Pair limits;	   // Rule limits for the sub-pattern
	  Pair eqchars;    // Equal number of characters in the limits
	  Pair blocks;	   // Blocks including the limits
	  uint lcp;	   // Length of the common prefix
	} PSpattern;
};
#endif  /* _UTILSBINREL_H */
