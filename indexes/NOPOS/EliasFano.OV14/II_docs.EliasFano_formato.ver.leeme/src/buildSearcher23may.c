//#include <iostream>
//#include <time.h>   //for timespec and clock_gettime
using namespace std;

#include "buildFacade.h"

/* macro to detect and notify errors  */
#define IFERROR(error) {{if (error) { fprintf(stderr, "%s\n", error_index(error)); exit(1); }}}

#define LOGTIMES

#ifdef LOGTIMES  // file used to log the results of the searches.
/**/ FILE *timeFile;
	 FILE *gnuFile;  //
#endif

//#define DO_LOG

#ifdef DO_LOG  // file used to log the results of the searches.
/**/ FILE *logFile;
#endif

#define GET_TIME_DIVIDER ((double) 1.0 )	 //getTime deals with "seconds" as time unit.
#define GET_TIME_UNIT "sec"
#define MSEC_TIME_DIVIDER  ((double) 1000.0) //1 sec = 1000 msec
#define MSEC_TIME_UNIT 	  "msec"
#define SEC_TIME_DIVIDER  ((double) 1.0	)	 //1 sec = 1 sec
#define SEC_TIME_UNIT 	  "sec"



/*
#define GET_TIME_DIVIDER ((double)1000000.0)        //getTime deals with "microseconds" as time unit.
#define GET_TIME_UNIT "microsec"
#define MSEC_TIME_DIVIDER  ((double)0.001)	//1 microsec = 0.001 msec
#define MSEC_TIME_UNIT 	  "msec"
#define SEC_TIME_DIVIDER  ((double)0.000001)	//1 microsec = 0.000001 sec
#define SEC_TIME_UNIT 	  "sec"
*/


/**------------------------------------------------------------------ 
  *  MAIN PROGRAM: FOR QUERIING THE INDEX
  *------------------------------------------------------------------ */
#ifdef SEARCHERFACADEWITHMAIN
#define COUNT 		('C')
#define LOCATE 		('L')
#define DISPLAY 	('D')
#define TEST 	('T')

#define INT2 ('I')
#define INTN ('N')
#define EXTRACT ('E')

#define WITHREPETITIONS //------------------------
uint REPETITIONSCOUNT=1;
uint REPETITIONSLOCATE=1;
uint REPETITIONSDISPLAY = 1;

uint REPETITIONS_INTERSECT = 1;

/*********************************************************************/

double getTime (void)
{
	double usertime, systime;
	struct rusage usage;

	getrusage (RUSAGE_SELF, &usage);
	usertime = (double) usage.ru_utime.tv_sec * GET_TIME_DIVIDER +
		(double) usage.ru_utime.tv_usec / ( ((double) 1000000.0) / GET_TIME_DIVIDER);
	systime = (double) usage.ru_stime.tv_sec * GET_TIME_DIVIDER +
		(double) usage.ru_stime.tv_usec / ( ((double)1000000.0) / GET_TIME_DIVIDER);
	//return (usertime + systime);
	return (usertime );
}



/*
void usage(char * progname) {	
	fprintf(stderr, "\nThe program loads <index> and then executes over it the\n");
	fprintf(stderr, "queries it receives from the standard input. The standard\n");
	fprintf(stderr, "input comes in the format of the files written by \n");
	fprintf(stderr, "genpatterns.\n");
	fprintf(stderr, "%s reports on the standard error time (and an output .txt file)\n", progname);
	fprintf(stderr, "statistics regarding to running the queries.\n\n");
	fprintf(stderr, "Usage:  %s <index> <type> [opts]<repeats> [parameters] ]\n", progname);
	fprintf(stderr, "\n\t<type>   denotes the type of queries:\n");
	fprintf(stderr, "\t         %c <repeats> counting queries;\n", COUNT);
	fprintf(stderr, "\t         %c <repeats> locating queries;\n", LOCATE);
	fprintf(stderr, "\t         %c <radix-words> <repeats> displaying queries;\n", DISPLAY);
	fprintf(stderr, "\t         %c Test: checks locate and display from command-line;\n\n", TEST);
	fprintf(stderr, "\n\t[params] must be provided to choose search options on the index\n");
	fprintf(stderr, "\t         typically:  \"int2=svs;intn=svs;fsearch=expM\"  \n\n");
	fprintf(stderr, "\t example of use:	\n");
	fprintf(stderr,"\t%s CRindex C 1 \"int2=svs;intn=svs;fsearch=expM\" < pattscr.xt100k_4\n",progname);
	fprintf(stderr,"\t%s CRindex L 1 \"int2=svs;intn=svs;fsearch=expM\" < pattscr.xt100k_4\n",progname);
	fprintf(stderr,"\t%s CRindex L 1 \"int2=svs;intn=svs;fsearch=seqM \" < pattscr.xt100k_4  #(si usando rice)\n",progname);
	fprintf(stderr,"\t%s CRindex D 10 1 \"int2=svs;intn=svs;fsearch=expM\" < pattscr.xt100k_4\n",progname);
	fprintf(stderr,"\t%s CRindex T \"int2=svs;intn=svs;fsearch=expM\" \n\n",progname);
}
*/
void usage(char * progname) {	
	fprintf(stderr, "\nThe program loads <index> and then executes over it the\n");
	fprintf(stderr, "queries it receives from the standard input. The standard\n");
	fprintf(stderr, "input comes in the format of the files written by \n");
	fprintf(stderr, "genpatterns.\n");
	fprintf(stderr, "%s reports on the standard error time (and an output .txt file)\n", progname);
	fprintf(stderr, "statistics regarding to running the queries.\n\n");
	fprintf(stderr, "Usage:  %s <index> <type> [opts]<repeats> [parameters] ]\n", progname);
	fprintf(stderr, "\n\t<type>   denotes the type of queries:\n");
	fprintf(stderr, "\t         %c <repeats> EXTRACT/INTERSECT queries;\n", EXTRACT);
	fprintf(stderr, "\t         %c <repeats> EXTRACT/INTERSECT queries;\n", INT2);
	fprintf(stderr, "\t         %c <repeats> EXTRACT/INTERSECT queries;\n", INTN);
//	fprintf(stderr, "\t         %c <repeats> COUNT queries;\n", COUNT);
//	fprintf(stderr, "\t         %c <repeats> locating queries;\n", LOCATE);
//	fprintf(stderr, "\t         %c <radix-words> <repeats> displaying queries;\n", DISPLAY);
	fprintf(stderr, "\t         %c Test: checks locate and display from command-line;\n\n", TEST);
	fprintf(stderr, "\n\t[params] must be provided to choose search options on the index\n");
	fprintf(stderr, "\t         typically:  \"int2=svs;intn=svs;fsearch=seqM\"  \n\n");
	fprintf(stderr, "\t example of use:	\n");
//	fprintf(stderr,"\t%s CRindex C 1 \"int2=svs;intn=svs;fsearch=expM\" < pattscr.xt100k_4\n",progname);
//	fprintf(stderr,"\t%s CRindex L 1 \"int2=svs;intn=svs;fsearch=expM\" < pattscr.xt100k_4\n",progname);
//	fprintf(stderr,"\t%s CRindex L 1 \"int2=svs;intn=svs;fsearch=seqM \" < pattscr.xt100k_4  #(si usando rice)\n",progname);
//	fprintf(stderr,"\t%s CRindex D 10 1 \"int2=svs;intn=svs;fsearch=expM\" < pattscr.xt100k_4\n",progname);
	fprintf(stderr,"\t%s CRindex E 1 \"nooptionsRequiredForExtract\" < pattscr.xt100k_4\n",progname);
	fprintf(stderr,"\t%s CRindex I 1 \"int2=svs;intn=svs;fsearch=expM\" < pattscr.xt100k_4\n",progname);
	fprintf(stderr,"\t%s CRindex N 1 \"int2=svs;intn=svs;fsearch=expM\" < pattscr.xt100k_4\n",progname);
	fprintf(stderr,"\t%s CRindex T \"int2=svs;intn=svs;fsearch=expM\" \n\n",progname);
}


void readNumPatterns (uint *numPhrase, uint *numOfWords);
uchar *inputWord( uint *len);
void loadPatterns(uchar ***patterns,uint **lens, uint *numpatt, uint *numwordsperpattern);
void loadIds(void *Index, uint ***theids, uint **thelens, uchar **patts, uint *lens, uint *numpatt);
void free_ids(uint **ids, uint *nids, uint numpatt);

void do_test (void *Index);
void do_count (void *Index);
void do_locate  (void *Index);
void do_display (void *Index, uint numWords );
void do_extract_intersect (void *Index);

void alighnnum(char *str, int len);

static double Load_time;
static ulong Index_size, Text_length;
static long Index_sizect,Index_sizeil;

static char operation;
static char *indexbasename;


char *parseParams(int argc, char *argv[], int first) {
	char *params = NULL;

	int i;	
	int nchars, len;
	nchars = argc-2;
	for(i=1;i<argc;i++)
		nchars += strlen(argv[i]);
	params = (char *) malloc((nchars+1)*sizeof(char));
	params[nchars] = '\0';
	nchars = 0;
	for(i=first;i<argc;i++) {
		len = strlen(argv[i]);
		strncpy(params+nchars,argv[i],len);
		params[nchars+len] = ' ';
		nchars += len+1;
	}
	params[nchars-1] = '\0';
	fprintf(stderr,"\n parameters: ==>  %s <==", params);

  //if (i==first) {free(params);params=NULL;}
  return params;  
}
 

/**************************************/
int main(int argc, char* argv[])
{
	char *infile;	// baseName for the index files
	char query_type;
	void *Index;
	int error=0;
	
	printf("\n*Searching a block-addressing index");
	printf("\n*CopyRight (c) 2007 [LBD & G.N.]\n\n");

	// Reads input parameters from command line.
	if (argc < 3) {	
		usage(argv[0]);
		exit(0);
	}
//	if(argc < 2) {
//		printf("Use: %s <index basename> [<C|L|D|s|T> [words-snippet] ] [\"par1=x; par2=y;...\"] \n", argv[0]);
//		exit(0);
//	}

	/** prepares parameters for the index **/	
	
	char *params = NULL;

	params = parseParams(argc, argv, 3);
	fprintf(stderr,"\n parameters for intersections: ==>  \"%s\" <==", params); 	

	// Reads params (input file, query_type
	infile = argv[1];
	query_type = argv[2][0];	
	operation = query_type;		
	
	#ifdef DO_LOG
		logFile = fopen("log.log","w");
		if (!logFile) {printf("\n could not open log_times file (%s)... exitting\n","log.log"); exit(0);}	  
	#endif

	#ifdef LOGTIMES  
	/**/	char outFilename[256]="";	
	/**/	char *progname = argv[0];
	/**/	{int i ; for (i =strlen(argv[0])-2;i>0;i--) { if (argv[0][i] =='/') {progname = &(argv[0][i]);progname++; break; } } }
	/**/	{int i ; for (i =strlen(argv[1])-2;i>0;i--) { if (argv[1][i] =='/') {indexbasename = &(argv[1][i]);indexbasename++; break; } } }
	/**/	sprintf(outFilename, "times_%c_%s.txt", query_type, progname);
	/**/	timeFile = fopen(outFilename,"a");
	/**/	if (!timeFile) {printf("\n could not open log_times file (%s)... exitting\n",outFilename); exit(0);}
	/**/	fprintf(timeFile,"\n Search: %s %s: %s\n",argv[0],argv[1], params);
	/**/
	/**/	char outFilenameGNU[256];	
	/**/	sprintf(outFilenameGNU,"%c.dat",query_type);
	/**/	struct stat bufgnu;
	/**/	if (-1 == stat(outFilenameGNU, &bufgnu)){
	/**/		gnuFile =fopen(outFilenameGNU,"w");
	/**/		if (!gnuFile) {printf("\n could not open GNUPLOT data file (%s)... exitting\n",outFilenameGNU); exit(0);}
	/**/		fprintf(gnuFile,"#%15s\t%15s\t%15s\t%15s\t%15s\t%s\n","sizeIlist(byte)","msec/pat","usec/occ","occs","numpats","#indexBaseName");
	/**/	}
	/**/	else {
	/**/		gnuFile = fopen(outFilenameGNU,"a");
	/**/		if (!gnuFile) {printf("\n could not open GNUPLOT data file (%s)... exitting\n",outFilenameGNU); exit(0);}
	/**/	}
	#endif	

	//**loading the index */	
	Load_time = getTime ();	
	error = load_index (infile, &Index);
	Load_time = getTime () - Load_time;
	IFERROR (error);
	
	/** shows the size of the index **/	
	error = index_size(Index, &Index_size);
	IFERROR (error);
	
	/** Returns the size of the representation of the inverted lists **/
	error = index_getsize_il(Index, &Index_sizeil);
	IFERROR (error);
	
	/** Returns the size of the representation of the compressed text **/
	error = index_getsize_ct(Index, &Index_sizect);
	IFERROR (error);
	
	
	/** shows the size of the original indexed text **/	
	error = get_length(Index, &Text_length);
	IFERROR (error);

	fprintf (stderr, "Load index time = %.6f %s\n", Load_time* SEC_TIME_DIVIDER, SEC_TIME_UNIT);	
	fprintf(stderr, "Index size = %lu bytes = (%lu Kbytes), source size = %lu bytes:: ratio = %2.3f%%\n", Index_size, (unsigned long) Index_size/1024,Text_length, Index_size*1.0/Text_length*100.0 );

	#ifdef LOGTIMES 

	/**/	fprintf(timeFile, "\tLoad index time = %.6f %s\n", Load_time* SEC_TIME_DIVIDER, SEC_TIME_UNIT);	
	/**/	fprintf(timeFile, "\tIndex size = %lu bytes = (%lu Kbytes)\n\t source size = %lu bytes ==> ratio = %2.3f%%\n", Index_size,Index_size/1024,Text_length, Index_size*1.0/Text_length*100.0 );	
	#endif 
					
	//error = setDefaultSearchOptions_il (Index, params);
	error = setDefaultIntersectionOptions(Index, params);
	IFERRORIL (error);
	free(params);
	
//	if(argc == 2) {
//		do_test(Index);
//	}
//	else 
	{		
		switch (query_type){
/*			case COUNT:		
				if (argc < 5) {
					printf("At least %d params are needed, you have given only %d\n",5,argc);
					printf("Use: %s <index basename> <C> <repeats> [params for search] \n", argv[0]);						
					//usage(argv[0]);
					exit (1);
				}			
				REPETITIONSCOUNT= atoi(argv[3]);
				do_count(Index);
				break;
			case LOCATE:
				if (argc < 5) {
					printf("At least %d params are needed, you have given only %d\n",5,argc);
					printf("Use: %s <index basename> <L> <repeats> [params for search] \n", argv[0]);						
					//usage(argv[0]);
					exit (1);
				}			
				REPETITIONSLOCATE= atoi(argv[3]);
				do_locate(Index);
				break;	
			case DISPLAY:
				if (argc < 6) {
					printf("At least %d params are needed, you have given only %d\n",6,argc);
					printf("Use: %s <index basename> <D> <#words-radix> <repeats>  [params for search] \n", argv[0]);	
					exit (1);
				}
				REPETITIONSDISPLAY = atoi(argv[4]);
				do_display(Index,(uint) atoi(argv[3]));
				break;
*/
			case EXTRACT:
			case INT2:
			case INTN:
				if (argc < 5) {
					printf("At least %d params are needed, you have given only %d\n",5,argc);
					printf("Use: %s <index basename> <%C> <repeats> [params for search] \n", argv[0],query_type);						
					//usage(argv[0]);
					exit (1);
				}			
				REPETITIONS_INTERSECT= atoi(argv[3]);
				do_extract_intersect(Index); 
				break;


			case TEST:
				if (argc < 4) {
					printf("Use: %s <index basename> <T> [params for search] \n", argv[0]);	
					usage(argv[0]);
					exit (1);
				}					
				do_test(Index);
				break;	
								
			default:
				fprintf (stderr, "Unknow option: main run\n");
				exit (1);
		}	
	}	
				
	free_index(Index);	

	#ifdef LOGTIMES  
	/**/	fclose(timeFile);
	/**/	fclose(gnuFile);
	#endif	
	#ifdef DO_LOG
		fclose (logFile);
	#endif	
}

/*********************************/
void do_test(void *Index) {
	// SEARCHING FOR A TEXT PATTERN (word/phrase).
	{unsigned char textPattern[MAX_TEXT_PATTERN_SIZE];
	 uint occ;
	 int len;
	 uint *occs;
	 uint i;
	 printf("\nSEARCH TEST for LOCATE\n");
		while(1) {	
			printf("Intro string: ");
			if (fgets((char*)textPattern, MAX_TEXT_PATTERN_SIZE, stdin) ==NULL) ;// {printf("\n unable to load patterns\n exitting... \n\n");exit(0);}
			
			len = strlen((char*)textPattern);
			if (!strcmp((char*)textPattern,"\n") ) break;
			textPattern[len-1] = '\0';
			len --;
			
			locate(Index, textPattern, len, &occs, &occ);
			
			printf("\n*** %s occurs %d times: In the source text in positions:\n\t",textPattern,occ);
			for (i=0;i<occ;i++) 
				printf("[%u]",occs[i]);
			fflush(stderr);	
			if (occs) free(occs);		
						
		if (!strcmp((char*)textPattern,"\n") ) break;
		}
	}	
	
			
	// DISPLAYING THE OCCURRENCES OF A TEXT PATTERN (word/phrase).
	{unsigned char textPattern[MAX_TEXT_PATTERN_SIZE];
	 int occ;
	 printf("\nSEARCH TEST for DISPLAY (non pizzachili)\n");
		while(1) {	
			printf("Intro string: ");
			if (fgets((char*)textPattern, MAX_TEXT_PATTERN_SIZE, stdin) ==NULL) ;
			
			if (!strcmp((char*)textPattern,"\n") ) break;
			textPattern[strlen((char*)textPattern)-1] = '\0';
			occ = displayTextOcurrences(Index,textPattern,strlen((char*)textPattern),8);
			printf("Ocurrences = %d\n", occ);
			if (!strcmp((char*)textPattern,"\n") ) break;
		}
	}		
}



/*************************************************/
void
do_count (void *Index)
{
	int error = 0;
	uint numocc, length;
	ulong tot_numocc=0;
	
	double time, tot_time = 0;
	
	uint i;

	uchar *pattern;
	uint numpatt,nwordsperpattern; 
	uchar ** patterns;
	uint *lens;
	
	loadPatterns(&patterns, &lens, &numpatt, &nwordsperpattern);
	
	fflush(stdout);fflush(stderr);
	fprintf (stderr,"\nStarting count over %u patterns:",numpatt);
	#ifdef WITHREPETITIONS
	fprintf(stderr," ** count: repetitions = %u\n",REPETITIONSCOUNT);
	#endif	
	
	time = getTime ();
	#ifdef WITHREPETITIONS
	uint j;
	for (j=0;j<REPETITIONSCOUNT;j++) {
		//fprintf(stderr,"\n iter %d of %d",j,REPETITIONSLOCATE);
	#endif
	  tot_numocc=0;
	  for (i=0; i<numpatt; i++) {	
		  pattern=patterns[i];
		  length = lens[i];			
		  //printf("\n iter %4d of %d [pattern = %s]",i,numpatt,pattern);
		  //printf("\b\b\b\b%4d",i);
		  /* Count */
		  error =	count (Index, pattern, length, &numocc);
		  //printf("\n iter %4lu of %4lu , occs=%6lu,  <<pattern = %s>>",i,numpatt,numocc,pattern);
		  tot_numocc += numocc;		
	  }
	  
   #ifdef WITHREPETITIONS
	fprintf(stderr,"#");
	}
   #endif
	tot_time += (getTime () - time);
	tot_time /= (double) REPETITIONSCOUNT;

	fprintf(stderr,"\n");
	fflush(stdout);fflush(stderr);
	//free patterns and lens arrays.
	for (i=0;i<numpatt; i++) {		
		free(patterns[i]);   
	}
	free(patterns);
	free(lens);

	//Show stats.
	fprintf (stderr, "Total Num occs found = %lu\n", tot_numocc);
	fprintf (stderr, "Count time = %.6f %s\n", (tot_time * SEC_TIME_DIVIDER) ,SEC_TIME_UNIT);
	fprintf (stderr, "Count_time/Num_occs = %.6f %s/occss\n", (tot_time * MSEC_TIME_DIVIDER) / tot_numocc, MSEC_TIME_UNIT);
	fprintf (stderr, "(Load_time+Count_time)/Num_occs = %.6f %s/occs\n\n", ((tot_time+Load_time) * MSEC_TIME_DIVIDER) / tot_numocc , MSEC_TIME_UNIT);

	#ifdef LOGTIMES
	/**/	fprintf (timeFile, "\t** Operation = %c ** numpatts = %u, nwordsperpattern=%u\n", operation, numpatt,nwordsperpattern);;
	/**/	fprintf (timeFile, "\tTotal Num occs found = %lu\n", tot_numocc);
	/**/	fprintf (timeFile, "\tCount time = %.6f %s\n", (tot_time * SEC_TIME_DIVIDER) ,SEC_TIME_UNIT);
	/**/	fprintf (timeFile, "\tCount_time/Num_occs = %.6f %s/occss\n", (tot_time * MSEC_TIME_DIVIDER) / tot_numocc, MSEC_TIME_UNIT);
	/**/	fprintf (timeFile, "\t\t(Load_time+Count_time)/Num_occs = %.6f  %s/occs\n\n", ((tot_time+Load_time) * MSEC_TIME_DIVIDER) / tot_numocc , MSEC_TIME_UNIT);		
	#endif

}

/*************************************************/
/*************************************************/
void
do_locate (void *Index)
{
	int error = 0;
	uint numocc, length, *occ;
	ulong tot_numocc=0;
	
	double time, tot_time = 0;
	
	uint i;

	uchar *pattern;
	uint numpatt,nwordsperpattern; 
	uchar ** patterns;
	uint *lens;
	
	loadPatterns(&patterns, &lens, &numpatt, &nwordsperpattern);
	//uint **ids; uint *nids; //patterns converted into ids;
	//loadIds(Index, &ids,&nids, patterns, lens, numpatt);
	

	fflush(stdout);fflush(stderr);
	fprintf (stderr,"\nStarting locate over %u patterns:",numpatt);
	#ifdef WITHREPETITIONS
	fprintf(stderr," ** locate: repetitions = %d\n",REPETITIONSLOCATE);
	#endif	
	
	
	time = getTime ();

	#ifdef WITHREPETITIONS
	uint j;
	for (j=0;j<REPETITIONSLOCATE;j++) {
		//fprintf(stderr,"\n iter %d of %d",j,REPETITIONSLOCATE);
	#endif
	  tot_numocc=0;
	  for (i=0; i<numpatt; i++) {	
		  pattern=patterns[i];
		  length = lens[i];			
		  //printf("\n iter %4d of %d [pattern = %s, len = %d bytes]",i,numpatt,pattern,length); fflush(stdout);

		  
		 //uint z;
		 //fprintf(stderr,"\n pattern = %s, IDS ANTES LOCATE=",pattern);
		 //	  for (z=0;z<nids[i];z++)
		 //		fprintf(stderr,"[%u]",ids[i][z]);
		 
		  /* Locate */
		  error =	locate (Index, pattern, length, &occ, &numocc);
		  //printf("\n iter %4lu of %4lu , occs=%6lu,  <<pattern = %s>>",i,numpatt,numocc,pattern);
	  		  
		  tot_numocc += numocc;
		  
		  if (occ) free (occ);	
		  fprintf(stderr,"@"); 			
	  }
	  
   #ifdef WITHREPETITIONS
	fprintf(stderr,"#");   
	}
   #endif
	tot_time += (getTime () - time);
	tot_time /= (double) REPETITIONSLOCATE;

	fflush(stdout);fflush(stderr);
	//free patterns and lens arrays.
	for (i=0;i<numpatt; i++) {		
		free(patterns[i]);   
	}
	free(patterns);
	free(lens);
	//free_ids (ids, nids, numpatt);

	//Show stats.
	fprintf (stderr, "Total Num occs found = %lu\n", tot_numocc);
	fprintf (stderr, "Locate time = %.6f %s\n", (tot_time * SEC_TIME_DIVIDER) ,SEC_TIME_UNIT);
	fprintf (stderr, "Locate_time/Num_occs = %.6f %s/occss\n", (tot_time * MSEC_TIME_DIVIDER) / tot_numocc, MSEC_TIME_UNIT);
	fprintf (stderr, "(Load_time+Locate_time)/Num_occs = %.6f %s/occs\n\n", ((tot_time+Load_time) * MSEC_TIME_DIVIDER) / tot_numocc , MSEC_TIME_UNIT);

	#ifdef LOGTIMES
	/**/	fprintf (timeFile, "\t** Operation = %c ** numpatts = %u, nwordsperpattern=%u\n", operation, numpatt,nwordsperpattern);
	/**/	fprintf (timeFile, "\tTotal Num occs found = %lu\n", tot_numocc);
	/**/	fprintf (timeFile, "\tLocate time = %.6f %s\n", (tot_time * SEC_TIME_DIVIDER) ,SEC_TIME_UNIT);
	/**/	fprintf (timeFile, "\tLocate_time/Num_occs = %.6f %s/occss\n", (tot_time * MSEC_TIME_DIVIDER) / tot_numocc, MSEC_TIME_UNIT);
	/**/	fprintf (timeFile, "\t\t(Load_time+Locate_time)/Num_occs = %.6f  %s/occs\n\n", ((tot_time+Load_time) * MSEC_TIME_DIVIDER) / tot_numocc , MSEC_TIME_UNIT);		
	#endif

}

/*************************************************/
void do_display(void *Index, uint radixNumWords) {
	
	ulong Noccs=0; ulong numoccs=0;
	uint  length, i;
	ulong tot_numcharext=0;
	double time, tot_time = 0;
	
	uchar *pattern;
	uint numpatt, nwordsperpattern; 
	uchar ** patterns;
	uint *lens;

	loadPatterns(&patterns, &lens, &numpatt,&nwordsperpattern);
	fflush(stdout);fflush(stderr);
	fprintf (stderr,"\nStarting display search over %u patterns, and radix = %u words",numpatt,radixNumWords);

	#ifdef WITHREPETITIONS
	fprintf(stderr,"\n** display-words: repetitions = %d\n",REPETITIONSDISPLAY );
	#endif	
		
	time = getTime ();

	#ifdef WITHREPETITIONS
	uint j;
	for (j=0;j<REPETITIONSDISPLAY;j++) {
	#endif	
	tot_numcharext=numoccs=0;	
	  for (i=0; i<numpatt; i++) {
		  //printf("\n iter %3lu of %3lu",i,numpatt);	
		  pattern=patterns[i];
		  length = lens[i];
		  
		  /* Display */
		  tot_numcharext += displayTextOcurrencesNoShow(Index, pattern, (ulong)length, radixNumWords, &Noccs);
		  numoccs += Noccs;
	  }
   #ifdef WITHREPETITIONS
	fprintf(stderr,"#");   
	}
   #endif
   
	tot_time = (getTime () - time);
	tot_time /= (double) REPETITIONSDISPLAY;
	
	fflush(stdout);fflush(stderr);		
	for (i=0;i<numpatt; i++) {		
		free(patterns[i]);   
	}
	free(patterns);
	free(lens);

	fflush(stderr);fflush(stdout);
	fprintf (stderr, "\nTotal num chars extracted = %lu, numoccs = %lu\n", tot_numcharext,numoccs);
	fprintf (stderr, "Display time = %.6f %s\n", (tot_time * SEC_TIME_DIVIDER) ,SEC_TIME_UNIT);
	fprintf (stderr, "Time_display/Num_occs      = %.8f %s/occss\n", (tot_time * MSEC_TIME_DIVIDER) / numoccs, MSEC_TIME_UNIT);
	fprintf (stderr, "\tDisplay_time/Tot_num_chars = %.6f %s/chars\n\n", (tot_time*MSEC_TIME_DIVIDER) / tot_numcharext, MSEC_TIME_UNIT);
	
		//fprintf (stderr, "(Load_time+Time_display)/Tot_num_chars = %.4f msecs/chars\n\n", ((Load_time+tot_time)*MSEC_TIME_DIVIDER) / tot_numcharext);


	#ifdef LOGTIMES
	/**/	fprintf (timeFile, "\n\t** Operation = %c ** numpatts = %u, nwordsperpattern=%u\n", operation, numpatt,nwordsperpattern);;
	
	/**/	fprintf (timeFile, "\tTotal num chars extracted = %lu, numoccs = %lu\n", tot_numcharext,numoccs);
	/**/	fprintf (timeFile, "\tDisplay time = %.6f %s\n", (tot_time * SEC_TIME_DIVIDER) ,SEC_TIME_UNIT);
	/**/	fprintf (timeFile, "\tDisplay_time/Num_occs  = %.8f %s/occss\n", (tot_time * MSEC_TIME_DIVIDER) / numoccs, MSEC_TIME_UNIT);
	/**/	fprintf (timeFile, "\tDisplay_time/Tot_num_chars = %.6f %s/chars\n", (tot_time*MSEC_TIME_DIVIDER) / tot_numcharext , MSEC_TIME_UNIT);
	/**/	fprintf (timeFile, "\t\t(Load_time+Locate_time)/Tot_num_chars = %.6f %s/chars\n\n", ((tot_time+Load_time) * MSEC_TIME_DIVIDER)  / tot_numcharext  , MSEC_TIME_UNIT);
	
		
	#endif		
}






/*************************************************/
/*************************************************/
void
do_extract_intersect (void *Index)
{
	int error = 0;
	uint numocc, *occ;
	ulong tot_numocc=0;
	
	double atime, tot_time = 0;
	
	uint i;

	//uchar *pattern;
	uint numpatt,nwordsperpattern; 
	uchar ** patterns;
	uint *lens;
	
	loadPatterns(&patterns, &lens, &numpatt, &nwordsperpattern);
	uint **ids; uint *nids; //patterns converted into ids;
	loadIds(Index, &ids,&nids, patterns, lens, &numpatt);
	
	uint minnw = nwordsperpattern;
	uint maxnw = nwordsperpattern;
	
	minnw/=10000000;
	maxnw%=10000000;
	maxnw/=1000;
	//we have phrases with a number of words between: minnw and maxnw
		

	fflush(stdout);fflush(stderr);
	fprintf (stderr,"\nStarting Extract/Intersection over %u patterns:",numpatt);
	fprintf (stderr,"\n\t patterns contain [from %u to %u] words:",minnw,maxnw);
	#ifdef WITHREPETITIONS
	fprintf(stderr," ** extract/intersect: repetitions = %d\n",REPETITIONS_INTERSECT);
	#endif	
		
	atime = getTime ();

	#ifdef WITHREPETITIONS
	uint j;
	for (j=0;j<REPETITIONS_INTERSECT;j++) {
		//fprintf(stderr,"\n iter %d of %d",j,REPETITIONS_INTERSECT);
	#endif
	  tot_numocc=0;
	  for (i=0; i<numpatt; i++) {	
		  //pattern=patterns[i];
		  //length = lens[i];			
		  //printf("\n iter %4d of %d [pattern = %s, len = %d bytes]",i,numpatt,pattern,length); fflush(stdout);

  		 /* {uint *ids, nids;
			  index_parseTextIntoIntegers(Index,pattern,length,&ids,&nids);			
			  int z;
			  fprintf(stderr,"\n pattern = %s, IDS ANTES LOCATE=",pattern);
			  for (z=0;z<nids;z++)
				fprintf(stderr,"[%u]",ids[z]);
		  }
		  */ 

		/* {
		 uint z;
		 fprintf(stderr,"\n pattern = %s, IDS TO EXTRACT/INTERSECT=",pattern);
			  for (z=0;z<nids[i];z++)
				fprintf(stderr,"[%u]",ids[i][z]);
		 }
		 */
		  /* Extract /intersect */
		  error = index_listDocuments(Index, ids[i], nids[i], &occ, &numocc);
		  //printf("\n iter %4lu of %4lu , occs=%6lu,  <<pattern = %s>>",i,numpatt,numocc,pattern);
	  		  
		  tot_numocc += numocc;

		  #ifdef DO_LOG
			fprintf(logFile,"\n iter = %u, occs= %u ",i,numocc);
			uint z;
			for (z=0;z<numocc;z++) fprintf(logFile,"\n %u --> id=%u",z,occ[z]);
			//exit(0);
		  #endif		  
		  
		  if (occ) free (occ);	
		  //fprintf(stderr,"@%u@",i); 			
		  fprintf(stderr,"@"); 			
		  

	  }
	  
   #ifdef WITHREPETITIONS
	fprintf(stderr,"#iter %d of %d finishes# ",j+1,REPETITIONS_INTERSECT);   
	}
   #endif
	tot_time += (getTime () - atime);
	tot_time /= (double) REPETITIONS_INTERSECT;		
	
	fflush(stdout);fflush(stderr);
	//free patterns and lens arrays.
	for (i=0;i<numpatt; i++) {		
		free(patterns[i]);   
	}
	free(patterns);
	free(lens);
	free_ids (ids, nids, numpatt);
	

	//Show stats.
	fprintf (stderr, "**size_Index= %lu bytes, size_Inv-lists= %ld bytes, size_compText= %ld bytes\n", Index_size, Index_sizeil, Index_sizect);
	fprintf (stderr, "Total Num occs found = %lu\n", tot_numocc);
	fprintf (stderr, "Extract:Intersect time = %.8f %s (numpatts = %u)\n", (tot_time * SEC_TIME_DIVIDER) ,SEC_TIME_UNIT, numpatt);
	fprintf (stderr, "Extract:Intersect_time/Num_occs = %.8f %s/occss\n", (tot_time * MSEC_TIME_DIVIDER) / tot_numocc, MSEC_TIME_UNIT);
	fprintf (stderr, "Extract:Intersect_time/pat = %.8f %s/pat\n", (tot_time * MSEC_TIME_DIVIDER) / numpatt, MSEC_TIME_UNIT);
	fprintf (stderr, "(Load_time+Extract:Intersect_time)/Num_occs = %.8f %s/occs\n\n", ((tot_time+Load_time) * MSEC_TIME_DIVIDER) / tot_numocc , MSEC_TIME_UNIT);

	char opname[25];
	if ((minnw==maxnw) && (minnw ==1)) {
		operation = 'E'; sprintf(opname,"%s","Extract");
	}
	else if ((minnw==maxnw) && (minnw ==2)) {
  		operation = 'I'; sprintf(opname,"%s","Int-2");
	}
	else {
   		operation = 'N'; sprintf(opname,"%s","Int-N");
	}


	#ifdef LOGTIMES
	/**/	fprintf (timeFile, "\t** Operation = %c ** numpatts = %u, wordsXpattern=%u-%u\n", operation, numpatt,minnw,maxnw);
	/**/	fprintf (timeFile, "\t**size_Index= %lu bytes, size_Inv-lists= %ld bytes, size_compText= %ld bytes\n", Index_size, Index_sizeil, Index_sizect);
	/**/	fprintf (timeFile, "\tTotal Num occs found = %lu\n", tot_numocc);
	/**/	fprintf (timeFile, "\tExtract:Intersect:Intersect time = %.6f %s\n", (tot_time * SEC_TIME_DIVIDER) ,SEC_TIME_UNIT);
	/**/	fprintf (timeFile, "\tExtract:Intersect/Num_occs = %.6f %s/occss\n", (tot_time * MSEC_TIME_DIVIDER) / tot_numocc, MSEC_TIME_UNIT);
	/**/	fprintf (timeFile, "\tExtract:Intersect_time/pat = %.8f %s/pat\n", (tot_time * MSEC_TIME_DIVIDER) / numpatt, MSEC_TIME_UNIT);
	/**/	fprintf (timeFile, "\t\t(Load_time+Extract:Intersect)/Num_occs = %.6f  %s/occs\n\n", ((tot_time+Load_time) * MSEC_TIME_DIVIDER) / tot_numocc , MSEC_TIME_UNIT);		

	/**/    char timeperpat[50]=""; sprintf(timeperpat,"%5.9f", (tot_time * MSEC_TIME_DIVIDER) / numpatt);//, MSEC_TIME_UNIT);
	/**/    char timeperocc[50]=""; sprintf(timeperocc,"%5.9f", (tot_time * MSEC_TIME_DIVIDER *1000) / tot_numocc);//, MSEC_TIME_UNIT);  // TIMES IN usec/occ
	/**/	alighnnum(timeperpat,15);alighnnum(timeperocc,15);
	/**/	fprintf(gnuFile," %15ld\t",Index_sizeil);
	/**/	fprintf(gnuFile,"%15s\t",timeperpat);
	/**/	fprintf(gnuFile,"%15s\t",timeperocc);
	/**/	fprintf(gnuFile,"%15lu\t",tot_numocc);
	/**/	fprintf(gnuFile,"%15u\t",numpatt);
	/**/	fprintf(gnuFile,"#%s\n",indexbasename);
	
	#endif

}

/*************************************************/
void readNumPatterns (uint *numPhrase, uint *numOfWords) {
	int errF= fscanf(stdin,"#numberOfFrases::%9d, #NumberOfWordsPerPhrase:: %9d\n",numPhrase, numOfWords);
	errF=0;
	//fprintf(stderr," numOfFrase = %9d and NumberOfnwordsPerPhrase = %9d",*numPhrase, *numOfWords);
}

/*************************************************/	
uchar *inputWord( uint *len) {
	uint tmp; //skips one integer.
	uchar *phrase;
	
	int errF= fscanf(stdin,"%9d %9d ", &tmp, len);	
	phrase = (uchar * ) malloc (sizeof(char) * ((*len)+1));
	errF = fread(phrase,sizeof(uchar), *len,stdin);
	phrase[*len] = '\0';
	//fscanf(stdin,"\n");
	
	return phrase;	
}

/*************************************************/
void loadPatterns(uchar ***patterns,uint **lens, uint *numpatt, uint *numwordsperpattern) {
	//uchar *pattern;
	uint i;
	readNumPatterns(numpatt,numwordsperpattern);  //number of patterns and number of words per phrase.
	
	
	uchar **pats;
	uint length, *thelens;
	pats = (uchar **) malloc (sizeof(uchar *) * (*numpatt));
	thelens = (uint *) malloc (sizeof(uint) * (*numpatt));
	
	for (i=0;i< (*numpatt);i++) {
		pats[i] = inputWord(&length);
		thelens[i]= length;
	}
	*patterns = pats;
	*lens=thelens; 
}



void loadIds(void *Index, uint ***theids, uint **thelens, uchar **patts, uint *lens, uint *numpatt) {
  uint **vids   = (uint **) malloc (sizeof(uint*) * (*numpatt));
  uint *lensids = (uint * ) malloc (sizeof(uint ) * (*numpatt));

  uint i;
  uint nids; uint *ids; //per pattern
  uint z=0;
  for (i=0;i< ((*numpatt));i++) {
	index_parseTextIntoIntegers(Index,patts[i],lens[i],&ids,&nids);
	if (!nids) continue;

	vids[z] = (uint *) malloc ( nids * sizeof(uint));
	uint j;
	for (j=0;j<nids;j++)
	  vids[z][j] = ids[j];
	
	lensids[z] = nids;
	z++;
  }
  *numpatt=z;
  *theids = vids;
  *thelens = lensids;  
}
	
void free_ids(uint **ids, uint *nids, uint numpatt) {
  uint i;
  for (i=0; i< numpatt ; i++) {
	free(ids[i]);
  }
  free(ids);
  free(nids);
}
	

/* used for gnuplot formatting file */
void alighnnum(char *str, int len){
	int n=strlen(str);
	int i;
	if (n<len) {
	  int shift = len-n;
	  for (i=len-1; i>=shift;i--)
		 str[i] = str[i-shift];
	  for (i=0;i<shift;i++) str[i]=' ';
	  
//for (i=0;i< len-n;i++)
//		str[i+n]=' ';
//
//	  for (i=0;i< len-n;i++)
//		str[i+n]=' ';
//	  str[len]='\0';
	}
}

	
	
#endif
