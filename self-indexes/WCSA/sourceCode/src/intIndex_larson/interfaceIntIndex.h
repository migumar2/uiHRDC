
// FUNCTION PROTOTYPES: SELF-INDEX ON INTEGERS.

int buildIntIndex (uint *intVector, uint n, char *build_options, void **index );
		
	//Saves the index to disk
int saveIntIndex(void *index, char *pathname);

	//Returns number of elements in the indexed sequence of integers
int sourceLenIntIndex(void *index, uint *numInts);

	//Loads the index from disk.
int loadIntIndex(char *pathname, void **index);
	
	//Frees the memory allocated to the int_index
int freeIntIndex(void *index);

	//Returns the size (in bytes) of the index over the sequence of integers.
int sizeIntIndex(void *index, uint *numBytes);

	// Shows detailed summary info of the self-index (memory usage of each structure)
int printInfoIntIndex(void *index, const char tab[]);
	
	//Number of occurrences of the pattern, and the interval [left,right] in the suffix array
int countIntIndex(void *index, uint *pattern, uint length, ulong *numocc, ulong *left, ulong *right);
	
	//returns an array with integers corresponding offsets to the occurrences of the pattern, 
	//as well as the number of occurrences
int locateIntIndex(void *index, uint *pattern, uint length, ulong **occ, ulong *numocc);
	
	//Returns the value of the source (array of integers) at a given offset.
	// (that is, the element "position" from the original array of uints)
int displayIntIndex(void *index, ulong position, uint *value);


