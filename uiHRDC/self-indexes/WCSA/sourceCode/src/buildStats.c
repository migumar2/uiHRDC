/*
 * Run Queries
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "buildFacade.h"

/* only for getTime() */
#include <sys/time.h>
#include <sys/resource.h>

/* macro to detect and to notify errors */
#define IFERROR(error) {{if (error) { fprintf(stderr, "%s\n", error_index(error)); exit(1); }}}


/* local headers */

double getTime (void);
void usage(char * progname);
  		
static void *Index;	 /* opauque data type */
static ulong Index_size, Text_length;
static double Load_time;



/*
 * Temporary usage: run_queries <index file> <type> [length] [V]
 */
int main (int argc, char *argv[])
{
	int error = 0;
	char *filename;
	char querytype;
	
	if (argc != 2)	{
		usage(argv[0]);	
		exit (1);
	}
	
	filename = argv[1];

	printf("\n Stats of index: %s\n",argv[1]);
	
	Load_time = getTime ();
	error = load_index (filename, &Index);
	IFERROR (error);
	Load_time = getTime () - Load_time;
	fprintf (stderr, "\tLoad index time = %.2f secs\n", Load_time);

	error = index_size(Index, &Index_size);
	IFERROR (error);

	error = get_length(Index, &Text_length);
	IFERROR (error);
	
	
	ulong text_len;
	error = get_length(Index, &text_len);
	IFERROR (error);
	
	error = printInfo(Index);
	IFERROR(error);		
	
	error = free_index(Index);
	IFERROR(error);
	


	fprintf(stdout,"\t===============================================\n");
	fprintf(stdout,"\tInput: %lu bytes (text) --> Output %lu bytes (wcsa).\n", text_len, Index_size);
	fprintf(stderr,"\tIndex size = %lu Kb\n", Index_size/1024);
	fprintf(stdout,"\tOverall compression --> %.2f%% (%.2f bits per char).\n",
     			(100.0*Index_size)/text_len, (Index_size*8.0)/text_len);
	fprintf(stdout,"\t===============================================\n");


	return 0;
}

double
getTime (void)
{
	double usertime, systime;
	struct rusage usage;

	getrusage (RUSAGE_SELF, &usage);

	usertime = (double) usage.ru_utime.tv_sec +
		(double) usage.ru_utime.tv_usec / 1000000.0;

	systime = (double) usage.ru_stime.tv_sec +
		(double) usage.ru_stime.tv_usec / 1000000.0;

	//return (usertime + systime);
	return (usertime );
}


void usage(char * progname) {	
	fprintf(stderr, "\nThe program loads <index> and shows some stats on it\n");
	fprintf(stderr, "Usage:  %s <index> \n", progname);
}
