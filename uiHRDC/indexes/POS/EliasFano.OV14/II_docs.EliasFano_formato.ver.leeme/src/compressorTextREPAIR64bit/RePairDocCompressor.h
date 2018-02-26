/* RePair.h
 * Copyright (C) 2010, Rodrigo Canovas & Miguel A. Martinez-Prieto
 * all rights reserved.
 *
 * RePair for repetitive text collections
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
 * License aint with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 *
 * Contacting the author:
 *   Rodrigo Canovas:  yigorc@gmail.com
 *   Miguel A Martinez-Prieto:  migumar2@infor.uva.es
 */


#ifndef _REPAIR_H
#define _REPAIR_H

#include <iostream>
#include <fstream>
#include <cstdlib>
#include <string.h>

#include "RePair/IRePair.h"
#include "utils/Array.h"
#include "utils/deltacodes.h"

static const size_t DELTA_SAMPLING = 16;

using namespace cds_utils;
using namespace std;

class RePairDocCompressor
{	
	public:
		RePairDocCompressor();

		int build_representation(unsigned char *text, unsigned int length, unsigned int *docBeginnings, unsigned int numdocs, uint sampling);
		int save_representation(char *filename);
		static RePairDocCompressor* load_representation(char *filename);
		int size_representation(uint *size);
		int extract_doc_representation(uint i, unsigned char **doc, unsigned int *len);
		int extract_from_to_char(uint from, uint to, unsigned char **snippet, unsigned int *slen);

			
		~RePairDocCompressor();

		unsigned int numdocs;	// Number of docs in the compressed sequence

	protected:
		unsigned int terminals;	// Terminal symbols in the grammar
		unsigned int rules;	// Rules in the grammar
		unsigned int length;	// Original sequence length
		unsigned int clength;	// Compressed sequence length

		uint sampling;		// Sampling value
		uint samples;		// Total samples
		unsigned int maxlength;	// Length of the largest document

		DeltaCodes *samplepos;	// Sampling for extraction
		Array *docs;		// Document boundaries
		Array *dictionary;	// Rules dictionary
		Array *sequence;	// Compressed sequence
			
		/** Expands the rule in the string **/ 
		uint expandRule(uint rule, uchar* string);
		/** Expands the rule and stores it if (ptr >= from && ptr <= to) **/ 
		uint expandRule(uint rule, uchar* string, uint *ptr, uint from, uint to);
};

#endif  /* _REPAIR_H */
