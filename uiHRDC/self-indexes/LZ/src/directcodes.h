/* Wavelet Tree over Dense Code. -- 
A word index using wavelet tree strategy over compressed text.

Programmed by Susana Ladra.

Author's contact: Susana Ladra, Databases Lab, University of
A Coruna. Campus de Elvina s/n. Spain  sladra@udc.es

Copyright (C) 2007  Nieves R. Brisaboa, Antonio Farina and Susana Ladra
Author's contact: susanaladra@gmail.com

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
*/
  


/*-----------------------------------------------------------------------
 Hash: Definition of HashTable class (Linear Hash)
 ------------------------------------------------------------------------*/
#ifndef __DIRECTCODES_H__
#define __DIRECTCODES_H__

#include <cstdio>
#include "basics.h"
#include "bitrankw32int.h"
namespace lz77index{
namespace basics{

typedef struct sFTRep {
	  unsigned int listLength;
	  unsigned char nLevels;
	  unsigned char * levels;
	  unsigned int * levelsIndex;
	  unsigned int * rankLevels;
	  bitRankW32Int * bS;	
	  unsigned int * bits_bitmap;
} FTRep;

// public:
FTRep* createFT(unsigned int *list,unsigned int listLength);
unsigned int accessFT(FTRep * listRep,unsigned int param);
void saveFT(FTRep * listRep, FILE* fp);
unsigned int * decompressFT(FTRep * listRep, unsigned int n);
FTRep* loadFT(FILE* fp);
void destroyFT(FTRep * listRep);
unsigned int sizeFT(FTRep* listRep);

}
}
#endif
