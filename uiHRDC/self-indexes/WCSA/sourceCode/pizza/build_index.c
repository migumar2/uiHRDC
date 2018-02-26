#include <stdio.h>
#include <stdlib.h>
#include <string.h>
       #include <sys/types.h>
       #include <sys/stat.h>
       #include <fcntl.h>
       #include <unistd.h>
       

#include "interface.h"

/* only for getTime() */
#include <sys/time.h>
#include <sys/resource.h>
 
/* macro to detect and notify errors  */
#define IFERROR(error) {{if (error) { fprintf(stderr, "%s\n", error_index(error)); exit(1); }}}

int read_file(char *filename, uchar **textt, ulong *length);
void print_usage(char *);
double getTime(void);


#define DOCBOUNDARIES_FILE_EXT "DOCBOUNDARIES"

/** **********************************************************************/
/** Some functions used during index construction                        */
/** **********************************************************************/
unsigned long fileSizeF (char *filename){
	FILE *fpText;
	unsigned long fsize;
	fpText = fopen(filename,"rb");
	fsize=0;
	if (fpText) {
		fseek(fpText,0,2);
		fsize= ftell(fpText);
		fclose(fpText);
		////fprintf(stderr,"fileSize = %ld",fsize);
	}
	return fsize;
}

	/* document beginnings within the source text are marked in offsets */
int loadDocBeginngins(uint **offsets, uint *docs, char *basename) {
	int file,readbytes;

	char *filename;	
	filename = (char *)malloc(sizeof(char)*(strlen(basename)+20));
	strcpy(filename, basename); strcat(filename, ".");	strcat(filename, DOCBOUNDARIES_FILE_EXT);

	fprintf(stderr,"\t Loading document boundaries (as integers) from disk %s...", filename);		
	if( (file = open(filename, O_RDONLY)) < 0) {
		printf("Cannot open file %s\n", filename);
		exit(0);
	}	

	uint ndocs = (uint) fileSizeF (filename)/sizeof(uint) -1;
	
	uint *docBeginnings = (uint *) malloc (sizeof(uint) * (ndocs +1));

	readbytes = read(file, docBeginnings, sizeof(uint) * (ndocs+1));		
	close(file);	
	*docs = ndocs;
	*offsets = docBeginnings;

	fprintf(stderr,"%u docs loaded \n", ndocs);		
	free(filename);
/*	
	{// just for debugging!!.
	 FILE *x = fopen("docboundaries.txt","w");
	 int i;
	 for (i=0; i< ndocs;i++)
		fprintf(x,"\n Doc %6u --> offset = %u",i,docBeginnings[i]);
	 fclose(x);
	}
*/	
	return 0;		
}



int main(int argc, char *argv[]) {

	char *infile, *outfile;
    uchar *text;
	char *params = NULL;
	ulong text_len;
	void *index;
	int error, i;
	double start, end;

    if (argc < 3) print_usage(argv[0]);
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
	error = read_file(infile, &text, &text_len);
	IFERROR(error);

	uint ndocs;
	uint *docboundaries;
	error =loadDocBeginngins(&docboundaries, &ndocs,(char *)infile);
	IFERROR(error);
	
	error = build_index(text, text_len, docboundaries, ndocs, params, &index);
	IFERROR(error);

	error = save_index(index, outfile);
	IFERROR(error);
	end = getTime();	

	fprintf(stderr, "Building time: %.3f secs\n", end-start );
	
	ulong index_len;
	index_size(index, &index_len);
	fprintf(stdout,"Input: %lu bytes --> Output %lu bytes.\n", text_len, index_len);
	fprintf(stdout,"Overall compression --> %.2f%% (%.2f bits per char).\n\n",
     			(100.0*index_len)/text_len, (index_len*8.0)/text_len);

	error = free_index(index);
	IFERROR(error);

	free(params);
	exit(0);
}

/* 
  Opens and reads a text file 
*/
int read_file(char *filename, uchar **textt, ulong *length) {

  uchar *text;
  unsigned long t; 
  FILE *infile;
  
  infile = fopen(filename, "rb"); // b is for binary: required by DOS
  if(infile == NULL) return 1;
  
  /* store input file length */
  if(fseek(infile,0,SEEK_END) !=0 ) return 1;
  *length = ftell(infile);
  
  /* alloc memory for text (the overshoot is for suffix sorting) */
  text = (uchar *) malloc((*length)*sizeof(*text)); 
  if(text == NULL) return 1;  
  
  /* read text in one sweep */
  rewind(infile);
  t = fread(text, sizeof(*text), (size_t) *length, infile);
  if(t!=*length) return 1;
  *textt = text;
  fclose(infile);
  
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

	return (usertime + systime);

}

void print_usage(char * progname) {
	fprintf(stderr, "Usage: %s <source file> <index file> [<parameters>]\n", progname);
	fprintf(stderr, "\nIt builds the index for the text in file <source file>,\n");
	fprintf(stderr, "storing it in <index file>. Any additional <parameters> \n");
	fprintf(stderr, "will be passed to the construction function.\n");
	fprintf(stderr, "At the end, the program sends to the standard error \n");
	fprintf(stderr, "performance measures on time to build the index.\n\n");
	exit(1);
}
