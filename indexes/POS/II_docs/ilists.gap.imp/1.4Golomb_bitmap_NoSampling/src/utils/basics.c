
// Basics

// #include "basics.h" included later to avoid macro recursion for malloc
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>



/*
		// Memory management
	
	void *Malloc (size_t n)
	
	   { void *p;
	     if (n == 0) return NULL;
	     p = (void*) malloc (n);
	     if (p == NULL)
	        { fprintf (stderr,"Malloc, Could not allocate %zu bytes\n", (size_t) n);
	          exit(1);
	        }
	     return p;
	   }
	
	void Free (void *p)
	
	   { if (p) free (p); 
	   }
	
	void *Realloc (void *p, size_t n)
	
	   { if (p == NULL) return Malloc (n);
	     if (n == 0) { Free(p); return NULL; }
	     p = (void*) realloc (p,n);
	     if (p == NULL)
	        { fprintf (stderr,"Realloc, Could not allocate %zu bytes\n",n);
	          exit(1);
	        }
	     return p;
	   }
*/
#include "basics.h"

        // bits needed to represent a number between 0 and n

uint bits (size_t n)

   { uint b = 0;
     while (n)
	{ b++; n >>= 1; }
     return b;
   }



        // returns e[p..p+len-1], assuming len <= W

uint bitread (uint *e, size_t p, size_t len)

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

void bitwrite (register uint *e, register size_t p, 
	       register size_t len, register uint s)

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



void bitzero (register uint *e, register size_t p, 
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





/******************************************************************************/
/** to work with 64bit-values, only 64bit code (non portable code!!!!)        */
/******************************************************************************/
        // returns e[p..p+len-1], assuming len <= W

size_t bitread64 (size_t *e, size_t p, size_t len)

   { size_t answ;
     e += p/WW; p %= WW;
     answ = *e >> p;
     if (len == WW)
	  { if (p) answ |= (*(e+1)) << (WW-p);
	  }
     else { if (p+len > WW) answ |= (*(e+1)) << (WW-p);
            answ &= ( ((size_t)1)<<len)-1;
	  }
     return answ;
   }


  	// writes e[p..p+len-1] = s, len <= W

void bitwrite64 (register size_t *e, register size_t p, 
	       register size_t len, register size_t s)

   { e += p/WW; p %= WW;
     if (len == WW)
	  { *e |= (*e & ((((size_t)1)<<p)-1)) | (s << p);
            if (!p) return;
            e++;
            *e = (*e & ~((((size_t)1)<<p)-1)) | (s >> (WW-p));
	  }
     else { if (p+len <= WW)
	       { *e = (*e & ~(((((size_t)1)<<len)-1)<<p)) | (s << p);
		 return;
	       }
	    *e = (*e & ((((size_t)1)<<p)-1)) | (s << p);
            e++; len -= WW-p;
            *e = (*e & ~((((size_t)1)<<len)-1)) | (s >> (WW-p));
	  }
   }
  	// writes e[p..p+len-1] = 0


