/* Occ.h
 * Copyright (C) 2012, Miguel A. Martinez-Prieto
 * all rights reserved.
 *
 * Structure for Occurrences
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


#ifndef _OCC_H
#define _OCC_H

namespace cds_static
{
	/** Occ
	 *   Stores information about occurrences.
	 *
	 *   @author Miguel A. Martinez-Prieto
	 */
	class Occ
	{	
		public:
			int id;		// Rule ID (label)
			int right;	// Right component
			int len;	// Len component

			Occ(){};
			~Occ(){};
	};
};

#endif  /* _Occ_H */
