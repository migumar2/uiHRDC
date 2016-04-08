#ifndef FILE_INFO_INCLUDED
#define FILE_INFO_INCLUDED

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>



	/*------------------------------------------------------------------
	 Obtains the size of a file.
	------------------------------------------------------------------ */
	unsigned long fileSize (char *filename);
	
	/* copies from infile to outfile */
	void copyFile (char *infile, char *outfile);

#endif

