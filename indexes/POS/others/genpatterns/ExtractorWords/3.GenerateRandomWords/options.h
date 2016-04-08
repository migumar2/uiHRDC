/* RANDOM PATTERNS CHOOSER -------------
   Given 
   	A file cointaining a given number of words (and /or separators) with their lenght and frequency ...
   	A value #maxIters
   	A value #patternsPerIter1, #patternsPerIter2, ..., #patternsPerIterN
   
   Chooses randomly #iters times #patternsPerIter random patterns, ensuring that no repeated patterns
   are generated in each iteration.
   
   Outputs...
   	N files containing #patternsPerIter_i * #iters  patterns.
   		The patterns in positions (0 < i <#patterns) + j*#patterns, j=0,1,2 ... #maxIters can not be repeated.
-----

Copyright (C) 2005 Antonio Fariña.

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

Author's contact: Antonio Fariña, Dept. of Computer Science, University of
A Coruña. Campus de Elviña s/n. Spain  fari@udc.es
*/


// The program can output: 
//	1- The random patterns (one in a line)
//	2- The frequency and the characters of the pattern

//uncomment to write only words in the output file.
//#define OPTION_ONLY_WORDS 

//uncomment to write both frequencies and words.
#define OPTION_FREQ_AND_WORDS 
