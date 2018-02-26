
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

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include "basics.h"

int u; // |text| and later current |C| with gaps

int *C; // compressed text

int c;  // real |C|

int alph; // max used terminal symbol

Tpair *R; // rules

int n; // |R|

char map[256];

char *ff;
FILE *f;

int maxdepth = 0;

int expand (int i, int d)

   { int ret = 1;
     while (i >= alph)
       { ret += expand(R[i-alph].left,d+1); 
	 i = R[i-alph].right; d++;
       }
     if (putc(map[i],f) == EOF)
	{ fprintf (stderr,"Error: cannot write file %s\n",ff);
	  exit(1);
	}
     if (d > maxdepth) maxdepth = d;
     return ret; 
   }

void main (int argc, char **argv)

   { char fname[1024];
     char *text;
     FILE *Tf,*Rf,*Cf;
     int i,len,c,u;
     struct stat s;
     if (argc != 2)
	{ fprintf (stderr,"Usage: %s <filename>\n"
			  "Decompresses <filename> from its .C and .R "
			  "extensions\n\n",argv[0]);
	  exit(1);
	}
     strcpy(fname,argv[1]);
     strcat(fname,".R");
     if (stat (fname,&s) != 0)
	{ fprintf (stderr,"Error: cannot stat file %s\n",fname);
	  exit(1);
	}
     len = s.st_size;
     Rf = fopen (fname,"r");
     if (Rf == NULL)
	{ fprintf (stderr,"Error: cannot open file %s for reading\n",fname);
	  exit(1);
	}
     if (fread(&alph,sizeof(int),1,Rf) != 1)
	{ fprintf (stderr,"Error: cannot read file %s\n",fname);
	  exit(1);
	}
     if (fread(&map,sizeof(char),alph,Rf) != alph)
	{ fprintf (stderr,"Error: cannot read file %s\n",fname);
	  exit(1);
	}
     n = (len-sizeof(int)-alph)/sizeof(Tpair);
     R = (void*)malloc(n*sizeof(Tpair));
     if (fread(R,sizeof(Tpair),n,Rf) != n)
	{ fprintf (stderr,"Error: cannot read file %s\n",fname);
	  exit(1);
	}
     fclose(Rf);

     strcpy(fname,argv[1]);
     strcat(fname,".C");
     if (stat (fname,&s) != 0)
	{ fprintf (stderr,"Error: cannot stat file %s\n",fname);
	  exit(1);
	}
     c = len = s.st_size/sizeof(int);
     Cf = fopen (fname,"r");
     if (Cf == NULL)
	{ fprintf (stderr,"Error: cannot open file %s for reading\n",fname);
	  exit(1);
	}
     Tf = fopen (argv[1],"w");
     if (Tf == NULL)
	{ fprintf (stderr,"Error: cannot open file %s for writing\n",argv[1]);
	  exit(1);
	}
     u = 0; f = Tf; ff = argv[1];
     for (;len>0;len--)
	{ if (fread(&i,sizeof(int),1,Cf) != 1)
	     { fprintf (stderr,"Error: cannot read file %s\n",fname);
	       exit(1);
	     }
	  u += expand(i,0);
	}
     fclose(Cf);
     if (fclose(Tf) != 0)
	{ fprintf (stderr,"Error: cannot close file %s\n",argv[1]);
	  exit(1);
	}
     fprintf (stderr,"DesPair succeeded\n\n");
     fprintf (stderr,"   Original chars: %i\n",u);
     fprintf (stderr,"   Number of rules: %i\n",n-alph);
     fprintf (stderr,"   Compressed sequence length: %i\n",c);
     fprintf (stderr,"   Maximum rule depth: %i\n",maxdepth);
     fprintf (stderr,"   Compression ratio: %0.2f%%\n",
                        (2.0*(n-alph)+c)*(float)blog(n-1)/(u*8.0)*100.0);

     exit(0);
   }

