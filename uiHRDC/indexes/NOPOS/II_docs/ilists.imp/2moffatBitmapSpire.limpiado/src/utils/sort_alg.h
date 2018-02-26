/* sort_alg.h
 * Copyright (C) 2011, Antonio Fariña, all rights reserved.
 * 
 * sort_alg.h: Some comparing funtions for qsort().
 *    
 * Implementation of an representation of compressed postings lists. It provides
 *   Culpeper & Moffat skiper structure over postings lists compressed with vbyte.
 * 
 * Contact info:
 * Antonio Fariña, University of A Coruña
 * http://vios.dc.fi.udc.es/
 * antonio.fari[@]gmail.com  
 * 
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
 */


#ifndef SORTINGINCLUDED
#define SORTINGINCLUDED

#ifndef uint
#define uint  unsigned int
#endif

	/* used for sorting functions */
	typedef struct {
		uint key;
		uint value;
	}tsort;


	/** sorting a tsort-vector by field "key" (O(n^2)) but valid as n<< **/
int simplesortAsc(tsort *V, uint n);	
int simplesortDesc(tsort *V, uint n);

	/** sorting a tsort-vector descending, (O(n^2)) but valid as n<< **/
	
int simplesortUint(uint *V, uint n);


#endif

