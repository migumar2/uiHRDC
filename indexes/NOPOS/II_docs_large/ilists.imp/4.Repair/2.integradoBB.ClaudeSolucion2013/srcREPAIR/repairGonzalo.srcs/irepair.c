
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

long PRNC = 0;
long PRNR = 0;
long PRNP = 0;
long PRNL = 0;

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

float factor = 0.75; // 1/extra space overhead; set closer to 1 for smaller and
		     // slower execution
long minsize = 256; // to avoid many reallocs at small sizes, should be ok as is

long u; // |text| and later current |C| with gaps

long *C; // compressed text

long c;  // real |C|

long INI_c; //fari... para mostrar numero de iteracion y progreso durante repair (mejora por cada nueva regla)
double CORTE_REPAIR = 0.0;


long alph; // max used terminal symbol

long n; // |R|

Tlist *L; // |L| = c;

Thash Hash; // hash table of pairs

Theap Heap; // special heap of pairs

Trarray Rec; // records




void prepare (long len)

  { long i;
	long id;
    Tpair pair;
    c = u = len;

    INI_c = c;  //fari... para mostrar numero de iteracion y progreso durante repair (mejora por cada nueva regla)
    
    alph = 0;
    for (i=0;i<u;i++) 
	{ if (C[i] > alph) alph = C[i];
	}
    n = ++alph;
    Rec = createRecords(factor,minsize);
    Heap = createHeap(u,&Rec,factor,minsize);
    Hash = createHash(256*256,&Rec);
    fprintf (stderr,"Prepare: Trying to allocated %ld bytes for List\n",u*sizeof(Tlist)); fflush(stderr);
    L = (void*)malloc(u*sizeof(Tlist));
    assocRecords (&Rec,&Hash,&Heap,L);
    for (i=0;i<c-1;i++) 
	{ pair.left = C[i]; pair.right = C[i+1];
	  id = searchHash (Hash,pair);
    	  if (id == -1) // new pair, insert
	     { id = insertRecord (&Rec,pair);
	       L[i].next = -1;
	     }
	  else 
	     { L[i].next = Rec.records[id].cpos;
	       L[L[i].next].prev = i;
	       incFreq (&Heap,id);
	     }
	  L[i].prev = -id-1;
	  Rec.records[id].cpos = i;
if (PRNL && (i%10000 == 0)) printf ("Processed %ld chars\n",i);
	}
    purgeHeap (&Heap);
  }

void prnSym(long c)
   { printf("%ld",c); 
   }

void prnC (void)

  { long i = 0;
    printf ("C[1..ld] = ",c);
    while (i<u)
      { prnSym(C[i]);
  	printf (" ");
	i++; 
	if ((i<u) && (C[i] < 0)) i = -C[i]-1; 
      }
    printf ("\n\n");
// printf ("Physical C[1..%i] = ",u);
// for (i=0;i<u;i++)
//       { prnSym(C[i]);
//  	printf (" ");
//       }
//     printf ("\n\n");
  }

void prnRec (void)

  { long i;
    printf ("Active pairs:\n");
    for (i=0;i<Rec.size;i++)
        { printf ("\t(");
	  prnSym(Rec.records[i].pair.left);
          printf (",");
	  prnSym(Rec.records[i].pair.right);
	  printf ("), %ld occs\n", Rec.records[i].freq);
	}
    printf ("\n");
  }

	// to avoid another list to access the sparse C we thread it using the
	// empty space. if next cell of an active cell is negative, it is 
	// (minus) a ptr to the next occ. idem previous cell to previous occ,
	// except that next ptr dominates over prev ptr if they must be in
 	// the same cell. but in this case one can find prev in O(1) anyway.


// Fari:
// Posto que alpha = num_terminais = valor do primeiro non-terminal.
//  ** a primeira regla sería: alpha <-- left1,right1
//  ** a  segunda regla sería: alpha +1 <-- left2, right2
// por iso no ficheiro de reglas só garda <alpha> e despois: left1, right1, left2, right2,... 
// pois xa sabe que os valores alpha, alpha+1,... son contiguos ;)
long repair (FILE *R)

  { long oid,id;
	long cpos;
    Trecord *rec,*orec;
    Tpair pair;
    int alpha_i = alph;
    if (fwrite(&alpha_i,sizeof(int),1,R) != 1) return -1;
if (PRNC) prnC();

long i=0;  //fari... solo para mostrar numero de iteracion
double prev_ratio =100.0;

while (n+1 > 0)
{ 
	if (PRNR) prnRec();
		oid = extractMax(&Heap);
		if (oid == -1) break; // the end!!
		orec = &Rec.records[oid];
		cpos = orec->cpos;
		
		Tpair_i p; p.left = orec->pair.left; p.right = orec->pair.right;
		if (fwrite (&p,sizeof(Tpair_i),1,R) != 1) return -1;
		//if (fwrite (&orec->pair,sizeof(Tpair),1,R) != 1) return -1;
	if (PRNP) 
		{ printf("Chosen pair %ld = (",n);
		  prnSym(orec->pair.left);
		  printf(",");
		  prnSym(orec->pair.right);
		  printf(") (%ld occs)\n",orec->freq);
		}
	while (cpos != -1)
	   { long ant,sgte,ssgte; 
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
		  id = searchHash(Hash,pair);
		  if (id != -1) // may not exist if purgeHeap'd
	             { if (id != oid) decFreq (&Heap,id); // not to my pair!
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
		  id = searchHash(Hash,pair);
	          if (id == -1) // new pair, insert
		     { id = insertRecord (&Rec,pair);
 		       rec = &Rec.records[id];
	               L[cpos].next = -1;
		     }
	          else 
 		     { incFreq (&Heap,id);
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
		  id = searchHash(Hash,pair);
		  if (id != -1) // may not exist if purgeHeap'd
	             { if (id != oid) decFreq (&Heap,id); // not to my pair!
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
		  id = searchHash(Hash,pair);
	          if (id == -1) // new pair, insert
		     { id = insertRecord(&Rec,pair);
 		       rec = &Rec.records[id];
	               L[ant].next = -1;
	             }
	          else 
	             { incFreq (&Heap,id);
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
	 removeRecord (&Rec,oid);
	 n++;
	 purgeHeap(&Heap); // remove freq 1 from heap
	 if (c < factor * u) // compact C
	    { long i,ni;
	      i = 0;
	      for (ni=0;ni<c-1;ni++) 
		{ C[ni] = C[i];
		  L[ni] = L[i];
		  if (L[ni].prev < 0) 
		     { if (L[ni].prev != NullFreq) // real ptr
			  Rec.records[-L[ni].prev-1].cpos = ni; 
		     }
		  else L[L[ni].prev].next = ni;
		  if (L[ni].next != -1) L[L[ni].next].prev = ni;
		  i++; if (C[i] < 0) i = -C[i]-1;
		}
	      C[ni] = C[i];
	      u = c;
	      C = realloc (C, c * sizeof(long));
	      L = realloc (L, c * sizeof(Tlist));
              assocRecords (&Rec,&Hash,&Heap,L);
	    }
       
       
    //*fari*/  
    i++;  //fari... para mostrar numero de iteracion 
    if ( ( i<10) || (! ((i-1)%100)) ) 
    	printf("Repair.compress: It: %ld compressed: %3.15f %% |c| = %ld , diff prev iter = %2.15f\n", i, 100.0*c/INI_c, c, (prev_ratio - (100.0*c/INI_c)) );
     //cout << "Repair.compress: It: " << i++ << " compressed: " << 100.*c/INI_c << "%" << " |c|=" << c << endl;
	//if ((100.*m/n)<43.0) break;
	//if ((100.0*c/INI_c)< CORTE_REPAIR) break;

	//if  (  ((prev_ratio - (100.0*c/INI_c)) < 0.00000009 )  ||  ((100.0*c/INI_c)< CORTE_REPAIR) ) break;
	
	//if  (  ((prev_ratio - (100.0*c/INI_c)) < 0.000000005 )  ||  (((prev_ratio - (100.0*c/INI_c)) < CORTE_REPAIR) ) ) 
	if    (((prev_ratio - (100.0*c/INI_c)) < CORTE_REPAIR) ) 
	break;

	  	
	prev_ratio = (100.0*c/INI_c);
	  	
} //while 
printf("Repair.compress: It: %ld compressed: %3.15f %% |c| = %ld , diff prev iter = %2.15f \n", i, 100.0*c/INI_c, c, (prev_ratio - (100.0*c/INI_c)) );
//printf("Repair.compress: It: %d compressed: %3.8f %% |c| = %d \n", i, 100.0*c/INI_c, c);
     return 0;
   }

void main (int argc, char **argv)

   { char fname[1024];
     FILE *Tf,*Rf,*Cf;
     long i,len;
     struct stat s;
     if ((argc < 2) || (argc > 3))
	{ fprintf (stderr,"Usage: %s <filename> [<ratio-break-repair [def=0.0 = no-break]>]\n"
			  "Compresses <filename> with repair and creates "
			  "<filename>.ext compressed files\n"
			  "This is a version for sequences of integers\n\n",
			argv[0]);
	  exit(1);
	}
	
	if (argc==3) {
		CORTE_REPAIR = atof(argv[2]);
		printf("\n CORTE_REPAIR set to %2.15f\n", CORTE_REPAIR);
	}
	else 
			printf("\n CORTE_REPAIR set to %1.1f: Repair is not broken up to the end\n", CORTE_REPAIR);
	
     if (stat (argv[1],&s) != 0)
	{ fprintf (stderr,"Error: cannot stat file %s\n",argv[1]);
	  exit(1);
	}
     len = s.st_size/sizeof(int);
     Tf = fopen (argv[1],"r");
     if (Tf == NULL)
	{ fprintf (stderr,"Error: cannot open file %s for reading\n",argv[1]);
	  exit(1);
	}
/*
      C = (void*)malloc(len*sizeof(int));
     if (fread(C,sizeof(int),len,Tf) != len)
	{ fprintf (stderr,"Error: cannot read file %s\n",argv[1]);
	  exit(1);
	}
	*/
     C = (void*)malloc(len*sizeof(long));
     
     int value;
     for (i=0;i<len;i++) {
		if (fread(&value,sizeof(int),1,Tf) != 1)
			{ fprintf (stderr,"Error: cannot read file %s\n",argv[1]);
			  exit(1);
			}		
		C[i]=value; 
	 }
	 fprintf (stderr,"Input file %s read: %ld bytes\n",argv[1],len*sizeof(int));fflush(stderr);
	
	 
     fclose(Tf);
     prepare (len);
     strcpy(fname,argv[1]);
     strcat(fname,".R");
     Rf = fopen (fname,"w");
     if (Rf == NULL)
	{ fprintf (stderr,"Error: cannot open file %s for writing\n",fname);
	  exit(1);
	}
     if (repair(Rf) != 0)
	{ fprintf (stderr,"Error: cannot write file %s\n",fname);
	  exit(1);
	}
     if (fclose(Rf) != 0)
	{ fprintf (stderr,"Error: cannot close file %s\n",fname);
	  exit(1);
	}
     strcpy(fname,argv[1]);
     strcat(fname,".C");
     Cf = fopen (fname,"w");
     if (Cf == NULL)
	{ fprintf (stderr,"Error: cannot open file %s for writing\n",fname);
	  exit(1);
	}
     i = 0;
     while (i<u)
        {   int value;
			value = (int) *(C+i);
			if (fwrite(&value,sizeof(int),1,Cf) != 1)
             { fprintf (stderr,"Error: cannot write file %s\n",fname);
               exit(1);
             }
          i++; if ((i < u) && (C[i] < 0)) i = -C[i]-1;
        }
     if (fclose(Cf) != 0)
	{ fprintf (stderr,"Error: cannot close file %s\n",fname);
	  exit(1);
	}
     fprintf (stderr,"RePair succeeded\n\n");
     fprintf (stderr,"   Original ints: %ld\n",len);
     fprintf (stderr,"   Number of rules: %ld\n",n-alph);
     fprintf (stderr,"   Final sequence length: %lu\n",c);
     fprintf (stderr,"   Compression ratio: %0.2f%%\n",
	    (2.0*(n-alph)+c)*(float)blog(n-1)/(float)(len*blog(alph-1))*100.0);

     fprintf (stderr,"   Compression ratio: %0.2f%%\n", 100.0*c/INI_c);
     exit(0);
   }

