/* bc.h
 * Copyright (C) 2011, Antonio Fariña, all rights reserved.
 * 
 * bc.h: coding functions for bytecodes.
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

#ifndef BC_INCLUDED
#define BC_INCLUDED

#define CEILLOG_2(x,v)                      \
{                                        \
    register unsigned _B_x = (x) - 1;       \
    (v) = 0;                                \
    for (; _B_x ; _B_x>>=1, (v)++);         \
}                 

#define DEC_TO_BC(bc, pos, num)             \
{ while (num >= 128) {                      \
 bc[pos++] = (byte)((num & 127) + 128);     \
 num = (num >> 7);                          \
}                                           \
bc[pos++] = (byte) num;                     \
}

#define SIZE_DEC_TO_BC(num,size)            \
{uint numtmp=num;                           \
  size=0;                                   \
  while (num >= 128) {                      \
   size++;                                  \
   num = (num >> 7);                        \
  }                                         \
size++; num=numtmp;                         \
}			
			
#define BC_TO_DEC(bc,pos,num) \
 { \
 	 register uint _pot, _x; \
 	 _pot=1; \
 	 _x = bc[pos++]; \
 	 num=0; \
 	 while (_x >= 128) { \
 	 	num = num + (_x-128)*_pot; \
 	 	_x=bc[pos++]; \
 	 	_pot<<=7; \
 	 } \
 	 num=num + _x*_pot; \
 }

//void check_BC_works(uint limit){
//	uint num, size,i;
//	num=0; SIZE_DEC_TO_BC(num,size);fprintf(stderr,"\n num %6d is encoded with %d bytes",num,size);
//	num=1; SIZE_DEC_TO_BC(num,size);fprintf(stderr,"\n num %6d is encoded with %d bytes",num,size);
//	num=128-1; SIZE_DEC_TO_BC(num,size);fprintf(stderr,"\n num %6d is encoded with %d bytes",num,size);
//	num=128+0; SIZE_DEC_TO_BC(num,size);fprintf(stderr,"\n num %6d is encoded with %d bytes",num,size);
//	num=128+1; SIZE_DEC_TO_BC(num,size);fprintf(stderr,"\n num %6d is encoded with %d bytes",num,size);
//
//	num=128*128-1; SIZE_DEC_TO_BC(num,size);fprintf(stderr,"\n num %6d is encoded with %d bytes",num,size);
//	num=128*128+0; SIZE_DEC_TO_BC(num,size);fprintf(stderr,"\n num %6d is encoded with %d bytes",num,size);
//	num=128*128+1; SIZE_DEC_TO_BC(num,size);fprintf(stderr,"\n num %6d is encoded with %d bytes",num,size);
//	
//	
//	byte code[10];
//	for (i=0;i<limit;i++) {
//		uint pos =0;
//		num =i;
//		DEC_TO_BC(code, pos, num);
//		pos =0;
//		BC_TO_DEC(code,pos,num);
//		if (i != num) {fprintf(stderr,"\n DISTINTOS !!!!");
//			return ;
//		}
//	}	
//	fflush(stderr);
//}

 #endif
