
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "interface.h"

/* only for getTime() */
#include <sys/time.h>
#include <sys/resource.h>
 
 
 
/* macro to detect and notify errors  */
#define IFERRORIL(error) {{if (error) { fprintf(stderr, "%s\n", error_il(error)); exit(1); }}}

int read_file(char *filename, uint **sourceInts, uint *length);
void print_il_usage(char *);
double getTime(void);

int main(int argc, char *argv[]) {

	char *infile, *outfile;
    uint *source;
	char *params = NULL;
	uint source_len;
	void *index;
	int error, i;
	double start, end;
	
	


    if (argc < 3) print_il_usage(argv[0]);
	if (argc > 3) { 
		int nchars, len;
		nchars = argc-3;
		for(i=2;i<argc;i++)
			nchars += strlen(argv[i]);
		params = (char *) malloc((nchars+1)*sizeof(char));
		params[nchars] = '\0';
		nchars = 0;
		for(i=3;i<argc;i++) {
			len = strlen(argv[i]);
			strncpy(params+nchars,argv[i],len);
			params[nchars+len] = ' ';
			nchars += len+1;
		}
		params[nchars-1] = '\0';
	}

	infile = argv[1];
	outfile = argv[2];

	start = getTime();
	error = read_file(infile, &source, &source_len);
	IFERRORIL(error);

	error = build_il(source, source_len, params, &index);
	IFERRORIL(error);

	error = save_il(index, outfile);
	IFERRORIL(error);
	end = getTime();	

	fprintf(stderr, "Building time: %.3f secs\n", end-start );
	
	uint index_len;
	size_il(index, &index_len);
	uint index_len_uncompressed;
	size_il_uncompressed(index, &index_len_uncompressed);
	
	fprintf(stdout,"Input: %lu bytes --> Output %lu bytes.\n", index_len_uncompressed, index_len);
	fprintf(stdout,"+-Overall compression --> %.2f%% (+-%.2f bytes per post).\n\n",
     			(100.0*index_len)/(index_len_uncompressed), (index_len*4.0)/(index_len_uncompressed));

	error = free_il(index);
	IFERRORIL(error);

	free(params);
	exit(0);
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

	return (usertime + systime);

}

void print_il_usage(char * progname) {
	fprintf(stderr, "Usage: %s <source postings file> <invLists_file> [<parameters>]\n", progname);
	fprintf(stderr, "  available parameters: %s", showParameters_il());
	fprintf(stderr, "\nIt builds the compressed representation of the postings in <source file>,\n");
	fprintf(stderr, "storing it in <invLists_file>. Any additional <parameters> \n");
	fprintf(stderr, "will be passed to the construction function.\n");
	fprintf(stderr, "At the end, the program sends to the standard error \n");
	fprintf(stderr, "performance measures on time to build the index.\n\n");
	exit(1);
}


/* Reads the file containing uints. */
int read_file(char *filename, uint **sourceInts, uint *length) {

  uint *source;
  uint t; 
  FILE *infile;
  
  infile = fopen(filename, "rb"); // b is for binary: required by DOS
  if(infile == NULL) return 30;
  
  /* store input file length */
  if(fseek(infile,0,SEEK_END) !=0 ) return 1;
  *length = ftell(infile)/(sizeof(uint));
  
  /* alloc memory for text (the overshoot is for suffix sorting) */
  source = (uint *) malloc((*length)*sizeof(uint)); 
  if(source == NULL) return 1;  
  
  /* read text in one sweep */
  rewind(infile);
  t = fread(source, sizeof(uint), (size_t) (*length), infile);
  if (t!=*length) return 1;
  *sourceInts = source;
  fclose(infile);
  
  return 0;
}
