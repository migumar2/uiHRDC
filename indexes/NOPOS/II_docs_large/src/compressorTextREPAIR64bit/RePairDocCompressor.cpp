/* RePairDocCompressor.cpp
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
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 *
 * Contacting the author:
 *   Miguel A Martinez-Prieto:  migumar2@infor.uva.es
 */


#include "RePairDocCompressor.h"

RePairDocCompressor::RePairDocCompressor()
{
}
			
int 
RePairDocCompressor::build_representation(unsigned char *text, unsigned int length, unsigned int *docBeginnings, unsigned int numdocs)
{
	this->length = length;
	this->clength = 0;
	this->numdocs = numdocs;
	this->maxlength = 0;
	
	// Augment the text stream with '0's to delimit documents and
	// transform it into an integer stream
	int slength = length+numdocs+1;
	int *stream = (int*)malloc(slength*sizeof(int));
	int pdocs = 0;
	
	for (int i=0; i<length; i++)
	{
		if (i == docBeginnings[pdocs])
		{
			stream[i+pdocs] = 0;
			
			if (pdocs > 0)
			{
				uint len = docBeginnings[pdocs]-docBeginnings[pdocs-1];
				if (len > maxlength) maxlength = len;
			}
			
			pdocs++;
		}
		
		stream[i+pdocs] = (int)(text[i]);
	}
	
	Tdiccarray *dicc;
	IRePair::compress(stream, slength, &terminals, &rules, &dicc);
	
	// Compact the sequence and extract 0s
	unsigned int* compDocBeginnings = (unsigned int*)malloc((numdocs+1)*sizeof(unsigned int));
	unsigned int readSymbols = 0;
	pdocs = 0;
		
	while (readSymbols < slength)
	{									
		if (stream[readSymbols] >= 0)
		{
			if (stream[readSymbols] == 0)
			{
				// New Document
				compDocBeginnings[pdocs] = clength;
				pdocs++;
			}
			else
			{
				stream[clength] = stream[readSymbols];
				clength++;
			}
			
			readSymbols++;
		}
		else
		{
			if (readSymbols < slength) readSymbols = -(stream[readSymbols]+1);
		}
	}
	
	/*fprintf (stderr,"RePair succeeded\n\n");
	fprintf (stderr,"   Original ints: %i\n",terminals);
	fprintf (stderr,"   Number of rules: %i\n",rules);
	fprintf (stderr,"   Original text length: %i\n",length);
	fprintf (stderr,"   Final sequence length: %i\n",clength);*/
	
	// Building the document boundaries array	
	docs = new Array(compDocBeginnings, numdocs+1);
	free(compDocBeginnings);
	
	// Building the rules dictionary
	dictionary = new Array(2*rules, rules+terminals);
	for (uint i=0; i<rules; i++)
	{								
		dictionary->setField(2*i, dicc->rules[i].rule.left);
		dictionary->setField((2*i)+1, dicc->rules[i].rule.right);
	}
	Dictionary::destroyDicc(dicc);

	// Building the compressed sequence
	sequence = new Array((uint*)stream, clength);
	free(stream);
	
	return 0;
}

int
RePairDocCompressor::save_representation (char *filename)
{
	ofstream fp(filename);

	if (fp.good())
	{
		saveValue<unsigned int>(fp, terminals);
		saveValue<unsigned int>(fp, rules);
		saveValue<unsigned int>(fp, length);
		saveValue<unsigned int>(fp, clength);
		saveValue<unsigned int>(fp, numdocs);
		saveValue<unsigned int>(fp, maxlength);
		
		docs->save(fp);
		dictionary->save(fp);
		sequence->save(fp);
		
		fp.close();
		
		return 0;
	}
	else
	{
		return -1;
	}
}

RePairDocCompressor*
RePairDocCompressor::load_representation (char *filename)
{
	ifstream fp(filename);

	if (fp.good())
	{
		RePairDocCompressor *comp = new RePairDocCompressor();
		
		comp->terminals = loadValue<unsigned int>(fp);
		comp->rules = loadValue<unsigned int>(fp);
		comp->length = loadValue<unsigned int>(fp);
		comp->clength = loadValue<unsigned int>(fp);
		comp->numdocs = loadValue<unsigned int>(fp);
		comp->maxlength = loadValue<unsigned int>(fp);
		
		comp->docs = new Array(fp);
		comp->dictionary = new Array(fp);
		comp->sequence = new Array(fp);
		
		return comp;
	}
	else
	{
		return NULL;
	}
}	

int
RePairDocCompressor::size_representation(uint *size)
{
	*size = docs->getSize() + dictionary->getSize() + sequence->getSize() + sizeof(RePairDocCompressor);	
	return 0;
}

int 
RePairDocCompressor::extract_doc_representation(uint i, unsigned char **doc, unsigned int *len)
{		
		*doc = (unsigned char*)malloc((maxlength+1)*sizeof(unsigned char));
		*len = 0;
		
		uint begin = docs->getField(i);
		uint end = docs->getField(i+1);
		
		for (; begin<end; begin++)
		{
			uint rule = sequence->getField(begin);
			
			if (rule >= terminals)
			{
				(*len) += expandRule(rule-terminals, (*doc)+(*len));
			}
			else
			{
				(*doc)[*len] = (uchar)rule;
				(*len)++;
			}
		}
		
		(*doc)[*len] = '\0';
		
		return 0;
}

uint
RePairDocCompressor::expandRule(uint rule, uchar *string)
{	
	uint pos = 0;
	uint left = dictionary->getField(2*rule);
	uint right = dictionary->getField((2*rule)+1);
	
	if (left >= terminals)
	{
		pos += expandRule(left-terminals, (string+pos));
	}
	else
	{
		string[pos] = (uchar)left;
		pos++;
	}
	
	if (right >= terminals)
	{
		pos += expandRule(right-terminals, (string+pos));
	}
	else
	{
		string[pos] = (uchar)right;
		pos++;
	}
	
	return pos;
}
			
RePairDocCompressor::~RePairDocCompressor()
{	
	delete docs;
	delete dictionary;
	delete sequence;
}
