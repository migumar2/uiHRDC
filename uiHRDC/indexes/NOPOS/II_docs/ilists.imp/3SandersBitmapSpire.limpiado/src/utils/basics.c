
// Basics

// #include "basics.h" included later to avoid macro recursion for malloc
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>

		// Memory management
	
	void *Malloc (size_t n)
	
	   { void *p;
	     if (n == 0) return NULL;
	     p = (void*) malloc (n);
	     if (p == NULL)
	        { fprintf (stderr,"Could not allocate %lu bytes\n",(unsigned long) n);
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
	        { fprintf (stderr,"Could not allocate %lu bytes\n",(unsigned long) n);
	          exit(1);
	        }
	     return p;
	   }

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
