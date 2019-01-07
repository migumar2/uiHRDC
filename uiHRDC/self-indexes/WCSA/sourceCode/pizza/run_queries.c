       #include <sys/types.h>
       #include <sys/stat.h>
       #include <unistd.h>

/*
 * Run Queries
 */

//#define LOGFILE


#define GET_TIME_DIVIDER ((double) 1.0 )	 //getTime deals with "seconds" as time unit.
#define GET_TIME_UNIT "sec"
#define MSEC_TIME_DIVIDER  ((double) 1000.0) //1 sec = 1000 msec
#define MSEC_TIME_UNIT 	  "msec"
#define SEC_TIME_DIVIDER  ((double) 1.0	)	 //1 sec = 1 sec
#define SEC_TIME_UNIT 	  "sec"

//#define GET_TIME_DIVIDER ((double)1000000.0)        //getTime deals with "microseconds" as time unit.
//#define GET_TIME_UNIT "microsec"
//#define MSEC_TIME_DIVIDER  ((double)0.001)	//1 microsec = 0.001 msec
//#define MSEC_TIME_UNIT 	  "msec"
//#define SEC_TIME_DIVIDER  ((double)0.000001)	//1 microsec = 0.000001 sec
//#define SEC_TIME_UNIT 	  "sec"


#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "interface.h"

/* only for getTime() */
#include <sys/time.h>
#include <sys/times.h>
#include <sys/resource.h>

#define COUNT 		('C')
#define LOCATE 		('L')
#define EXTRACT 	('E')
#define DISPLAY 	('D')
#define VERBOSE 	('V')
#define LOCATEWORD 	('l')
#define DISPLAYWORD	('d')
#define DISPLAYKWORDS	('s')
#define EXTRACTWORDS ('e')  

#define OFFSETS_DOC_LOCATE ('o')
#define MEASURE_EXTRACT_TIME ('M')

#define TEST 		('T')


#define WITHREPETITIONS //------------------------
uint REPETITIONSCOUNT=1;
uint REPETITIONSLOCATE=1;
uint REPETITIONSDISPLAYKWORDS = 1;


/* macro to detect and to notify errors */
#define IFERROR(error) {{if (error) { fprintf(stderr, "%s\n", error_index(error)); exit(1); }}}

/**/ FILE *timeFile;
 	 FILE *gnuFile; 

#ifdef LOGFILE
/**/ FILE *logFile;
#endif

/* local headers */
void do_test (void);
void do_locate (void);
void do_count (void);
void do_extract (void);
void do_display(ulong length);
void do_locate_word (ulong wordsbefore);
void do_display_word (ulong numc, ulong wordsbefore);
void do_display_kWords (ulong numc, ulong wordsbefore);
void do_extract_words(void);

void do_locate_docs (void);
void do_Measure_Extract_time(void *Index);

/* some auxiliar functions to deal with patterns and obtain the name of the search-program */
void findFileName(char **dst,char *src);
void readNumPatterns (ulong *numPhrase, ulong *numOfWords) ;
uchar *inputWord( ulong *len);
void loadPatterns(uchar ***patterns,ulong **lens, ulong *numpatt, ulong *numwordsperpattern);
/* others */
void pfile_info (ulong *length, ulong *numpatt);
double getTime (void);

int int_rand_range(uint  a,uint b);
void makeRandom (uint *v, uint n);
void alighnnum(char *str, uint len);

void usage(char * progname);



static void *Index;	 /* opauque data type */
static int Verbose = 0; 
static ulong Index_size, Text_length;
static double Load_time;

static char operation;
static int  npatterns;
static char progname[1000];

static char *indexbasename;




/*
 * Temporary usage: run_queries <index file> <type> [length] [V]
 */



int main (int argc, char *argv[])
{
	int error = 0;
	char *filename;
	char querytype;
	
	if (argc < 3)	{
		usage(argv[0]);	
		exit (1);
	}
	
	filename = argv[1];
	querytype = *argv[2];

	char timeFileName[256];
	char *dstptr;
	findFileName(&dstptr,argv[0]);  
	sprintf(timeFileName,"times_%c_%s.txt",querytype,dstptr);
	fprintf(stderr,"TIMEFILENAME= %s\n",timeFileName);
	timeFile = fopen(timeFileName,"a");
	//timeFile = fopen("timessWCSA.txt","a");
	fprintf(timeFile,"\n Search sWCSA: %s",argv[1]);

#ifdef LOGFILE
/**/ logFile = fopen("log.txt","w");
#endif
	{
		int i;
		 strcpy(progname,"");
		for (i=0; i<argc;i++) {
			strcat(progname,argv[i]);
			strcat(progname," ");
		}
	}
	
	findFileName(&indexbasename,filename);
	
	
	///////////////////////////////////////////////////////////////////
	/**/	char outFilename[256]="";	
	//	char *progname = argv[0];
	//    findFileName(&progname,argv[0]);
	//	findFileName(&indexbasename,argv[1]);

	/**/	char outFilenameGNU[256];	
	/**/	sprintf(outFilenameGNU,"%c.dat",querytype);
	/**/	struct stat bufgnu;
	/**/	if (-1 == stat(outFilenameGNU, &bufgnu)){
	/**/		gnuFile =fopen(outFilenameGNU,"w");
	/**/		if (!gnuFile) {printf("\n could not open GNUPLOT data file (%s)... exitting\n",outFilenameGNU); exit(0);}
				if ( (querytype != MEASURE_EXTRACT_TIME) && (querytype != EXTRACTWORDS)) {
					fprintf(gnuFile,"#%15s\t%15s\t%15s\t%15s\t%15s\t%s\n","totaSize(byte)","msec/pat","usec/occ","occs","numpats","#indexBaseName");
				}
				else {
					fprintf(gnuFile,"#%15s\t%10s\t%15s\t%15s\t%15s\t%15s\t%s\n","totaSize(byte)", "ratio (%)","msec/doc","usec/char","chars","ndocs",  "#indexBaseName");

		  
				}
				fprintf(stderr,"\n a new file GNU-DATA (%s) will be created",outFilenameGNU);
	/**/	}
	/**/	else {
				fprintf(stderr,"\n a the file GNU-DATA (%s) is open for appending data",outFilenameGNU);
	/**/		gnuFile = fopen(outFilenameGNU,"a");
	/**/		if (!gnuFile) {printf("\n could not open GNUPLOT data file (%s)... exitting\n",outFilenameGNU); exit(0);}
	/**/	}
	///////////////////////////////////////////////////////////////////







	
	Load_time = getTime ();
	error = load_index (filename, &Index);
	IFERROR (error);
	Load_time = getTime () - Load_time;
	fprintf (stderr, "Load index time = %.2f secs\n", Load_time);


	error = index_size(Index, &Index_size);
	IFERROR (error);
	error = get_length(Index, &Text_length);
	IFERROR (error);
	fprintf(stderr, "Index size = %lu Kb\n", Index_size/1024);

	operation = querytype;
	
	switch (querytype){
	  
		case COUNT:
			if (argc < 4) {
				usage(argv[0]);	
				exit (1);
			}
			if (argc > 4) 
				if (*argv[4] == VERBOSE){
						Verbose = 1;
						fprintf(stdout,"%c", COUNT);

				}
		  
			REPETITIONSCOUNT= atol(argv[3]);
			do_count();
			break;	
/*			
		case COUNT:
			if (argc > 3) 
				if (*argv[3] == VERBOSE) {
					Verbose = 1;
					fprintf(stdout,"%c", COUNT);
				}				
			do_count();
			break;
*/			
		case LOCATE:
			if (argc > 3) 
				if (*argv[3] == VERBOSE) {
					Verbose = 1;
					fprintf(stdout,"%c", LOCATE);
				}
			do_locate();
			break;	
		case EXTRACT:
			if (argc > 3) 
				if (*argv[3] == VERBOSE) {
						Verbose = 1;
						fprintf(stdout,"%c", EXTRACT);
				}

			do_extract();
			break;
		case DISPLAY:
			if (argc < 4) {
				usage(argv[0]);	
				exit (1);
			}
			if (argc > 4) 
				if (*argv[4] == VERBOSE){
						Verbose = 1;
						fprintf(stdout,"%c", DISPLAY);

				}
			do_display((ulong) atol(argv[3]));
			break;
/*			
		case LOCATEWORD:
			if (argc < 4) {
				usage(argv[0]);	
				exit (1);
			}
			if (argc > 4) 
				if (*argv[4] == VERBOSE){
						Verbose = 1;
						fprintf(stdout,"%c", LOCATEWORD);
				}
			do_locate_word((ulong)atol(argv[3]));
			break;	
*/

		case LOCATEWORD:
			if (argc < 5) {
				usage(argv[0]);	
				exit (1);
			}
			if (argc > 5) 
				if (*argv[5] == VERBOSE){
						Verbose = 1;
						fprintf(stdout,"%c", LOCATEWORD);
				}
			REPETITIONSLOCATE = atol(argv[4]);
			do_locate_word((ulong)atol(argv[3]));
			break;	



		case DISPLAYWORD:
			if (argc < 5) {
				usage(argv[0]);	
				exit (1);
			}
			if (argc > 5) 
				if (*argv[5] == VERBOSE){
						Verbose = 1;
						fprintf(stdout,"%c", DISPLAYWORD);

				}
			do_display_word((ulong) atol(argv[3]),(ulong) atol(argv[4]));
			break;
			
/*		case DISPLAYKWORDS:
			if (argc < 5) {
				usage(argv[0]);	
				exit (1);
			}
			if (argc > 5) 
				if (*argv[5] == VERBOSE){
						Verbose = 1;
						fprintf(stdout,"%c", DISPLAYWORD);

				}
			do_display_kWords((ulong) atol(argv[3]),(ulong) atol(argv[4]));
			break;
*/
		case DISPLAYKWORDS:
			if (argc < 6) {
				usage(argv[0]);	
				exit (1);
			}
			if (argc > 6) 
				if (*argv[6] == VERBOSE){
						Verbose = 1;
						fprintf(stdout,"%c", DISPLAYWORD);

				}
			REPETITIONSDISPLAYKWORDS = atol(argv[5]);
			do_display_kWords((ulong) atol(argv[3]),(ulong) atol(argv[4]));
			break;

		case EXTRACTWORDS:
			if (argc > 3) 
				if (*argv[3] == VERBOSE) {
						Verbose = 1;
						fprintf(stdout,"%c", EXTRACTWORDS);
				}

			do_extract_words();
			break;			



		/** DOC-OFFSETLOCATE */
		case OFFSETS_DOC_LOCATE:
			if (argc < 4) {
				usage(argv[0]);	
				exit (1);
			}
			if (argc > 4) 
				if (*argv[4] == VERBOSE){
						Verbose = 1;
						fprintf(stdout,"%c", OFFSETS_DOC_LOCATE);
				}
			REPETITIONSLOCATE = atol(argv[3]);
			do_locate_docs();
			break;	
			
		/* MEASURE AVG doc-extractiong time */
		case MEASURE_EXTRACT_TIME:		
			if (argc < 3) {
				printf("Use: %s <index basename> <M> \n", argv[0]);	
				usage(argv[0]);
				exit (1);
			}					
			do_Measure_Extract_time(Index);
			break;				
			
			
		case TEST:
			do_test();
			break;
			
			
		default:
			fprintf (stderr, "Unknow option: main ru\n");
			exit (1);
	}

	error = free_index(Index);
	IFERROR(error);

/**/ fclose(timeFile);
/**/	fclose(gnuFile);

	return 0;
}


/************************************************************************************************/
void do_test() {	

  	// SEARCHING FOR A TEXT PATTERN (word/phrase).
	{unsigned char textPattern[1000];
	 ulong occ;
	 int len;
	 ulong *occs;
	 int i;
	 printf("\nSEARCH TEST for LOCATEWORDS\n");
		while(1) {	
			printf("Intro string[empty to end]: ");
			fgets((char*)textPattern, 1000, stdin);
			len = strlen((char*)textPattern);
			if (!strcmp((char*)textPattern,"\n") ) break;
			textPattern[len-1] = '\0';
			len --;
			
			// locate(Index, textPattern, len, (ulong **)&occs, (ulong *)&occ);
			locateWord(Index, textPattern, len, &occs, &occ, 0);
			
			printf("\n*** %s occurs %lu times: In the source text at positions:\n\t",textPattern,occ);
			for (i=0;i<occ;i++) 
				printf("[%lu]",occs[i]);
			fflush(stderr);	
			if (occ) free(occs);		
			
			if (!strcmp((char*)textPattern,"\n") ) break;
		}
	}	
	

  
  // DISPLAYING THE OCCURRENCES OF A TEXT PATTERN (word/phrase).
	{unsigned char textPattern[1000];
	int error = 0;
	ulong numocc,numc, length, i, *snippet_len, tot_numcharext = 0, numpatt;
	uchar *pattern, *snippet_text;
		
	pattern = textPattern;
	printf("\nSEARCH TEST for DISPLAYWORDS (pizzachili interface)\n");
	  while(1) {	
		  printf("Intro string[empty to end]: ");
		  fgets((char*)textPattern, 1000, stdin);
		  if (!strcmp((char*)textPattern,"\n") ) break;
			textPattern[strlen((char*)textPattern)-1] = '\0';

		  length = strlen( (char*)textPattern);
		  numc=100;

		  error =	displayWords (Index, textPattern, length, numc, &numocc, 
							&snippet_text, &snippet_len,4);
		  
		  if (error){ fprintf(stderr, "An error ocurred during display [err code = %d]\n", error);exit(0);}

			  fprintf(stderr,"\n display finishes:");fflush(stderr);			
		  {//show the results
			  ulong j, len = length + 2*numc;
			  char blank = '\0';
			  fprintf(stderr,"\n length = %lu",length);
			  fprintf(stderr,"\n pattern = %s",pattern);fflush(stderr);
			  fprintf(stderr,"\n numocc = %lu",numocc);fflush(stderr);
			  fprintf(stderr,"\n snippet len = %lu",len);fflush(stderr);
			  fprintf(stderr,"\n =========");fflush(stderr);		
			  for (i = 0; i < numocc; i++){
				  fprintf(stderr,"\n[%2lu][len=%3lu]<<",i+1,snippet_len[i]);fflush(stderr);
				  fwrite(snippet_text+len*i,sizeof(uchar),snippet_len[i],stderr);fflush(stderr);
				  fprintf(stderr,">>");fflush(stderr);
			  }
		  }
		  numpatt--;
		  
		  for(i=0; i<numocc; i++) {
			  tot_numcharext += snippet_len[i];
		  }
			  
		  if (numocc) {
			  free (snippet_len);
			  free (snippet_text);
		  }
		  
		  printf("Ocurrences = %lu\n", numocc);
		  if (!strcmp((char*)textPattern,"\n") ) break;
	  }
	}  
	
#ifdef LOGFILE
/**/ fclose(logFile);
#endif	
}
/************************************************************************************************/
void
do_count ()
{
	int error = 0;
	ulong numocc, length, tot_numocc = 0, res_patt;
	double time, tot_time = 0;
	uchar *pattern;
	
	ulong numpatt, numpatttmp, wordsPerPhrase;   //number of patterns and number of words per phrase.	
	uchar **words;
	ulong kk,i;

	readNumPatterns(&numpatt,&wordsPerPhrase);  //number of patterns and number of words per phrase.
	npatterns=numpatt;
	numpatttmp = numpatt;
	double time0 = getTime ();

	#ifdef WITHREPETITIONS
	fprintf(stderr,"\n** count: repetitions = %d",REPETITIONSCOUNT);
	#endif
	
	uint iters=0;
	while (numpatt) {
	
		pattern = inputWord(&length); 		                                                                  
		//fprintf(stderr,"\n leido pattron <<%s>> con len = %d\n",pattern,length);fflush(stderr);     

		/* Count */
		time = getTime ();
		for (iters=0;iters<REPETITIONSCOUNT;iters++) {
		  error = count (Index, pattern, length, &numocc);
		}
		tot_time += (getTime () - time)/ ((double)iters);   /*calculates time per iteration*/
		tot_numocc += numocc;

		IFERROR (error);	
		if (Verbose) {
			fwrite(&length, sizeof(length), 1, stdout);
			fwrite(pattern, sizeof(*pattern), length, stdout);
			fwrite(&numocc, sizeof(numocc), 1, stdout);
		}		
		
		#ifdef LOGFILE
		//	fwrite(&length, sizeof(length), 1, logFile);
		//	fwrite(pattern, sizeof(*pattern), length, logFile);
		//	fwrite(&numocc, sizeof(numocc), 1, logFile);		

			fprintf(logFile,"[\noccs=%7lu]",numocc);
			fprintf(logFile,"[len=%3lu]",length);
			fwrite(pattern, sizeof(*pattern), length, logFile);			
		#endif
		
		numpatt--;

	free (pattern); 
	}

	fprintf (stderr, "Total Num occs found = %lu\n", tot_numocc);
	fprintf (stderr, "Count time = %.6f msecs\n", tot_time*1000);
	fprintf (stderr, "Count_time/Num_patterns = %.8f msecs/patterns\n\n",(tot_time * 1000) / numpatttmp);
	fprintf (stderr, "Count_time/Num_patterns = %.8f msecs/occs\n\n",(tot_time * 1000) / tot_numocc);
	fprintf (stderr, "(Load_time+Count_time)/Num_patterns = %.6f msecs/patterns\n\n",((Load_time+tot_time) * 1000) / numpatttmp);	

	fprintf (timeFile, "\n\t ** Prog = %s ** \n", progname);
	fprintf (timeFile, "\t** Operation = %c ** numpatts = %d, wordsperphrase=%lu\n", operation, npatterns,wordsPerPhrase);
	fprintf (timeFile, "\tIndex size = %lu bytes, ratio = %3.3f\n", Index_size, (double)Index_size/(double)Text_length*100);
	
	fprintf (timeFile, "\tTotal Num occs found = %lu\n", tot_numocc);
	fprintf (timeFile, "\tCount time = %.6f msecs\n", tot_time*1000);
	fprintf (timeFile, "\tCount_time/Num_patterns = %.8f msecs/patterns\n\n",(tot_time * 1000) / numpatttmp);
	fprintf (timeFile, "\tCount_time/Num_occs = %.8f msecs/occs\n\n",(tot_time * 1000) / tot_numocc);
	fprintf (timeFile, "\t(Load_time+Count_time)/Num_patterns = %.6f msecs/patterns\n\n",((Load_time+tot_time) * 1000) / numpatttmp);	
                                                    
}

void
do_locate ()
{
	int error = 0;
	ulong numocc, length, *occ, tot_numocc = 0;
	uchar *pattern;
		
	double time, tot_time = 0;
	
	ulong numpatt,wordsPerPhrase;   //number of patterns and number of words per phrase.	
	uchar **words;
	ulong kk,i;

	readNumPatterns(&numpatt,&wordsPerPhrase);  //number of patterns and number of words per phrase.
	npatterns=numpatt;

	double time0 = getTime ();
	while (numpatt) {
	
		pattern = inputWord(&length);                                                                   
		//fprintf(stderr,"\n leido pattron <<%s>> con len = %d\n",pattern,length);fflush(stderr);     

		/* Locate */
		time = getTime ();
		error =	locate (Index, pattern, length, &occ, &numocc);
		tot_time += (getTime () - time);
		IFERROR (error);

		//	fprintf(stderr,"\n locate = %5d ocurrs for <<%s>>",numocc,pattern);	fflush(stderr);	
		//	{int i;
		//		fprintf(stderr,"\n*** %s appears in the source text in positions:\n\t",pattern);
		//		for (i=0;i<numocc;i++) 
		//			fprintf(stderr,"[%u]",occ[i]);
		//		fflush(stderr);
		//	}
		//fprintf(stderr,"\n");
	
		tot_numocc += numocc;
		numpatt--;
		
		if (Verbose) {
			fwrite(&length, sizeof(length), 1, stdout);
			fwrite(pattern, sizeof(*pattern), length, stdout);
			fwrite(&numocc, sizeof(numocc), 1, stdout);
			fwrite(occ, sizeof(*occ), numocc, stdout);
			}
			
		#ifdef LOGFILE
			fwrite(&length, sizeof(length), 1, logFile);
			fwrite(pattern, sizeof(*pattern), length, logFile);
			fwrite(&numocc, sizeof(numocc), 1, logFile);
			fwrite(occ, sizeof(*occ), numocc, logFile);		
		#endif			
			
		if (numocc) free (occ);			
		free(pattern);   
	}
	
	fprintf(stderr,"\n TOTAL DE TOTAL !! = %f",getTime()-time0);

	fprintf (stderr, "Total Num occs found = %lu\n", tot_numocc);
	fprintf (stderr, "Locate time = %.6f secs\n", tot_time);
	fprintf (stderr, "Locate_time/Num_occs = %.6f msec/occss\n\n", (tot_time * 1000) / tot_numocc);
	fprintf (stderr, "(Load_time+Locate_time)/Num_occs = %.6f msecs/occs\n\n", ((tot_time+Load_time) * 1000) / tot_numocc);

	fprintf (timeFile, "\n\t ** Prog = %s ** \n", progname);
	fprintf (timeFile, "\t** Operation = %c ** numpatts = %d, wordsperphrase=%lu\n", operation, npatterns,wordsPerPhrase);
	fprintf (timeFile, "\tIndex size = %lu bytes, ratio = %3.3f\n", Index_size, (double)Index_size/(double)Text_length*100);
	
	fprintf (timeFile, "\tTotal Num occs found = %lu\n", tot_numocc);
	fprintf (timeFile, "\tLocate time = %.6f secs\n", tot_time);
	fprintf (timeFile, "\tLocate_time/Num_occs = %.6f msec/occss\n", (tot_time * 1000) / tot_numocc);
	fprintf (timeFile, "\t\t(Load_time+Locate_time)/Num_occs = %.6f msecs/occs\n\n", ((tot_time+Load_time) * 1000) / tot_numocc);
}

 
void do_display(ulong numc) {
	
	int error = 0;
	ulong numocc, length, i, *snippet_len, tot_numcharext = 0;
	double time, tot_time = 0;
	uchar *pattern, *snippet_text;

	ulong numpatt, numpatttmp, wordsPerPhrase;   //number of patterns and number of words per phrase.	
	uchar **words;
	ulong kk;
	
	readNumPatterns(&numpatt,&wordsPerPhrase);  //number of patterns and number of words per phrase.
	npatterns=numpatt;
	
	double time0 = getTime ();

	while (numpatt) {
	
		pattern = inputWord(&length);                                                                   
		//fprintf(stderr,"\n leido pattron <<%s>> con len = %d\n",pattern,length);fflush(stderr);     
		        
		/* Display */
		time = getTime ();
		error =	display (Index, pattern, length, numc, &numocc, 
				    	 &snippet_text, &snippet_len);
		tot_time += (getTime () - time);		
		IFERROR (error);
		
		// 		if (Verbose) {/**pizza-chile's verbose style */
		// 			ulong j, len = length + 2*numc;
		// 		    char blank = '\0';
		// 			fwrite(&length, sizeof(length), 1, stdout);
		// 			fwrite(pattern, sizeof(*pattern), length, stdout);
		// 			fwrite(&numocc, sizeof(numocc), 1, stdout);
		// 			fwrite(&len, sizeof(len), 1, stdout);
		// 		
		// 			for (i = 0; i < numocc; i++){
		// 				fwrite(snippet_text+len*i,sizeof(uchar),snippet_len[i],stdout);
		// 				fprintf(stdout,"\n");fflush(stdout);
		// 				for(j=snippet_len[i];j<len;j++)
		// 				   fwrite(&blank,sizeof(uchar),1,stdout);
		// 			}
		// 		}

		if (Verbose) { /**fari's verbose style */
			ulong j, len = length + 2*numc;
		    char blank = '\0';		
			fprintf(stdout,"\n*************************************************");
			fprintf(stdout,"\n****[iter: %lu], Pattern: %s, numOcurr =%lu",numpatt,pattern,numocc);
			fprintf(stdout,"\n*************************************************\n");
			for (i = 0; i < numocc; i++){
				fprintf(stdout,"[[%lu]]",i);
				fwrite(snippet_text+len*i,sizeof(uchar),snippet_len[i],stdout);
				fprintf(stdout,"\n");fflush(stdout);
				for(j=snippet_len[i];j<len;j++)
				   fwrite(&blank,sizeof(uchar),1,stdout);
			}
		}


		#ifdef LOGFILE
			{ /**fari's verbose style */
			ulong j, len = length + 2*numc;
		    char blank = '\0';		
			fprintf(logFile,"\n*************************************************");
			fprintf(logFile,"\n****[iter: %lu], Pattern: %s, numOcurr =%lu",numpatt,pattern,numocc);
			fprintf(logFile,"\n*************************************************\n");
			for (i = 0; i < numocc; i++){
				fprintf(logFile,"[[%lu]]",i);
				fwrite(snippet_text+len*i,sizeof(uchar),snippet_len[i],logFile);
				fprintf(logFile,"\n");fflush(logFile);
				for(j=snippet_len[i];j<len;j++)
				   fwrite(&blank,sizeof(uchar),1,logFile);
			}
			}	
		#endif	

		numpatt--;
		
		for(i=0; i<numocc; i++) {
			tot_numcharext += snippet_len[i];
			
		}
			
		if (numocc) {
			free (snippet_len);
			free (snippet_text);
		}
		free(pattern);   

	}

	fprintf (stderr, "Total num chars extracted = %lu\n", tot_numcharext);
	fprintf (stderr, "Display time = %.6f secs\n", tot_time);
	fprintf (stderr, "Time_display/Num_occs      = %.8f msec/occss\n", (tot_time * 1000) / numocc);	
	fprintf (stderr, "Time_display/Tot_num_chars = %.6f msec/chars\n\n", (tot_time*1000) / tot_numcharext);
	fprintf (stderr, "(Load_time+Time_display)/Tot_num_chars = %.6f msecs/chars\n\n", ((Load_time+tot_time)*1000) / tot_numcharext);

/**/    fprintf (timeFile, "\n\t ** Prog = %s ** \n", progname);
/**///    fprintf (timeFile, "\t** Operation = %c ** \n", operation);
/**/    fprintf (timeFile, "\t** Operation = %c **, numc=%lu, numpatts = %d, wordsperphrase=%lu\n", operation, numc, npatterns,wordsPerPhrase); 
	fprintf (timeFile, "\tIndex size = %lu bytes, ratio = %3.3f\n", Index_size, (double)Index_size/(double)Text_length*100);

/**/    fprintf (timeFile, "\tTotal num chars extracted = %lu\n", tot_numcharext);
/**/    fprintf (timeFile, "\tDisplay time = %.6f secs\n", tot_time);
/**/	fprintf (timeFile, "\tTime_display/Num_occs      = %.8f msec/occss\n", (tot_time * 1000) / numocc);	
/**/    fprintf (timeFile, "\tTime_display/Tot_num_chars = %.6f msecs/chars\n\n", (tot_time*1000) / tot_numcharext);
/**/    fprintf (timeFile, "\t(Load_time+Time_display)/Tot_num_chars = %.6f msecs/chars\n\n", ((Load_time+tot_time)*1000) / tot_numcharext);
       
	
}


/* Open patterns file and read header */
void
pfile_info (ulong * length, ulong * numpatt)
{
	int error;
	uchar c;
	uchar origfilename[257];

	error = fscanf (stdin, "# number=%lu length=%lu file=%s forbidden=", numpatt,
					length, origfilename);
	if (error != 3)
	{
		fprintf (stderr, "Error: Patterns file header not correct\n");
		perror ("run_queries");
		exit (1);
	}

	fprintf (stderr, "# patterns = %lu length = %lu file = %s forbidden chars = ", *numpatt, *length, origfilename);

	while ( (c = fgetc(stdin)) != 0) {
		if (c == '\n') break;
		fprintf (stderr, "%d",c);
	}

	fprintf(stderr, "\n");

}


void
do_extract()
{
	int error = 0;
	uchar *text, orig_file[257];
	ulong num_pos, from, to, numchars, readen, tot_ext = 0;
	double time, tot_time = 0;

	error = fscanf(stdin, "# number=%lu length=%lu file=%s\n", &num_pos, &numchars, orig_file);
	npatterns=num_pos;
	
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
		if (Verbose) { /**fari's verbose style */

		fprintf(stdout,"\n*************************************************");
		//fprintf(stdout,"\n****[iter: %d], from %d, to %d => len = %d, \ntext => []",numPat-num_pos,from,to,readen,text); //shows it			
		fprintf(stdout,"\n****[iter: %lu], from %lu, to %lu => len = %lu, \ntext => [%s]",numPat-num_pos,from,to,readen,text); //shows it			
		fprintf(stdout,"\n*************************************************\n");
	}
	
		#ifdef LOGFILE
		{ /**fari's verbose style */
			fprintf(logFile,"\n*************************************************");
			fprintf(logFile,"\n****[iter: %lu], from %lu, to %lu => len = %lu, \ntext => [%s]",numPat-num_pos,from,to,readen,text); //shows it			
			fprintf(logFile,"\n*************************************************\n");
		}		
		#endif	

		num_pos--;
		free(text);
	}
	
	fprintf (stderr, "Total num chars extracted = %lu\n", tot_ext);
	fprintf (stderr, "Extract time = %.6f secs\n", tot_time);
	fprintf (stderr, "Extract_time/Num_chars_extracted = %.6f msecs/chars\n\n", (tot_time * 1000) / tot_ext);
	fprintf (stderr, "(Load_time+Extract_time)/Num_chars_extracted = %.6f msecs/chars\n\n",((Load_time+tot_time) * 1000) / tot_ext);
	
	fprintf (timeFile, "\n\t ** Prog = %s ** \n", progname);
	fprintf (timeFile, "\t** Operation = %c **,  numchars = %lu, numIntervals = %d \n", operation, numchars, npatterns); 	
	fprintf (timeFile, "\tIndex size = %lu bytes, ratio = %3.3f\n", Index_size, (double)Index_size/(double)Text_length*100);
	
	fprintf (timeFile, "\tTotal num chars extracted = %lu\n", tot_ext);
	fprintf (timeFile, "\tExtract time = %.6f secs\n", tot_time);
	fprintf (timeFile, "\tExtract_time/Num_chars_extracted = %.6f msecs/chars\n\n", (tot_time * 1000) / tot_ext);
	fprintf (timeFile, "\t(Load_time+Extract_time)/Num_chars_extracted = %.6f msecs/chars\n\n",((Load_time+tot_time) * 1000) / tot_ext);
		
}


/**** WORD ORIENTED !**********************************************************************************/
/**** WORD ORIENTED !**********************************************************************************/
/**** WORD ORIENTED !**********************************************************************************/

/// ************************************************************

void
do_locate_word (ulong wordsbefore)
{
	int error = 0;
	ulong numocc, length, *occ, tot_numocc;
	uchar *pattern;
		
	double time, tot_time = 0;
	
	ulong numpatt,wordsPerPhrase;   //number of patterns and number of words per phrase.	
	uchar **words;
	ulong kk,i;

	uchar ** patterns; ulong *lens;
	loadPatterns(&patterns, &lens, &numpatt, &wordsPerPhrase);
	npatterns=numpatt;
		
	#ifdef WITHREPETITIONS
	fprintf(stderr,"\n** locateWord: repetitions = %d\n",REPETITIONSLOCATE);
	#endif
	double time0 = getTime ();

	#ifdef WITHREPETITIONS
	int j;
	for (j=0;j<REPETITIONSLOCATE;j++) {
		//fprintf(stderr,"\n iter %d of %d",j,REPETITIONSLOCATE);
	#endif
		tot_numocc=0;
		for (i=0;i<numpatt;i++) {	
			pattern=patterns[i];
			length = lens[i];
																		   
			/* Locate */
			time = getTime ();
			error =	locateWord (Index, pattern, length, &occ, &numocc,wordsbefore);
			IFERROR (error);
			tot_time += (getTime () - time);
			tot_numocc += numocc;		
			
			if (Verbose) {
				fwrite(&length, sizeof(length), 1, stdout);
				fwrite(pattern, sizeof(*pattern), length, stdout);
				fwrite(&numocc, sizeof(numocc), 1, stdout);
				fwrite(occ, sizeof(*occ), numocc, stdout);
			}

			#ifdef LOGFILE
				{
				fwrite(&length, sizeof(length), 1, logFile);
				fwrite(pattern, sizeof(*pattern), length, logFile);
				fwrite(&numocc, sizeof(numocc), 1, logFile);
				fwrite(occ, sizeof(*occ), numocc, logFile);
				}		
			#endif	
							
			if (numocc) free (occ);			
		}

   #ifdef WITHREPETITIONS
	}
   #endif

	tot_time /= (double) REPETITIONSLOCATE;   
	fprintf(stderr,"\n TOTAL time = %f %s\n",(getTime()-time0)* SEC_TIME_DIVIDER/REPETITIONSLOCATE , SEC_TIME_UNIT);
	
	//Show stats.
	fprintf (stderr, "Total Num occs found = %lu\n", tot_numocc);
	fprintf (stderr, "Locate time = %.6f %s\n", (tot_time * SEC_TIME_DIVIDER) ,SEC_TIME_UNIT);
	fprintf (stderr, "Locate_time/Num_occs = %.6f %s/occss\n", (tot_time * MSEC_TIME_DIVIDER) / tot_numocc, MSEC_TIME_UNIT);
	fprintf (stderr, "(Load_time+Locate_time)/Num_occs = %.6f %s/occs\n\n", ((tot_time+Load_time) * MSEC_TIME_DIVIDER) / tot_numocc , MSEC_TIME_UNIT);

	fprintf (timeFile, "\n\t ** Prog = %s ** \n", progname);
	fprintf (timeFile, "\tIndex size = %lu bytes, ratio = %3.3f\n", Index_size, (double)Index_size/(double)Text_length*100);
	fprintf (timeFile, "\t** Operation = %c ** numpatts = %d, wordsperphrase=%lu, wordsbefore = %lu\n", operation, npatterns,wordsPerPhrase, wordsbefore);
	
	fprintf (timeFile, "\tTotal Num occs found = %lu\n", tot_numocc);
	fprintf (timeFile, "\tLocate time = %.6f %s\n", (tot_time * SEC_TIME_DIVIDER) ,SEC_TIME_UNIT);
	fprintf (timeFile, "\tLocate_time/Num_occs = %.6f %s/occss\n", (tot_time * MSEC_TIME_DIVIDER) / tot_numocc, MSEC_TIME_UNIT);
	fprintf (timeFile, "\t\t(Load_time+Locate_time)/Num_occs = %.6f  %s/occs\n\n", ((tot_time+Load_time) * MSEC_TIME_DIVIDER) / tot_numocc , MSEC_TIME_UNIT);	

	for (i=0;i<numpatt; i++) {		
		free(patterns[i]);   
	}
	free(patterns);
	free(lens);
}


/// ************************************************************



void do_display_word (ulong numc, ulong wordsbefore) {
	

	int error = 0;
	ulong numocc, length, i, *snippet_len, tot_numcharext = 0;
	double time, tot_time = 0;
	uchar *pattern, *snippet_text;

	ulong numpatt, numpatttmp, wordsPerPhrase;   //number of patterns and number of words per phrase.	
	uchar **words;
	ulong kk;
  
  ulong totalocurrences =0;

	readNumPatterns(&numpatt,&wordsPerPhrase);  //number of patterns and number of words per phrase.
	npatterns=numpatt;
	
	double time0 = getTime ();

	while (numpatt) {
	
		pattern = inputWord(&length);                                                                   
		//fprintf(stderr,"\n leido pattron <<%s>> con len = %d\n",pattern,length);fflush(stderr);     
		        

		/* Display Word-based */
		time = getTime ();
		error =	displayWords (Index, pattern, length, numc, &numocc, 
				    	 &snippet_text, &snippet_len,wordsbefore);
		totalocurrences += numocc;
		
		tot_time += (getTime () - time);
		IFERROR (error);
		
// 		if (Verbose) {/**pizza-chile's verbose style */
// 			ulong j, len = length + 2*numc;
// 		    char blank = '\0';
// 			fwrite(&length, sizeof(length), 1, stdout);
// 			fwrite(pattern, sizeof(*pattern), length, stdout);
// 			fwrite(&numocc, sizeof(numocc), 1, stdout);
// 			fwrite(&len, sizeof(len), 1, stdout);
// 		
// 			for (i = 0; i < numocc; i++){
// 				fwrite(snippet_text+len*i,sizeof(uchar),snippet_len[i],stdout);
// 				fprintf(stdout,"\n");fflush(stdout);
// 				for(j=snippet_len[i];j<len;j++)
// 				   fwrite(&blank,sizeof(uchar),1,stdout);
// 			}
// 		}

		if (Verbose) { /**fari's verbose style */
			ulong j, len = length + 2*numc;
		    char blank = '\0';		
			fprintf(stdout,"\n*************************************************");
			fprintf(stdout,"\n****[iter: %lu], Pattern: %s, numOcurr =%lu",numpatt,pattern,numocc);
			fprintf(stdout,"\n*************************************************\n");
			for (i = 0; i < numocc; i++){
				fprintf(stdout,"[[%lu, snip len = %lu]]",i, snippet_len[i]);
				fwrite(snippet_text+len*i,sizeof(uchar),snippet_len[i],stdout);
				fprintf(stdout,"\n");fflush(stdout);
				for(j=snippet_len[i];j<len;j++)
				   fwrite(&blank,sizeof(uchar),1,stdout);
			}
		}

		#ifdef LOGFILE
			{ /**fari's verbose style */
			ulong j;
			//ulong len = 2*numc;
			ulong len = length + 2*numc;
		    char blank = '\0';		
			fprintf(logFile,"\n*************************************************");
			fprintf(logFile,"\n****[iter: %lu], Pattern: %s, numOcurr =%lu",numpatt,pattern,numocc);
			fprintf(logFile,"\n*************************************************\n");
			for (i = 0; i < numocc; i++){
				fprintf(logFile,"[[%lu, snip len = %lu]]",i, snippet_len[i]);
				fwrite(snippet_text+len*i,sizeof(uchar),snippet_len[i],logFile);
				fprintf(logFile,"\n");fflush(logFile);
				for(j=snippet_len[i];j<len;j++)
				   fwrite(&blank,sizeof(uchar),1,logFile);
			}
			}		
		#endif	
		
		numpatt--;
		
		for(i=0; i<numocc; i++) {
			tot_numcharext += snippet_len[i];			
		}
			
		if (numocc) {
			free (snippet_len);
			free (snippet_text);
		}
		free(pattern);   

	}

	fprintf (stderr, "\nTotal ocurrences = %lu\n", totalocurrences);
	fprintf (stderr, "Total num chars extracted = %lu\n", tot_numcharext);
	fprintf (stderr, "Display time = %.6f secs\n", tot_time);
	fprintf (stderr, "Time_display/Num_occs      = %.8f msec/occss\n", (tot_time * 1000) / numocc);		
	fprintf (stderr, "Time_display/Tot_num_chars = %.6f msecs/chars\n\n", (tot_time*1000) / tot_numcharext);
	fprintf (stderr, "(Load_time+Time_display)/Tot_num_chars = %.6f msecs/chars\n\n", ((Load_time+tot_time)*1000) / tot_numcharext);

	fprintf (timeFile, "\n\t ** Prog = %s ** \n", progname);
	fprintf (timeFile, "\t** Operation = %c **, numc=%lu, wordsbefore = %lu, numpatts = %d, wordsperphrase=%lu\n", operation, numc, wordsbefore, npatterns,wordsPerPhrase); 
	fprintf (timeFile, "\tIndex size = %lu bytes, ratio = %3.3f\n", Index_size, (double)Index_size/(double)Text_length*100);
	
	fprintf (timeFile, "\n\tTotal ocurrences = %lu\n", totalocurrences);
	fprintf (timeFile, "\tTotal num chars extracted = %lu\n", tot_numcharext);
	fprintf (timeFile, "\tDisplay time = %.6f secs\n", tot_time);
	fprintf (timeFile, "\tTime_display/Num_occs      = %.8f msec/occss\n", (tot_time * 1000) / numocc);	
	fprintf (timeFile, "\tTime_display/Tot_num_chars = %.6f msecs/chars\n\n", (tot_time*1000) / tot_numcharext);
	fprintf (timeFile, "\t(Load_time+Time_display)/Tot_num_chars = %.6f msecs/chars\n\n", ((Load_time+tot_time)*1000) / tot_numcharext);

}


/** simulates extration of text process, but do not actually returns anything at all 
  * Extracts upto <=2K words from K=wordsbefore words before each occurrence of a pattern.
  * Less than 2K words can be extracted if more than numc characters have been already obtained.
  * Do nothing else... do not return the text **/
void do_display_kWords (ulong numc, ulong wordsbefore){

	int error = 0;
	ulong numocc, length, i, tot_numcharext = 0;
	double time, tot_time = 0;

	uchar *pattern;
	ulong numpatt; 
	uchar ** patterns;
	ulong *lens;
	ulong numCharsExtracted;

	ulong wordsPerPhrase;
	readNumPatterns(&numpatt,&wordsPerPhrase);  //number of patterns and number of words per phrase.
	npatterns=numpatt;
		
	patterns = (uchar **) malloc (sizeof(uchar*) * numpatt);
	lens = (ulong *) malloc (sizeof(ulong) * numpatt);

	i=0;
	while (i<numpatt) {
		patterns[i] = inputWord(&length); 
		lens[i]=length;
		i++;
	}

	fflush(stdout);fflush(stderr);
	fprintf (stderr,"\n** Starting display_k_words search over %lu patterns,",numpatt);
	fprintf (stderr,"\n** and extracting = 2*%lu words and max %lu bytes per snippet",wordsbefore,numc);
	#ifdef WITHREPETITIONS
	fprintf (stderr,"\n** #display_k_words: repetitions = %d\n",REPETITIONSDISPLAYKWORDS);		
	#endif

	time = getTime ();

	#ifdef WITHREPETITIONS
	int j;
	for (j=0;j<REPETITIONSDISPLAYKWORDS;j++) {
		//fprintf(stderr,"\n iter %d of %d",j,REPETITIONSDISPLAYKWORDS);			
	#endif
		numCharsExtracted=0;				
		for (i=0; i<numpatt; i++) {
			pattern=patterns[i];
			length = lens[i];
			/* Display */
			numCharsExtracted += displayTextOcurrencesNoShow(Index, pattern, length, wordsbefore, numc);	 
		}
		
	#ifdef WITHREPETITIONS
	}
	#endif

	tot_time = (getTime () - time) / ((double)REPETITIONSDISPLAYKWORDS);

	for (i=0;i<numpatt; i++) {		
		free(patterns[i]);   
	}
	free(patterns);
	free(lens);

	tot_numcharext=numCharsExtracted;

	fprintf (stderr, "\nDisplay [%lu chars], time = %.6f %s\n", numCharsExtracted, (tot_time * SEC_TIME_DIVIDER) ,SEC_TIME_UNIT);
	fprintf (stderr, "\tDisplay_time/Tot_num_chars = %.6f %s/chars\n", (tot_time*MSEC_TIME_DIVIDER) / tot_numcharext, MSEC_TIME_UNIT);
	fprintf (stderr, "\t(Load_time+Time_display)/Tot_num_chars = %.6f %s/chars\n\n", ((Load_time+tot_time)*MSEC_TIME_DIVIDER) / tot_numcharext,  MSEC_TIME_UNIT);

	fprintf (timeFile, "\n\t ** Prog = %s ** \n", progname);	
	fprintf (timeFile, "\t** Operation = %c **, numc=%lu, wordsbefore = %lu, numpatts = %d, wordsperphrase=%lu\n", operation, numc, wordsbefore, npatterns,wordsPerPhrase); 
	fprintf (timeFile, "\tIndex size = %lu bytes, ratio = %3.3f\n", Index_size, (double)Index_size/(double)Text_length*100);
	fprintf (timeFile, "\n\tDisplay [%lu chars], time = %.6f %s\n", tot_numcharext,  (tot_time * SEC_TIME_DIVIDER) ,SEC_TIME_UNIT);
	fprintf (timeFile, "\tDisplay_time/Tot_num_chars = %.6f %s/chars\n", (tot_time*MSEC_TIME_DIVIDER) / numCharsExtracted, MSEC_TIME_UNIT);
	//fprintf (timeFile, "\t\t(Load_time+Locate_time)/Tot_num_chars = %.6f %s/chars\n\n", ((tot_time+Load_time) * MSEC_TIME_DIVIDER)  / numCharsExtracted, MSEC_TIME_UNIT);
     		
	}





/**** WORD ORIENTED !**********************************************************************************/
/**** WORD ORIENTED !**********************************************************************************/


void
do_extract_words()
{
	int error = 0;
	uchar *text, orig_file[257];
	ulong num_pos, from, to, numwords, readen, tot_ext = 0;
	double time, tot_time = 0;

	//error = fscanf(stdin, "# number=%lu numwords=%lu\n", &num_pos, &numwords);
	//fprintf(stderr,"\n  # number=%lu numwords=%lu \n", num_pos, numwords);

	error = fscanf(stdin, "# number=%lu numwords=%lu file=%s\n", &num_pos, &numwords, orig_file);	
	fprintf(stderr, "# number=%lu numwords=%lu file=%s\n", num_pos, numwords, orig_file);	
		npatterns=num_pos;	
	
	
	if (error != 3)
	{
		fprintf (stderr, "Error: Intervals file header is not correct\n");
		perror ("run_queries");
		exit (1);
	}

	uint numPat= num_pos;
	while(num_pos) {
	
		if(fscanf(stdin,"%lu,%lu\n", &from, &to) != 2) {
			fprintf(stderr, "Cannot read correctly intervals file\n");
			exit(1);
		}

		time = getTime();
		error = extractWords(Index, from, to, &text, &readen);
		
		IFERROR(error);
		tot_time += (getTime() - time);
	
		tot_ext += readen;
		
		//fprintf(stderr,"\n from %d, to %d => len = %d, \ntext => [%s]",from,to,readen," "); //shows it
		
/*		if (Verbose) {
			fwrite(&from,sizeof(ulong),1,stdout);
			fwrite(&readen,sizeof(ulong),1,stdout);
			fwrite(text,sizeof(uchar),readen, stdout);
		}
*/

		if (Verbose) { /**fari's verbose style */
	
			fprintf(stdout,"\n*************************************************");
			//fprintf(stdout,"\n****[iter: %d], from %d, to %d => len = %d, \ntext => []",numPat-num_pos,from,to,readen,text); //shows it			
			fprintf(stdout,"\n****[iter: %lu], from %lu, to %lu => len = %lu, \ntext => [%s]",numPat-num_pos,from,to,readen,text); //shows it			
			fprintf(stdout,"\n*************************************************\n");
		}
		
		
		#ifdef LOGFILE
			{ /**fari's verbose style */
			fprintf(logFile,"\n*************************************************");
			fprintf(logFile,"\n****[iter: %lu], from %lu, to %lu => len = %lu, \ntext => [%s]",numPat-num_pos,from,to,readen,text); //shows it			
			fprintf(logFile,"\n*************************************************\n");
			}		
		#endif			

		num_pos--;
		free(text);
	}
	
	fprintf (stderr, "Total num chars extracted = %lu\n", tot_ext);
	fprintf (stderr, "Extract time = %.6f secs\n", tot_time);
	fprintf (stderr, "Extract_time/Num_chars_extracted = %.6f msecs/chars\n\n",(tot_time * 1000) / tot_ext);
	fprintf (stderr, "(Load_time+Extract_time)/Num_chars_extracted = %.6f msecs/chars\n\n",((Load_time+tot_time) * 1000) / tot_ext);

/**/    fprintf (timeFile, "\n\t ** Prog = %s ** \n", progname);
/**///    fprintf (timeFile, "\t** Operation = %c ** \n", operation);
/**/    fprintf (timeFile, "\t** Operation = %c **,  numwords = %lu, intervals = %d \n", operation, numwords, npatterns); 	
	fprintf (timeFile, "\tIndex size = %lu bytes, ratio = %3.3f\n", Index_size, (double)Index_size/(double)Text_length*100);

/**/    fprintf (timeFile, "\tTotal num chars extracted = %lu\n", tot_ext);
/**/    fprintf (timeFile, "\tExtract time = %.6f secs\n", tot_time);
/**/    fprintf (timeFile, "\tExtract_time/Num_chars_extracted = %.6f msecs/chars\n\n",(tot_time * 1000) / tot_ext);
/**/    fprintf (timeFile, "\t(Load_time+Extract_time)/Num_chars_extracted = %.6f msecs/chars\n\n",((Load_time+tot_time) * 1000) / tot_ext);



	ulong tot_chars = tot_ext;

	/**/    char timeperdoc[50]; sprintf(timeperdoc,"n.d.");//, MSEC_TIME_UNIT);
	/**/    char timeperchar[50]; sprintf(timeperchar,"%5.9f", (tot_time * MSEC_TIME_DIVIDER *1000) / tot_chars);//, MSEC_TIME_UNIT);  // TIMES IN usec/char
	/**/    char cratio[50]; sprintf(cratio,"%3.3f", 100.0 * Index_size / Text_length);//, MSEC_TIME_UNIT);  // TIMES IN usec/char
	/**/	
	/**/
	fprintf(timeFile,"\n\n#%15s\t%10s\t%15s\t%15s\t%15s\t%15s\t%s\n","totaSize(byte)", "ratio (%)","msec/doc","usec/char","chars","ndocs",  "#indexBaseName");
	
	/**/	fprintf(timeFile," %15lu\t",Index_size);
	/**/	fprintf(timeFile,"%10s\t",cratio);
	/**/	fprintf(timeFile,"%15s\t",timeperdoc);
	/**/	fprintf(timeFile,"%15s\t",timeperchar);
	/**/	fprintf(timeFile,"%15lu\t",tot_chars);
	/**/	fprintf(timeFile,"%15s\t","n.d.");
	/**/	fprintf(timeFile,"#%s\n",indexbasename);
	fprintf(timeFile,"\n------------------------\n\n");
	

	/**/	fprintf(gnuFile," %15lu\t",Index_size);
	/**/	fprintf(gnuFile,"%10s\t",cratio);
	/**/	fprintf(gnuFile,"%15s\t",timeperdoc);
	/**/	fprintf(gnuFile,"%15s\t",timeperchar);
	/**/	fprintf(gnuFile,"%15lu\t",tot_chars);
	/**/	fprintf(gnuFile,"%15s\t","n.d.");
	/**/	fprintf(gnuFile,"#%s\n",indexbasename);






}












/** locate returning doc-wise results <occs><doc,off><doc,off>... */
void do_locate_docs (void)
{
	int error = 0;
	ulong length, tot_numocc;
	uint numocc, *occ;
	uchar *pattern;
		
	double time, tot_time = 0;
	
	ulong numpatt,wordsPerPhrase;   //number of patterns and number of words per phrase.	
	uchar **words;
	ulong kk,i;

	uchar ** patterns; ulong *lens;
	loadPatterns(&patterns, &lens, &numpatt, &wordsPerPhrase);
	npatterns=numpatt;
	
	uint minnw = wordsPerPhrase;
	uint maxnw = wordsPerPhrase;	
	minnw/=10000000;
	maxnw%=10000000;
	maxnw/=1000;	
		
	#ifdef WITHREPETITIONS
	fprintf(stderr,"\n** locate DocOriented: repetitions = %d\n",REPETITIONSLOCATE);
	#endif
	double time0 = getTime ();

	#ifdef WITHREPETITIONS
	int j;
	for (j=0;j<REPETITIONSLOCATE;j++) {
		//fprintf(stderr,"\n iter %d of %d",j,REPETITIONSLOCATE);
	#endif
		tot_numocc=0;
		for (i=0;i<numpatt;i++) {	
			pattern=patterns[i];
			length = lens[i];
																		   
			/* Locate */
			time = getTime ();
			error = LocateDocumentOffsets(Index, pattern, length, &occ, &numocc);
			IFERROR (error);
			tot_time += (getTime () - time);
			tot_numocc += numocc;		
			
			if (Verbose) {
				fwrite(&length, sizeof(length), 1, stdout);
				fwrite(pattern, sizeof(*pattern), length, stdout);
				fwrite(&numocc, sizeof(numocc), 1, stdout);
				fwrite(occ+1, sizeof(*occ), numocc*2, stdout);
			}

			#ifdef LOGFILE
				{
				//fwrite(&length, sizeof(length), 1, logFile);
				//fwrite(pattern, sizeof(*pattern), length, logFile);
				//fwrite(&numocc, sizeof(numocc), 1, logFile);
				//fwrite(occ+1, sizeof(*occ), numocc*2, logFile);
			fprintf(logFile,"\n ===========================================");
			fprintf(logFile,"\n #iter %lu of %lu: pattern = \"%s\" ",i, numpatt, pattern);
			fprintf(logFile,"\n    n-occs = %u ", numocc); 
			fprintf(logFile,"\n    showing pairs [doc,offset]: -->"); 
					uint z;
					for (z=0;z<numocc;z++) fprintf(logFile,"[%u,%u]", occ[z+numocc], occ[z]);
			fprintf(logFile,"\n ===========================================");
			fflush(logFile);
				}		
			#endif	
							
			if (numocc) free (occ);		
			fprintf(stderr, "Processing %.1f%%\r", (float)((i+1)+ (j*numpatt))/(numpatt*REPETITIONSLOCATE)*100);fflush(stderr);	
		}

   #ifdef WITHREPETITIONS
	}
   #endif
	fprintf(stderr, "Processing %.1f%%", (float)100); fflush(stderr);

	tot_time /= (double) REPETITIONSLOCATE;   
	fprintf(stderr,"\n TOTAL time = %f %s\n",(getTime()-time0)* SEC_TIME_DIVIDER/REPETITIONSLOCATE , SEC_TIME_UNIT);
	
	//Show stats.
	fprintf (stderr, "\tIndex size = %lu bytes, ratio = %3.3f\n", Index_size, (double)Index_size/(double)Text_length*100);
	fprintf (stderr, "\t** Operation = %c ** numpatts = %d, wordsperphrase=%lu: [%u-%u]\n", operation, npatterns,wordsPerPhrase, minnw,maxnw);

	fprintf (stderr, "Total Num occs found = %lu\n", tot_numocc);
	fprintf (stderr, "Locate time = %.6f %s\n", (tot_time * SEC_TIME_DIVIDER) ,SEC_TIME_UNIT);
	fprintf (stderr, "Locate_time/Num_occs = %.6f %s/occss\n", (tot_time * MSEC_TIME_DIVIDER) / tot_numocc, MSEC_TIME_UNIT);
	fprintf (stderr, "(Load_time+Locate_time)/Num_occs = %.6f %s/occs\n\n", ((tot_time+Load_time) * MSEC_TIME_DIVIDER) / tot_numocc , MSEC_TIME_UNIT);

	fprintf (timeFile, "\n\t ** Prog = %s ** \n", progname);
	fprintf (timeFile, "\tIndex size = %lu bytes, ratio = %3.3f\n", Index_size, (double)Index_size/(double)Text_length*100);
	fprintf (timeFile, "\t** Operation = %c ** numpatts = %d, wordsperphrase=%lu: [%u-%u]\n", operation, npatterns,wordsPerPhrase, minnw,maxnw);
	
	fprintf (timeFile, "\tTotal Num occs found = %lu\n", tot_numocc);
	fprintf (timeFile, "\tLocate time = %.6f %s\n", (tot_time * SEC_TIME_DIVIDER) ,SEC_TIME_UNIT);
	fprintf (timeFile, "\tLocate_time/Num_occs = %.6f %s/occss\n", (tot_time * MSEC_TIME_DIVIDER) / tot_numocc, MSEC_TIME_UNIT);
	fprintf (timeFile, "\t\t(Load_time+Locate_time)/Num_occs = %.6f  %s/occs\n\n", ((tot_time+Load_time) * MSEC_TIME_DIVIDER) / tot_numocc , MSEC_TIME_UNIT);	

fprintf(timeFile,"#%15s\t%15s\t%15s\t%15s\t%15s\t%s\n","totaSize(byte)","msec/pat","usec/occ","occs","numpats","#indexBaseName");
fprintf(timeFile,"\n##############################################################################\n\n");

	/**/    char timeperpat[50]; sprintf(timeperpat,"%5.9f", (tot_time * MSEC_TIME_DIVIDER) / npatterns);//, MSEC_TIME_UNIT);
	/**/    char timeperocc[50]; sprintf(timeperocc,"%5.9f", (tot_time * MSEC_TIME_DIVIDER *1000) / tot_numocc);//, MSEC_TIME_UNIT);  // TIMES IN usec/occ
	/**/	alighnnum(timeperpat,15);alighnnum(timeperocc,15);
	/**/	fprintf(timeFile," %15lu\t",Index_size);
	/**/	fprintf(timeFile,"%15s\t",timeperpat);
	/**/	fprintf(timeFile,"%15s\t",timeperocc);
	/**/	fprintf(timeFile,"%15lu\t",tot_numocc);
	/**/	fprintf(timeFile,"%15u\t",npatterns);
	/**/	fprintf(timeFile,"#%s\n",indexbasename);
	
	/**/	fprintf(gnuFile," %15lu\t",Index_size);
	/**/	fprintf(gnuFile,"%15s\t",timeperpat);
	/**/	fprintf(gnuFile,"%15s\t",timeperocc);
	/**/	fprintf(gnuFile,"%15lu\t",tot_numocc);
	/**/	fprintf(gnuFile,"%15u\t",npatterns);
	/**/	fprintf(gnuFile,"#%s\n",indexbasename);


	for (i=0;i<numpatt; i++) {		
		free(patterns[i]);   
	}
	free(patterns);
	free(lens);
}





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

	//for (i=0;i<nids;i++)
	//	printf("\n to extract doc = %u", docIds[i]);


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
	fprintf (stderr, "**size_Index= %lu bytes\n", Index_size);
	fprintf (stderr, "Total Num chars extracted found = %lu\n", tot_chars);
	fprintf (stderr, "Extract time = %.8f %s (numdocs = %u)\n", (tot_time * SEC_TIME_DIVIDER) ,SEC_TIME_UNIT, ndocs);
	fprintf (stderr, "Extract_time/chars = %.8f %s/char\n", (tot_time * MSEC_TIME_DIVIDER) / tot_chars, MSEC_TIME_UNIT);
	fprintf (stderr, "Extract_time/doc = %.8f %s/doc\n", (tot_time * MSEC_TIME_DIVIDER) / ndocs, MSEC_TIME_UNIT);
	fprintf (stderr, "(Load_time+Extract_time)/char = %.8f %s/char\n\n", ((tot_time+Load_time) * MSEC_TIME_DIVIDER) / tot_chars , MSEC_TIME_UNIT);

	char opname[25];
	sprintf(opname,"%s","AVG doc extrac time");
	

	fprintf (timeFile, "\t** Operation = %c ** \n", operation);
	fprintf (timeFile, "**size_Index= %lu bytes\n", Index_size);
	fprintf (timeFile, "Total Num chars extracted found = %lu\n", tot_chars);
	fprintf (timeFile, "Extract time = %.8f %s (numdocs = %u)\n", (tot_time * SEC_TIME_DIVIDER) ,SEC_TIME_UNIT, ndocs);
	fprintf (timeFile, "Extract_time/chars = %.8f %s/char\n", (tot_time * MSEC_TIME_DIVIDER) / tot_chars, MSEC_TIME_UNIT);
	fprintf (timeFile, "Extract_time/doc = %.8f %s/doc\n", (tot_time * MSEC_TIME_DIVIDER) / ndocs, MSEC_TIME_UNIT);
	fprintf (timeFile, "(Load_time+Extract_time)/char = %.8f %s/char\n\n", ((tot_time+Load_time) * MSEC_TIME_DIVIDER) / tot_chars , MSEC_TIME_UNIT);

	/**/    char timeperdoc[50]; sprintf(timeperdoc,"%5.9f", (tot_time * MSEC_TIME_DIVIDER) / ndocs);//, MSEC_TIME_UNIT);
	/**/    char timeperchar[50]; sprintf(timeperchar,"%5.9f", (tot_time * MSEC_TIME_DIVIDER *1000) / tot_chars);//, MSEC_TIME_UNIT);  // TIMES IN usec/char
	/**/    char cratio[50]; sprintf(cratio,"%3.3f", 100.0 * Index_size / Text_length);//, MSEC_TIME_UNIT);  // TIMES IN usec/char
	/**/	
	/**/
	fprintf(timeFile,"\n\n#%15s\t%10s\t%15s\t%15s\t%15s\t%15s\t%s\n","totaSize(byte)", "ratio (%)","msec/doc","usec/char","chars","ndocs",  "#indexBaseName");
	
	/**/	fprintf(timeFile," %15lu\t",Index_size);
	/**/	fprintf(timeFile,"%10s\t",cratio);
	/**/	fprintf(timeFile,"%15s\t",timeperdoc);
	/**/	fprintf(timeFile,"%15s\t",timeperchar);
	/**/	fprintf(timeFile,"%15lu\t",tot_chars);
	/**/	fprintf(timeFile,"%15u\t",ndocs);
	/**/	fprintf(timeFile,"#%s\n",indexbasename);
	fprintf(timeFile,"\n------------------------\n\n");
	

	/**/	fprintf(gnuFile," %15lu\t",Index_size);
	/**/	fprintf(gnuFile,"%10s\t",cratio);
	/**/	fprintf(gnuFile,"%15s\t",timeperdoc);
	/**/	fprintf(gnuFile,"%15s\t",timeperchar);
	/**/	fprintf(gnuFile,"%15lu\t",tot_chars);
	/**/	fprintf(gnuFile,"%15u\t",ndocs);
	/**/	fprintf(gnuFile,"#%s\n",indexbasename);


}







/***********************************************************************************************/

void usage(char * progname) {	
	fprintf(stderr, "\nThe program loads <index> and then executes over it the\n");
	fprintf(stderr, "queries it receives from the standard input. The standard\n");
	fprintf(stderr, "input comes in the format of the files written by \n");
	fprintf(stderr, "genpatterns or genintervals.\n");
	fprintf(stderr, "%s reports on the standard error time statistics\n", progname);
	fprintf(stderr, "regarding to running the queries.\n\n");
	fprintf(stderr, "Usage:  %s <index> <type> [options][V]\n", progname);
	fprintf(stderr, "\n\t<type>   denotes the type of queries:\n");
	fprintf(stderr, "\t         %c Online check for locateWords and displayWords;\n", TEST);
	fprintf(stderr, "\t         %c <numrepetitions> counting queries;\n", COUNT);
	fprintf(stderr, "\t\t    -> <numrepetitions> repeat process several times for \"timing\" purposes\n\n");
	fprintf(stderr, "\t         %c locating queries;\n\n", LOCATE);
	
	fprintf(stderr, "\t         %c <radix-num-char> displaying queries;\n", DISPLAY);	
	fprintf(stderr, "\t\t    -> <radix-num-char> denotes the number of characters to display\n");
	fprintf(stderr, "\t\t         before and after each pattern occurrence.\n\n");
		
	fprintf(stderr, "\t         %c extracting queries.\n\n", EXTRACT);
	
	fprintf(stderr, "\t         %c <wordsbefore> locating queries (word-oriented);\n", LOCATEWORD);
	fprintf(stderr, "\t\t    -> <wordsbefore> sets returned location -wordsbefore- word positions\n");
	fprintf(stderr, "\t\t         before each pattern occurrence.\n");
	fprintf(stderr, "\t\t    -> <numrepetitions> repeat process several times for \"timing\" purposes\n\n");
		
	fprintf(stderr, "\t         %c <radix-bytes> <wordsbefore>. displaying queries (word-oriented);\n", DISPLAYWORD);
	fprintf(stderr, "\t\t    ->will extract 2*radix-bytes + lenght(pattern) bytes starting\n");
	fprintf(stderr, "\t\t         extraction -wordsbefore- words before an occurrence.\n\n");
		
	fprintf(stderr, "\t         %c <maxnumc><wordsbefore><numrepetitions> displaying queries. Word-oriented display\n", DISPLAYKWORDS);
	fprintf(stderr, "\t           extracts up to 2*wordsbefore words, starting -wordsbefore- positions\n");
	fprintf(stderr, "\t           before each occurrence. The max number of chars extracted is maxnumc;\n");	
	fprintf(stderr, "\t\t    -> <numrepetitions> repeat process several times for \"timing\" purposes\n");

	
	fprintf(stderr, "\t         %c OFFSETS doc oriented (locate returning <doc,off> pairs \n", OFFSETS_DOC_LOCATE);
	fprintf(stderr, "\t         %c MEASURE_AVG EXTRACT_TIME operation;\n", MEASURE_EXTRACT_TIME);
	
	fprintf(stderr, "\n\t[V]      with this option it reports on the standard output\n");
	fprintf(stderr, "\t         the results of the queries. The results file should be\n");
	fprintf(stderr, "\t         compared with trusted one by compare program.\n\n");
}



/***********************************************************************************************/
/* some auxiliar functions to deal with patterns and obtain the name of the search-program*/
void readNumPatterns (ulong *numPhrase, ulong *numOfWords) {
	fscanf(stdin,"#numberOfFrases::%9lu, #NumberOfWordsPerPhrase:: %9lu\n",numPhrase, numOfWords);
	fprintf(stderr," numOfFrase = %9lu and numberOfWordsPerPhrase = %9lu",*numPhrase, *numOfWords);
}

//reads a phrase (a phrase composed of n words)
uchar *inputWord( ulong *len) {
	int tmp; //skips one integer.
	int tmplen;
	uchar *phrase;
	
	
	//fscanf(stdin,"%9d %9d ", &tmp, len);	
	fscanf(stdin,"%9d %9d ", &tmp, &tmplen);
	*len = (ulong) tmplen;
	phrase = (uchar * ) malloc (sizeof(char) * ((*len)+1));
	fread(phrase,sizeof(uchar), *len,stdin);
	phrase[*len] = '\0';	
	return phrase;	
}

void loadPatterns(uchar ***patterns,ulong **lens, ulong *numpatt, ulong *numwordsperpattern) {
	
	ulong i;
	readNumPatterns(numpatt,numwordsperpattern);  //number of patterns and number of words per phrase.
	
	
	uchar **pats;
	ulong length, *thelens;
	pats = (uchar **) malloc (sizeof(uchar *) * (*numpatt));
	thelens = (ulong *) malloc (sizeof(ulong) * (*numpatt));
	
	for (i=0;i< (*numpatt);i++) {
		pats[i] = inputWord(&length);
		thelens[i]= length;
	}
	*patterns = pats;
	*lens=thelens; 
}

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



/*********************************************************************************************/

double getTime (void)
{
	double usertime, systime;
	struct rusage usage;

	getrusage (RUSAGE_SELF, &usage);
	usertime = (double) usage.ru_utime.tv_sec * GET_TIME_DIVIDER +
		(double) usage.ru_utime.tv_usec / ( ((double) 1000000.0) / GET_TIME_DIVIDER);
		//fprintf(stderr,"\n in gettime: time is %f+%f =%f\n",
		//	usage.ru_utime.tv_sec , usage.ru_utime.tv_usec, usage.ru_utime.tv_sec + usage.ru_utime.tv_usec);

		//fprintf (stderr,"<%ld::%06ld>\n", usage.ru_utime.tv_sec, usage.ru_utime.tv_usec);

	systime = (double) usage.ru_stime.tv_sec * GET_TIME_DIVIDER +
		(double) usage.ru_stime.tv_usec / ( ((double)1000000.0) / GET_TIME_DIVIDER);
	//return (usertime + systime);
	return (usertime );
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
