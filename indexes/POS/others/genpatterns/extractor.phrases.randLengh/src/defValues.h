#ifndef DEFVALUES_INCLUDED
#define DEFVALUES_INCLUDED

//#define MAX_LEN_VOCABULARY 3000000    //number of different words --> (number of variants)
#define MAX_MEANINGFUL_WORDS 100000000  //number of words (non separators)
#define MAX_SIZE_OF_WORD 1000000 //255  //size of word
#define MAX_SIZE_OF_GAP 1000000 //1000  //size of separator
	//#define MAX_STOPW0RDS_SIZE 255        //size of stopword 
#define MAX_SIZE_OF_ANY 1000000 //1000


#define DEFAULT_SUFFIX_ARRAY_SIZE 50000000
#define DEFAULT_SAMPLE_PERIOD_Z 512
#define DEFAULT_SAMPLE_PERIOD_B 256


//for queries
#define MAX_INTEGER_PATTERN_SIZE 20
#define MAX_TEXT_PATTERN_SIZE 100   //maximum number of valid words in a searched pattern

#ifndef DEBUG_ON
	// #define DEBUG_ON
#endif

#define byte unsigned char


//#define CSA_ON	//IF uncommented --> presentation layer + CSA is built
				//generates the CSA or only the "presentation layer + SE_file"
				//where SE_file is a file with the array of integers indexed [plus an ending "0"].

// Extensions of created  files

#define STOPWORDS_FILE_EXT "stop"

#define CANONICAL_WORDS_FILE_EXT "can"
#define VARIANTS_OF_WORDS_FILE_EXT "Hvar"
#define VARIANTS_OF_WORDS_FILE_EXT2 "Hvar2"
#define HUFFWORD_CANONICAL_FILE_EXT "HfW"
#define ASCII_OF_SEPARATORS_FILE_EXT "HSepW"
#define HUFFMAN_SEPARATORS_FILE_EXT "HSep"

#define TOH_FILE_EXT "toh"
#define B_COMPR_FILE_EXT "b"
#define B_COMPR_FILE_BITMAP "bB"
#define SE_FILE_EXT "se"
#define Z_SAMPLES_FILE_EXT "zS"
#define CONSTANTS_FILE_EXT "cte"

#endif
