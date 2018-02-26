
/*

IRePair -- an implementation of Larsson and Moffat's compression and
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

	// extendible array for pairs

#include <stdlib.h>
#include "IRePair.h"

int 
IRePair::compress(int *text, unsigned int length, 
		  int **ctext, unsigned int *clength, 
		  unsigned int *csymbols, unsigned int *crules, 
		  Tdiccarray **rules)
{
	prepare (text, length);
	repair();
	 
	(*ctext) = (int*)malloc(c*sizeof(int));	
	int i = 0, x = 0;
	
	while (i<u)
	{ 
		(*ctext)[x] = *(C+i);
		x++; i++; 
		
		if ((i < u) && (C[i] < 0)) i = -C[i]-1;
	}

	free(C); free(L);
	Heap::destroyHeap(&Heap);
	Hash::destroyHash(&Hash);
	
	// Linking results with function parameters
	*csymbols = alph-1;
	*crules = n-alph;
	*rules = &Dicc;
	*clength = (unsigned int)c;

//	fprintf (stderr,"RePair succeeded\n\n");
//	fprintf (stderr,"   Original symbols: %i\n",length);
//	fprintf (stderr,"   Number of terminals/rules: %i/%i\n",*csymbols, *crules);
//	fprintf (stderr,"   Final sequence length: %i\n",*clength);
//	fprintf (stderr,"   Compression ratio: %0.2f%%\n", (2.0*(*crules)+*clength)*(float)blog(n-1)/(length*8.0)*100.0);
	
	return 0;
}

int 
IRePair::repair()
{
    int oid,id,cpos;
    Trecord *rec,*orec;
    Tpair pair;
    
if (PRNC) prnC();
    while (n+1 > 0)
      { 
if (PRNR) prnRec();
	oid = Heap::extractMax(&Heap);
	if (oid == -1) break; // the end!!
 	orec = &Rec.records[oid];
 	cpos = orec->cpos;
	
		// Adding a new rule to the dictionary
		int lrule = 0;
		
		if (orec->pair.left < alph) lrule++; 
		else lrule += Dicc.rules[orec->pair.left-alph].len;
		
		if (orec->pair.right < alph) lrule++; 
		else lrule += Dicc.rules[orec->pair.right-alph].len;
		
		Trule nrule = {orec->pair, lrule};
		Dictionary::insertRule (&Dicc, nrule);
	
if (PRNP) 
    { printf("Chosen pair %i = (",n);
      prnSym(orec->pair.left);
      printf(",");
      prnSym(orec->pair.right);
      printf(") (%i occs)\n",orec->freq);
    }
	while (cpos != -1)
	   { int ant,sgte,ssgte; 
		// replacing bc->e in abcd, b = cpos, c = sgte, d = ssgte
	     if (C[cpos+1] < 0) sgte = -C[cpos+1]-1; 
	     else sgte = cpos+1; 
	     if ((sgte+1 < u) && (C[sgte+1] < 0)) ssgte = -C[sgte+1]-1;
	     else ssgte = sgte+1; 
		// remove bc from L
	     if (L[cpos].next != -1) L[L[cpos].next].prev = -oid-1;
	     orec->cpos = L[cpos].next;
	     if (ssgte != u) // there is ssgte
		{ 	// remove occ of cd
		  pair.left = C[sgte]; pair.right = C[ssgte];
		  id = Hash::searchHash(Hash,pair);
		  if (id != -1) // may not exist if purgeHeap'd
	             { if (id != oid) Heap::decFreq (&Heap,id); // not to my pair!
		       if (L[sgte].prev != NullFreq) //still exists(not removed)
 		          { rec = &Rec.records[id];
		            if (L[sgte].prev < 0) // this cd is head of its list
		               rec->cpos = L[sgte].next;
		            else L[L[sgte].prev].next = L[sgte].next;
		            if (L[sgte].next != -1) // not tail of its list
		               L[L[sgte].next].prev = L[sgte].prev;
			  }
		     }
			// create occ of ed
	          pair.left = n;
		      id = Hash::searchHash(Hash,pair);
	          if (id == -1) // new pair, insert
		     { id = Records::insertRecord (&Rec,pair);
 		       rec = &Rec.records[id];
	               L[cpos].next = -1;
		     }
	          else 
 		     { Heap::incFreq (&Heap,id);
 		       rec = &Rec.records[id]; 
	               L[cpos].next = rec->cpos;
	               L[L[cpos].next].prev = cpos;
	             }
	          L[cpos].prev = -id-1;
	          rec->cpos = cpos;
		}
	     if (cpos != 0) // there is ant
		{ 	// remove occ of ab
	          if (C[cpos-1] < 0) 
		     { ant = -C[cpos-1]-1; 
		       if (ant == cpos) // sgte and ant clashed -> 1 hole
			  ant = cpos-2;
		     }
	          else ant = cpos-1; 
		  pair.left = C[ant]; pair.right = C[cpos];
		  id = Hash::searchHash(Hash,pair);
		  if (id != -1) // may not exist if purgeHeap'd
	             { if (id != oid) Heap::decFreq (&Heap,id); // not to my pair!
		       if (L[ant].prev != NullFreq) //still exists (not removed)
 		          { rec = &Rec.records[id];
		            if (L[ant].prev < 0) // this ab is head of its list
		                 rec->cpos = L[ant].next;
		            else L[L[ant].prev].next = L[ant].next;
		            if (L[ant].next != -1) // it is not tail of its list
		               L[L[ant].next].prev = L[ant].prev;
			  }
		     }
			// create occ of ae
	          pair.right = n;
		      id = Hash::searchHash(Hash,pair);
	          if (id == -1) // new pair, insert
		     { id = Records::insertRecord(&Rec,pair);
 		       rec = &Rec.records[id];
	               L[ant].next = -1;
	             }
	          else 
	             { Heap::incFreq (&Heap,id);
 		       rec = &Rec.records[id];
	               L[ant].next = rec->cpos;
	               L[L[ant].next].prev = ant;
	             }
	          L[ant].prev = -id-1;
	          rec->cpos = ant;
		}
	     C[cpos] = n;
	     if (ssgte != u) C[ssgte-1] = -cpos-1;
	     C[cpos+1] = -ssgte-1;
	     c--;
 	     orec = &Rec.records[oid]; // just in case of Rec.records realloc'd
	     cpos = orec->cpos;
	   }
if (PRNC) prnC();
	 Records::removeRecord (&Rec,oid);
	 n++;
	 Heap::purgeHeap(&Heap); // remove freq 1 from heap
       }
     return 0;
}

int 
IRePair::expand (int i, int d)
{
}

void
IRePair::prepare(int *text, int len)
{
    int i,id;
    Tpair pair;
    c = u = len;
    C = (int*)malloc(u*sizeof(int));
    alph = 0;
    for (i=0;i<u;i++) 
	{ C[i] = text[i];
	  if (C[i] > alph) alph = C[i];
	}
    n = ++alph;

    Rec = Records::createRecords(factor,minsize);
    Heap = Heap::createHeap(u,&Rec,factor,minsize);
    Hash = Hash::createHash(256*256,&Rec);
    L = (Tlist*)malloc(u*sizeof(Tlist));
    Records::assocRecords (&Rec,&Hash,&Heap,L);
	Dicc = Dictionary::createDicc(factor,minsize);
	
    for (i=0;i<c-1;i++) 
	{ pair.left = C[i]; pair.right = C[i+1];
	  id = Hash::searchHash (Hash,pair);
    	  if (id == -1) // new pair, insert
	     { id = Records::insertRecord (&Rec,pair);
	       L[i].next = -1;
	     }
	  else 
	     { L[i].next = Rec.records[id].cpos;
	       L[L[i].next].prev = i;
	       Heap::incFreq (&Heap,id);
	     }
	  L[i].prev = -id-1;
	  Rec.records[id].cpos = i;
if (PRNL && (i%10000 == 0)) printf ("Processed %i chars\n",i);
	}
    Heap::purgeHeap (&Heap);
}


void 
IRePair::prnSym(int c)
{
	printf("%i",c); 
}
	
void 
IRePair::prnC(void)
{
    int i = 0;
    printf ("C[1..%i] = ",c);
    while (i<u)
      { prnSym(C[i]);
  	printf (" ");
	i++; 
	if ((i<u) && (C[i] < 0)) i = -C[i]-1; 
      }
    printf ("\n\n");
}

void 
IRePair::prnRec(void)
{
    int i;
    printf ("Active pairs:\n");
    for (i=0;i<Rec.size;i++)
        { printf ("\t(");
	  prnSym(Rec.records[i].pair.left);
          printf (",");
	  prnSym(Rec.records[i].pair.right);
	  printf ("), %i occs\n", Rec.records[i].freq);
	}
    printf ("\n");
}
