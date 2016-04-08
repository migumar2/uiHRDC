/* ildefconfig.h
 * Copyright (C) 2011, Antonio Fariña, all rights reserved.
 * 
 * ildefconfig.h: Some default constants used in the compressed representation.
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


#ifndef DEFVALUES_INCLUDED
#define DEFVALUES_INCLUDED

// Extensions of created  files

#define POSTINGS "il.spire07_bis"
#define DEFAULTKSAMPLE 4
#define DEFAULT_LEN_BITMAP_DIV 8  // Using a bitmap instead of bytecodes if
								  // lenlist[i] > NumDocIDS / DEFAULT_LEN_BITMAP_DIV
#endif
