/* basics.h
 * Copyright (C) 2011, Antonio Fariña, all rights reserved.
 * 
 * basics.h: Basic bitwise operations + bitVector manipulation, and others.
 * Original version from pizza-chili code.
 * Modified by Antonio Fariña in 2010.
 *    
 * Implementation of an representation of compressed postings lists. It provides
 *   Culpeper & Moffat skiper structure over postings lists compressed with vbyte.
 * 
 * Contact info:
 * Antonio Fariña, University of A Coruña
 * http://vios.dc.fi.udc.es/
 * antonio.fari[@]gmail.com  
 * 
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
 */


// Basics

// #include "basics.h" included later to avoid macro recursion for malloc
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>

#include "basics.h"

    // bits needed to represent a number between 0 and n

uint bits (uint n)

   { uint b = 0;
     while (n)
	{ b++; n >>= 1; }
     return b;
   }

        // returns e[p..p+len-1], assuming len <= W

uint bitread (uint *e, uint p, uint len)

   { uint answ;
     e += p/W; p %= W;
     answ = *e >> p;
     if (len == W)
	  { if (p) answ |= (*(e+1)) << (W-p);
	  }
     else { if (p+len > W) answ |= (*(e+1)) << (W-p);
            answ &= (1<<len)-1;
	  }
     return answ;
   }


  	// writes e[p..p+len-1] = s, len <= W

void bitwrite (register uint *e, register uint p, 
	       register uint len, register uint s)

   { e += p/W; p %= W;
     if (len == W)
	  { *e |= (*e & ((1<<p)-1)) | (s << p);
            if (!p) return;
            e++;
            *e = (*e & ~((1<<p)-1)) | (s >> (W-p));
	  }
     else { if (p+len <= W)
	       { *e = (*e & ~(((1<<len)-1)<<p)) | (s << p);
		 return;
	       }
	    *e = (*e & ((1<<p)-1)) | (s << p);
            e++; len -= W-p;
            *e = (*e & ~((1<<len)-1)) | (s >> (W-p));
	  }
   }
  	// writes e[p..p+len-1] = 0

void bitzero (register uint *e, register uint p, 
	       register uint len)

   { e += p/W; p %= W;
     if (p+len >= W)
	{ *e &= ~((1<<p)-1);
	  len -= p;
	  e++; p = 0;
	}
     while (len >= W)
	{ *e++ = 0;
	  len -= W;
	}
     if (len > 0)
	*e &= ~(((1<<len)-1)<<p);
   }
