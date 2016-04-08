
// Basics

// #include "basics.h" included later to avoid macro recursion for malloc
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>

		// Memory management
/*	
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
*/
#include "basics_wi.h"

        // bits needed to represent a number between 0 and n

uint bits_wi (uint n)

   { uint b = 0;
     while (n)
	{ b++; n >>= 1; }
     return b;
   }

        // returns e[p..p+len-1], assuming len <= WI32

uint bitread_wi (uint *e, uint p, uint len)

   { uint answ;
     e += p/WI32; p %= WI32;
     answ = *e >> p;
     if (len == WI32)
	  { if (p) answ |= (*(e+1)) << (WI32-p);
	  }
     else { if (p+len > WI32) answ |= (*(e+1)) << (WI32-p);
            answ &= (1<<len)-1;
	  }
     return answ;
   }


  	// writes e[p..p+len-1] = s, len <= WI32

void bitwrite_wi (register uint *e, register uint p, 
	       register uint len, register uint s)

   { e += p/WI32; p %= WI32;
     if (len == WI32)
	  { *e |= (*e & ((1<<p)-1)) | (s << p);
            if (!p) return;
            e++;
            *e = (*e & ~((1<<p)-1)) | (s >> (WI32-p));
	  }
     else { if (p+len <= WI32)
	       { *e = (*e & ~(((1<<len)-1)<<p)) | (s << p);
		 return;
	       }
	    *e = (*e & ((1<<p)-1)) | (s << p);
            e++; len -= WI32-p;
            *e = (*e & ~((1<<len)-1)) | (s >> (WI32-p));
	  }
   }
  	// writes e[p..p+len-1] = 0

void bitzero_wi (register uint *e, register uint p, 
	       register uint len)

   { e += p/WI32; p %= WI32;
     if (p+len >= WI32)
	{ *e &= ~((1<<p)-1);
	  len -= p;
	  e++; p = 0;
	}
     while (len >= WI32)
	{ *e++ = 0;
	  len -= WI32;
	}
     if (len > 0)
	*e &= ~(((1<<len)-1)<<p);
   }
