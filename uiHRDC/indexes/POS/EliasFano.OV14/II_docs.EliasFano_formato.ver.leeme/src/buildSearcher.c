//#define DO_LOG_OUTPUT


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

#define MEASURE_EXTRACT_TIME ('M')
#define MEASURE_EXTRACT_TIME_INTERVALS ('m')

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
	fprintf(stderr, "\t         %c <repeats> Extract (locate of words) queries;\n", EXTRACT);
	fprintf(stderr, "\t         %c <repeats> Int2 (locate of 2wordphrases) queries;\n", INT2);
	fprintf(stderr, "\t         %c <repeats> IntN (locate of phrases) queries;\n", INTN);
	fprintf(stderr, "\t         %c MEASURE_AVG EXTRACT_TIME operation;\n", MEASURE_EXTRACT_TIME);
	fprintf(stderr, "\t         %c MEASURE_EXTRACT_TIME_INTERVALS EXTRACT_TIME operation;\n", MEASURE_EXTRACT_TIME_INTERVALS);
	
	
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
	fprintf(stderr,"\t%s CRindex M  \"no opts\" \n",progname);
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
void do_Measure_Extract_time(void *Index);
void do_Measure_Extract_time_intervals(void *Index);

void alighnnum(char *str, uint len);
int int_rand_range(uint  a,uint b);
void makeRandom (uint *v, uint n);

static double Load_time;
static ulong Index_size, Text_length;
static uint Index_sizect,Index_sizeil;

static char operation;
static char *indexbasename;


/* obtains  the ending filename of a path */
void findFileName(char **dst,char *src){
  int i = strlen(src)-1;
  while (i>0) {  //finds the last '/'
	if (src[i] =='/') {break;}
	i--;
  }
  if (src[i] =='/') i++;
  *dst = &(src[i]);
}

char *parseParams(int argc, char *argv[], int first) {
	char *params = NULL;

	uint i;	
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
	/**/	//{int i ; for (i =strlen(argv[0])-2;i>0;i--) { if (argv[0][i] =='/') {progname = &(argv[0][i]);progname++; break; } } }
	/**/    findFileName(&progname,argv[0]);
	/**/	//{int i ; for (i =strlen(argv[1])-2;i>0;i--) { if (argv[1][i] =='/') {indexbasename = &(argv[1][i]);indexbasename++; break; } } }
	/**/	findFileName(&indexbasename,argv[1]);
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
				if ((query_type != MEASURE_EXTRACT_TIME) && (query_type != MEASURE_EXTRACT_TIME_INTERVALS)) {
	/**/			fprintf(gnuFile,"#%15s\t%15s\t%15s\t%15s\t%15s\t%15s\t%15s\t%s\n","totaSize(byte)","msec/pat","usec/occ","occs","numpats", "sizeCT(byte)", "sizeIlist(byte)","#indexBaseName");
	 				fflush(gnuFile);
				}
				else {
					if (query_type == MEASURE_EXTRACT_TIME)	{
		/**/		//	fprintf(gnuFile,"#%15s\t%15s\t%15s\t%15s\t%15s\t%s\n","sizeIlist(byte)","msec/doc","usec/char","chars","ndocs","#indexBaseName");
					  fprintf(gnuFile,"#%15s\t%10s\t%15s\t%15s\t%15s\t%15s\t%15s\t%15s\t%s\n","totaSize(byte)", "ratio (%)","msec/doc","usec/char","chars","ndocs", "sizeCT(byte)", "sizeIlist(byte)", "#indexBaseName");
					  fflush(gnuFile);
				  }
				  else {  //query_type != MEASURE_EXTRACT_TIME_INTERVALS)
					  fprintf(gnuFile,"#%15s\t%10s\t%15s\t%15s\t%15s\t%15s\t%15s\t%15s\t%s\n","totaSize(byte)", "ratio (%)","msec/interval","usec/char","chars","intervals", "sizeCT(byte)", "sizeIlist(byte)", "#indexBaseName");		
					  fflush(gnuFile); 
				  }
				}
				fprintf(stderr,"\n a new file GNU-DATA (%s) will be created",outFilenameGNU);
	/**/	}
	/**/	else {
				fprintf(stderr,"\n a the file GNU-DATA (%s) is open for appending data",outFilenameGNU);
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

			case MEASURE_EXTRACT_TIME:		
				if (argc < 4) {
					printf("Use: %s <index basename> <M> [params for search] \n", argv[0]);	
					usage(argv[0]);
					exit (1);
				}					
				do_Measure_Extract_time(Index);
				break;					

			case MEASURE_EXTRACT_TIME_INTERVALS:		
				if (argc < 4) {
					printf("Use: %s <index basename> <m> [params for search] \n", argv[0]);	
					usage(argv[0]);
					exit (1);
				}					
				do_Measure_Extract_time_intervals(Index);
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
	 int i;
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
/*  
	int error = 0;
	uint numocc, length;
	ulong tot_numocc;
	
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
	int j;
	for (j=0;j<REPETITIONSCOUNT;j++) {
		//fprintf(stderr,"\n iter %d of %d",j,REPETITIONSLOCATE);
	#endif
	  tot_numocc=0;
	  for (i=0; i<numpatt; i++) {	
		  pattern=patterns[i];
		  length = lens[i];			
		  //printf("\n iter %4d of %d [pattern = %s]",i,numpatt,pattern);
		  //printf("\b\b\b\b%4d",i);
		  // * Count * //
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
		fprintf (timeFile, "\t** Operation = %c ** numpatts = %u, nwordsperpattern=%u\n", operation, numpatt,nwordsperpattern);;
		fprintf (timeFile, "\tTotal Num occs found = %lu\n", tot_numocc);
		fprintf (timeFile, "\tCount time = %.6f %s\n", (tot_time * SEC_TIME_DIVIDER) ,SEC_TIME_UNIT);
		fprintf (timeFile, "\tCount_time/Num_occs = %.6f %s/occss\n", (tot_time * MSEC_TIME_DIVIDER) / tot_numocc, MSEC_TIME_UNIT);
		fprintf (timeFile, "\t\t(Load_time+Count_time)/Num_occs = %.6f  %s/occs\n\n", ((tot_time+Load_time) * MSEC_TIME_DIVIDER) / tot_numocc , MSEC_TIME_UNIT);		
	#endif
*/

}

/*************************************************/
/*************************************************/
void
do_locate (void *Index)
{
/*
	int error = 0;
	uint numocc, length, *occ;
	ulong tot_numocc;
	
	double time, tot_time = 0;
	
	uint i;

	uchar *pattern;
	uint numpatt,nwordsperpattern; 
	uchar ** patterns;
	uint *lens;
	
	loadPatterns(&patterns, &lens, &numpatt, &nwordsperpattern);
	uint **ids; uint *nids; //patterns converted into ids;
	//loadIds(Index, &ids,&nids, patterns, lens, numpatt);
	

	fflush(stdout);fflush(stderr);
	fprintf (stderr,"\nStarting locate over %u patterns:",numpatt);
	#ifdef WITHREPETITIONS
	fprintf(stderr," ** locate: repetitions = %d\n",REPETITIONSLOCATE);
	#endif	
	
	
	time = getTime ();

	#ifdef WITHREPETITIONS
	int j;
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
		 
		  // * Locate * //
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
		fprintf (timeFile, "\t** Operation = %c ** numpatts = %u, nwordsperpattern=%u\n", operation, numpatt,nwordsperpattern);
		fprintf (timeFile, "\tTotal Num occs found = %lu\n", tot_numocc);
		fprintf (timeFile, "\tLocate time = %.6f %s\n", (tot_time * SEC_TIME_DIVIDER) ,SEC_TIME_UNIT);
		fprintf (timeFile, "\tLocate_time/Num_occs = %.6f %s/occss\n", (tot_time * MSEC_TIME_DIVIDER) / tot_numocc, MSEC_TIME_UNIT);
		fprintf (timeFile, "\t\t(Load_time+Locate_time)/Num_occs = %.6f  %s/occs\n\n", ((tot_time+Load_time) * MSEC_TIME_DIVIDER) / tot_numocc , MSEC_TIME_UNIT);		
	#endif
*/

}

/*************************************************/
void do_display(void *Index, uint radixNumWords) {
/*	
	ulong Noccs=0; ulong numoccs;
	uint  length, i;
	ulong tot_numcharext;
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
	int j;
	for (j=0;j<REPETITIONSDISPLAY;j++) {
	#endif	
	tot_numcharext=numoccs=0;	
	  for (i=0; i<numpatt; i++) {
		  //printf("\n iter %3lu of %3lu",i,numpatt);	
		  pattern=patterns[i];
		  length = lens[i];
		  
		  // * Display *  //
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
		fprintf (timeFile, "\n\t** Operation = %c ** numpatts = %u, nwordsperpattern=%u\n", operation, numpatt,nwordsperpattern);;
	
		fprintf (timeFile, "\tTotal num chars extracted = %lu, numoccs = %lu\n", tot_numcharext,numoccs);
		fprintf (timeFile, "\tDisplay time = %.6f %s\n", (tot_time * SEC_TIME_DIVIDER) ,SEC_TIME_UNIT);
		fprintf (timeFile, "\tDisplay_time/Num_occs  = %.8f %s/occss\n", (tot_time * MSEC_TIME_DIVIDER) / numoccs, MSEC_TIME_UNIT);
		fprintf (timeFile, "\tDisplay_time/Tot_num_chars = %.6f %s/chars\n", (tot_time*MSEC_TIME_DIVIDER) / tot_numcharext , MSEC_TIME_UNIT);
		fprintf (timeFile, "\t\t(Load_time+Locate_time)/Tot_num_chars = %.6f %s/chars\n\n", ((tot_time+Load_time) * MSEC_TIME_DIVIDER)  / tot_numcharext  , MSEC_TIME_UNIT);
			
	#endif		
*/

}


/*************************************************/
/*************************************************/
void writelog(FILE *f, uint *ids, uint nids) {
	uint i;
	for (i=0;i<nids-1;i++) 
		fprintf(f,"%u\t", ids[i]);
	fprintf(f,"%u\n", ids[i]);
}


void
do_extract_intersect (void *Index)
{
	int error = 0;
	uint numocc, length, *occ;
	ulong tot_numocc;
	
	double atime, tot_time = 0;
	
	uint i;

	uchar *pattern;
	uint numpatt,nwordsperpattern; 
	uchar ** patterns;
	uint *lens;
	
	loadPatterns(&patterns, &lens, &numpatt, &nwordsperpattern);
	uint **ids; uint *nids; //patterns converted into ids;
	uint numpattsrc = numpatt;
	loadIds(Index, &ids,&nids, patterns, lens, &numpatt);
	
	{//logs the ids  (to be used by the queries-program of the ELIAS-FANO-PARTITIONED (sigir-2014 paper)
		 FILE* f = fopen("fids.pat","w");
		 for (i=0; i<numpatt; i++) {
			 writelog(f,ids[i], nids[i]);
		 }
		 fclose(f);
	}
	
	numpatt = numpattsrc;

	fprintf (stderr,"\nStarting Locate over %u patterns (1):::",numpatt);
	
	uint **occs = (uint **) malloc (sizeof(uint*)* numpatt);
	uint *noccs = (uint *)  malloc (sizeof(uint) * numpatt);	
	tot_numocc=0;
	for (i=0; i<numpatt; i++) {	
	 pattern=patterns[i];
	 length = lens[i];			
	 
	  /* locate*/
	  error =	locate1 (Index, pattern, length, &occs[i], &noccs[i]);	  	
	  //error =	locate1 (Index, pattern, length, &occ, &numocc);
	  numocc= noccs[i];
	  tot_numocc += numocc;
	}
	//now occs[i] is the array of occurrences of pattern i obtained by doing extract/intersect of posting lists.
	
	
	printf("\n locate 1 occs = %lu\n", tot_numocc);
	fflush(stdout);fflush(stderr);
	
	uint minnw = nwordsperpattern;
	uint maxnw = nwordsperpattern;
	
	minnw/=10000000;
	maxnw%=10000000;
	maxnw/=1000;
	//we have phrases with a number of words between: minnw and maxnw

#ifdef DO_LOG_OUTPUT
char flogname[100];
sprintf(flogname,"log_%c.txt",operation);
FILE *FLOG = fopen(flogname,"w");
#endif

	fflush(stdout);fflush(stderr);
	fprintf (stderr,"\nStarting Extract/Intersection over %u patterns:",numpatt);
	fprintf (stderr,"\n\t patterns contain [from %u to %u] words:",minnw,maxnw);
	fprintf(stderr," ** extract/intersect: repetitions = %d\n",REPETITIONS_INTERSECT);


	uint ***occsR = (uint ***) malloc (sizeof (uint **) * REPETITIONS_INTERSECT);
	{int i=0; int j=0;
		for (i=0; i< REPETITIONS_INTERSECT; i++) {
			occsR[i]= (uint **) malloc (sizeof(uint*)* numpatt);
			for (j=0; j<numpatt;j++) {
				occsR[i][j] = (uint *) malloc(sizeof(uint) * noccs[j]);
				uint z;
				for (z=0;z<noccs[j];z++)
					occsR[i][j][z] = occs[j][z]; 
			}
		}
	}

	
	/********* timing on *******************/	
	atime = getTime ();

	fprintf(stderr,"\n#iter %d of %d # ",1,REPETITIONS_INTERSECT);   
	int j;
	for (j=0;j<REPETITIONS_INTERSECT;j++) {
	  tot_numocc=0;
	  for (i=0; i<numpatt; i++) {	
		 pattern=patterns[i];
		 length = lens[i];			
		 
		  /* locate*/
		  error =	locate2 (Index, pattern, length, &occsR[j][i], &noccs[i]);	  		  
		  //error =	locate2 (Index, pattern, length, &occ, &numocc);
		  occ=	occsR[j][i];
		  numocc= noccs[i];	  
		  tot_numocc += numocc;

		  #ifdef DO_LOG
			fprintf(logFile,"\n iter = %u, occs= %u ",i,numocc);
			uint z;
			for (z=0;z<numocc;z++) fprintf(logFile,"\n %u --> id=%u",z,occ[z]);
		  #endif

		  
		  if (occ) free (occ);	
		  //fprintf(stderr,"@"); 			

			#ifdef DO_LOG_OUTPUT
			fprintf(FLOG,"\n #iter %d of %d: pattern = \"%s\" ",i, numpatt, pattern);
			fprintf(FLOG,"\n    n-occs = %u ", numocc); 
			#endif	  

	  }
	  
	//fprintf(stderr,"#iter %d of %d finishes# ",j+1,REPETITIONS_INTERSECT);
	fprintf(stderr,"\r#iter %d of %d finished# ",j+1,REPETITIONS_INTERSECT);   

	}
	
   
	tot_time += (getTime () - atime);
	tot_time /= (double) REPETITIONS_INTERSECT;		
	
	/************ timing off *******************/
	fprintf(stderr,"\n");		
	fflush(stdout);fflush(stderr);


	//free array occsR[]	
	for (i=0; i< REPETITIONS_INTERSECT; i++)
		free(occsR[i]);
	free(occsR);

	//free array occs[]
	for (i=0;i<numpatt;i++) {
		free(occs[i]);
	}	
	free(occs);
	free(noccs);
	
	//free patterns and lens arrays.
	for (i=0;i<numpatt; i++) {		
		free(patterns[i]);   
	}
	free(patterns);
	free(lens);
	free_ids (ids, nids, numpatt);
	
	
	#ifdef DO_LOG_OUTPUT
	fprintf(FLOG,"\n\n TOTAL  NOCCS WHOLE EXPERIMENT =  = %lu ", tot_numocc); 
	fclose (FLOG);
	#endif

	//Show stats.
	fprintf (stderr, "**size_Index= %lu bytes, size_Inv-lists= %u bytes, size_compText= %u bytes\n", Index_size, Index_sizeil, Index_sizect);
	fprintf (stderr, "Total Num occs found = %lu\n", tot_numocc);
	fprintf (stderr, "Locate time = %.8f %s (numpatts = %u)\n", (tot_time * SEC_TIME_DIVIDER) ,SEC_TIME_UNIT, numpatt);
	fprintf (stderr, "Locate_time/Num_occs = %.8f %s/occss\n", (tot_time * MSEC_TIME_DIVIDER) / tot_numocc, MSEC_TIME_UNIT);
	fprintf (stderr, "Locate_time/pat = %.8f %s/pat\n", (tot_time * MSEC_TIME_DIVIDER) / numpatt, MSEC_TIME_UNIT);
	fprintf (stderr, "(Locate_time)/Num_occs = %.8f %s/occs\n\n", ((tot_time+Load_time) * MSEC_TIME_DIVIDER) / tot_numocc , MSEC_TIME_UNIT);

	char opname[25];
	if ((minnw==maxnw) && (minnw ==1)) {
		operation = 'E'; sprintf(opname,"%s","loc-words");
	}
	else if ((minnw==maxnw) && (minnw ==2)) {
  		operation = 'I'; sprintf(opname,"%s","loc-2words");
	}
	else {
   		operation = 'N'; sprintf(opname,"%s","loc-Nwords");
	}


	#ifdef LOGTIMES
	/**/	fprintf (timeFile, "\t** Operation = %c ** numpatts = %u, wordsXpattern=%u-%u\n", operation, numpatt,minnw,maxnw);
	/**/	fprintf (timeFile, "\t**size_Index= %lu bytes, size_Inv-lists= %u bytes, size_compText= %u bytes\n", Index_size, Index_sizeil, Index_sizect);
	/**/	fprintf (timeFile, "\tTotal Num occs found = %lu\n", tot_numocc);
	/**/	fprintf (timeFile, "\tLocate time = %.6f %s\n", (tot_time * SEC_TIME_DIVIDER) ,SEC_TIME_UNIT);
	/**/	fprintf (timeFile, "\tLocate time/Num_occs = %.6f %s/occss\n", (tot_time * MSEC_TIME_DIVIDER) / tot_numocc, MSEC_TIME_UNIT);
	/**/	fprintf (timeFile, "\tLocate_time/pat = %.8f %s/pat\n", (tot_time * MSEC_TIME_DIVIDER) / numpatt, MSEC_TIME_UNIT);
	/**/	fprintf (timeFile, "\t\t(Load_time+locate time)/Num_occs = %.6f  %s/occs\n\n", ((tot_time+Load_time) * MSEC_TIME_DIVIDER) / tot_numocc , MSEC_TIME_UNIT);		

	/**/    char timeperpat[50]; sprintf(timeperpat,"%5.9f", (tot_time * MSEC_TIME_DIVIDER) / numpatt);//, MSEC_TIME_UNIT);
	/**/    char timeperocc[50]; sprintf(timeperocc,"%5.9f", (tot_time * MSEC_TIME_DIVIDER *1000) / tot_numocc);//, MSEC_TIME_UNIT);  // TIMES IN usec/occ
	/**/	alighnnum(timeperpat,15);alighnnum(timeperocc,15);
	/**/	fprintf(gnuFile," %15lu\t",Index_size);
	/**/	fprintf(gnuFile,"%15s\t",timeperpat);
	/**/	fprintf(gnuFile,"%15s\t",timeperocc);
	/**/	fprintf(gnuFile,"%15lu\t",tot_numocc);
	/**/	fprintf(gnuFile,"%15u\t",numpatt);
	/**/	fprintf(gnuFile,"%15lu\t",(ulong)Index_sizect);
	/**/	fprintf(gnuFile,"%15lu\t",(ulong)Index_sizeil);	
	/**/	fprintf(gnuFile,"#%s\n",indexbasename);
	
	#endif

}







/*************************************************/
/*************************************************/
  /*

void
do_extract_intersect (void *Index)
{
	int error = 0;
	uint numocc, length, *occ;
	ulong tot_numocc;
	
	double atime, tot_time = 0;
	
	uint i;

	uchar *pattern;
	uint numpatt,nwordsperpattern; 
	uchar ** patterns;
	uint *lens;
	
	loadPatterns(&patterns, &lens, &numpatt, &nwordsperpattern);
	fprintf (stderr,"\nStarting Locate over %u patterns (1):::",numpatt);

	
	uint minnw = nwordsperpattern;
	uint maxnw = nwordsperpattern;
	
	minnw/=10000000;
	maxnw%=10000000;
	maxnw/=1000;
	//we have phrases with a number of words between: minnw and maxnw

#ifdef DO_LOG_OUTPUT
char flogname[100];
sprintf(flogname,"log_%c.txt",operation);
FILE *FLOG = fopen(flogname,"w");
#endif

	fflush(stdout);fflush(stderr);
	fprintf (stderr,"\nStarting Extract/Intersection over %u patterns:",numpatt);
	fprintf (stderr,"\n\t patterns contain [from %u to %u] words:",minnw,maxnw);
	#ifdef WITHREPETITIONS
	fprintf(stderr," ** extract/intersect: repetitions = %d\n",REPETITIONS_INTERSECT);
	#endif	
		
	atime = getTime ();

	#ifdef WITHREPETITIONS
	int j;
	for (j=0;j<REPETITIONS_INTERSECT;j++) {
		//fprintf(stderr,"\n iter %d of %d",j,REPETITIONS_INTERSECT);
	#endif
	  tot_numocc=0;
	  for (i=0; i<numpatt; i++) {	
		 pattern=patterns[i];
		 length = lens[i];			
		 
		  // locate
		  //error = index_listDocuments(Index, ids[i], nids[i], &occ, &numocc);
		  error =	locate (Index, pattern, length, &occ, &numocc);
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

#ifdef DO_LOG_OUTPUT
fprintf(FLOG,"\n #iter %d of %d: pattern = \"%s\" ",i, numpatt, pattern);
fprintf(FLOG,"\n    n-occs = %u ", numocc); 
#endif	  

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
	
	
#ifdef DO_LOG_OUTPUT
fprintf(FLOG,"\n\n TOTAL  NOCCS WHOLE EXPERIMENT =  = %lu ", tot_numocc); 
fclose (FLOG);
#endif

	//Show stats.
	fprintf (stderr, "**size_Index= %lu bytes, size_Inv-lists= %u bytes, size_compText= %u bytes\n", Index_size, Index_sizeil, Index_sizect);
	fprintf (stderr, "Total Num occs found = %lu\n", tot_numocc);
	fprintf (stderr, "Locate time = %.8f %s (numpatts = %u)\n", (tot_time * SEC_TIME_DIVIDER) ,SEC_TIME_UNIT, numpatt);
	fprintf (stderr, "Locate_time/Num_occs = %.8f %s/occss\n", (tot_time * MSEC_TIME_DIVIDER) / tot_numocc, MSEC_TIME_UNIT);
	fprintf (stderr, "Locate_time/pat = %.8f %s/pat\n", (tot_time * MSEC_TIME_DIVIDER) / numpatt, MSEC_TIME_UNIT);
	fprintf (stderr, "(Locate_time)/Num_occs = %.8f %s/occs\n\n", ((tot_time+Load_time) * MSEC_TIME_DIVIDER) / tot_numocc , MSEC_TIME_UNIT);

	char opname[25];
	if ((minnw==maxnw) && (minnw ==1)) {
		operation = 'E'; sprintf(opname,"%s","loc-words");
	}
	else if ((minnw==maxnw) && (minnw ==2)) {
  		operation = 'I'; sprintf(opname,"%s","loc-2words");
	}
	else {
   		operation = 'N'; sprintf(opname,"%s","loc-Nwords");
	}


	#ifdef LOGTIMES
		fprintf (timeFile, "\t** Operation = %c ** numpatts = %u, wordsXpattern=%u-%u\n", operation, numpatt,minnw,maxnw);
		fprintf (timeFile, "\t**size_Index= %lu bytes, size_Inv-lists= %u bytes, size_compText= %u bytes\n", Index_size, Index_sizeil, Index_sizect);
		fprintf (timeFile, "\tTotal Num occs found = %lu\n", tot_numocc);
		fprintf (timeFile, "\tLocate time = %.6f %s\n", (tot_time * SEC_TIME_DIVIDER) ,SEC_TIME_UNIT);
		fprintf (timeFile, "\tLocate time/Num_occs = %.6f %s/occss\n", (tot_time * MSEC_TIME_DIVIDER) / tot_numocc, MSEC_TIME_UNIT);
		fprintf (timeFile, "\tLocate_time/pat = %.8f %s/pat\n", (tot_time * MSEC_TIME_DIVIDER) / numpatt, MSEC_TIME_UNIT);
		fprintf (timeFile, "\t\t(Load_time+locate time)/Num_occs = %.6f  %s/occs\n\n", ((tot_time+Load_time) * MSEC_TIME_DIVIDER) / tot_numocc , MSEC_TIME_UNIT);		

	    char timeperpat[50]; sprintf(timeperpat,"%5.9f", (tot_time * MSEC_TIME_DIVIDER) / numpatt);//, MSEC_TIME_UNIT);
	    char timeperocc[50]; sprintf(timeperocc,"%5.9f", (tot_time * MSEC_TIME_DIVIDER *1000) / tot_numocc);//, MSEC_TIME_UNIT);  // TIMES IN usec/occ
		alighnnum(timeperpat,15);alighnnum(timeperocc,15);
		fprintf(gnuFile," %15lu\t",Index_size);
		fprintf(gnuFile,"%15s\t",timeperpat);
		fprintf(gnuFile,"%15s\t",timeperocc);
		fprintf(gnuFile,"%15lu\t",tot_numocc);
		fprintf(gnuFile,"%15u\t",numpatt);
		fprintf(gnuFile,"%15lu\t",(ulong)Index_sizect);
		fprintf(gnuFile,"%15lu\t",(ulong)Index_sizeil);	
		fprintf(gnuFile,"#%s\n",indexbasename);
	
	#endif

}


*/



/****************************************************************************/

void do_Measure_Extract_time_intervals(void *Index) {
	int error = 0;
	uchar *text, orig_file[257];
	ulong num_pos, from, to, numchars, readen, tot_ext = 0;
	double time, tot_time = 0;

	error = fscanf(stdin, "# number=%lu length=%lu file=%s\n", &num_pos, &numchars, orig_file);
	
	if (error != 3)
	{
		fprintf (stderr, "Error: Intervals file header is not correct\n");
		perror ("run_queries");
		exit (1);
	}
	fprintf(stderr, "# number=%lu length=%lu file=%s\n", num_pos, numchars, orig_file);

	uint numPat= num_pos;
	
	
	while(num_pos) {
	
		if(fscanf(stdin,"%lu,%lu\n", &from, &to) != 2) {
			fprintf(stderr, "Cannot read correctly intervals file\n");
			exit(1);
		}

		time = getTime();
		error = extract(Index, from, to, &text, &readen);
		
		IFERROR(error);
		tot_time += (getTime() - time);
	
		tot_ext += readen;
		
/*		if (Verbose) {  //pizza-chili verbose extract
			fwrite(&from,sizeof(ulong),1,stdout);
			fwrite(&readen,sizeof(ulong),1,stdout);
			fwrite(text,sizeof(uchar),readen, stdout);
		}
	*/	
		//if (Verbose) { /**fari's verbose style */
		//
		//fprintf(stdout,"\n*************************************************");
		//fprintf(stdout,"\n****[iter: %lu], from %lu, to %lu => len = %lu, \ntext => [%s]",numPat-num_pos,from,to,readen,text); //shows it			
		//fprintf(stdout,"\n*************************************************\n");
		//}
	
		#ifdef LOGFILE
		{ /**fari's verbose style */
			fprintf(logFile,"\n*************************************************");
			fprintf(logFile,"\n****[iter: %lu], from %lu, to %lu => len = %lu, \ntext => [%s]",numPat-num_pos,from,to,readen,text); //shows it			
			fprintf(logFile,"\n*************************************************\n");
		}		
		#endif	

		num_pos--;
		free(text);
		
		fprintf(stderr, "Processing %.1f%%\r", (float)(numPat-num_pos)/numPat*100);fflush(stderr);
	}
	fprintf(stderr, "Processing %.1f%%", (float)100); fflush(stderr);
	

	ulong tot_chars = tot_ext;

	
	fprintf (stderr, "Total num chars extracted = %lu\n", tot_ext);
	fprintf (stderr, "Extract time = %.6f secs\n", tot_time);
	fprintf (stderr, "Extract_time/Num_chars_extracted = %.6f msecs/chars\n\n", (tot_time * 1000) / tot_ext);
	fprintf (stderr, "(Load_time+Extract_time)/Num_chars_extracted = %.6f msecs/chars\n\n",((Load_time+tot_time) * 1000) / tot_ext);

	//Show stats.
	fprintf (stderr, "\t** Operation = %c ** \n", operation);
	fprintf (stderr, "**size_Index= %lu bytes, size_Inv-lists= %u bytes, size_compText= %u bytes\n", Index_size, Index_sizeil, Index_sizect);
	fprintf (stderr, "Total Num chars extracted found = %lu\n", tot_chars);
	fprintf (stderr, "Extract time = %.8f %s \n", (tot_time * SEC_TIME_DIVIDER) ,SEC_TIME_UNIT);
	fprintf (stderr, "Extract_time/chars = %.8f %s/char\n", (tot_time * MSEC_TIME_DIVIDER) / tot_chars, MSEC_TIME_UNIT);
	fprintf (stderr, "(Load_time+Extract_time)/char = %.8f %s/char\n\n", ((tot_time+Load_time) * MSEC_TIME_DIVIDER) / tot_chars , MSEC_TIME_UNIT);

	char opname[25];	
	
	#ifdef LOGTIMES
	fprintf (timeFile, "\t** Operation = %c ** \n", operation);
	fprintf (timeFile, "**size_Index= %lu bytes, size_Inv-lists= %u bytes, size_compText= %u bytes\n", Index_size, Index_sizeil, Index_sizect);
	fprintf (timeFile, "Total Num chars extracted found = %lu\n", tot_chars);
	fprintf (timeFile, "Extract time = %.8f %s \n", (tot_time * SEC_TIME_DIVIDER) ,SEC_TIME_UNIT);
	fprintf (timeFile, "Extract_time/chars = %.8f %s/char\n", (tot_time * MSEC_TIME_DIVIDER) / tot_chars, MSEC_TIME_UNIT);
	fprintf (timeFile, "(Load_time+Extract_time)/char = %.8f %s/char\n\n", ((tot_time+Load_time) * MSEC_TIME_DIVIDER) / tot_chars , MSEC_TIME_UNIT);



	/**/    char timeperinterval[50]; sprintf(timeperinterval,"%5.9f", (tot_time * MSEC_TIME_DIVIDER) / numPat);//, MSEC_TIME_UNIT);
	/**/    char timeperchar[50]; sprintf(timeperchar,"%5.9f", (tot_time * MSEC_TIME_DIVIDER *1000) / tot_chars);//, MSEC_TIME_UNIT);  // TIMES IN usec/char
	/**/    char cratio[50]; sprintf(cratio,"%3.3f", 100.0 * Index_size / Text_length);//, MSEC_TIME_UNIT);  // TIMES IN usec/char
	/**/	alighnnum(timeperinterval,15);alighnnum(timeperchar,15);
	/**/	fprintf(gnuFile," %15lu\t",Index_size);
	/**/	fprintf(gnuFile,"%10s\t",cratio);
	/**/	fprintf(gnuFile,"%15s\t",timeperinterval);
	/**/	fprintf(gnuFile,"%15s\t",timeperchar);
	/**/	fprintf(gnuFile,"%15lu\t",tot_chars);
	/**/	fprintf(gnuFile,"%15u\t",numPat);
	/**/	fprintf(gnuFile,"%15lu\t",(ulong)Index_sizect);
	/**/	fprintf(gnuFile,"%15lu\t",(ulong)Index_sizeil);
	/**/	fprintf(gnuFile,"#%s\n",indexbasename);
	

	#endif

}

/*************************************************/
/*************************************************/
void
do_Measure_Extract_time (void *Index)
{
	int error = 0;
	ulong tot_chars;
	uint ndocs;
	
	double atime, tot_time = 0;
	
	uint i;

	error = getNumDocs(Index, &ndocs);

	fflush(stdout);fflush(stderr);
	fprintf (stderr,"\n Measuring Avg-doc extraction time. All the %lu docs will be extracted randomly", (ulong)ndocs);
		

	
	uint *docIds ; 	uint nids;
	uchar **texts; 	uint *lens;
	
	nids = ndocs;
	docIds = (uint *) malloc (sizeof(uint) * ndocs);
	for (i=0;i<ndocs;i++)
		docIds[i] = i;
	
	makeRandom(docIds,ndocs);
	
	atime = getTime ();

	extractTextOfSelectedDocuments(Index, docIds, nids, &texts, &lens, &tot_chars);
	  
	tot_time += (getTime () - atime);
	
	free(lens);
	for (i=0;i<ndocs;i++)
		free(texts[i]);
	free(texts);
	free(docIds);
	
	fflush(stdout);fflush(stderr);

	//Show stats.
	fprintf (stderr, "\t** Operation = %c ** \n", operation);
	fprintf (stderr, "**size_Index= %lu bytes, size_Inv-lists= %u bytes, size_compText= %u bytes\n", Index_size, Index_sizeil, Index_sizect);
	fprintf (stderr, "Total Num chars extracted found = %lu\n", tot_chars);
	fprintf (stderr, "Extract time = %.8f %s (numdocs = %u)\n", (tot_time * SEC_TIME_DIVIDER) ,SEC_TIME_UNIT, ndocs);
	fprintf (stderr, "Extract_time/chars = %.8f %s/char\n", (tot_time * MSEC_TIME_DIVIDER) / tot_chars, MSEC_TIME_UNIT);
	fprintf (stderr, "Extract_time/doc = %.8f %s/doc\n", (tot_time * MSEC_TIME_DIVIDER) / ndocs, MSEC_TIME_UNIT);
	fprintf (stderr, "(Load_time+Extract_time)/char = %.8f %s/char\n\n", ((tot_time+Load_time) * MSEC_TIME_DIVIDER) / tot_chars , MSEC_TIME_UNIT);

	char opname[25];
	sprintf(opname,"%s","AVG doc extrac time");
	

	#ifdef LOGTIMES
	fprintf (timeFile, "\t** Operation = %c ** \n", operation);
	fprintf (timeFile, "**size_Index= %lu bytes, size_Inv-lists= %u bytes, size_compText= %u bytes\n", Index_size, Index_sizeil, Index_sizect);
	fprintf (timeFile, "Total Num chars extracted found = %lu\n", tot_chars);
	fprintf (timeFile, "Extract time = %.8f %s (numdocs = %u)\n", (tot_time * SEC_TIME_DIVIDER) ,SEC_TIME_UNIT, ndocs);
	fprintf (timeFile, "Extract_time/chars = %.8f %s/char\n", (tot_time * MSEC_TIME_DIVIDER) / tot_chars, MSEC_TIME_UNIT);
	fprintf (timeFile, "Extract_time/doc = %.8f %s/doc\n", (tot_time * MSEC_TIME_DIVIDER) / ndocs, MSEC_TIME_UNIT);
	fprintf (timeFile, "(Load_time+Extract_time)/char = %.8f %s/char\n\n", ((tot_time+Load_time) * MSEC_TIME_DIVIDER) / tot_chars , MSEC_TIME_UNIT);

	/**/    char timeperdoc[50]; sprintf(timeperdoc,"%5.9f", (tot_time * MSEC_TIME_DIVIDER) / ndocs);//, MSEC_TIME_UNIT);
	/**/    char timeperchar[50]; sprintf(timeperchar,"%5.9f", (tot_time * MSEC_TIME_DIVIDER *1000) / tot_chars);//, MSEC_TIME_UNIT);  // TIMES IN usec/char
	/**/    char cratio[50]; sprintf(cratio,"%3.3f", 100.0 * Index_size / Text_length);//, MSEC_TIME_UNIT);  // TIMES IN usec/char
	/**/	alighnnum(timeperdoc,15);alighnnum(timeperchar,15);alighnnum(timeperdoc,10);
	/**/	fprintf(gnuFile," %15lu\t",Index_size);
	/**/	fprintf(gnuFile,"%10s\t",cratio);
	/**/	fprintf(gnuFile,"%15s\t",timeperdoc);
	/**/	fprintf(gnuFile,"%15s\t",timeperchar);
	/**/	fprintf(gnuFile,"%15lu\t",tot_chars);
	/**/	fprintf(gnuFile,"%15u\t",ndocs);
	/**/	fprintf(gnuFile,"%15lu\t",(ulong)Index_sizect);
	/**/	fprintf(gnuFile,"%15lu\t",(ulong)Index_sizeil);
	/**/	fprintf(gnuFile,"#%s\n",indexbasename);
	
	#endif

}
/****************************************************************************/










/*************************************************/
void readNumPatterns (uint *numPhrase, uint *numOfWords) {
	int errF= fscanf(stdin,"#numberOfFrases::%9d, #NumberOfWordsPerPhrase:: %9d\n",numPhrase, numOfWords);
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
	uchar *pattern;
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
	if (!nids) {
	  printf("\n pattern of len %u <<%s>> could not be parsed into ids (skipped) ",lens[i],patts[i]); fflush(stderr);fflush(stdout);
	  continue;
	}

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
void alighnnum(char *str, uint len){
	int n=strlen(str);
	int i;
	if (n<len) {
	  int shift = len-n;
	  for (i=len-1; i>=shift;i--)
		 str[i] = str[i-shift];
	  for (i=0;i<shift;i++) str[i]=' ';
	  str[len]='\0';
	  
//for (i=0;i< len-n;i++)
//		str[i+n]=' ';
//
//	  for (i=0;i< len-n;i++)
//		str[i+n]=' ';
//	  str[len]='\0';
	}
}

int int_rand_range(uint  a,uint b){
	uint n = (b-a+1);
	return a+ ((int)  ((n*1.0*rand()/ (RAND_MAX + 1.0)) ));
}	

/* Reorders randomly all the elments of V */
void makeRandom (uint *V, uint n) {
  int r=n-1;
  uint i;
  uint rdnpos;
  
  for (i=0;i<n-1; i++) {
	rdnpos = int_rand_range(0,r);
	
	uint tmp;
	tmp = V[r];
	V[r]= V[rdnpos];
	V[rdnpos] = tmp;
	r--;
  }
  
 // for (i=0;i<n;i++)
 //	  printf("[%u]",V[i]);
  
}
	
#endif
