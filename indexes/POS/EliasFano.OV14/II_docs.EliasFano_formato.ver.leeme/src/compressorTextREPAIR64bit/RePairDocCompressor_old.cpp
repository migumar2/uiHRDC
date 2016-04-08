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
RePairDocCompressor::build_representation(unsigned char *text, unsigned int length, unsigned int *docBeginnings, unsigned int numdocs, uint sampling)
{
	this->length = length;
	this->clength = 0;
	this->numdocs = numdocs;
	this->maxlength = 0;
	this->sampling = sampling;
	
	// Augment the text stream with '0's to delimit documents and
	// transform it into an integer stream
	uint slength = length+numdocs+1;
	int *stream = (int*)malloc(slength*sizeof(int));
	int pdocs = 0;
	
	for (uint i=0; i<length; i++)
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
	// Also build the sampling!!!
	unsigned int readSymbols = 0;
	pdocs = 0;
	docs = (unsigned int*)malloc((numdocs+1)*sizeof(unsigned int));

	samples = 0;
	unsigned int sampsize = numdocs;
	uint *spos = (unsigned int*)malloc(sampsize*sizeof(unsigned int));
		
	while (readSymbols < slength)
	{									
		if (stream[readSymbols] >= 0)
		{
			if (stream[readSymbols] == 0)
			{
				// New Document
				docs[pdocs] = clength;
				pdocs++;
			}
			else
			{			
				// Checking for sampling
				if ((clength % sampling) == 0)
				{
					if (samples == sampsize)
					{
						sampsize *= 2;
						spos = (uint*)realloc(spos, sampsize*sizeof(unsigned int));
					}
					
					spos[samples] = readSymbols-pdocs;
					samples++;
				}

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

	// Building the samples
	spos[samples] = readSymbols-pdocs;
	for (unsigned int i=0; i<samples; i++) spos[i] = spos[i+1]-spos[i];
	samplepos = new DeltaCodes(spos, samples, DELTA_SAMPLING);
	free(spos);
	
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
		saveValue<unsigned int>(fp, sampling);
		saveValue<unsigned int>(fp, samples);

		saveValue<unsigned int>(fp, docs, numdocs+1);
		samplepos->save(fp);
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
		comp->sampling = loadValue<unsigned int>(fp);
		comp->samples = loadValue<unsigned int>(fp);

		comp->docs = loadValue<unsigned int>(fp, comp->numdocs+1);
		comp->samplepos = DeltaCodes::load(fp);
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
	cout << "++++++++++++++++++++++++++" << endl;
	cout << "Boundaries: " << (numdocs+1)*sizeof(uint) << endl;
	cout << "Samples: " << samplepos->size() << endl;
	cout << "Rules: " << dictionary->getSize() << endl;
	cout << "Sequence: " << sequence->getSize() << endl;
	cout << "Structure: " << sizeof(RePairDocCompressor) << endl;
	cout << "++++++++++++++++++++++++++" << endl; 

	*size = ((numdocs+1)*sizeof(uint)) + samplepos->size() + dictionary->getSize() + sequence->getSize() + sizeof(RePairDocCompressor);
	return 0;
}



int 
RePairDocCompressor::extract_from_to_char(uint from, uint to, unsigned char **snippet, unsigned int *slen)
{
	*snippet = (unsigned char*)malloc((to-from+2)*sizeof(unsigned char));
	*slen = to-from+1;

	// Binary search
	uint l = 0, r = samples-1, c;
	uint val;
	bool found = false;

	while (l <= r)
	{
		c = (l+r)/2;
		val = samplepos->select(c)+1;

		if (val < from) l = c+1;
		else
		{
			if (val > from) r = c-1;
			else { found=true; break; }
		}
	}

	if (val > from) c--;
	uint ptr = samplepos->select(c)+1;
	uint pos = sampling*c;
	uint res;

	do
	{
		uint rule = sequence->getField(pos);

		if (rule >= terminals) res = expandRule(rule-terminals, *snippet, &ptr, from, to);
		else
		{
			if (ptr >= from)
			{
				(*snippet)[ptr-from] = (uchar)rule;
				ptr++;
				if (ptr>to) res=to;
			}
		}

		pos++;
	}
	while (res != to);

	(*snippet)[*slen] = '\0';
}

int 
RePairDocCompressor::extract_doc_representation(uint i, unsigned char **doc, unsigned int *len)
{		
	*doc = (unsigned char*)malloc((maxlength+1)*sizeof(unsigned char));
	*len = 0;
		
	uint begin = docs[i];
	uint end = docs[i+1];
		
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

uint 
RePairDocCompressor::expandRule(uint rule, uchar* string, uint *ptr, uint from, uint to)
{
	uint left = dictionary->getField(2*rule);
	uint right = dictionary->getField((2*rule)+1);
	
	if (left >= terminals)
	{
		if (expandRule(left-terminals, string, ptr, from, to) == to) return to;
	}
	else
	{
		if (*ptr >= from)
		{
			string[*ptr-from] = (uchar)left;
			(*ptr)++;
			if (*ptr > to) return to;
		}
		else (*ptr)++;
	}
	
	if (right >= terminals)
	{
		if (expandRule(right-terminals, string, ptr, from, to) == to) return to;
	}
	else
	{
		if (*ptr >= from)
		{
			string[*ptr-from] = (uchar)right;
			(*ptr)++;
			if (*ptr > to) return to;
		}
		else (*ptr)++;
	}
	
	return 0;
}
			
RePairDocCompressor::~RePairDocCompressor()
{	
	delete [] docs;
	delete dictionary;
	delete sequence;
}
