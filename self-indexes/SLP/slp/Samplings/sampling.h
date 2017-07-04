/* Sampling.h
 * Copyright (C) 2010, Miguel A. Martinez-Prieto
 * all rights reserved.
 *
 * This class implements a sampling structure which stores positions for a set 
 * of different q-grams.
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

#ifndef SAMPLING_H
#define SAMPLING_H

#include "../Utils/deltacodes.h"
#include "CSD.h"

using namespace std;
#include <fstream>
#include <iostream>

class Sampling
{		
  public:		
    Sampling();
    Sampling(uchar *samples, uint length, uint *pointers, uint cptrs, uint blocksize, uint terminals, uint *posterms, uint delta_sampling);
    ~Sampling();

    uint extract(size_t id, uchar* s);
    void locatePrefix(uchar *pattern, uint len, uint *left, uint *right, uint *bleft, uint *bright);
    void locatePattern(uchar *pattern, uint len, uint *left, uint *right, uint *block);

    size_t leftBlockForTerminal(uint terminal);
    size_t rightBlockForTerminal(uint terminal);

    size_t getSize();
    size_t getBlocks();
    size_t getMaxLength();
    uint getTerminals();

    void save(ofstream & fp);
    static Sampling* load(ifstream & fp);

    void decompress();

  protected:
    uint prefixes;
    csd::CSD_HTFC *dictionary;

    DeltaCodes *ptrRules;
    uint length;

    uint terminals;
    uint *posterms;
};

#endif // SAMPLING_H
