/* bp_hb.h
 * Copyright (C) 2003, Gonzalo Navarro, all rights reserved.
 * Adapted by Rodrigo Canovas, 2010.
 * Diego Arroyuelo fixed minor details.
 *
 * Hash-based balanced parentheses definition
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

#ifndef BP_HB_H
#define	BP_HB_H

/*Gonzalo Navarro implementation for
 * balanced parentheses.
 *
 * note that an opening bracket is represented with a bit 0
 * and a closing bracket with a bit 1
 * */
#include "hash.h"
#include "static_bitsequence.h"

namespace lz77index{
namespace basics{

class bp_hb{
	private:
		unsigned int n;            // # of parentheses
		unsigned int sbits;        // bits for near pointers
		unsigned int b;            // bits per block
		hash sftable;      // table of far forward pointers
		hash sbtable;      // table of far backward pointers
		hash bftable;      // table of near forward pointers
		hash bbtable;      // table of near backward pointers
		hash open_sbtable; // table of far backward pointers (for findopen)
		hash open_bbtable; // table of near backward pointers (for findopen)
		
		unsigned char FwdPos[256][W/2];
		unsigned char BwdPos[256][W/2];
		char Excess[256];

		bp_hb();
		unsigned int calcsizes(unsigned int posparent, unsigned int posopen, unsigned int *near, unsigned int *far, unsigned int *pnear, unsigned int *pfar);
		unsigned int filltables(unsigned int posparent, unsigned int posopen, bool bwd);
		void fcompchar (unsigned char x, unsigned char *pos, char *excess);
		void bcompchar (unsigned char x, unsigned char *pos);

	public:
		unsigned int *data;                     // the parentheses sequence
		static_bitsequence *bdata;      // bitmap supporting rank and select over parentheses

		/** hp_hb: creates a parentheses structure from a bitstring, which gets *owned*.
		  length is the total number of parentheses, opening + closing
		  bwd says if you will want to perform findopen and enclose */
		bp_hb(unsigned int *string, unsigned int length, unsigned int block_size, bool bwd);
		~bp_hb();
		
		/** excess(v): computes the excess of the parenthesis at
		    position v of the sequence */
    		unsigned int excess(int v);

         	/** close(v): finds the closing parenthesis matching the
         	    one at position v of the sequence */
		unsigned int close(int v);

        	/** open(v): finds the opening parenthesis matching the
         	    one at position v of the sequence */
		unsigned int open(int v);

        	/** parent(v): finds the nearest parenthesis enclosing the
         	    one at position v of the sequence */
		unsigned int parent(int v);

        	/** enclose(v): finds the nearest parenthesis enclosing the
         	    one at position v of the sequence */
		unsigned int enclose(int v);

        	/** size(): size of the representation, in unsigned chars */
		unsigned int size();

        	/** save(fp): stores the representation in file fp */
		int save(FILE *fp);

         	/** load(fp): loads the representation from file fp */
		static bp_hb * load(FILE *fp);
};

}
}

#endif


