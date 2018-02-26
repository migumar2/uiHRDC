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

#define EXTRACT 	('E')
#define EXTRACTNOMALLOC 	('e')
#define INTERSECT2 	('I')
#define INTERSECTN 	('N')
#define VERBOSE 	('V')



/* macro to detect and notify errors  */
#define IFERRORIL(error) {{if (error) { fprintf(stderr, "%s\n", error_il(error)); exit(1); }}}

/**/ FILE *timeFile;

/* local headers */
void do_extract(uint numIdsToExtract, uint REPETITIONS);
void do_extract_no_malloc(uint numIdsToExtract, uint REPETITIONS);

void do_intersect2(uint numPhrases, uint REPETITIONS);
void do_intersectN(uint numPhrases, uint REPETITIONS);


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

#ifdef DEBUG_ON
	fprintf(stderr," numIdsPerPattern = %9d and numOfPatterns = %9d",*numIdsPerPattern, *numOfPatterns);
#endif	
	uint **V;
	uint i,j,value;
	V = (uint **) malloc(sizeof(uint *) * (*numOfPatterns));
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
	//fprintf(stderr,"\n entering to freePatternSetIntersect %d", numOfPatterns);fflush(stderr);
	for (i=0;i<numOfPatterns;i++) free(patternSets[i]);
	free(patternSets);
	//fprintf(stderr,"\n exitting  freePatternSetIntersect");fflush(stderr);
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

#ifdef DEBUG_ON
	fprintf(stderr,"\n !!!! numIdsPerPattern = %9d and numOfPatterns = %9d",numIdsPerPattern, numOfPatterns);
	fprintf(stderr,"##numOfIDS to extract: %9d\n",*numOfIDs);	
	fflush(stderr);
#endif	

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
	uint i;
	
	if (argc < 5)	{
		print_search_usage(argv[0]);	
		exit (1);
	}
	
	filename = argv[1];
	querytype = *argv[2];


	//parameters for intersect-operations
	char *params = NULL;
	int nchars, len;
	nchars = argc-5;
	//nchars = argc;
	
	for(i=3;i<argc;i++)
		nchars += strlen(argv[i]);
	
	params = (char *) malloc((nchars+1)*sizeof(char));
	params[nchars] = '\0';
	nchars = 0;
	for(i=4;i<argc;i++) {
		len = strlen(argv[i]);
		strncpy(params+nchars,argv[i],len);
		params[nchars+len] = ' ';
		nchars += len+1;
	}
	params[nchars-1] = '\0';
	// end parameters		

	
/**/	timeFile = fopen("timesSearch.txt","a");
/**/	
/**/	fprintf(timeFile,"\n Searching!!: %s %s %s %s %s",argv[0],argv[1],argv[2],argv[3], params);

	
	Load_time = getTime ();
	error = load_il (filename, &Index);
	IFERRORIL (error);
	Load_time = getTime () - Load_time;
	fprintf (stderr, "Load index time = %.6f secs\n", Load_time);

	error = setDefaultSearchOptions_il (Index, params);
	IFERRORIL (error);

	error = size_il(Index, &Index_size);
	IFERRORIL (error);
	error = size_il_uncompressed(Index, &Uncompressed_Index_size);
	IFERRORIL (error);
	Index_size /=1024;
	fprintf(stderr,"Index size = %lu Kb\n", Index_size);

	switch (querytype){

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

		case INTERSECT2:
			if (argc < 5) {
				print_search_usage(argv[0]);	
				exit (1);
			}
			if (argc > 5) 
				if (*argv[5] == VERBOSE){
						Verbose = 1;
						fprintf(stderr,"%c", INTERSECT2);

				}
			do_intersect2((uint) atoi(argv[3]), (uint) atoi(argv[4]) );
			break;

		case INTERSECTN:
			if (argc < 5) {
				print_search_usage(argv[0]);	
				exit (1);
			}
			if (argc > 5) 
				if (*argv[5] == VERBOSE){
						Verbose = 1;
						fprintf(stderr,"%c", INTERSECTN);

				}
			do_intersectN((uint) atoi(argv[3]), (uint) atoi(argv[4]) );
			break;
						
		default:
			fprintf (stderr, "Unknow option: main ru\n");
			exit (1);
	}

	error = free_il(Index);
	IFERRORIL(error);

/**/ fclose(timeFile);

	free(params);

	return 0;
}


	
	
	



 

void
do_extract(uint numIdsToExtract, uint REPETITIONS)
{
	uint i,iters,currID;
	
	int error = 0;
	double time, tot_time = 0;

	uint *IDs; uint numOfIDs;

	readPatternSetsExtract (&numOfIDs, &IDs); //max number of ids to extract 

#ifdef DEBUG_ON	
	fprintf(stderr,"\n IDS = ");
	for (i=0;i<numOfIDs; i++) {fprintf(stderr,"[%d]", IDs[i]);}
	fprintf(stderr,"\n");
#endif	
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
	

#ifdef DEBUG_ON	
		fprintf(stderr,"\n to extract List of ID <<%ld>> \n", IDs[currID]);
#endif		        
		/* extract */
		time = getTime ();
				
		for (iters=0;iters<REPETITIONS;iters++) {
			if (numocc) free (occs);
			error =  extractList_il (Index, IDs[currID], &occs, &numocc); 
		}

		tot_time += (getTime () - time)/REPETITIONS;
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
		 		
/**/	fprintf(timeFile,"\n\t occs = %9lu :: time = %.8f msecs ",tot_numocc,tot_time*1000);
/**/	fprintf(timeFile,"\n\t    avg time per pattern = %.8f msecs ",(tot_time * 1000) / numOfIDs);
/**/	fprintf(timeFile,"\n\t      avg time per occur = %.8f msecs \n",(tot_time * 1000) / tot_numocc);
		 		
		 		 
}




void
do_extract_no_malloc(uint numIdsToExtract, uint REPETITIONS)
{
	uint i,iters,currID;
	
	int error = 0;
	double time, tot_time = 0;

	uint *IDs; uint numOfIDs;

	readPatternSetsExtract (&numOfIDs, &IDs); //max number of ids to extract 

#ifdef DEBUG_ON		
	fprintf(stderr,"\n IDS = ");
	for (i=0;i<numOfIDs; i++) {fprintf(stderr,"[%d]", IDs[i]);}
	fprintf(stderr,"\n");
#endif
	
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

#ifdef DEBUG_ON		
		fprintf(stderr,"\n to extract List of ID <<%ld>> \n", IDs[currID]);
#endif		        
		/* extract */
		time = getTime ();
				
		for (iters=0;iters<REPETITIONS;iters++) {
			error =  extractListNoMalloc_il (Index, IDs[currID], occs, &numocc); 
		}

		tot_time += (getTime () - time)/REPETITIONS;
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

/**/	fprintf(timeFile,"\n\t occs = %9lu :: time = %.8f msecs ",tot_numocc,tot_time*1000);
/**/	fprintf(timeFile,"\n\t    avg time per pattern = %.8f msecs ",(tot_time * 1000) / numOfIDs);
/**/	fprintf(timeFile,"\n\t      avg time per occur = %.8f msecs \n",(tot_time * 1000) / tot_numocc);
		 		
			  
}




void
do_intersect2(uint numPhrases, uint REPETITIONS)
{
	uint i,j,iters,currPhrase;
	
	int error = 0;
	double time, tot_time = 0;

	uint **patternSets; uint numIdsPerPattern; uint numOfPatterns;

	readPatternSetsIntersect (&numIdsPerPattern, &numOfPatterns, &patternSets);

#ifdef DEBUG_ON	
	fprintf(stderr,"\n parameters: maxNunPhr %d, numPhr %d, numIdPerPhr=%d",numPhrases , numOfPatterns,numIdsPerPattern);
	fprintf(stderr,"\n patterns to intersect (1 list per row) = \n");
	for (i=0;i<numOfPatterns; i++) {
		for (j=0;j<numIdsPerPattern;j++)
			fprintf(stderr,"[%d]", patternSets[i][j]);
		fprintf(stderr,"\n");
	}
	fprintf(stderr,"\n");
#endif
	
	if (numPhrases > numOfPatterns) {
		fprintf(stderr,"\nBuild_il: Error during intersect2 %ld/%ld IDs\n", numPhrases, numOfPatterns);
		exit(0);
	}
	
	//numPhrases = numOfPatterns;
	double time0 = getTime ();

	uint *occs; uint numocc=0; uint tot_numocc=0;
	
	iters=0;
	currPhrase=0;
	for (currPhrase=0; currPhrase<numPhrases; currPhrase++) {
	
#ifdef DEBUG_ON	
		fprintf(stderr,"\n intersect Phrase-Pattern <<%ld>> -->", currPhrase); 
		for (i=0;i<numIdsPerPattern;i++) fprintf(stderr,"[%d]", patternSets[currPhrase][i]);
#endif
		        
		/* extract */
		time = getTime ();
				
		for (iters=0;iters<REPETITIONS;iters++) {
			if (iters) free (occs);  //not in the first one.
			error = intersect_2_il (Index, patternSets[currPhrase][0], patternSets[currPhrase][1],
			                               &numocc, &occs);
		}

		tot_time += (getTime () - time)/REPETITIONS;
		tot_numocc += numocc;

		IFERRORIL (error);	
		if (Verbose) {
			fwrite(&(currPhrase), sizeof(uint), 1, stdout);
			fwrite(&numocc, sizeof(numocc), 1, stdout);
			}		
#ifdef DEBUG_ON	
		fprintf(stderr,"\n NUMOCCS = %d ",numocc);fflush(stderr);
#endif			
		//if (numocc)
		free (occs);	//the resulting list has always allocated memory even though numoccs==0;
	}

	freePatternSetsIntersect(patternSets, numOfPatterns);

	fprintf (stderr, "\nTotal Num occs found = %lu\n", tot_numocc);
	fprintf (stderr, "Int2 time = %.8f msecs\n", tot_time*1000);
	fprintf (stderr, "Int2_time/numOfPatterns = %.8f msecs/patt\n\n",
		 (tot_time * 1000) / numOfPatterns);
	fprintf (stderr, "(Load_time+Int2_time)/numOfPatterns = %.8f msecs/patt\n\n",
		 ((Load_time+tot_time) * 1000) / numOfPatterns);	

	fprintf (stderr, "Int2_time/occ = %.8f msecs per post \n\n",
		 (tot_time * 1000) / tot_numocc);   
		 
/**/	fprintf(timeFile,"\n\t occs = %9lu :: time = %.8f msecs ",tot_numocc,tot_time*1000);
/**/	fprintf(timeFile,"\n\t    avg time per pattern = %.8f msecs ",(tot_time * 1000) / numOfPatterns);
/**/	fprintf(timeFile,"\n\t      avg time per occur = %.8f msecs \n",(tot_time * 1000) / tot_numocc);
		 		
			 
}







void do_intersectN(uint numPhrases, uint REPETITIONS){
	uint i,j,iters,currPhrase;
	
	int error = 0;
	double time, tot_time = 0;

	uint **patternSets; uint numIdsPerPattern; uint numOfPatterns;

	readPatternSetsIntersect (&numIdsPerPattern, &numOfPatterns, &patternSets);

#ifdef DEBUG_ON	
	fprintf(stderr,"\n parameters: maxNunPhr %d, numPhr %d, numIdPerPhr=%d",numPhrases , numOfPatterns,numIdsPerPattern);
	fprintf(stderr,"\n patterns to intersect (1 list per row) = \n");
	for (i=0;i<numOfPatterns; i++) {
		for (j=0;j<numIdsPerPattern;j++)
			fprintf(stderr,"[%d]", patternSets[i][j]);
		fprintf(stderr,"\n");
	}
	fprintf(stderr,"\n");
#endif
	
	if (numPhrases > numOfPatterns) {
		fprintf(stderr,"\nBuild_il: Error during intersect2 %ld/%ld IDs\n", numPhrases, numOfPatterns);
		exit(0);
	}
	
	//numPhrases = numOfPatterns;
	double time0 = getTime ();

	uint *occs;  uint numocc=0; uint tot_numocc=0;
	
	iters=0;
	currPhrase=0;
	for (currPhrase=0; currPhrase<numPhrases; currPhrase++) {

#ifdef DEBUG_ON	
		fprintf(stderr,"\n intersect Phrase-Pattern <<%ld>> \n", currPhrase); 
#endif
		        
		/* extract */
		time = getTime ();
				
		for (iters=0;iters<REPETITIONS;iters++) {
			if (numocc) free (occs);
			error = intersect_N_il (Index, patternSets[currPhrase], numIdsPerPattern, 
			                               &numocc, &occs);			
		}

		tot_time += (getTime () - time)/REPETITIONS;
		tot_numocc += numocc;

		IFERRORIL (error);	
		if (Verbose) {
			fwrite(&(currPhrase), sizeof(uint), 1, stdout);
			fwrite(&numocc, sizeof(numocc), 1, stdout);
			}		

#ifdef DEBUG_ON			
		fprintf(stderr,"\n NUMOCCS = %d ",numocc);fflush(stderr);
#endif
		if (numocc) free (occs);
		numocc=0;
		
	}

	freePatternSetsIntersect(patternSets, numOfPatterns);

	fprintf (stderr, "Total Num occs found = %lu\n", tot_numocc);
	fprintf (stderr, "IntN time = %.8f msecs\n", tot_time*1000);
	fprintf (stderr, "IntN_time/numOfPatterns = %.8f msecs/patt\n\n",
		 (tot_time * 1000) / numOfPatterns);
	fprintf (stderr, "(Load_time+IntN_time)/numOfPatterns = %.8f msecs/patt\n\n",
		 ((Load_time+tot_time) * 1000) / numOfPatterns);	

	fprintf (stderr, "IntN_time/occ = %.8f msecs per post \n\n",
		 (tot_time * 1000) / tot_numocc);  	
		 
/**/	fprintf(timeFile,"\n\t occs = %9lu :: time = %.8f msecs ",tot_numocc,tot_time*1000);
/**/	fprintf(timeFile,"\n\t    avg time per pattern = %.8f msecs ",(tot_time * 1000) / numOfPatterns);
/**/	fprintf(timeFile,"\n\t      avg time per occur = %.8f msecs \n",(tot_time * 1000) / tot_numocc);
			 
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
	fprintf(stderr, "Usage:  %s <index> <type> <numPatts> <repetitions> [V] [search_opts] < patt_file\n", progname);
	fprintf(stderr, "\n\t<type>   denotes the type of queries:\n");
	//fprintf(stderr, "\t         %c counting queries;\n", COUNT);
	//fprintf(stderr, "\t         %c locating queries;\n", LOCATE);
	fprintf(stderr, "\t         %c <numListsToExtract><iterations per list>\n", EXTRACT);
	fprintf(stderr, "\t             Extracts the values of the lists: memory is allocated\n");
	fprintf(stderr, "\t         %c <numListsToExtract><iterations per list>\n", EXTRACTNOMALLOC);
	fprintf(stderr, "\t             Extracts the values of the lists: memory is reserved only once.\n");
	fprintf(stderr, "\t         %c <numFrases><iterations per frase>\n", INTERSECT2);
	fprintf(stderr, "\t             Intersection of two lists (diff. opts. depending on [search_opts])\n");
	fprintf(stderr, "\t         %c <numFrases><iterations per frase>\n", INTERSECTN);
	fprintf(stderr, "\t             Intersection of n lists (diff. opts. depending on [search_opts])\n");
	fprintf(stderr, "\n\t[V]      with this option it reports on the standard output\n");
	fprintf(stderr, "\t         the results of the queries. The results file might be\n");
	fprintf(stderr, "\t         compared with a trusted one by the compare program.\n\n");
}
