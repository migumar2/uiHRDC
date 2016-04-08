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

#ifndef REPAIR
#define REPAIR

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include <iostream>

#include "basics.h"
#include "records.h"
#include "hash.h"
#include "heap.h"
#include "dictionary.h"

#define MINSIZE 256

#define PRNC 0
#define PRNR 0
#define PRNP 0
#define PRNL 0

using namespace std;

class RePair
{
public:
	int compress(unsigned char *text, unsigned int length,
			    int **ctext, unsigned int *clength,
			    unsigned char **symbols, unsigned int *csymbols,
			    Tdiccarray **rules, unsigned int *crules);
private:
	int u;		// |text| and later current |C| with gaps
	int *C; 		// compressed text
	int c;  		// real |C|
	int alph;	// max used terminal symbol
	int n; 		// |R|
	Tlist *L; 	// |L| = c;
	Thash Hash; 	// hash table of pairs
	Theap Heap; 	// special heap of pairs
	Trarray Rec; 	// records
	Tdiccarray Dicc;	// compressed dictionary

	int *chars;
	unsigned char *map;

	int repair();

	void prepare(unsigned char *text, int len);
	void prnSym(int c);
	void prnC(void);
	void prnRec(void);
};
#endif


