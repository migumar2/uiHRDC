/* basics.c
 * Copyright (C) 2011, Antonio Fariña, all rights reserved.
 * 
 * basics.c: Basic bitwise operations + bitVector manipulation, and others.
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
 
#ifndef BASICSINCLUDED
#define BASICSINCLUDED

  // Includes 

#include <sys/types.h>
#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/times.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>

  // Memory management

  // Data types

#ifndef byte
	#define byte unsigned char
#endif 


#define max(x,y) ((x)>(y)?(x):(y))
#define min(x,y) ((x)<(y)?(x):(y))

  // Bitstream management

//#define W (8*sizeof(uint))
#define W (32)

	// bits needed to represent a number between 0 and n
uint bits (uint n);
        // returns e[p..p+len-1], assuming len <= W
uint bitread (uint *e, uint p, uint len);
        // writes e[p..p+len-1] = s, assuming len <= W
void bitwrite (uint *e, uint p, uint len, uint s);
    // writes e[p..p+len-1] = 0, no assumption on len
        
    /**/ //FARI. WITH ASSUMPTION ON LEN, OR IT CRASHES 
         //NOt WORKING UPON THE LIMIT OF THE STARTING uint.
void bitzero (uint *e, uint p, uint len);
	// reads bit p from e
#define bitget(e,p) (((e)[(p)/W] >> ((p)%W)) & 1)
	// sets bit p in e
#define bitset(e,p) ((e)[(p)/W] |= (1<<((p)%W)))
	// cleans bit p in e
#define bitclean(e,p) ((e)[(p)/W] &= ~(1<<((p)%W)))



/* bitRead and bitWrite as MACROS */
	// returns e[p..p+len-1], assuming len <= W
	//mybitread (uint returned value, uint *e, uint p, uint len)
#define mybitread(answ, v, p, len) \
   { uint *e ; \
   	 e=v;\
     e += p/W; p %= W; \
     answ = *e >> p; \
     if (len == W) \
	  { if (p) answ |= (*(e+1)) << (W-p); \
	  } \
     else { if (p+len > W) answ |= (*(e+1)) << (W-p); \
            answ &= (1<<len)-1; \
	  } \
   }


  	// writes e[p..p+len-1] = s, len <= W
	//void bitwrite (uint *e, uint p, uint len, uint s)
#define mybitwrite(v, p, len, s) \
   { uint *e ; \
   	 e=v; \
    e += p/W; p %= W; \
     if (len == W) \
	  { *e |= (*e & ((1<<p)-1)) | (s << p); \
            if (p) { \
            	e++; \
            	*e = (*e & ~((1<<p)-1)) | (s >> (W-p)); \
        	} \
	  } \
     else { if (p+len <= W) \
	       { *e = (*e & ~(((1<<len)-1)<<p)) | (s << p); \
	       } \
	       else { \
	    	*e = (*e & ((1<<p)-1)) | (s << p); \
            e++; len -= W-p; \
            *e = (*e & ~((1<<len)-1)) | (s >> (W-p)); \
           } \
	  } \
   } 

#endif
