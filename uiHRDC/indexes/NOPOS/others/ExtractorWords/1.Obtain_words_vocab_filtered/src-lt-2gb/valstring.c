/* End-Tagged Dense Code. -- 
A semistatic word-based byte oriented compressor for text files.
Lightweight Natural Language Text Compression: Information Retrieval 2006
Copyright (C) 2005 Antonio Fariña.

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

Author's contact: Antonio Fariña, Dept. of Computer Science, University of
A Coruña. Campus de Elviña s/n. Spain  fari@udc.es
*/


/*-----------------------------------------------------------------------
 ValString: 
	Using an array to rapidly check if a character is an alpha-numeric character.
 ------------------------------------------------------------------------*/
 
/* Thanks to Edleno Silva de Moura for the ideas in hashing I found
   in his source code.
*/

#include "valstring.h"

char  _Valid[256];
char  _Invalid[256];


/*Initialization of vectors _Valid and _Invalid */
void StartValid() {

  unsigned x;

  for(x=0;x<128;x++) {
    if(ValidCh(x)) {
      _Valid[x]=1;
      _Invalid[x]=0;
    }
    else {
      _Valid[x]=0;
      _Invalid[x]=1;
    }
  }
  for(x=128;x<256;x++) {
    _Valid[x]=0;
    _Invalid[x]=1;
  }
  _Valid[0]=_Invalid[0]=0;

}
