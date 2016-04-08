
#ifndef SORTINGINCLUDED_REP
#define SORTINGINCLUDED_REP

#ifndef uint
#define uint  unsigned int
#endif

	/* used for sorting functions */
	typedef struct {
		uint key;
		uint value;
		uint gap;
	}tsort_Rep;


	/** sorting a tsort-vector by field "key" (O(n^2)) but valid as n<< **/
int simplesortAsc_Rep(tsort_Rep *V, uint n);	
int simplesortDesc(tsort_Rep *V, uint n);

	/** sorting a tsort-vector descending, (O(n^2)) but valid as n<< **/
	
int simplesortUint_Rep(uint *V, uint n);


#endif

