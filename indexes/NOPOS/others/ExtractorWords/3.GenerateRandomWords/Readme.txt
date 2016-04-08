Words extractor from a text file

Extracts the different words in a text file. It counts their number of occurrences.
It also apply a filter by frequency and/or lenght of word.
Only the words passing the filter are output to the target file.

The text file cannot contain the character '\0'. Otherwise process may have unexpected results.

** Syntax: ./RANDOMCHOOSER <AllPatternsFile> <outPrefix> <MaxIters> <numPatterns1> [<numPatterns2>] [<numPatterns3>] ...

	-AllPatternsFile: The source file (the output of the program EXTRACTWORDS)
	-outPrefix: A prefix added to the fileName of the files generated (which contain the random patterns).
	-MaxIters: Max number of iterations for which patterns are generated.
		*In each iteration numPatterns_k are generated, and no repetitions can occur.
		*A total MaxIters x numPatterns_k are output to file_k
	- numPatterns1, numPatterns2, ... numPatternsN.
		The process creates N outputFiles: 
			The first  one contains MaxIters * numPatterns1
			The second one contains MaxIters * numPatterns2
			...
			
			
Note:
The output files will contain 1 line per pattern. The format of the line is:
	[frequency of pattern] Pattern\n.

Where the frequency of pattern has format %9ld is an optional value. To hide it the file "options.h" contains a 
definition that permits make "RANDOMCHOOSER" to skip the [frequency of pattern], in such a way that lines will contain only the pattern
			
