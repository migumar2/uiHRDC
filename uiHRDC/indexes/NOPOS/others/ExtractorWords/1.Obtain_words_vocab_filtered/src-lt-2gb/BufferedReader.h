/* Words extractor -- 

Parses a text file and computes the different words in it.
Then, it applies a filter by frequency of word and/or the lenght
of the words, in such a way that only the "filtered words" are
output to the target file.

This target file is useful since it contains a set of patterns that
can be later searched inside the input text file. 

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
   
/*
BufferedReader is a modification of the buffering routines found in
"Nrgrep -- a fast and flexible pattern matching tool.
Copyright (C) 2000 Gonzalo Navarro"
http://www.dcc.uchile.cl/~gnavarro/software/sindex.html
*/


#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#include <string.h>
#include <fcntl.h>

#ifndef BUFFERINCLUDED
#define BUFFERINCLUDED

#define BUFFER_SIZE 1024*1024

#define byte char
/* Buffer management */

typedef struct sBuffer
  { int size;		/* size of this buffer */
    FILE *file;		/* associated file */
    int fpos;		/* pos of current buffer in file */
    byte *data;		/* current text */
    int dsize;		/* size of current text */
    int minDataInBuffer;
  } *BufferRead;

/* Creates a new buffer */
BufferRead bufCreate (unsigned int minDataInBuffer);

/* Terminates use of buffer B */
void bufDestroy (BufferRead B);

/* Assigns a file to B */
void bufSetFile (BufferRead B, FILE *file);

/* Gets buffer size of B */
int bufSizer (BufferRead B);

/* Reads a new buffer starting at position pnext */
void bufLoad (BufferRead B, byte *pnext);

/* Tells whether there is no more data in B + its file */
int bufEmpty (BufferRead B);

/* Tells whether there is no more data to read in B's file */
int bufEof (BufferRead B);

/* Tells the file position of the current buffer */
int bufTextPos (BufferRead B);

/* Gives the area of memory of the current buffer */
/* One text position before the given buffer can be touched */
void bufCurrent (BufferRead B, byte **base, byte **top);

#endif
