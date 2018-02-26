
#ifndef SORTINGINCLUDED
#define SORTINGINCLUDED

#ifndef uint
#define uint  unsigned int
#endif

	/* used for sorting functions */
	typedef struct {
		uint key;
		uint value;
		uint p;
	}tsort;


	/** sorting a tsort-vector by field "key" (O(n^2)) but valid as n<< **/
int simplesortAsc(tsort *V, uint n);	
int simplesortDesc(tsort *V, uint n);

	/** sorting a tsort-vector descending, (O(n^2)) but valid as n<< **/
	
int simplesortUint(uint *V, uint n);


#endif

