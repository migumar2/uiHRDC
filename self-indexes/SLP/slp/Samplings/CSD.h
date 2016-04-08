/* CSD.h
 * Copyright (C) 2011, Rodrigo Canovas & Miguel A. Martinez-Prieto
 * all rights reserved.
 *
 * Abstract class for implementing Compressed String Dictionaries following:
 *
 *   ==========================================================================
 *     "Compressed String Dictionaries"
 *     Nieves R. Brisaboa, Rodrigo Canovas, Francisco Claude, 
 *     Miguel A. Martinez-Prieto and Gonzalo Navarro.
 *     10th Symposium on Experimental Algorithms (SEA'2011), p.136-147, 2011.
 *   ==========================================================================
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
 * Contacting the authors:
 *   Rodrigo Canovas:  rcanovas@dcc.uchile.cl
 *   Miguel A. Martinez-Prieto:  migumar2@infor.uva.es
 */

#ifndef _COMPRESSEDSTRINGDICTIONARY_H
#define _COMPRESSEDSTRINGDICTIONARY_H

#include <iostream>
#include <fstream>
#include <cassert>
using namespace std;

#include <libcdsBasics.h>
using namespace cds_utils;

namespace csd
{
static const size_t HTFC = 3;
static const int NOTFOUND = -1;

class CSD
{		
  public:			
    /** General destructor */
    virtual ~CSD() {};

    /** Returns the ID that identify s[1..length]. If it does not exist, 
	returns 0. 
	@s: the string to be located.
	@len: the length (in characters) of the string s.
    */
    virtual size_t locate(uchar *s, size_t len, uint *left, uint *bleft)=0;
//    virtual size_t locate(uchar *s, size_t len)=0;

    /** Returns the string identified by id.
	@id: the identifier to be extracted.
    */
    virtual uchar * extract(size_t id)=0;
    virtual uint extract(size_t id, uchar * string)=0;

    /** Obtains the original Tdict from its CSD representation. Each string is
	separated by '\n' symbols.
	@dict: the plain uncompressed dictionary.
	@return: number of total symbols in the dictionary.
    */
    virtual uint decompress(uchar **dict)=0;

    /** Returns the size of the structure in bytes. */
    virtual size_t getSize()=0;

    /** Returns the number of strings in the dictionary. */
    size_t getLength();

    /** Returns the length of th largest string in the dictionary. */
    size_t getMaxLength();

    /** Stores a CSD structure given a file pointer.
	@fp: pointer to the file saving a CSD structure.
    */
    virtual void save(ofstream & fp)=0;

    /** Loads a CSD structure from a file pointer.
	@fp: pointer to the file storing a CSD structure. */
    static CSD * load(ifstream & fp);
		
  protected:
    size_t type; 	//! Dictionary type.
    size_t tlength;	//! Original Tdict size.
    size_t length;	//! Number of elements in the dictionary.
    size_t maxlength; 	//! Length of the largest string in the dictionary.
  };
};

#include "CSD_HTFC.h"

#endif  
