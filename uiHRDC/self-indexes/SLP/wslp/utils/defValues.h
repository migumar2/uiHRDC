#ifndef DEFVALUES_INCLUDED
#define DEFVALUES_INCLUDED

#define MAX_MEANINGFUL_WORDS 100000000  //number of words (non separators)
#define MAX_SIZE_OF_WORD 1000000 //255  //size of word
#define MAX_SIZE_OF_GAP 1000000 //1000  //size of separator
#define MAX_SIZE_OF_ANY 1000000 //1000

#define DEFAULT_SUFFIX_ARRAY_SIZE 50000000

#define DEFAULT_SAMPLE_PERIOD_Z 32
#define DEFAULT_SAMPLE_PERIOD_B 32

//for queries
#define MAX_INTEGER_PATTERN_SIZE 20
#define MAX_TEXT_PATTERN_SIZE 100   //maximum number of valid words in a searched pattern

#ifndef DEBUG_ON
	// #define DEBUG_ON
#endif

#define CSA_ON				//generates the CSA or only "presentation layer"

// Extensions of created  files

#define VOCABULARY_WORDS_FILE_EXT "words"
#define SE_FILE_EXT "se"
#define CONSTANTS_FILE_EXT "cte"

#endif
