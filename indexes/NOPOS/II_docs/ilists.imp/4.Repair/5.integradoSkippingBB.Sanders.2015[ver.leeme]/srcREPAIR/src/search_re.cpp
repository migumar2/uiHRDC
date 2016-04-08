/*
 * Run Queries
 */


//#define DEBUG_ON

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <iostream>
#include "RepairPost.h"

/* only for getTime() */
#include <sys/time.h>
#include <sys/resource.h>

#define EXTRACT 	('E')
#define VERBOSE 	('V')
#define INTERSECT2_SVS_U 	('U')    //version fsearchUncompress
#define INTERSECT2_MERGE	('M')    //version uncompress-then-merge
#define INTERSECT2_SKIP_LOOKUP 	('L')    //version ADAPTED-LOOKUP
#define RECOVER 	('R')


//#define DEBUG_ON

/* macro to detect and notify errors  */
#define IFERRORIL(error) {{if (error) { fprintf(stderr, "An error occrred #%d\n",error); exit(1); }}}

/**/ FILE *timeFile;





/* local headers */
void do_extract(uint numIdsToExtract, uint REPETITIONS);

void do_intersect2svsUnc(uint numPhrases, uint REPETITIONS);
void do_intersect2merge(uint numPhrases, uint REPETITIONS);

void do_intersect2LOOKUP(uint numPhrases, uint REPETITIONS);

void do_recover(char *filename) ;
double getTime (void);

#define TIMEFACTOR_TO_SEC (1.0/1000)   // it is given in milliseconds

void print_search_usage(char * progname);
  		
  		
  		
static RepairPost * g;
static int Verbose = 0; 
static uint Index_size, Uncompressed_Index_size ;
static double Load_time;





#define MAXRESULTS 300000
static double acumTimes[MAXRESULTS];
static uint    ncount[MAXRESULTS];
static char matlab[100]="skipping";
static char outfilename[100]="skipping_";



void initResults(char *matlabsuffix, char *outsuffix) {
	uint i;
	for (i=0;i<MAXRESULTS; i++) {
		acumTimes[i] = 0.0;
		ncount [i] =0;	
	}
	strcat(matlab,matlabsuffix);
	strcat(outfilename,outsuffix);
	strcat(outfilename,".m");
	unlink(outfilename);
	
}
	
	

void plotResults(uint id1, uint id2, double tiempo, uint iter, char *matlab, char* outfile) {
	FILE *f = fopen(outfile,"a");
	uint a,b,x;
	double y;
	
					//fprintf(stderr,"\n call to plot id1=%d, id2=%d, tiempo = %f, iter = %d, matlab = %s, outfile = %s",id1,id2,tiempo,iter,matlab,outfile);
	 a= g->lenPost[id1];
	 b= g->lenPost[id2];
	 				//fprintf(stderr,"\n a= %d, b=%d",a,b);
	 x= ((a+(b-1))/b);
	 //fprintf(stderr,"\n x=%d",x);
	fprintf(stderr,"#");
	if ((iter%100)==0) fprintf(stderr,"<%d>",iter);
	y=tiempo;
	fprintf(f,"\n%sX(%d)=%d;",matlab,iter,x);
	fprintf(f,"\n%sY(%d)=%f;",matlab,iter,y);
	
	acumTimes[x] += y;
	ncount[x]++;
	fclose(f);		
}

void endResults(  char *matlab, char* outfile, uint numIters) {
	FILE *f = fopen(outfile,"a");
	uint i,x;
	double y,sum;
	uint diffs =1;
	

	for (i=0; i<MAXRESULTS;i++) {
		if (ncount[i] !=0)	{
			sum=acumTimes[i];
			x=i;
			y= sum / ((double)ncount[i]);

			//fprintf(stderr,"\ni=%d, %savgX(%d)=%d;",i,matlab,diffs,x);
			//fprintf(stderr,"\ni=%d, %savgY(%d)=%f;",i,matlab,diffs,y);
			
			fprintf(f,"\n%savgX(%d)=%d;",matlab,diffs,x);
			fprintf(f,"\n%savgY(%d)=%f;",matlab,diffs,y);
			diffs++;
		}
	}

	 
	fprintf(f,"\nfigure(1)");
	fprintf(f,"\nhold on;");
	//for (i=1;i<=numIters;i++) fprintf(f,"\n plot(%sX(%d),%sY(%d),'x');",matlab,i,matlab,i);
	fprintf(f,"\n plot(%sX,%sY,'*k');",matlab,matlab);
	fprintf(f,"\n plot(%savgX,%savgY,'.-k');",matlab,matlab);

	fclose(f);
}



	
void endResultsQUITARZEROS(  char *matlab, char* outfile, uint numIters) {
	FILE *f = fopen(outfile,"a");
	uint i,x;
	double y,sum;
	uint diffs =1;
	

	for (i=0; i<MAXRESULTS;i++) {
		if ((ncount[i] !=0) && (acumTimes[i]>0.00001))	{
			sum=acumTimes[i];
			x=i;
			y= sum / ((double)ncount[i]);

			//fprintf(stderr,"\ni=%d, %savgX(%d)=%d;",i,matlab,diffs,x);
			//fprintf(stderr,"\ni=%d, %savgY(%d)=%f;",i,matlab,diffs,y);
			
			fprintf(f,"\n%savgX(%d)=%d;",matlab,diffs,x);
			fprintf(f,"\n%savgY(%d)=%f;",matlab,diffs,y);
			diffs++;
		}
	}	

	 
	fprintf(f,"\nfigure(1)");
	fprintf(f,"\nhold on;");
	//for (i=1;i<=numIters;i++) fprintf(f,"\n plot(%sX(%d),%sY(%d),'x');",matlab,i,matlab,i);
	fprintf(f,"\n plot(%sX,%sY,'x');",matlab,matlab);
	fprintf(f,"\n plot(%savgX,%savgY,'-r');",matlab,matlab);

	fclose(f);
}








//Reads the Sets of phrase-patterns that can be searched for.
void readPatternSetsIntersect (uint *numIdsPerPattern, uint *numOfPatterns, uint ***patternSets) {
	int rf;
	rf= fscanf(stdin,"##numIdsPerPattern: %9d\n",numIdsPerPattern);
	rf= fscanf(stdin,"##numOfPatterns : %9d\n",numOfPatterns);

#ifdef DEBUG_ON
	fprintf(stderr," numIdsPerPattern = %9d and numOfPatterns = %9d",*numIdsPerPattern, *numOfPatterns);
#endif	
	uint **V;
	uint i,j,value;
	V = (uint **) malloc(sizeof(uint *) * (*numOfPatterns));
	for (i=0; i< *numOfPatterns; i++) {
		V[i] = (uint *) malloc (sizeof(uint) * (*numIdsPerPattern));
		for (j=0;j< (*numIdsPerPattern);j++) {
			rf= fscanf(stdin,"%9d\n",&value);
			value ++;  // ids of patterns needed for repair should be increased by 1 unit.
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
	
	int rf;
	rf= fscanf(stdin,"##numIdsPerPattern: %9d\n",&numIdsPerPattern);
	rf= fscanf(stdin,"##numOfPatterns : %9d\n",&numOfPatterns);
	*numOfIDs = numIdsPerPattern * numOfPatterns;
	
	uint *V;
	uint i,value;
	V = (uint *) malloc(sizeof(uint ) * (*numOfIDs) );
	for (i=0; i< (*numOfIDs); i++) {
		rf= fscanf(stdin,"%9d\n",&value); 
		value ++;
		V[i]=value; 
	}
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




void writeSampleK_b(char *file, uint sampleK) {
	  //** reads some constants: maxLenPosting and sizeUncompressed from "convert program".
	  char * fname = new char[strlen(file)+50];
	
	  sprintf(fname,"%s.const.samplek",file);
	  unlink (fname);
	  FILE * fp = fopen64(fname,"w");
	  assert(fp!=NULL);
	  size_t rf;
	  rf=fprintf(fp,"sampleK=%u",sampleK);
	  //rf= fwrite(&sampleK,sizeof(uint),1,fp);    //max len of any posting list. ==> needed for malloc on info[]
	  fclose(fp);
	  delete []fname;
	  cerr << "\n sampleK (writen-to-disk) is " << sampleK ;

}
/*
 * Temporary usage: run_queries <index file> <type> [length] [V]
 */
int main (int argc, char *argv[])
{
	int error = 0;
	error=0;
	char *filename;
	char querytype;
	int i;
	uint B =8;
	
	if (argc < 6)	{
		print_search_usage(argv[0]);	
		exit (1);
	}
	
	filename = argv[1];
	B = atoi(argv[2]);
	querytype = *argv[3];

	cerr << "\n B value is " << B << " BUT IT IS NOT USING ACTUALLY";


	//parameters for intersect-operations
	char *params = NULL;
	int nchars, len;
	nchars = argc-5;
	//nchars = argc;
	
	for(i=4;i<argc;i++)
		nchars += strlen(argv[i]);
	
	params = (char *) malloc((nchars+1)*sizeof(char));
	params[nchars] = '\0';
	nchars = 0;
	for(i=5;i<argc;i++) {
		len = strlen(argv[i]);
		strncpy(params+nchars,argv[i],len);
		params[nchars+len] = ' ';
		nchars += len+1;
	}
	params[nchars-1] = '\0';
	// end parameters		

	
/**/	timeFile = fopen("timesSearch.txt","a");
/**///	initResults(argv[1],argv[1]);
/**/	initResults("repair_moffat","repair_moffat");
/**/	fprintf(timeFile,"\n Searching!!: %s %s %s %s %s %s",argv[0],argv[1],argv[2],argv[3],argv[4], params);

	
	Load_time = getTime ();
	//error = load_il (filename, &Index);
	//IFERRORIL (error);

	writeSampleK_b(filename,B);
	g = new RepairPost(filename);
	
	Load_time = getTime () - Load_time;
	Load_time *= TIMEFACTOR_TO_SEC;
	fprintf (stderr, "Load index time = %.6f secs\n", Load_time);
	fflush(stderr);

	//error = setDefaultSearchOptions_il (Index, params);
	//IFERRORIL (error);

	//error = size_il(Index, &Index_size);
	//IFERRORIL (error);
	//error = size_il_uncompressed(Index, &Uncompressed_Index_size);
	//IFERRORIL (error);
	
	Index_size = g->size();
	Uncompressed_Index_size = g->sizeUncompressed;
	fprintf(stderr,"Index size = %u bytes\n", Index_size); fflush(stderr);
	Index_size /=1024;
	fprintf(stderr,"Index size = %u Kb\n", Index_size); fflush(stderr);

	switch (querytype){

		case EXTRACT:
			if (argc < 6) {
				print_search_usage(argv[0]);	
				exit (1);
			}
			if (argc > 6) 
				if (*argv[6] == VERBOSE){
						Verbose = 1;
						fprintf(stderr,"%c", EXTRACT);

				}
			do_extract((uint) atoi(argv[4]), (uint) atoi(argv[5]) );
			break;

		case RECOVER:
			printf("\n******** %d ******\n" , argc); fflush(stdout);fflush(stderr);
			if (argc < 6) {
				print_search_usage(argv[0]);	
				exit (1);
			}

			if (argc > 6) 
				if (*argv[6] == VERBOSE){
						Verbose = 1;
						fprintf(stderr,"%c", RECOVER);
				}

			do_recover(filename);
			break;

		case INTERSECT2_SVS_U:
			if (argc < 6) {
				print_search_usage(argv[0]);	
				exit (1);
			}
			if (argc > 6) 
				if (*argv[6] == VERBOSE){
						Verbose = 1;
						fprintf(stderr,"%c", INTERSECT2_SVS_U);

				}
			do_intersect2svsUnc((uint) atoi(argv[4]), (uint) atoi(argv[5]) );
			break;

		case INTERSECT2_MERGE:
			if (argc < 6) {
				print_search_usage(argv[0]);	
				exit (1);
			}
			if (argc > 6) 
				if (*argv[6] == VERBOSE){
						Verbose = 1;
						fprintf(stderr,"%c", INTERSECT2_SVS_U);

				}
			do_intersect2merge((uint) atoi(argv[4]), (uint) atoi(argv[5]) );
			break;
									
		case INTERSECT2_SKIP_LOOKUP:
			if (argc < 6) {
				print_search_usage(argv[0]);	
				exit (1);
			}
			if (argc > 6) 
				if (*argv[6] == VERBOSE){
						Verbose = 1;
						fprintf(stderr,"%c", INTERSECT2_SKIP_LOOKUP);

				}
			do_intersect2LOOKUP((uint) atoi(argv[4]), (uint) atoi(argv[5]) );
			break;	
						
						
						
						
		default:
			fprintf (stderr, "Unknow option: main ru\n");
			exit (1);
	}

	//error = free_il(Index);
	//IFERRORIL(error);
	delete g;

/**/ fclose(timeFile);

	free(params);

	return 0;
}


	
	
	


void do_recover(char *filename) {

	char * outfilename = new char [5000];
	sprintf(outfilename,"%s.POSTS.RECOVERED", filename);
	FILE *f = fopen(outfilename, "w");
	uint numOfIDs = g->nodes; 
	uint maxLenPosting = g->maxLenPosting;
	printf("\n numOfwords = %u, maxlenPosting = %u\n", numOfIDs, maxLenPosting);

	uint total_size = 100000000;
	uint *buffer = new uint[total_size];
	uint buffer_pos=0;

	buffer[buffer_pos++]=numOfIDs;
	buffer[buffer_pos++]=maxLenPosting-1;
	fwrite(buffer,sizeof(uint),buffer_pos, f);
	
	uint *occs;  uint numocc=0; 
	uint tot_numocc=0;
	uint * adj=NULL;

	uint currID=0;

	/* extract */

	double tot_time;
	double time = getTime ();
	uint lbuff;
	for (currID=1; currID<=numOfIDs; currID++) {
		lbuff=0;
		adj = g->adj(currID);
		numocc=adj[0];
		if (numocc) occs = adj+1;
		tot_numocc += numocc;
		uint j=0;
		//buffer[buffer_pos++]=numocc;
		buffer[lbuff++]=numocc; 
		for (j=0;j<numocc;j++){
			//buffer[buffer_pos++]=occs[j] -1;
			buffer[lbuff++]=occs[j] -1;
		}
		free(adj);	
		numocc=0;
		fwrite(buffer,sizeof(uint),lbuff, f);
	}

	//fwrite(buffer,sizeof(uint),buffer_pos, f);

	tot_time = (getTime () - time);

	fprintf (stderr, "Total Num occs found = %u\n", tot_numocc);
	fprintf (stderr, "extract RECOVER time = %.8f msecs\n", tot_time);
	delete [] buffer;
	delete [] outfilename;
	fclose(f);
}	


 

void
do_extract(uint numIdsToExtract, uint REPETITIONS)
{
	uint iters,currID;
	
	int error = 0;
	double time, tot_time = 0;

	uint *IDs; uint numOfIDs;

	readPatternSetsExtract (&numOfIDs, &IDs); //max number of ids to extract 

#ifdef DEBUG_ON	
	uint i;
	fprintf(stderr,"\n IDS = ");
	for (i=0;i<numOfIDs; i++) {fprintf(stderr,"[%d]", IDs[i]);}
	fprintf(stderr,"\n");
#endif	
	if (numIdsToExtract > numOfIDs) {
		fprintf(stderr,"\nBuild_il: Error during extract %u/%u IDs\n", numIdsToExtract, numOfIDs);
		exit(0);
	}
	
	numOfIDs = numIdsToExtract;
	//double time0 = getTime ();

	uint *occs;  uint numocc=0; uint tot_numocc=0;
	uint * adj=NULL;
	
	iters=0;
	currID=0;
	for (currID=0; currID<numOfIDs; currID++) {
	

#ifdef DEBUG_ON	
		fprintf(stderr,"\n to extract List of ID <<%u>> \n", IDs[currID]);
#endif		        
		/* extract */
		time = getTime ();
				
		for (iters=0;iters<REPETITIONS;iters++) {
			//if (numocc) free (occs);
			//error =  extractList_il (Index, IDs[currID], &occs, &numocc); 
			adj = g->adj(IDs[currID]);
			numocc=adj[0];
			if (numocc) occs = adj+1;
			
		}

		tot_time += (getTime () - time)/REPETITIONS;
		tot_numocc += numocc;


		IFERRORIL (error);	
		if (Verbose) {
			uint value = IDs[currID] -1;
			fwrite(&value, sizeof(uint), 1, stdout);
			fwrite(&numocc, sizeof(numocc), 1, stdout);
			}		
			
		//if (numocc) free (occs);
		free(adj);
		numocc=0;
		
	}

	freePatternSetsExtract(IDs);

	fprintf (stderr, "Total Num occs found = %u\n", tot_numocc);
	fprintf (stderr, "extract time = %.8f msecs\n", tot_time*1);
	fprintf (stderr, "extract_time/Num_IDs = %.8f msecs/ID\n\n",
		 		(tot_time * 1) / numOfIDs);
	fprintf (stderr, "(Load_time+extract_time)/Num_IDs = %.8f msecs/ID\n\n",
		 		((Load_time+tot_time) * 1) / numOfIDs);	
	fprintf (stderr, "extract_time/occ = %.8f msecs per post \n\n",
		 		(tot_time * 1) / tot_numocc);  
		 		
/**/	fprintf(timeFile,"\n\t occs = %9u :: time = %.8f msecs ",tot_numocc,tot_time*1);
/**/	fprintf(timeFile,"\n\t    avg time per pattern = %.8f msecs ",(tot_time * 1) / numOfIDs);
/**/	fprintf(timeFile,"\n\t      avg time per occur = %.8f msecs \n",(tot_time * 1) / tot_numocc);
/**/	fprintf(timeFile,"\n\t      index size = %.4f Mbytes \n",Index_size/1024.0);  //in Megabytes
		 		
		 		 
}







void
do_intersect2svsUnc(uint numPhrases, uint REPETITIONS)
{
	uint iters,currPhrase;
	
	int error = 0;
	double time, tot_time = 0;

	uint **patternSets; uint numIdsPerPattern; uint numOfPatterns;

	readPatternSetsIntersect (&numIdsPerPattern, &numOfPatterns, &patternSets);

#ifdef DEBUG_ON	
	{ uint i,j;
	fprintf(stderr,"\n parameters: maxNunPhr %d, numPhr %d, numIdPerPhr=%d",numPhrases , numOfPatterns,numIdsPerPattern);
	fprintf(stderr,"\n patterns to intersect (1 list per row) = \n");
	for (i=0;i<numOfPatterns; i++) {
		for (j=0;j<numIdsPerPattern;j++)
			fprintf(stderr,"[%d]", patternSets[i][j]);
		fprintf(stderr,"\n");
	}
	fprintf(stderr,"\n");
	}
#endif
	
	if (numPhrases > numOfPatterns) {
		fprintf(stderr,"\nBuild_il: Error during intersect2 %u/%u IDs\n", numPhrases, numOfPatterns);
		exit(0);
	}
	
	//numPhrases = numOfPatterns;
	//double time0 = getTime ();

	uint *occs; uint numocc=0; uint tot_numocc=0;
	
	iters=0;
	currPhrase=0;
	for (currPhrase=0; currPhrase<numPhrases; currPhrase++) {
	
#ifdef DEBUG_ON	
	{ uint i;
		fprintf(stderr,"\n intersect Phrase-Pattern <<%u>> -->", currPhrase); 
		for (i=0;i<numIdsPerPattern;i++) fprintf(stderr,"[%d]", patternSets[currPhrase][i]);
	}
#endif
		        
		/* extract */
		time = getTime ();
				
		for (iters=0;iters<REPETITIONS;iters++) {
			if (iters) free (occs);  //not in the first one.
				
			//error = intersect_2_il (Index, patternSets[currPhrase][0], patternSets[currPhrase][1],
			//                               &numocc, &occs);
			g->svs2Unc( patternSets[currPhrase][0], patternSets[currPhrase][1],  &numocc, &occs);
		}

		tot_time += (getTime () - time)/REPETITIONS;
		tot_numocc += numocc;
		
		fprintf(stderr,"\n ********  %d Ocurrencias found ...",numocc);
		for (uint i=0;i<numocc; i++) {fprintf(stderr,"\n occ[%3d] ==> %d",i,occs[i]-1);}
				

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

	fprintf (stderr, "\nTotal Num occs found = %u\n", tot_numocc);
	fprintf (stderr, "Int2 time = %.8f msecs\n", tot_time*1);
	fprintf (stderr, "Int2_time/numOfPatterns = %.8f msecs/patt\n\n",
		 (tot_time * 1) / numOfPatterns);
	fprintf (stderr, "(Load_time+Int2_time)/numOfPatterns = %.8f msecs/patt\n\n",
		 ((Load_time+tot_time) * 1) / numOfPatterns);	

	fprintf (stderr, "Int2_time/occ = %.8f msecs per post \n\n",
		 (tot_time * 1) / tot_numocc);   
		 
/**/	fprintf(timeFile,"\n\t occs = %9u :: time = %.8f msecs ",tot_numocc,tot_time*1);
/**/	fprintf(timeFile,"\n\t    avg time per pattern = %.8f msecs ",(tot_time * 1) / numOfPatterns);
/**/	fprintf(timeFile,"\n\t      avg time per occur = %.8f msecs \n",(tot_time * 1) / tot_numocc);
/**/	fprintf(timeFile,"\n\t      index size = %.4f Mbytes \n",Index_size/1024.0);  //in Megabytes
		 					 
}


/********************************************************/
void
do_intersect2merge(uint numPhrases, uint REPETITIONS)
{

	uint iters,currPhrase;
	
	int error = 0;
	double time, timeE, tot_time = 0;

	uint **patternSets; uint numIdsPerPattern; uint numOfPatterns;

	readPatternSetsIntersect (&numIdsPerPattern, &numOfPatterns, &patternSets);

#ifdef DEBUG_ON	
	{ uint i,j;
	fprintf(stderr,"\n parameters: maxNunPhr %d, numPhr %d, numIdPerPhr=%d",numPhrases , numOfPatterns,numIdsPerPattern);
	fprintf(stderr,"\n patterns to intersect (1 list per row) = \n");
	for (i=0;i<numOfPatterns; i++) {
		for (j=0;j<numIdsPerPattern;j++)
			fprintf(stderr,"[%d]", patternSets[i][j]);
		fprintf(stderr,"\n");
	}
	fprintf(stderr,"\n");
	}
#endif
	
	if (numPhrases > numOfPatterns) {
		fprintf(stderr,"\nBuild_il: Error during intersect2 %u/%u IDs\n", numPhrases, numOfPatterns);
		exit(0);
	}
	
	//numPhrases = numOfPatterns;
	//double time0 = getTime ();

	uint *occs; uint numocc=0; uint tot_numocc=0;
	
	iters=0;
	currPhrase=0;
	for (currPhrase=0; currPhrase<numPhrases; currPhrase++) {
	
#ifdef DEBUG_ON	
	{	uint i;
		fprintf(stderr,"\n intersect Phrase-Pattern <<%u>> -->", currPhrase); 
		for (i=0;i<numIdsPerPattern;i++) fprintf(stderr,"[%d]", patternSets[currPhrase][i]);
	}
#endif
		        
		/* extract */
		time = getTime ();
				
		for (iters=0;iters<REPETITIONS;iters++) {
			if (iters) free (occs);  //not in the first one.
				
			//error = intersect_2_il (Index, patternSets[currPhrase][0], patternSets[currPhrase][1],
			//                               &numocc, &occs);
			g->merge2 ( patternSets[currPhrase][0], patternSets[currPhrase][1],  &numocc, &occs);
		}
		

		timeE=(getTime () - time)/REPETITIONS;
		tot_time += timeE;
		//tot_time += (getTime () - time)/REPETITIONS;
		tot_numocc += numocc;
//
//		fprintf(stderr,"\n ********  %d Ocurrencias found ...",numocc);
//		for (uint i=0;i<numocc; i++) {fprintf(stderr,"\n occ[%3d] ==> %d",i,occs[i]-1);}

		plotResults(patternSets[currPhrase][0], patternSets[currPhrase][1], timeE, currPhrase+1, matlab, outfilename);
		
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
	endResults(  matlab, outfilename, numPhrases);

	freePatternSetsIntersect(patternSets, numOfPatterns);

	fprintf (stderr, "\nTotal Num occs found = %u\n", tot_numocc);
	fprintf (stderr, "Int2 time = %.8f msecs\n", tot_time*1); //*1000);
	fprintf (stderr, "Int2_time/numOfPatterns = %.8f msecs/patt\n\n",
		 (tot_time * 1) / numOfPatterns);
	fprintf (stderr, "(Load_time+Int2_time)/numOfPatterns = %.8f msecs/patt\n\n",
		 ((Load_time+tot_time) * 1) / numOfPatterns);	

	fprintf (stderr, "Int2_time/occ = %.8f msecs per post \n\n",
		 (tot_time * 1000) / tot_numocc);   
		 
/**/	fprintf(timeFile,"\n\t occs = %9u :: time = %.8f msecs ",tot_numocc,tot_time*1);
/**/	fprintf(timeFile,"\n\t    avg time per pattern = %.8f msecs ",(tot_time * 1) / numOfPatterns);
/**/	fprintf(timeFile,"\n\t      avg time per occur = %.8f msecs \n",(tot_time * 1) / tot_numocc);
/**/	fprintf(timeFile,"\n\t      index size = %.4f Mbytes \n",Index_size/1024.0);  //in Megabytes
		 		
			 
		 					 
}



/********************************************************/
void
do_intersect2LOOKUP(uint numPhrases, uint REPETITIONS)
{
	uint iters,currPhrase;
	
	int error = 0;
	double time, timeE, tot_time = 0;

	uint **patternSets; uint numIdsPerPattern; uint numOfPatterns;

	readPatternSetsIntersect (&numIdsPerPattern, &numOfPatterns, &patternSets);

#ifdef DEBUG_ON	
	{ uint i,j;
	fprintf(stderr,"\n parameters: maxNunPhr %d, numPhr %d, numIdPerPhr=%d",numPhrases , numOfPatterns,numIdsPerPattern);
	fprintf(stderr,"\n patterns to intersect (1 list per row) = \n");
	for (i=0;i<numOfPatterns; i++) {
		for (j=0;j<numIdsPerPattern;j++)
			fprintf(stderr,"[%d]", patternSets[i][j]);
		fprintf(stderr,"\n");
	}
	fprintf(stderr,"\n");
	}
#endif
	
	if (numPhrases > numOfPatterns) {
		fprintf(stderr,"\nBuild_il: Error during intersect2 %u/%u IDs\n", numPhrases, numOfPatterns);
		exit(0);
	}
	
	//numPhrases = numOfPatterns;
	//double time0 = getTime ();

	uint *occs; uint numocc=0; uint tot_numocc=0;
	
	iters=0;
	currPhrase=0;
	for (currPhrase=0; currPhrase<numPhrases; currPhrase++) {
//	for (currPhrase=4966; currPhrase<numPhrases; currPhrase++) {
	
#ifdef DEBUG_ON	
	{	uint i;
		fprintf(stderr,"\n intersect Phrase-Pattern <<%u>> -->", currPhrase); 
		for (i=0;i<numIdsPerPattern;i++) fprintf(stderr,"[%d]", patternSets[currPhrase][i]);
	}
#endif
		        
		// patternSets[currPhrase][0]=54; patternSets[currPhrase][1]=204; 
		        
		/* extract */
		time = getTime ();
				
		for (iters=0;iters<REPETITIONS;iters++) {
			if (iters) free (occs);  //not in the first one.
				
			//error = intersect_2_il (Index, patternSets[currPhrase][0], patternSets[currPhrase][1],
			//                               &numocc, &occs);
			g->int2SkippingLookup ( patternSets[currPhrase][0], patternSets[currPhrase][1],  &numocc, &occs);
		}
	

		timeE=(getTime () - time)/REPETITIONS;
		tot_time += timeE;
		//tot_time += (getTime () - time)/REPETITIONS;
		tot_numocc += numocc;
//
//		fprintf(stderr,"\n ********  %d Ocurrencias found ...",numocc);
//		for (uint i=0;i<numocc; i++) {fprintf(stderr,"\n occ[%3d] ==> %d",i,occs[i]-1);}

		plotResults(patternSets[currPhrase][0], patternSets[currPhrase][1], timeE, currPhrase+1, matlab, outfilename);

	
#ifdef DEBUG_ON		
	//if (currPhrase>50)
	{ //COMPARES WITH MERGE...	
		uint numoccM;
		uint *occsM;
		g->merge2 ( patternSets[currPhrase][0], patternSets[currPhrase][1],  &numoccM, &occsM);
		
		{ uint f,m;
			m=min(numoccM, numocc);
						
			if (numoccM != numocc) {

		g->info = (uint *) malloc (1000000);
		if (g->lenPost[patternSets[currPhrase][0]] <= g->lenPost[patternSets[currPhrase][1]]) 
				g->showInfoSampling(patternSets[currPhrase][1]);
		else  
			g->showInfoSampling(patternSets[currPhrase][0]);
		free(g->info);
				
				fprintf(stdout,"\n LOOKUP y MERGE DIFIEREN con ids = %d y %d, occs= (%d,%d)\n",
						patternSets[currPhrase][0], patternSets[currPhrase][1], numocc, numoccM);			
				fprintf(stderr,"\nLOOKUP::");
				for (f=0; f<numocc;f++) {
					fprintf(stderr,"[%6d]",occs[f]);						
				}
				fprintf(stderr,"\nMERGE:::");
				for (f=0; f<numoccM;f++) {					
					fprintf(stderr,"[%6d]",occsM[f]);						
				}
				exit(0);
			}
			else {
				uint a;
				for (a=0;a<numocc;a++) {
					if (occs[a] != occsM[a]) {
						fprintf(stdout,"\n LOOKUP Y MERGE DIFIEREN en occs[%d], (%d,%d)",a,occs[a],occsM[a]);
						exit(0);
					}
				}				
			}
			fprintf(stdout,"\n[%d] LOOKUP y MERGE devuelven el mismo resultado para par: (%d,%d)",currPhrase, patternSets[currPhrase][0], patternSets[currPhrase][1]);
			//fprintf(stderr,"\n");

		}
		
		free(occsM);
	}
#endif	
		
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
	endResults(  matlab, outfilename, numPhrases);

	freePatternSetsIntersect(patternSets, numOfPatterns);

	fprintf (stderr, "\nTotal Num occs found = %u\n", tot_numocc);
	fprintf (stderr, "Int2 time = %.8f msecs\n", tot_time*1);
	fprintf (stderr, "Int2_time/numOfPatterns = %.8f msecs/patt\n\n",
		 (tot_time * 1) / numOfPatterns);
	fprintf (stderr, "(Load_time+Int2_time)/numOfPatterns = %.8f msecs/patt\n\n",
		 ((Load_time+tot_time) * 1) / numOfPatterns);	

	fprintf (stderr, "Int2_time/occ = %.8f msecs per post \n\n",
		 (tot_time * 1) / tot_numocc);   
		 
/**/	fprintf(timeFile,"\n\t occs = %9u :: time = %.8f msecs ",tot_numocc,tot_time*1);
/**/	fprintf(timeFile,"\n\t    avg time per pattern = %.8f msecs ",(tot_time * 1) / numOfPatterns);
/**/	fprintf(timeFile,"\n\t      avg time per occur = %.8f msecs \n",(tot_time * 1) / tot_numocc);
/**/	fprintf(timeFile,"\n\t      index size = %.4f Mbytes \n",Index_size/1024.0);  //in Megabytes

		 		
			 
}



double
getTimeOriginal (void)
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

double
getTimeMSold (void)   // OJO!!! EN MILISEGUNDOS ALENEX!!!
{
	double usertime;// , systime;
	struct rusage usage;

	getrusage (RUSAGE_SELF, &usage);

	usertime = (double) usage.ru_utime.tv_sec *1000.0 +
		(double) usage.ru_utime.tv_usec / 1000.0;

	//systime = (double) usage.ru_stime.tv_sec*1000.0 +
	//	(double) usage.ru_stime.tv_usec / 1000.0;

	//return (usertime + systime);
	return (usertime );
}

double
getTime (void)   // OJO!!! EN MILISEGUNDOS CORREXIDO SIGIR!!!
{
	double usertime;  //, systime;
	struct rusage usage;

	getrusage (RUSAGE_SELF, &usage);

	usertime = (double) usage.ru_utime.tv_sec *1000.0 +
		(double) usage.ru_utime.tv_usec / 1000.0;

	//systime = (double) usage.ru_stime.tv_sec*1000.0 +
	//	(double) usage.ru_stime.tv_usec / 1000.0;

	//return (usertime + systime);
	//return (usertime/1000 );
	return (usertime);   // RETURNED IN SECONDS * 1000  --> MILLISECONDS
}



void print_search_usage(char * progname) {	
	fprintf(stderr, "\nThe program loads <index> and then executes over it the\n");
	fprintf(stderr, "queries it receives from the standard input. The standard\n");
	fprintf(stderr, "input comes in the format of the files written by \n");
	fprintf(stderr, "genpatterns or genintervals.\n");
	fprintf(stderr, "%s reports on the standard error time statistics\n", progname);
	fprintf(stderr, "regarding to running the queries.\n\n");
	fprintf(stderr, "Usage:  %s <index> <B>  <type> <numPatts> <repetitions> [V] [search_opts] < patt_file\n", progname);
	fprintf(stderr, "\n\t<type>   denotes the type of queries:\n");
	fprintf(stderr, "\t         %c <numListsToExtract><iterations per list>\n", EXTRACT);
	fprintf(stderr, "\t             Extracts the values of the lists: memory is allocated\n");
	fprintf(stderr, "\t         %c <numFrases><iterations per frase>\n", INTERSECT2_SVS_U);
	fprintf(stderr, "\t             Intersection of two lists (Using Uncompress-fsearch)\n");
	fprintf(stderr, "\t         %c <numFrases><iterations per frase>\n", INTERSECT2_MERGE);
	fprintf(stderr, "\t             Intersection of two lists (Using uncompress+merge approach)\n");

	fprintf(stderr, "\t         %c <numFrases><iterations per frase>\n", INTERSECT2_SKIP_LOOKUP);
	fprintf(stderr, "\t             Intersection of two lists (Using skipping-lookup-sampling)\n");	
	fprintf(stderr, "\n\t[V]      with this option it reports on the standard output\n");
	fprintf(stderr, "\t         the results of the queries. The results file might be\n");
	fprintf(stderr, "\t         compared with a trusted one by the compare program.\n\n");
}



















