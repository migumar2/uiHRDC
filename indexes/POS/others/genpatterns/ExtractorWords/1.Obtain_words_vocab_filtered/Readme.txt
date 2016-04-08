Words extractor from a text file

Extracts the different words in a text file. It counts their number of occurrences.
It also apply a filter by frequency and/or lenght of word.
Only the words passing the filter are output to the target file.

The text file cannot contain the character '\0'. Otherwise process may have unexpected results.

** Syntax: ./EXTRACTWORDS  <in file>  <out file> <minFreq> <maxFreq> <minLen> <maxLen> <onlyWords>

	-inFile: The source file
	-outFile: The target file that will contain the filtered words.
	- Filtering words i, such that: minFreq <=  freq(word_i)  <=maxFreq.
	- Filtering words i, such that: minLen  <= strlen(word_i) <=maxLen.
