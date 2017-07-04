/* CSD_HTCF.h
 * Copyright (C) 2011, Rodrigo Canovas & Miguel A. Martinez-Prieto
 * all rights reserved.
 *
 * This class implements a Hu-Tucker compressed Front-Coding for compression of
 * string dictionaries.
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
 *   Rodrigo Canovas:  rcanovas@dcc.uchile.cl
 *   Miguel A. Martinez-Prieto:  migumar2@infor.uva.es
 */

#include "CSD_HTFC.h"

namespace csd
{
	CSD_HTFC::CSD_HTFC()
	{
		this->HTtree = NULL;
		this->search = true;
	}
	
	CSD_HTFC::CSD_HTFC(char *filename, size_t blocksize)
	{
		ifstream in(filename);
		
		if (in.good())
		{
			// Initialising values	
			this->type = HTFC;
			this->length = 0;
			this->maxlength = 0;
			this->bytes = 0;
			this->blocksize = blocksize;
			this->nblocks = 0;

			in.seekg(0,ios_base::end);
			tlength = in.tellg()/sizeof(uchar);
			in.seekg(0,ios_base::beg);

			uchar* dict = loadValue<uchar>(in, tlength);

			in.close();
			build(dict);

			delete [] dict;
		}
		else
		{
			abort();
		}
	}

	CSD_HTFC::CSD_HTFC(uchar *dict, uint tlength, size_t blocksize)
	{
		// Initialising values	
		this->type = HTFC;
		this->length = 0;
		this->tlength = tlength;
		this->maxlength = 0;
		this->bytes = 0;
		this->blocksize = blocksize;
		this->nblocks = 0;

		build(dict);
	}

    	size_t
	CSD_HTFC::locateByPrefix(uchar *prefix, uint len, uint *left, uint *right, uint *bleft, uint *bright)
	{
		int ilblock = *bleft, irblock = *bright;

		// Binary search for locating left and right blocks
		locateLimitBlocks(prefix, len, &ilblock, &irblock);

		if (ilblock == NOTFOUND) {
			*left = 0; 
			*right = 0;
			return 0;
		}
		else 
		{
			*bleft = ilblock;
			*bright = irblock;
		}

		uint lblock = ilblock, rblock = irblock;

		if (lblock == rblock)
		{
			// All prefixes are in the same block	
			int aux1 = 0, aux2 = 0;		
			locateLimitsForPrefixes(&lblock, &aux1, &aux2, prefix, len);
			
			if (aux1 != NOTFOUND)
			{
				*left = ((*bleft)*blocksize)+aux1+1;
				*right = ((*bright)*blocksize)+aux2+1;
			}
			else
			{
				*left = 0; 
				*right = 0;
				return 0;
			}
		}
		else
		{
			// All prefixes are not in the same block
			uint aux = 0;

			locateLeftLimitForPrefixes(bleft, &aux, prefix, len);
			*left = ((*bleft)*blocksize)+aux+1;
			locateRightLimitForPrefixes(&rblock, &aux, prefix, len);
			*right = ((*bright)*blocksize)+aux;
		}

		return (*right)-(*left)+1;
	}


	size_t 
	CSD_HTFC::locate(uchar *s, size_t len, uint *left, uint *bleft)
	{
		// Locating the candidate block for 's'
		int xbleft;
		bool cmp = locateBlock(s, len, &xbleft);

		// 's' is smaller than the first string in the dictionary
		if (xbleft < 0) { *left = 0; *bleft = 0; } 
		else { *bleft = xbleft; }
		
		if (cmp)
		{
			// The string is located at the first position of the block
			*left = (*bleft*blocksize)+1;
		}
		else
		{
			// The block is sequentially scanned to find the string
			uint idblock = locateInBlock(*bleft, s, len);
			
			// If idblock = 0, the URI is not in the dictionary
			if (idblock != 0) *left = (*bleft*blocksize)+idblock+1;
			else *left = idblock;
		}

		return 1;
	}

    	uint 
	CSD_HTFC::extract(size_t id, uchar* s)
	{
		if ((id > 0) && (id <= length))
		{		
			// Calculating block and offset
			uint block = (id-1)/blocksize;
			uint offset = (id-1)%blocksize;
			
			return extractInBlock(block, offset, s);
		}
		else
		{
			s = NULL;
			return 0;
		}
	}

	uchar* 
	CSD_HTFC::extract(size_t id)
	{
		// Allocating memory for the string
		uchar *s = new uchar[maxlength];
		extract(id, s);
		return s;
	}

	uint 
	CSD_HTFC::decompress(uchar **dict)
	{
		*dict = new uchar[tlength];
		uchar *deltaseq = new uchar[DELTA];
		uint prev = 0, delta = 0, offset = 0;
		uint ptr = 0, pos = 0;	// Pointers to current positions
					// in the decoded string and the 
					// compressed text.

		// Decompressing all blocks except the last one...
		for (uint i=0; i<nblocks-1; i++)
		{
			// Extracting the first string
			prev = ptr;
			pos += VByte2::decode(&delta, text+pos);
			ptr += decompressFirstWord(text, &pos, (*dict)+ptr);

			(*dict)[ptr] = '\n';
			ptr++;

			for (uint j=1; j<blocksize; j++)
			{
				// Decoding the prefix (delta)
				decompressDelta(text, &pos, &offset, deltaseq);
				VByte2::decode(&delta, deltaseq);

				// Copying the shared prefix
				strncpy((char*)(*dict+ptr), (char*)(*dict+prev), delta);
				prev = ptr;
				ptr += delta;

				// Decoding the suffix
				ptr += decompressWord(text, &pos, &offset, (*dict)+ptr);
				(*dict)[ptr] = '\n';
				ptr++;
			}

			if (offset > 0) pos++;
			offset = 0;
		}

		// Last block
		{
			// Extracting the first string
			prev = ptr;
			pos += VByte2::decode(&delta, text+pos);
			ptr += decompressFirstWord(text, &pos, (*dict)+ptr);

			(*dict)[ptr] = '\n';
			ptr++;

			for (; pos<blocks->getField(nblocks)-1;)
			{
				// Decoding the prefix (delta)
				decompressDelta(text, &pos, &offset, deltaseq);
				VByte2::decode(&delta, deltaseq);

				// Copying the shared prefix
				strncpy((char*)(*dict+ptr), (char*)(*dict+prev), delta);
				prev = ptr;
				ptr += delta;

				// Decoding the suffix
				ptr += decompressWord(text, &pos, &offset, (*dict)+ptr);
				(*dict)[ptr] = '\n';
				ptr++;
			}

			if (offset > 0) pos++;
			offset = 0;
		}

		delete [] deltaseq;
		return ptr;
	}

	size_t 
	CSD_HTFC::getSize()
	{
		return bytes*sizeof(uchar) + blocks->getSize() + tree->getSize() + (tree->getLength()/2)*sizeof(uint) + leafs*sizeof(Tcode) + sizeof(CSD_HTFC);
	}

	void 
	CSD_HTFC::save(ofstream & fp)
	{
		saveValue<size_t>(fp, type);
		saveValue<size_t>(fp, length); 
		saveValue<size_t>(fp, tlength); 
		saveValue<size_t>(fp, maxlength+1);
		saveValue<size_t>(fp, bytes);
		saveValue<uchar>(fp, text, bytes);
		saveValue<size_t>(fp, blocksize);
		saveValue<size_t>(fp, nblocks);
		blocks->save(fp);
		tree->save(fp);

		saveValue<uint>(fp, leafs);
		for (uint i=0; i<leafs; i++)
		{
			saveValue<uint>(fp, HTcode[i].code);
			saveValue<uint>(fp, HTcode[i].cbits);
		}
	}

	CSD* 
	CSD_HTFC::load(ifstream & fp)
	{
		size_t type = loadValue<size_t>(fp);
		if(type != HTFC) return NULL;
		
		CSD_HTFC *dicc = new CSD_HTFC();
		
		dicc->type = type;
		dicc->length = loadValue<size_t>(fp);
		dicc->tlength = loadValue<size_t>(fp);
		dicc->maxlength = loadValue<size_t>(fp);
		dicc->bytes = loadValue<size_t>(fp);
		dicc->text = loadValue<uchar>(fp, dicc->bytes);
		dicc->blocksize = loadValue<size_t>(fp);
		dicc->nblocks = loadValue<size_t>(fp);
		dicc->blocks = new Array(fp);
		dicc->tree = new BitString(fp);

		// Building HTtree
		dicc->HTtree = new Node[dicc->tree->getLength()/2];

		vector<uint> traversing;
		uint node = 0, symbol = 0;
		
		for (uint i=0; i<dicc->tree->getLength(); i++)
		{						
			if (dicc->tree->getBit(i) == 0)
			{				
				dicc->HTtree[node].children[0] = -1;
				dicc->HTtree[node].children[1] = -1;
				dicc->HTtree[node].symbol = 0;
				
				if (traversing.size() > 0)
				{
					uint parent = traversing[traversing.size()-1];
						
					if (dicc->HTtree[parent].symbol == 0) dicc->HTtree[parent].children[0] = node;
					else dicc->HTtree[parent].children[1] = node;

					dicc->HTtree[parent].symbol++;
				}
				
				traversing.push_back(node);
				node++;
			}
			else
			{
				uint last = traversing[traversing.size()-1];
				traversing.pop_back();
				
				if (last == (node-1))
				{
					dicc->HTtree[last].symbol = symbol;
					symbol++;
				}
				else
				{
					dicc->HTtree[last].symbol = -1;
				}
			}
		}

		dicc->leafs = loadValue<uint>(fp);
		dicc->HTcode = new Tcode[dicc->leafs];

		for (uint i=0; i<dicc->leafs; i++)
		{
			dicc->HTcode[i].code = loadValue<uint>(fp);
			dicc->HTcode[i].cbits = loadValue<uint>(fp);
		}
				
		return dicc;

	}

	void 
	CSD_HTFC::build(uchar *tdict)
	{
		uint tsize = (tlength*FACTOR_HTFC)/100;
		uchar *textfc = (uchar*)malloc(tsize*sizeof(uchar));
		uint bytesfc = 0;

		vector<uint> xblocks; // Temporal storage for start positions

		uchar *l = tdict, *r = NULL, *p = NULL;
		uint slength = 0;

		while (((uint)(l-tdict)) < tlength)
		{
			r = (uchar*)strchr((char*)l, '\0');
			slength = r-l;
			if (slength > maxlength) maxlength = slength;

			// Checking the current size of the encoded 
			// sequence: realloc if necessary
			if ((bytesfc+slength+1) >= tsize)
			{
				if (slength < tsize) tsize*=2;
				else tsize+=(2*slength);
				textfc = (uchar*)realloc(textfc, tsize*sizeof(uchar));
			}

			if ((length % blocksize) == 0)
			{
				// First string in the current block!
				// ==================================
				// The current byte is the first one for the 
				// current block,
				xblocks.push_back(bytesfc);
				nblocks++;
			
				// The string is explicitly copied to the 
				// encoded sequence.
				strncpy((char*)(textfc+bytesfc), (char*)l, slength);
				bytesfc+=slength;
			}
			else
			{
				// Regular string
				// ==============

				// Calculating the length of the long common prefix
				uint delta;
				lcp(&delta, p, l, l-p-1, slength);

				// The prefix is differentially encoded
				bytesfc += VByte2::encode(delta, textfc+bytesfc);

				// The suffix is copied to the sequence
				strncpy((char*)(textfc+bytesfc), (char*)l+delta, slength-delta);
				bytesfc+=slength-delta;
			}

			textfc[bytesfc] = '\0';
			bytesfc++;

			// New string processed
			length++;
			p = l;
			l = r+1;
		}

		// Storing the final byte position in the vector of positions
		xblocks.push_back(bytesfc);

		// Obtaining symbol frequencies (all values initialised to 1)
		int freqs[256];
		for (uint i=0; i<256; i++) freqs[i] = 1;
		for (uint i=0; i<bytesfc; i++) freqs[(int)(textfc[i])]++;

		HuTucker2<int> ht(freqs,256);
		leafs = ht.getCodes(&HTcode, &tree);

		// Initialishing text structure
		tsize = (tlength*FACTOR_HTFC)/100;
		text = (uchar*)malloc(tsize*sizeof(uchar));
		for (uint i=0; i<tsize; i++) text[i] = 0;

		// Auxiliar variables for Hu-Tucker encoding
		uint offset = 0, cblocks = 0;
		uint i = 0;

		while (i < bytesfc)
		{
			// Checking the current size of the encoded 
			// sequence: realloc if necessary
			if ((bytes+maxlength+1) >= tsize)
			{
				if (slength < tsize) tsize*=2;
				else tsize+=(2*slength);
				text = (uchar*)realloc(text, tsize*sizeof(uchar));

				for (uint j=bytes+1; j<tsize; j++) text[j] = 0;
			}

			if (i == xblocks[cblocks])
			{
				// Starting a new block: 'bytes' and 'offset'
				// are reseted (if offset > 0) to obtain 
				// byte-alinged blocks.
				if (offset > 0){ bytes++; offset = 0; }
				xblocks[cblocks] = bytes;
				cblocks++;

				// Encoding the first string
				uchar *first = new uchar[maxlength*sizeof(int)]; 
				first[0] = 0;
				uint fb = 0, fo = 0; // Variables managing bytes and offsets in the string 'first'

				while (true)
				{
					encodeHT(HTcode[(int)textfc[i]].code, HTcode[(int)textfc[i]].cbits, first, &fb, &fo);
					if (textfc[i] == '\0') break;
					i++;
				}

				if (fo > 0) fb++;

				// Encoding the string length
				bytes += VByte2::encode(fb, text+bytes);

				// Copying the encoded string
				for (uint i=0; i<fb; i++) text[bytes+i] = first[i];
				bytes += fb;

				delete [] first;
			}
			else
			{
				while (true)
				{
					encodeHT(HTcode[(int)textfc[i]].code, HTcode[(int)textfc[i]].cbits, text, &bytes, &offset);
					if (textfc[i] == '\0') break;
					i++;
				}
			}

			i++;
		}

		// Storing the final byte position in the vector of positions
		bytes++;
		xblocks[cblocks] = bytes;
		cblocks++;

		// Representing the vector of positions with log(bytes) bits
		blocks = new Array(xblocks, bits(bytes));		

		free(textfc);
		this->HTtree = NULL;
		search = false;
	}

	bool 
	CSD_HTFC::locateBlock(uchar *s, uint slen, int *block)
	{
		uchar *encoded = new uchar[2*slen];
		encoded[0] = 0;

		// Pattern (s) encoding
		uint encpos = 0, encoffset = 0;
		for (uint i=0; i<slen; i++) encodeHT(HTcode[s[i]].code, HTcode[s[i]].cbits, encoded, &encpos, &encoffset);
		if (encoffset > 0) encpos++;

		int l = 0, r = nblocks-1, c;
		uint delta;
		int cmp;
		
		while (l <= r)
		{			
			c = (l+r)/2;
			uint pos = blocks->getField(c);
			
			// Reading the compressed string length
			pos += VByte2::decode(&delta, text+pos);
			cmp = memcmp((text+pos), encoded, encpos);

			if (cmp > 0)
			{
				// The required string is in any preceding block
				r = c-1;
			}
			else
			{
				if (cmp < 0)
				{
					// The required string is in any subsequent block
					l = c+1;
				}
				else
				{
					if (encpos <= delta)
					{
						// The required string is the first one in the c-th block
						*block = c;	
						delete [] encoded;
						return true;
					}
					else
					{
						r = c-1;
					}
				}
			}
		}

		// If (cmp < 0) the URI is in the current block (c)
		// If (cmp > 0) the URI is in the preceding block (c-1)
		if (cmp < 0) *block = c;
		else *block = c-1;
		
		delete [] encoded;
		return false;
	}

	void
	CSD_HTFC::locateLimitBlocks(uchar *s, uint slen, int *left, int *right)
	{
		uchar *encoded = new uchar[2*slen];
		encoded[0] = 0;

		// Pattern (s) encoding
		uint encpos = 0, encoffset = 0;
		for (uint i=0; i<slen; i++) encodeHT(HTcode[s[i]].code, HTcode[s[i]].cbits, encoded, &encpos, &encoffset);
		if (encoffset > 0) encpos++;

		uint delta = 0, c = 0;
		int cmp = 0;

		uchar *read = new uchar[encpos];
		uchar mask = (uchar)255;
		mask = mask >> encoffset;
		mask = ~mask;

		while (*left <= *right)
		{
			c = ((*left)+(*right))/2;
			uint pos = blocks->getField(c);
			
			// Reading the compressed string length
			pos += VByte2::decode(&delta, text+pos);

			// Copying the bytes exclusively required to compare
			// the prefix
			memcpy(read, text+pos, encpos);
			if ((delta >= encpos) && (encoffset != 0)) read[encpos-1] = read[encpos-1] & mask;
			cmp = memcmp(read, encoded, encpos);

			if (cmp > 0) *right = c-1;
			else if (cmp < 0) *left = c+1;
			else if (encpos <= delta) break;
			else *right = c-1;
		}

		if (cmp != 0)
		{ 
			// All prefixes are included in the same block
			delete [] encoded;
			delete [] read;

			if (cmp > 0) { *left = c-1; *right = c-1; }
			else { *left = c; *right = c; }

			return;
		}

		// Locating left limit
		uint lc = *left, rc = *right+1;
		*right = c;

		if (c > 0) 
		{
			*left = c-1;

			while (lc <= *left)
			{
				c = ((*left)+(lc))/2;
				uint pos = blocks->getField(c);
			
				pos += VByte2::decode(&delta, text+pos);			
				memcpy(read, text+pos, encpos);
				if ((delta >= encpos) && (encoffset != 0)) read[encpos-1] = read[encpos-1] & mask;
				cmp = memcmp(read, encoded, encpos);

				if (cmp != 0) lc = c+1;
				else if (c != 0) *left = c-1;
				else { *left = 0; break; }
			}
		}

		// Locating right limit
		while ((int)*right < (int)(rc-1))
		{
			c = ((*right)+(rc))/2;
			uint pos = blocks->getField(c);
			
			pos += VByte2::decode(&delta, text+pos);			
			memcpy(read, text+pos, encpos);
			if ((delta >= encpos) && (encoffset != 0)) read[encpos-1] = read[encpos-1] & mask;
			cmp = memcmp(read, encoded, encpos);
			
			if (cmp != 0) rc = c;
			else *right = c;
		}

		delete [] encoded;
		delete [] read;
	}


	uint
	CSD_HTFC::locateInBlock(uint block, uchar *s, uint len)
	{
		uchar *deltaseq = new uchar[DELTA];
		uchar *tmp = new uchar[maxlength];
		uint delta, tmplen;
		uint offset = 0;

		uint pos = blocks->getField(block);
		pos += VByte2::decode(&delta, text+pos);
		tmplen = decompressFirstWord(text, &pos, tmp);

		uint plcp_len = 0, clcp_len, lcprefix;
		int cmp = lcp(&clcp_len, tmp, s, tmplen, len);

		uint read = 1;
		uint id = 0;

		uint scanneable = blocksize;

		if (block == nblocks-1) scanneable = (length%blocksize);

		while (read < scanneable)
		{
			// Decoding the prefix (delta)
			decompressDelta(text, &pos, &offset, deltaseq);
			VByte2::decode(&delta, deltaseq);

			if (delta < clcp_len)
			{
				// delta is less than the clcp_len value, so
				// the current string is subsequent to the 
				// required one -> it is not in the dictionary!
				id = 0;
				break;
			}
			else
			{
				// Decoding the suffix
				tmplen = decompressWord(text, &pos, &offset, tmp+delta)+delta;

				plcp_len = clcp_len;
				cmp = lcp(&lcprefix, tmp+clcp_len, s+clcp_len, tmplen-clcp_len, len-clcp_len);
				clcp_len += lcprefix;

				if ((tmplen == len) && (clcp_len == len))
				{
					// This is the required string
					id = read;
					break;
				}
				else if ((clcp_len < plcp_len) || ((clcp_len >= plcp_len) && (cmp > 0)))
				{
					// The string is not in the dictionary
					id = 0;
					break;
				}
			}

			read++;
		}	

		delete [] tmp;
		delete [] deltaseq;

		return id;
	}

	void 
	CSD_HTFC::locateLeftLimitForPrefixes(uint *block, uint *ppos, uchar *s, size_t slen)
	{
		uchar *deltaseq = new uchar[DELTA];
		uchar *tmp = new uchar[maxlength];
		uint delta, tmplen;
		uint offset = 0;

		uint pos = blocks->getField(*block);
		pos += VByte2::decode(&delta, text+pos);
		tmplen = decompressFirstWord(text, &pos, tmp);

		uint plcp_len = 0, clcp_len, lcprefix;
		int cmp  = lcp(&clcp_len, tmp, s, tmplen, slen);

		uint read = 1;
		uint limit = blocksize;

		// Checking if the first element prefixes s
		if (clcp_len < slen)
		{
			// Scanning the block until the first s-prefixed string
			// (or the first greater one) is reached.
			while (read < limit)
			{
				// Decoding the prefix (delta)
				decompressDelta(text, &pos, &offset, deltaseq);
				VByte2::decode(&delta, deltaseq);

				if (delta < clcp_len) 
				{
					*block = 0; 
					delete [] tmp; delete [] deltaseq;
					return; 
				}
				else
				{
					// Decoding the suffix
					tmplen = decompressWord(text, &pos, &offset, tmp+delta)+delta;

					plcp_len = clcp_len;
					cmp = lcp(&lcprefix, tmp+clcp_len, s+clcp_len, tmplen-clcp_len, slen-clcp_len);
					clcp_len += lcprefix;

					if ((tmplen >= slen) && (clcp_len == slen))
					{
						// This is the required string
						*ppos = read;
						delete [] tmp; delete [] deltaseq;
						return;
					}
					else if ((clcp_len < plcp_len) || ((clcp_len >= plcp_len) && (cmp > 0)))
					{ 
						// The string is not in the dictionary
						*block = 0; 
						delete [] tmp; 	delete [] deltaseq;
						return; 
					}
				}
	
				read++;
			}
		}

		if (read == limit)
		{
			// The first s-prefixed string is in the next block
			(*block)++; *ppos=0;
		}

		delete [] tmp; delete [] deltaseq;
	}

	void 
	CSD_HTFC::locateRightLimitForPrefixes(uint *block, uint *ppos, uchar *s, size_t slen)
	{
		uchar *deltaseq = new uchar[DELTA];
		uchar *tmp = new uchar[maxlength];
		uint delta, tmplen;
		uint offset = 0;

		uint pos = blocks->getField(*block);
		pos += VByte2::decode(&delta, text+pos);
		tmplen = decompressFirstWord(text, &pos, tmp);

		uint clcp_len = 0;
		lcp(&clcp_len, tmp, s, tmplen, slen);

		uint read = 1;
		uint limit = blocksize;

		if (*block == nblocks-1)
		{
			uint mod = length % blocksize;
			if (mod != 0) limit = mod;
		}

		while (read < limit)
		{
			decompressDelta(text, &pos, &offset, deltaseq);
			VByte2::decode(&delta, deltaseq);

			if (delta < slen) break;

			tmplen = decompressWord(text, &pos, &offset, tmp+delta)+delta;
			read++;
		}

		*ppos = read;

		delete [] tmp; delete [] deltaseq;
	}

	void 
	CSD_HTFC::locateLimitsForPrefixes(uint *block, int *lpos, int *rpos, uchar *s, size_t slen)
	{
		uchar *deltaseq = new uchar[DELTA];
		uchar *tmp = new uchar[maxlength];
		uint delta, tmplen;
		uint offset = 0;

		uint pos = blocks->getField(*block);
		pos += VByte2::decode(&delta, text+pos);
		tmplen = decompressFirstWord(text, &pos, tmp);

		uint plcp_len = 0, clcp_len, lcprefix;
		int cmp  = lcp(&clcp_len, tmp, s, tmplen, slen);

		uint read = 1;
		uint limit = blocksize;

		if (*block == nblocks-1)
		{
			uint mod = length % blocksize;
			if (mod != 0) limit = mod;
		}

		// Checking if the first element prefixes s
		if (clcp_len < slen)
		{
			// Scanning the block until the first s-prefixed string
			// (or the first greater one) is reached.
			while (read < limit)
			{			
				// Decoding the prefix (delta)
				decompressDelta(text, &pos, &offset, deltaseq);
				VByte2::decode(&delta, deltaseq);

				if (delta < clcp_len) 
				{
					*lpos = NOTFOUND; 
					delete [] tmp; delete [] deltaseq;
					return; 
				}
				else
				{
					// Decoding the suffix
					tmplen = decompressWord(text, &pos, &offset, tmp+delta)+delta;

					plcp_len = clcp_len;
					cmp = lcp(&lcprefix, tmp+clcp_len, s+clcp_len, tmplen-clcp_len, slen-clcp_len);
					clcp_len += lcprefix;

					if ((tmplen >= slen) && (clcp_len == slen))
					{
						// This is the required string
						*lpos = read;
						read++;
						break;
					}
					else if ((clcp_len < plcp_len) || ((clcp_len >= plcp_len) && (cmp > 0)))
					{ 
						// The string is not in the dictionary
						*lpos = NOTFOUND; 
						delete [] tmp;
						delete [] deltaseq;
						return; 
					}
				}

				read++;
			}
		}

		if (read == limit)
		{
			if (*lpos == (int)read-1)
			{
				// The string is the last in the bucket
				*rpos = *lpos;
			}
			else
			{
				// The string is not in the dictionary
				*lpos = NOTFOUND; 
			}

			delete [] tmp;
			delete [] deltaseq;
			return;
		}

		while (read < limit)
		{
			decompressDelta(text, &pos, &offset, deltaseq);
			VByte2::decode(&delta, deltaseq);

			if (delta < slen) break;

			tmplen = decompressWord(text, &pos, &offset, tmp+delta)+delta;
			read++;
		}

		*rpos = read-1;

		delete [] tmp; delete [] deltaseq;
	}

	uint 
	CSD_HTFC::extractInBlock(uint block, uint o, uchar *s)
	{
		uchar *deltaseq = new uchar[DELTA];
		uint delta;
		uint offset = 0;

		uint pos = blocks->getField(block);
		pos += VByte2::decode(&delta, text+pos);
		delta = decompressFirstWord(text, &pos, s);

		for (uint j=0; j<o; j++)
		{
			// Decoding the prefix (delta)
			decompressDelta(text, &pos, &offset, deltaseq);
			VByte2::decode(&delta, deltaseq);

			// Decoding the suffix
			delta += decompressWord(text, &pos, &offset, s+delta);
		}

		s[delta] = '\0';
		delete [] deltaseq;
		return delta;
	}

	void
	CSD_HTFC::decompressDelta(uchar *seq, uint *pos, uint *offset, uchar *deltaseq)
	{
		uint i = 0; 

		do
		{
			deltaseq[i] = (uchar)decodeHT(seq, pos, offset);
			i++;
		}
		while (deltaseq[i-1] < 128);
	}

    	uint
	CSD_HTFC::decompressFirstWord(uchar *seq, uint *pos, uchar *word)
	{
		uint ptr = 0, offset = 0;

		while (true)
		{
			word[ptr] = decodeHT(seq, pos, &offset);
			if (word[ptr] == '\0') break;
			ptr++;
		}

		if (offset > 0) (*pos)++;
		return ptr;
	}

	uint
	CSD_HTFC::decompressWord(uchar *seq, uint *pos, uint* offset, uchar *suffix)
	{
		uint ptr = 0;

		while (true)
		{
			suffix[ptr] = decodeHT(seq, pos, offset);
			if (suffix[ptr] == '\0') break;
			ptr++;
		}

		return ptr;
	}

	uchar 
	CSD_HTFC::decodeHT(uchar *seq, uint *pos, uint *offset)
	{
		uint node = 0;
		
		while (HTtree[node].symbol < 0)
		{
			bool bit = ((seq[*pos] >> (7-(*offset))) & 1);
			node = HTtree[node].children[bit];

			(*offset)++;
			
			if ((*offset) == 8)
			{				
				(*pos)++;
				(*offset) = 0;
			}
		}
		
		return (uchar)HTtree[node].symbol;
	}

	void 
	CSD_HTFC::encodeHT(uint code, uint len, uchar *seq, uint *pos, uint *offset)
	{
		uchar uccode;
		uint uicode;	
		uint processed = 0;
		
		while ((len-processed) >= (8-(*offset)))
		{		
			uicode = code << (W-len+processed);
			uccode = (uchar)(uicode >> (W-(8-(*offset))));
			seq[*pos] = seq[*pos] | uccode;
			
			processed += 8-(*offset);
			(*pos)++;
			seq[*pos] = 0;
			(*offset) = 0;			
		}
		
		if (len-processed > 0)
		{
			uicode = code << (W-len+processed);
			uccode = (uchar)(uicode >> (W-(8-(*offset))));
			seq[*pos] = seq[*pos] | uccode;
			(*offset) += len-processed;
		}
	}

	int
	CSD_HTFC::lcp(uint *lcprefix, uchar* str1, uchar* str2, uint lstr1, uint lstr2)
	{
		*lcprefix = 0;
		int cmp = 0;
		uint length = lstr1;
		if (length > lstr2) length = lstr2;
		
		for (uint i=0; i<length; i++)
		{
			if (str1[i] == str2[i]) (*lcprefix)++;
			else
			{
				cmp = str1[i] - str2[i];
				break;
			}
		}
		
		return cmp;
	}

	size_t 
	CSD_HTFC::getBlocks()
	{
		return nblocks;
	}

	CSD_HTFC::~CSD_HTFC()
	{
		if (HTtree != NULL) delete [] HTtree;
		delete tree;
		delete blocks;
		delete [] HTcode;

		if (search) delete [] text;
		else free(text);
	}
};
