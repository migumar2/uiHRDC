/*

Repair -- an implementation of Larsson and Moffat's compression and
decompression algorithms.
Copyright (C) 2010-current_year Gonzalo Navarro

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

Author's contact: Gonzalo Navarro, Dept. of Computer Science, University of
Chile. Blanco Encalada 2120, Santiago, Chile. gnavarro@dcc.uchile.cl

*/

	// extendible array for records

#ifndef IREPAIR
#define IREPAIR

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include "basics.h"
#include "records.h"
#include "hash.h"
#include "heap.h"
#include "dictionary.h"

static int PRNC = 0;
static int PRNR = 0;
static int PRNP = 0;
static int PRNL = 0;

// to avoid many reallocs at small sizes, should be ok as is
static int minsize = 256;

static int u;		// |text| and later current |C| with gaps
static int *C; 		// compressed text
static int c;  		// real |C|
static int alph;	// max used terminal symbol
static int n; 		// |R|
static Tlist *L; 	// |L| = c;
static Thash Hash; 	// hash table of pairs
static Theap Heap; 	// special heap of pairs
static Trarray Rec; 	// records
static Tdiccarray Dicc;	// compressed dictionary

static Tpair *R; 	// rules
static char *ff;
static FILE *f;
static int maxdepth = 0;

class IRePair
{
public:
	static int compress(int *text, unsigned int length, 
			    int **ctext, unsigned int *clength, 
			    unsigned int *csymbols, unsigned int *crules, 
			    Tdiccarray **rules);

private:
	static int repair();
	static int expand (int i, int d);

	static void prepare(int *text, int len);
	static void prnSym(int c);
	static void prnC(void);
	static void prnRec(void);
};
#endif


