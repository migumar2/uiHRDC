#include "SuffixArray.h"

SuffixArray::SuffixArray()
{
}

int* 
SuffixArray::sort(unsigned char *text, ulong length)
{
   int c, *x, *p;
   int k, l, nn;
#if COMPACT==2
   unsigned char q[UCHAR_MAX+1];
#endif
#if CHECK || PRINT==2
   unsigned char *s;
#endif

   p=(int *)malloc((length+1)*sizeof *p);
   x=(int *)malloc((length+1)*sizeof *x);
   if (! p || ! x) {
      fprintf(stderr, "malloc failed\n");
   }

#if COMPACT==1
   l=UCHAR_MAX;
   k=1;
   nn=0;
   for (; nn<(long)length; nn++) {
      x[nn]=c=text[nn];
      if (c<l)
         l=c;
      if (c>=k)
         k=c+1;
   }
#else
   for (nn; nn<length; nn++) {
      x[nn]=text[nn];
#if COMPACT==0
   l=0;
   k=UCHAR_MAX+1;
#elif COMPACT==2
   for (i=0; i<=UCHAR_MAX; ++i)
      q[i]=0;
   for (pi=x; pi<x+length; ++pi)
      q[*pi]=1;
   for (i=k=0; i<=UCHAR_MAX; ++i)
      if (q[i])
         q[i]=k++;
   for (pi=x; pi<x+length; ++pi)
      *pi=q[*pi]+1;
   l=1;
   ++k;
#endif
#endif

#if CHECK || PRINT==2
   s=malloc(length * sizeof *s);
   if (! s) {
      fprintf(stderr, "malloc failed\n");
      return 1;
   }
   for (i=0; i<length; ++i)
      s[i]=(unsigned char) (x[i]-l);
#endif
   
   suffixsort(x, p, length, k, l);
   
#if CHECK

   fprintf(stderr, "checking...\n");
   for (i=0; i<length; ++i) {
      if (scmp3(s+p[i], s+p[i+1], & j, MIN(length-p[i], length-p[i+1]))>=0)
         fprintf(stderr, "i %d p[i] %d p[i+1] %d\n", i, p[i], p[i+1]);
   }
   fprintf(stderr, "done.\n");
   
#endif

   free(x);
   
   return p;
}

int 
SuffixArray::scmp3(unsigned char *p, unsigned char *q, int *l, int maxl)
{
   int i;
   i = 0;
   while (maxl>0 && *p==*q) {
      p++; q++; i++;
      maxl--;
   }
   *l = i;
   if (maxl>0) return *p-*q;
   return q-p;
}

/* Subroutine for select_sort_split and sort_split. Sets group numbers for a
   group whose lowest position in I is pl and highest position is pm.*/
void 
SuffixArray::update_group(int *pl, int *pm)
{
   int g;

   g=pm-I;                      /* group number.*/
   V[*pl]=g;                    /* update group number of first position.*/
   if (pl==pm)
      *pl=-1;                   /* one element, sorted group.*/
   else
      do                        /* more than one element, unsorted group.*/
         V[*++pl]=g;            /* update group numbers.*/
      while (pl<pm);
}

/* Quadratic sorting method to use for small subarrays. To be able to update
   group numbers consistently, a variant of selection sorting is used.*/
void 
SuffixArray::select_sort_split(int *p, int n) {
   int *pa, *pb, *pi, *pn;
   int f, v, tmp;

   pa=p;                        /* pa is start of group being picked out.*/
   pn=p+n-1;                    /* pn is last position of subarray.*/
   while (pa<pn) {
      for (pi=pb=pa+1, f=KEY(pa); pi<=pn; ++pi)
         if ((v=KEY(pi))<f) {
            f=v;                /* f is smallest key found.*/
            SWAP(pi, pa);       /* place smallest element at beginning.*/
            pb=pa+1;            /* pb is position for elements equal to f.*/
         } else if (v==f) {     /* if equal to smallest key.*/
            SWAP(pi, pb);       /* place next to other smallest elements.*/
            ++pb;
         }
      update_group(pa, pb-1);   /* update group values for new group.*/
      pa=pb;                    /* continue sorting rest of the subarray.*/
   }
   if (pa==pn) {                /* check if last part is single element.*/
      V[*pa]=pa-I;
      *pa=-1;                   /* sorted group.*/
   }
}

/* Subroutine for sort_split, algorithm by Bentley & McIlroy.*/
int 
SuffixArray::choose_pivot(int *p, int n) {
   int *pl, *pm, *pn;
   int s;
   
   pm=p+(n>>1);                 /* small arrays, middle element.*/
   if (n>7) {
      pl=p;
      pn=p+n-1;
      if (n>40) {               /* big arrays, pseudomedian of 9.*/
         s=n>>3;
         pl=MED3(pl, pl+s, pl+s+s);
         pm=MED3(pm-s, pm, pm+s);
         pn=MED3(pn-s-s, pn-s, pn);
      }
      pm=MED3(pl, pm, pn);      /* midsize arrays, median of 3.*/
   }
   return KEY(pm);
}

/* Sorting routine called for each unsorted group. Sorts the array of integers
   (suffix numbers) of length n starting at p. The algorithm is a ternary-split
   quicksort taken from Bentley & McIlroy, "Engineering a Sort Function",
   Software -- Practice and Experience 23(11), 1249-1265 (November 1993). This
   function is based on Program 7.*/
void 
SuffixArray::sort_split(int *p, int n)
{
   int *pa, *pb, *pc, *pd, *pl, *pm, *pn;
   int f, v, s, t, tmp;

   if (n<7) {                   /* multi-selection sort smallest arrays.*/
      select_sort_split(p, n);
      return;
   }

   v=choose_pivot(p, n);
   pa=pb=p;
   pc=pd=p+n-1;
   while (1) {                  /* split-end partition.*/
      while (pb<=pc && (f=KEY(pb))<=v) {
         if (f==v) {
            SWAP(pa, pb);
            ++pa;
         }
         ++pb;
      }
      while (pc>=pb && (f=KEY(pc))>=v) {
         if (f==v) {
            SWAP(pc, pd);
            --pd;
         }
         --pc;
      }
      if (pb>pc)
         break;
      SWAP(pb, pc);
      ++pb;
      --pc;
   }
   pn=p+n;
   if ((s=pa-p)>(t=pb-pa))
      s=t;
   for (pl=p, pm=pb-s; s; --s, ++pl, ++pm)
      SWAP(pl, pm);
   if ((s=pd-pc)>(t=pn-pd-1))
      s=t;
   for (pl=pb, pm=pn-s; s; --s, ++pl, ++pm)
      SWAP(pl, pm);

   s=pb-pa;
   t=pd-pc;
   if (s>0)
      sort_split(p, s);
   update_group(p+s, p+n-t-1);
   if (t>0)
      sort_split(p+n-t, t);
}

/* Bucketsort for first iteration.

   Input: x[0...n-1] holds integers in the range 1...k-1, all of which appear
   at least once. x[n] is 0. (This is the corresponding output of transform.) k
   must be at most n+1. p is array of size n+1 whose contents are disregarded.

   Output: x is V and p is I after the initial sorting stage of the refined
   suffix sorting algorithm.*/
void 
SuffixArray::bucketsort(int *x, int *p, int n, int k)
{
   int *pi, i, c, d, g;

   for (pi=p; pi<p+k; ++pi)
      *pi=-1;                   /* mark linked lists empty.*/
   for (i=0; i<=n; ++i) {
      x[i]=p[c=x[i]];           /* insert in linked list.*/
      p[c]=i;
   }
   for (pi=p+k-1, i=n; pi>=p; --pi) {
      d=x[c=*pi];               /* c is position, d is next in list.*/
      x[c]=g=i;                 /* last position equals group number.*/
      if (d>=0) {               /* if more than one element in group.*/
         p[i--]=c;              /* p is permutation for the sorted x.*/
         do {
            d=x[c=d];           /* next in linked list.*/
            x[c]=g;             /* group number in x.*/
            p[i--]=c;           /* permutation in p.*/
         } while (d>=0);
      } else
         p[i--]=-1;             /* one element, sorted group.*/
   }
}

/* Transforms the alphabet of x by attempting to aggregate several symbols into
   one, while preserving the suffix order of x. The alphabet may also be
   compacted, so that x on output comprises all integers of the new alphabet
   with no skipped numbers.

   Input: x is an array of size n+1 whose first n elements are positive
   integers in the range l...k-1. p is array of size n+1, used for temporary
   storage. q controls aggregation and compaction by defining the maximum value
   for any symbol during transformation: q must be at least k-l; if q<=n,
   compaction is guaranteed; if k-l>n, compaction is never done; if q is
   INT_MAX, the maximum number of symbols are aggregated into one.
   
   Output: Returns an integer j in the range 1...q representing the size of the
   new alphabet. If j<=n+1, the alphabet is compacted. The global variable r is
   set to the number of old symbols grouped into one. Only x[n] is 0.*/
int 
SuffixArray::transform(int *x, int *p, int n, int k, int l, int q)
{
   int b, c, d, e, i, j, m, s;
   int *pi, *pj;
   
   for (s=0, i=k-l; i; i>>=1)
      ++s;                      /* s is number of bits in old symbol.*/
   e=INT_MAX>>s;                /* e is for overflow checking.*/
   for (b=d=r=0; r<n && d<=e && (c=d<<s|(k-l))<=q; ++r) {
      b=b<<s|(x[r]-l+1);        /* b is start of x in chunk alphabet.*/
      d=c;                      /* d is max symbol in chunk alphabet.*/
   }
   m=(1<<(r-1)*s)-1;            /* m masks off top old symbol from chunk.*/
   x[n]=l-1;                    /* emulate zero terminator.*/
   if (d<=n) {                  /* if bucketing possible, compact alphabet.*/
      for (pi=p; pi<=p+d; ++pi)
         *pi=0;                 /* zero transformation table.*/
      for (pi=x+r, c=b; pi<=x+n; ++pi) {
         p[c]=1;                /* mark used chunk symbol.*/
         c=(c&m)<<s|(*pi-l+1);  /* shift in next old symbol in chunk.*/
      }
      for (i=1; i<r; ++i) {     /* handle last r-1 positions.*/
         p[c]=1;                /* mark used chunk symbol.*/
         c=(c&m)<<s;            /* shift in next old symbol in chunk.*/
      }
      for (pi=p, j=1; pi<=p+d; ++pi)
         if (*pi)
            *pi=j++;            /* j is new alphabet size.*/
      for (pi=x, pj=x+r, c=b; pj<=x+n; ++pi, ++pj) {
         *pi=p[c];              /* transform to new alphabet.*/
         c=(c&m)<<s|(*pj-l+1);  /* shift in next old symbol in chunk.*/
      }
      while (pi<x+n) {          /* handle last r-1 positions.*/
         *pi++=p[c];            /* transform to new alphabet.*/
         c=(c&m)<<s;            /* shift right-end zero in chunk.*/
      }
   } else {                     /* bucketing not possible, don't compact.*/
      for (pi=x, pj=x+r, c=b; pj<=x+n; ++pi, ++pj) {
         *pi=c;                 /* transform to new alphabet.*/
         c=(c&m)<<s|(*pj-l+1);  /* shift in next old symbol in chunk.*/
      }
      while (pi<x+n) {          /* handle last r-1 positions.*/
         *pi++=c;               /* transform to new alphabet.*/
         c=(c&m)<<s;            /* shift right-end zero in chunk.*/
      }
      j=d+1;                    /* new alphabet size.*/
   }
   x[n]=0;                      /* end-of-string symbol is zero.*/
   return j;                    /* return new alphabet size.*/
}

/* Makes suffix array p of x. x becomes inverse of p. p and x are both of size
   n+1. Contents of x[0...n-1] are integers in the range l...k-1. Original
   contents of x[n] is disregarded, the n-th symbol being regarded as
   end-of-string smaller than all other symbols.*/
void 
SuffixArray::suffixsort(int *x, int *p, int n, int k, int l)
{
   int *pi, *pk;
   int i, j, s, sl;
   
   V=x;                         /* set global values.*/
   I=p;
   
   if (n>=k-l) {                /* if bucketing possible,*/
      j=transform(V, I, n, k, l, n);
      bucketsort(V, I, n, j);   /* bucketsort on first r positions.*/
   } else {
      transform(V, I, n, k, l, INT_MAX);
      for (i=0; i<=n; ++i)
         I[i]=i;                /* initialize I with suffix numbers.*/
      h=0;
      sort_split(I, n+1);       /* quicksort on first r positions.*/
   }
   h=r;                         /* number of symbols aggregated by transform.*/
   
   while (*I>=-n) {
      pi=I;                     /* pi is first position of group.*/
      sl=0;                     /* sl is negated length of sorted groups.*/
      do {
         if ((s=*pi)<0) {
            pi-=s;              /* skip over sorted group.*/
            sl+=s;              /* add negated length to sl.*/
         } else {
            if (sl) {
               *(pi+sl)=sl;     /* combine sorted groups before pi.*/
               sl=0;
            }
            pk=I+V[s]+1;        /* pk-1 is last position of unsorted group.*/
            sort_split(pi, pk-pi);
            pi=pk;              /* next group.*/
         }
      } while (pi<=I+n);
      if (sl)                   /* if the array ends with a sorted group.*/
         *(pi+sl)=sl;           /* combine sorted groups at end of I.*/
      h=2*h;                    /* double sorted-depth.*/
   }

   for (i=0; i<=n; ++i)         /* reconstruct suffix array from inverse.*/
      I[V[i]]=i;
}  


SuffixArray::~SuffixArray()
{
}
