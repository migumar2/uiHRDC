#ifndef SUFFIXARRAY_H_
#define SUFFIXARRAY_H_

#include <stdlib.h>
#include <stdio.h>
#include <limits.h>

#define KEY(p)          (V[*(p)+(h)])
#define SWAP(p, q)      (tmp=*(p), *(p)=*(q), *(q)=tmp)
#define MED3(a, b, c)   (KEY(a)<KEY(b) ?                        \
        (KEY(b)<KEY(c) ? (b) : KEY(a)<KEY(c) ? (c) : (a))       \
        : (KEY(b)>KEY(c) ? (b) : KEY(a)>KEY(c) ? (c) : (a)))
#ifndef MIN
#define MIN(a,b) (a < b) ? a : b
#endif

class SuffixArray
{
public:
	SuffixArray();
	void suffixsort(int *x, int *p, int n, int k, int l);
	~SuffixArray();
	
protected:
	int *I,                  /* group array, ultimately suffix array.*/
	   *V,                          /* inverse array, ultimately inverse of I.*/
	   r,                           /* number of symbols aggregated by transform.*/
	   h;                           /* length of already-sorted prefixes.*/	

	int scmp3(unsigned char *p, unsigned char *q, int *l, int maxl);	

	void update_group(int *pl, int *pm);
	void select_sort_split(int *p, int n);
	int choose_pivot(int *p, int n);
	void sort_split(int *p, int n);
	void bucketsort(int *x, int *p, int n, int k);
	int transform(int *x, int *p, int n, int k, int l, int q);
};


#endif /*SUFFIXARRAY_H_*/
