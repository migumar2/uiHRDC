/*
 * Run Queries
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "interface.h"

/* only for getTime() */
#include <sys/time.h>
#include <sys/resource.h>

#define COUNT 		('C')
#define EXTRACT 	('E')
#define EXTRACTNOMALLOC 	('e')
#define VERBOSE 	('V')


//#define REPETITIONSCOUNT 1000


/* macro to detect and notify errors  */
#define IFERRORIL(error) {{if (error) { fprintf(stderr, "%s\n", error_il(error)); exit(1); }}}

/**/ FILE *timeFile;

/* local headers */
void do_count (void);
void do_extract(uint numIdsToExtract, uint REPETITIONSCOUNT);
void do_extract_no_malloc(uint numIdsToExtract, uint REPETITIONSCOUNT);

double getTime (void);
void print_search_usage(char * progname);
  		
  		
  		
static void *Index;	 /* opauque data type */
static int Verbose = 0; 
static uint Index_size, Uncompressed_Index_size ;
static double Load_time;



//Reads the Sets of phrase-patterns that can be searched for.
void readPatternSetsIntersect (uint *numIdsPerPattern, uint *numOfPatterns, uint ***patternSets) {
	fscanf(stdin,"##numIdsPerPattern: %9d\n",numIdsPerPattern);
	fscanf(stdin,"##numOfPatterns : %9d\n",numOfPatterns);
	fprintf(stderr," numIdsPerPattern = %9d and numOfPatterns = %9d",*numIdsPerPattern, *numOfPatterns);
	uint **V;
	uint i,j,value;
	V = (uint **) malloc(sizeof(uint **) * 1);
	for (i=0; i< *numOfPatterns; i++) {
		V[i] = (uint *) malloc (sizeof(uint) * (*numIdsPerPattern));
		for (j=0;j< (*numIdsPerPattern);j++) {
			fscanf(stdin,"%9d\n",&value);
			V[i][j]=value; 	
		}	
	}
	*patternSets = V;
}


void freePatternSetsIntersect(uint **patternSets, uint numOfPatterns) {
	uint i;
	for (i=0;i<numOfPatterns;i++) free(patternSets[i]);
	free(patternSets);
}


//Reads the Sets of ids that can be extracted.
void readPatternSetsExtract (uint *numOfIDs, uint **IDs) {
	uint numIdsPerPattern, numOfPatterns;
	
	fscanf(stdin,"##numIdsPerPattern: %9d\n",&numIdsPerPattern);
	fscanf(stdin,"##numOfPatterns : %9d\n",&numOfPatterns);
	*numOfIDs = numIdsPerPattern * numOfPatterns;
	
	uint *V;
	uint i,value;
	V = (uint *) malloc(sizeof(uint ) * (*numOfIDs) );
	for (i=0; i< (*numOfIDs); i++) {fscanf(stdin,"%9d\n",&value); V[i]=value; }
	*IDs = V;

	fprintf(stderr," !!!! numIdsPerPattern = %9d and numOfPatterns = %9d",numIdsPerPattern, numOfPatterns);
	fprintf(stderr,"##numOfIDS to extract: %9d\n",*numOfIDs);
	fflush(stderr);

}


void freePatternSetsExtract(uint *IDs) {
	free(IDs);
}


/*
 * Temporary usage: run_queries <index file> <type> [length] [V]
 */
int main (int argc, char *argv[])
{
	int error = 0;
	char *filename;
	char querytype;
	
	if (argc < 3)	{
		print_search_usage(argv[0]);	
		exit (1);
	}
	
	filename = argv[1];
	querytype = *argv[2];

/**/	timeFile = fopen("timesPLAINssa.txt","a");
/**/	fprintf(timeFile,"\n Search Plain+SSA: %s",argv[1]);

	
	Load_time = getTime ();
	error = load_il (filename, &Index);
	IFERRORIL (error);
	Load_time = getTime () - Load_time;
	fprintf (stderr, "Load index time = %.2f secs\n", Load_time);


	error = size_il(Index, &Index_size);
	IFERRORIL (error);
	error = size_il_uncompressed(Index, &Uncompressed_Index_size);
	IFERRORIL (error);
	Index_size /=1024;
	fprintf(stderr,"Index size = %lu Kb\n", Index_size);

	switch (querytype){
		case COUNT:
			if (argc > 3) 
				if (*argv[3] == VERBOSE) {
					Verbose = 1;
					fprintf(stdout,"%c", COUNT);
				}				
			do_count();
			break;
//		case LOCATE:
//			if (argc > 3) 
//				if (*argv[3] == VERBOSE) {
//					Verbose = 1;
//					fprintf(stderr,"%c", LOCATE);
//				}
//			do_locate();
//			break;	
		case EXTRACT:
			if (argc < 5) {
				print_search_usage(argv[0]);	
				exit (1);
			}
			if (argc > 5) 
				if (*argv[5] == VERBOSE){
						Verbose = 1;
						fprintf(stderr,"%c", EXTRACT);

				}
			do_extract((uint) atoi(argv[3]), (uint) atoi(argv[4]) );
			break;

		case EXTRACTNOMALLOC:
			if (argc < 5) {
				print_search_usage(argv[0]);	
				exit (1);
			}
			if (argc > 5) 
				if (*argv[5] == VERBOSE){
						Verbose = 1;
						fprintf(stderr,"%c", EXTRACTNOMALLOC);

				}
			do_extract_no_malloc((uint) atoi(argv[3]), (uint) atoi(argv[4]) );
			break;

						
		default:
			fprintf (stderr, "Unknow option: main ru\n");
			exit (1);
	}

	error = free_il(Index);
	IFERRORIL(error);

/**/ fclose(timeFile);

	return 0;
}


	
	
	
void
do_count ()
{
//	int error = 0;
//	ulong numocc, length, tot_numocc = 0, res_patt;
//	double time, tot_time = 0;
//	uchar *pattern;
//
//	
//	ulong numpatt, numpatttmp, wordsPerPhrase;   //number of patterns and number of words per phrase.	
//	uchar **words;
//	ulong kk,i;
//
//
//	readNumPatterns(&numpatt,&wordsPerPhrase);  //number of patterns and number of words per phrase.
//	numpatttmp = numpatt;
//	double time0 = getTime ();
//
//uint iters=0;
//	while (numpatt) {
//	
//		pattern = inputWord(&length); 		                                                                  
//		//fprintf(stderr,"\n leido pattron <<%s>> con len = %d\n",pattern,length);fflush(stderr);     
//		        
//		//skips the "separate words of the pattern"
//		//words = (uchar **) inputSeparateWords (wordsPerPhrase);                                                                                        
//
//		/* Count */
//		time = getTime ();
//		
//		
//		for (iters=0;iters<REPETITIONSCOUNT;iters++) {
//		error = count (Index, pattern, length, &numocc);
//
//		}
//		
//		
//		tot_time += (getTime () - time)/iters;
//		tot_numocc += numocc;
//
//		IFERROR (error);	
//		if (Verbose) {
//			fwrite(&length, sizeof(length), 1, stdout);
//			fwrite(pattern, sizeof(*pattern), length, stdout);
//			fwrite(&numocc, sizeof(numocc), 1, stdout);
//			}		
//		numpatt--;
//
//	free (pattern);
//	//freeSeparateWords (words, wordsPerPhrase); 
//	}
//
//	fprintf (stderr, "Total Num occs found = %lu\n", tot_numocc);
//	fprintf (stderr, "Count time = %.4f msecs\n", tot_time*1000);
//	fprintf (stderr, "Count_time/Pattern_chars = %.4f msecs/chars\n",
//		 (tot_time * 1000) / (length * numpatttmp));
//	fprintf (stderr, "Count_time/Num_patterns = %.4f msecs/patterns\n\n",
//		 (tot_time * 1000) / numpatttmp);
//	fprintf (stderr, "(Load_time+Count_time)/Pattern_chars = %.4f msecs/chars\n",
//		 ((Load_time+tot_time) * 1000) / (length * numpatttmp));
//	fprintf (stderr, "(Load_time+Count_time)/Num_patterns = %.4f msecs/patterns\n\n",
//		 ((Load_time+tot_time) * 1000) / numpatttmp);	
//                                                                           
}




void
do_locate ()
{
//	int error = 0;
//	ulong numocc, length, *occ, tot_numocc = 0;
//	uchar *pattern;
//	
//	
//	double time, tot_time = 0;
//	
//	ulong numpatt,wordsPerPhrase;   //number of patterns and number of words per phrase.	
//	uchar **words;
//	ulong kk,i;
//
//
////*	pfile_info (&length, &numpatt);    //lee el n?mero de elementos y su longitud. (todos misma longitud)
//	readNumPatterns(&numpatt,&wordsPerPhrase);  //number of patterns and number of words per phrase.
//double time0 = getTime ();
//	while (numpatt) {
//	
//		pattern = inputWord(&length);                                                                   
//		//fprintf(stderr,"\n leido pattron <<%s>> con len = %d\n",pattern,length);fflush(stderr);     
//		        
//		//skips the "separate words of the pattern"
//		//words = (uchar **) inputSeparateWords (wordsPerPhrase);                                                                                        
//
//
//		/* Locate */
//		time = getTime ();
//		
//		error =	locate (Index, pattern, length, &occ, &numocc);
//		//error =	locateWord (Index, pattern, length, &occ, &numocc,0);
//		tot_time += (getTime () - time);
//
//		IFERROR (error);
//
//	//	fprintf(stderr,"\n locate = %5d ocurrs for <<%s>>",numocc,pattern);	fflush(stderr);	
//
//				//	{int i;
//				//		fprintf(stderr,"\n*** %s appears in the source text in positions:\n\t",pattern);
//				//		for (i=0;i<numocc;i++) 
//				//			fprintf(stderr,"[%u]",occ[i]);
//				//		fflush(stderr);
//				//	}
//	//fprintf(stderr,"\n");
//	
//		tot_numocc += numocc;
//		numpatt--;
//		
//		if (Verbose) {
//			fwrite(&length, sizeof(length), 1, stdout);
//			fwrite(pattern, sizeof(*pattern), length, stdout);
//			fwrite(&numocc, sizeof(numocc), 1, stdout);
//			fwrite(occ, sizeof(*occ), numocc, stdout);
//			}
//			
//		if (numocc) free (occ);
//			
//		free(pattern);   
//		//freeSeparateWords (words, wordsPerPhrase); 			
//	}
//	
//	
//	fprintf(stderr,"\n TOTAL DE TOTAL !! = %f",getTime()-time0);
//
//	fprintf (stderr, "Total Num occs found = %lu\n", tot_numocc);
//	fprintf (stderr, "Locate time = %.2f secs\n", tot_time);
//	fprintf (stderr, "Locate_time/Num_occs = %.4f msec/occss\n\n", (tot_time * 1000) / tot_numocc);
//	fprintf (stderr, "(Load_time+Locate_time)/Num_occs = %.4f msecs/occs\n\n", ((tot_time+Load_time) * 1000) / tot_numocc);
//
///**/ fprintf (timeFile, "\tTotal Num occs found = %lu\n", tot_numocc);
///**/ fprintf (timeFile, "\tLocate time = %.4f secs\n", tot_time);
///**/ fprintf (timeFile, "\tLocate_time/Num_occs = %.4f msec/occss\n", (tot_time * 1000) / tot_numocc);
///**/ fprintf (timeFile, "\t\t(Load_time+Locate_time)/Num_occs = %.4f msecs/occs\n\n", ((tot_time+Load_time) * 1000) / tot_numocc);
//	                                                                           

}

 

void
do_extract(uint numIdsToExtract, uint REPETITIONSCOUNT)
{
	uint i,iters,currID;
	
	int error = 0;
	double time, tot_time = 0;

	uint *IDs; uint numOfIDs;

	readPatternSetsExtract (&numOfIDs, &IDs); //max number of ids to extract 
	
	fprintf(stderr,"\n IDS = ");
	for (i=0;i<numOfIDs; i++) {fprintf(stderr,"[%d]", IDs[i]);}
	fprintf(stderr,"\n");
	
	if (numIdsToExtract > numOfIDs) {
		fprintf(stderr,"\nBuild_il: Error during extract %ld/%ld IDs\n", numIdsToExtract, numOfIDs);
		exit(0);
	}
	
	numOfIDs = numIdsToExtract;
	double time0 = getTime ();

	uint *occs;  uint numocc=0; uint tot_numocc=0;
	
	iters=0;
	currID=0;
	for (currID=0; currID<numOfIDs; currID++) {
	
		fprintf(stderr,"\n to extract List of ID <<%ld>> \n", IDs[currID]);
		fflush(stderr);
		        
		/* extract */
		time = getTime ();
				
		for (iters=0;iters<REPETITIONSCOUNT;iters++) {
			if (numocc) free (occs);
			//error = count (Index, pattern, length, &numocc);
			error =  extractList_il (Index, IDs[currID], &occs, &numocc); 
			//for (i=0;i< 100000;i++);
		}

		tot_time += (getTime () - time)/REPETITIONSCOUNT;
		tot_numocc += numocc;

		IFERRORIL (error);	
		if (Verbose) {
			fwrite(&(IDs[currID]), sizeof(uint), 1, stdout);
			fwrite(&numocc, sizeof(numocc), 1, stdout);
			}		
			
		if (numocc) free (occs);
		numocc=0;
		
	}

	freePatternSetsExtract(IDs);

	fprintf (stderr, "Total Num occs found = %lu\n", tot_numocc);
	fprintf (stderr, "extract time = %.8f msecs\n", tot_time*1000);
	fprintf (stderr, "extract_time/Num_IDs = %.8f msecs/ID\n\n",
		 (tot_time * 1000) / numOfIDs);
	fprintf (stderr, "(Load_time+extract_time)/Num_IDs = %.8f msecs/ID\n\n",
		 ((Load_time+tot_time) * 1000) / numOfIDs);	

	fprintf (stderr, "extract_time/occ = %.8f msecs per post \n\n",
		 (tot_time * 1000) / tot_numocc);   
}




void
do_extract_no_malloc(uint numIdsToExtract, uint REPETITIONSCOUNT)
{
	uint i,iters,currID;
	
	int error = 0;
	double time, tot_time = 0;

	uint *IDs; uint numOfIDs;

	readPatternSetsExtract (&numOfIDs, &IDs); //max number of ids to extract 
	
	fprintf(stderr,"\n IDS = ");
	for (i=0;i<numOfIDs; i++) {fprintf(stderr,"[%d]", IDs[i]);}
	fprintf(stderr,"\n");
	
	if (numIdsToExtract > numOfIDs) {
		fprintf(stderr,"\nBuild_il: Error during extract %ld/%ld IDs\n", numIdsToExtract, numOfIDs);
		exit(0);
	}
	
	numOfIDs = numIdsToExtract;
	double time0 = getTime ();

	uint *occs;  uint numocc=0; uint tot_numocc=0;
	
	uint maxNumOccs;
	maxSizePosting_il(Index,  &maxNumOccs);
	occs = (uint *) malloc (sizeof(uint) * maxNumOccs);
	
	iters=0;
	currID=0;
	for (currID=0; currID<numOfIDs; currID++) {
	
		fprintf(stderr,"\n to extract List of ID <<%ld>> \n", IDs[currID]);
		fflush(stderr);
		        
		/* extract */
		time = getTime ();
				
		for (iters=0;iters<REPETITIONSCOUNT;iters++) {
			error =  extractListNoMalloc_il (Index, IDs[currID], occs, &numocc); 
		}

		tot_time += (getTime () - time)/REPETITIONSCOUNT;
		tot_numocc += numocc;

		IFERRORIL (error);	
		if (Verbose) {
			fwrite(&(IDs[currID]), sizeof(uint), 1, stdout);
			fwrite(&numocc, sizeof(numocc), 1, stdout);
			}		
					
	}

	if (maxNumOccs) free (occs);

	freePatternSetsExtract(IDs);

	fprintf (stderr, "Total Num occs found = %lu\n", tot_numocc);
	fprintf (stderr, "extract time = %.8f msecs\n", tot_time*1000);
	fprintf (stderr, "extract_time/Num_IDs = %.8f msecs/ID\n\n",
		 (tot_time * 1000) / numOfIDs);
	fprintf (stderr, "(Load_time+extract_time)/Num_IDs = %.8f msecs/ID\n\n",
		 ((Load_time+tot_time) * 1000) / numOfIDs);	

	fprintf (stderr, "extract_time/occ = %.8f msecs per post \n\n",
		 (tot_time * 1000) / tot_numocc);   
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

void print_search_usage(char * progname) {	
	fprintf(stderr, "\nThe program loads <index> and then executes over it the\n");
	fprintf(stderr, "queries it receives from the standard input. The standard\n");
	fprintf(stderr, "input comes in the format of the files written by \n");
	fprintf(stderr, "genpatterns or genintervals.\n");
	fprintf(stderr, "%s reports on the standard error time statistics\n", progname);
	fprintf(stderr, "regarding to running the queries.\n\n");
	fprintf(stderr, "Usage:  %s <index> <type> [length] [wordsbefore][V]\n", progname);
	fprintf(stderr, "\n\t<type>   denotes the type of queries:\n");
	fprintf(stderr, "\t         %c counting queries;\n", COUNT);
	//fprintf(stderr, "\t         %c locating queries;\n", LOCATE);
	fprintf(stderr, "\t         %c [numListsToExtract][iterations per list]\n", EXTRACT);
	fprintf(stderr, "\t         %c [numListsToExtract][iterations per list]\n", EXTRACTNOMALLOC);
	fprintf(stderr, "\n\t[V]      with this option it reports on the standard output\n");
	fprintf(stderr, "\t         the results of the queries. The results file should be\n");
	fprintf(stderr, "\t         compared with a trusted one by the compare program.\n\n");
}
