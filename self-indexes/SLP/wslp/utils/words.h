/* words.h
 * Copyright (C) 2011, Antonio Fariña
 * all rights reserved.
 *
 * Utils for word managemente
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
 *   Antono Fariña:  fari@udc.es
 */


#ifndef _WORDS_H
#define _WORDS_H

#include "defValues.h"
#include "HashWM.h"
#include "MemoryManager.h"
#include "valstring.h"

typedef struct 
{
	unsigned long slot;   // the position in the hash table of the canonical word
	uchar *word; //makes alphanumerical sorting easier...
} tposInHT;


typedef struct SzoneMem 
{  //a large block of memory to load a file into mem.
	uchar *zone;  //block of mem.
	uint size;  //number of bytes	 	 
} tZoneMem;

typedef struct
{
	uint *words;
	uint elemSize;  //the size (in bits) of each pointer.
	tZoneMem wordsZoneMem; // a block of memory where the canonical words are loaded (from file).
} twords;

#endif  /* _WORDS_H */
