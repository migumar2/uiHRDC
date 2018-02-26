/* Multipattern random searches using set Horspool's algorithm adapted to 
search inside a text compressed with End-Tagged Dense Code.
Lightweight Natural Language Text Compression: Information Retrieval 2006
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

/*-----------------------------------------------------------------------
  constants.h:     Defines some constants to adjust some parameters of the searcher.
 ------------------------------------------------------------------------*/
#define LOGGING_ENABLED

#define MAX_CODEWORD_SIZE 4
#define MAX_SIZE_OF_WORD 32768

#ifndef DEFBYTE	
	#define byte unsigned char
	#define DEFBYTE
#endif

#define AND &&
#define OR ||

/**** About the searches **************************************************/

#ifndef VERSIONETDC 			
#define VERSIONETDC
#endif

/**** Options used for debugging purposes. **********************************/

// #ifndef LOGGING_ENABLED    			//Enables writting in file LOG1 and LOG2
// #define LOGGING_ENABLED			//Hence, by using diff log1 log2, we compare the real #occurrences of the patterns
// #endif							//    with the number of occurrences that where found.	
//
// #ifndef CONTADOR_BYTES_ENABLED    	//Enables counting the number of bytes "of the file".
// #define CONTADOR_BYTES_ENABLED
// #endif

// #define INITPOS_NORANDOMSEARCH 10
// #ifndef NO_RANDOM_SEARCH 				//Fixes searching no random patterns
// #define NO_RANDOM_SEARCH				//      but those from given positions (from a given position, see searcher.h, searchFullFile)
// #endif

