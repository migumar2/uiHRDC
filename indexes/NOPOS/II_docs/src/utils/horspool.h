/* Searches using  Horspool's algorithm adapted to 
search inside a text compressed with End-Tagged Dense Code.
Lightweight Natural Language Text Compression: Information Retrieval 2006

Programmed by Antonio Fari�a.

Author's contact: Antonio Fari�a, Databases Lab, University of
A Coru�a. Campus de Elvi�a s/n. Spain  fari@udc.es

Copyright (C) 2006  Nieves R. Brisaboa, Gonzalo Navarro, Antonio Fari�a and Jos� R. Param�
Author's contact: antonio.fari@gmail.com

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
  horspool.h: Searching over ETDC  			
 ------------------------------------------------------------------------*/

#ifndef SEARCHERINCLUDED
#define SEARCHERINCLUDED
#endif

#include <string.h>
#include <stdlib.h>
#include <math.h>

#include <malloc.h>
#include <stdio.h>

#ifdef REPAIR_FARI_NO_COMPRESS
#include "../repairFARI/RepairPost.h"
#endif 

#ifdef REPAIR_MIGUEL
#include "../repair/RepairPost.h"
#endif

#ifndef byte
	#define byte unsigned char
#endif 

#define MIN(a,b) (a < b) ? a : b
#define MAX(a,b) (a > b) ? a : b
						
	void inicialize_d(byte *code, unsigned int len, byte *d);
	uint searchPositionsFullFile (uint startOffset, byte *cbeg, byte *cend, byte *code, int len,
																uint *positions, uint *size, byte *d);
