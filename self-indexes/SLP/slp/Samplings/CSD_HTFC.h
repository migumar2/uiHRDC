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


#ifndef _CSDHTFC_H
#define _CSDHTFC_H

#include <iostream>
#include <fstream>
#include <cassert>
#include <string.h>
using namespace std;

#include <Array.h>
#include <libcdsBasics.h>
using namespace cds_utils;

#include "CSD.h"
#include "VByte2.h"
#include "hutucker2.h"

namespace csd
{
typedef struct
{
	int children[2];
	int symbol;
} Node;	// Node for the Hu-Tucker tree


static const size_t FACTOR_HTFC = 20; // Percentual fraction (of the original 
				      // dict size) for memory allocation in 
				      // building.
static const size_t DELTA = 5;        // Maxixum possible length for a VByte 
				      // encoding delta.

class CSD_HTFC : public CSD
{		
  public:		
    /** General constructor **/
    CSD_HTFC();

    /** Constructor reading Tdict from a file named 'filename'. The file must
	contain one word per line. 
	@filename: path to the file containing the string dictionary.
	@blocksize: number of strings stored in each dictionary block.
    */
    CSD_HTFC(char *filename, size_t blocksize);

    /** Constructor receiving Tdict as a sequence of 'tlength' uchars. Tdict
	must concatenate all strings separated by '\n' characters.  
	@dict: uchar stream representing the string dictionary.
	@tlength: number of integer symbols in the stream.
	@blocksize: number of strings stored in each dictionary block.
    */
    CSD_HTFC(uchar *dict, uint tlength, size_t blocksize);

    /** Returns the ID that identify s[1..length]. If it does not exist, 
	returns 0. 
	@s: the string to be located.
	@len: the length (in characters) of the string s.
    */
    size_t locate(uchar *s, size_t len, uint *left, uint *bleft);

    /** Returns the string identified by id.
	@id: the identifier to be extracted.
    */
    uchar * extract(size_t id);
    uint extract(size_t id, uchar* s);

    /** Obtains the original Tdict from its CSD_HTFC representation. Each string is
	separated by '\n' symbols.
	@dict: the plain uncompressed dictionary.
	@return: number of total symbols in the dictionary.
    */
    uint decompress(uchar **dict);

    /** Returns the size of the structure in bytes. */
    size_t getSize();

    /** Stores a CSD_HTFC structure given a file pointer.
	@fp: pointer to the file saving a CSD_HTFC structure.
    */
    void save(ofstream & fp);

    /** Loads a CSD_HTFC structure from a file pointer.
	@fp: pointer to the file storing a CSD_HTFC structure. */
    static CSD * load(ifstream & fp);
	
    /** General destructor. */		
    ~CSD_HTFC();	

    size_t getBlocks();


    /** Return the left and right limits of the range 
	containing all uris prefixed by the given pattern */
    size_t locateByPrefix(uchar *prefix, uint len, uint *left, uint *right, uint *bleft, uint *bright);
		
  protected:
    uint bytes;		//! Size of the Front-Coding encoded sequence (in bytes).
    uchar *text;	//! Hu-Tucker Front-Coding encoded sequence.

    size_t blocksize;	//! Number of strings stored in each block.
    size_t nblocks;	//! Number of total blocks in the dictionary.
    Array *blocks;	//! Start positions of dictionary blocks.

    BitString *tree;	//! Hu-Tucker tree shape (ESPECIFICAR COMO CRESTA SE CODIFICA!!!!)
    Node *HTtree;	//! Hu-Tucker tree
    uint leafs;		//! Leafs in the Hu-Tucker tree.
    Tcode *HTcode;	//! Vector assigning Hu-Tucker codes to the symbols in the text.

    bool search;

    /** Builds the CSD_HTFC representation from a the Tdict comprising 'tlength'
	unsigned chars. 
	@tdict: uchar stream representing the string dictionary.
    */    
    void build(uchar *tdict);

    /** Locates the block in where the string 's' can be stored. This method is
	based on a binary search comparing the first string in each block and
	the given string 's'.
	@s: the string to be located.
	@slen: the length (in characters) of the string s.
	@block: the candidate block.
	@return: a boolean value pointing if the string is located (this only
	 occurs when 's' is the first string in 'block').
    */
    bool locateBlock(uchar *s, uint slen, int *block);


    /** Locates the limit blocks (left and right) in where all strings prefixed
	by 's' can be stored. This method is based on a binary search comparing 
	the first string in each block and the given string 's'.
	@s: the pefix to be located.
	@slen: the length (in characters) of the string s.
	@left: the candidate left block.
	@right: the candidate right block.
    */
    void locateLimitBlocks(uchar *s, uint slen, int *left, int *right);

    /** Locates the offset for 's' in 'block' (returning its global ID) or 
	return 0 if it is  not exist 
	@block: block to be queried.
	@s: the required string.
	@len: the length (in characters) of the string s.
	@return: the ID for 's' or 0 if it is not exist.
    */
    uint locateInBlock(uint block, uchar *s, uint len);

    void locateLeftLimitForPrefixes(uint *block, uint *ppos, uchar *s, size_t slen);
    void locateRightLimitForPrefixes(uint *block, uint *ppos, uchar *s, size_t slen);
    void locateLimitsForPrefixes(uint *block, int *lpos, int *rpos, uchar *s, size_t slen);

    /** Extracts the o-th string in the given 'block'.
	@block: block to be accesed.
	@o: internal offset for the required string in the block.
	@s: the extracted string.
	@return: number of characters extracted.
    */
    uint extractInBlock(uint block, uint o, uchar *s);

    /** Decompress a VByte code encoding the 'delta' value respect to the
	previous string.
	@seq: string containing the text to be decoded.
	@pos: pointer to the byte at which start decoding.
	@offset: offset within this last byte.
	@deltaseq: the VByte subsequence encoding 'delta'.
    */
    void decompressDelta(uchar *seq, uint *pos, uint *offset, uchar *deltaseq);

    /** Decompress the suffix associate to a given word.
	@seq: string containing the text to be decoded.
	@pos: pointer to the byte at which start decoding.
	@offset: offset within this last byte.
	@suffix: pointer to store the suffix word.
	@return: number of characters extracted.
    */
    uint decompressWord(uchar *seq, uint *pos, uint *offset, uchar *suffix);

    /** Decompress the first word in a given block (starting in pos).
	@seq: string containing the text to be decoded.
	@pos: pointer to the byte at which start decoding.
	@word: pointer to store the word.
	@return: number of characters extracted.
    */
    uint decompressFirstWord(uchar *seq, uint *pos, uchar *word);


    /** Performs a Hu-Tucker encoding of code by using len bits.
	@code: value to be encoded.
	@len: number of bits using for encoding.
	@seq: string for storing the Hu-Tucker encoding.
	@pos: pointer to the byte at which start encoding.
	@offset: offset within this last byte.
    */
    void encodeHT(uint code, uint len, uchar *seq, uint *pos, uint *offset);

    /** Decodes a Hu-Tucker code.
	@seq: string containing the text to be decoded.
	@pos: pointer to the byte at which start decoding.
	@offset: offset within this last byte.
	@return: the decoded char.
    */
    uchar decodeHT(uchar *seq, uint *pos, uint *offset);

    /** Obtains the length of the long common prefix (lcp) of str1 and str2.
	@lcprefix: long common prefix value
	@str1: first string in the comparison.
	@str2: second string in the comparison.
	@lstr1: length of the first string.
	@lstr2: length of the second string.
	@return: the comparison value between the strings
    */
    int lcp(uint *lcprefix, uchar* str1, uchar* str2, uint lstr1, uint lstr2);
  };
};

#endif  
