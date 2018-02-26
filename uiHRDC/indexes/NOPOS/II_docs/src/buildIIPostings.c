#include "buildFacade.h"

#define LOGTIMES

#ifdef LOGTIMES  // file used to log the results of the searches.
/**/ FILE *timeFile; 
#endif

/**------------------------------------------------------------------ 
  *  MAIN PROGRAM: FOR BUILDING THE INDEX
  *------------------------------------------------------------------ */
#ifdef FACADEWITHMAIN

/* macro to detect and notify errors  */
#define IFERROR(error) {{if (error) { fprintf(stderr, "%s\n", error_index(error)); exit(1); }}}


	/*********************************************************************/
	
	double getTime (void)
	{
		double usertime, systime;
		struct rusage usage;
	
		getrusage (RUSAGE_SELF, &usage);
		usertime = (double) usage.ru_utime.tv_sec  +
			(double) usage.ru_utime.tv_usec / ( ((double) 1000000.0) );
		systime = (double) usage.ru_stime.tv_sec  +
			(double) usage.ru_stime.tv_usec / ( ((double)1000000.0) );
		//return (usertime + systime);
		return (usertime );
	}

	int main(int argc, char* argv[])
	{
		
		char *infile, *outbasename;	// Name of in/out files
		//byte *inputBuffer;
		//ulong finsize;
		int error=0;
	
	 	//int f_in;
		void *Index;

		//uint ksample, blocksize;
		double start, end;
		start = getTime();
		
		printf("\n*Building a block-addressing index, over text compressed with SCDC");
		printf("\n*CopyRight (c) 2007 [LBD & G.N.]\n\n");
	
		// Reads input parameters from command line.
		if(argc < 4) {
			printf("Use: %s <in file> <out basename (without.post)> \"[parameters:: blockSize=8192; qgram=8 ; ...]\">  \n", argv[0]);
			printf("Example of use:");
			printf("%s inbasenamefile outBasename \"blocksize=8192;qgram=4;path2repaircompressor=./src/repair64bit/repairCompressor;path2lzmaencoder=./lib/lzmaEncoder;lenBitmapDiv=8; int2=svs;intn=svs;fsearch=expM\"",argv[0]);
			printf("\n\n");
			exit(0);
		}
	
		
		// Reads params (input file, output basename, and stopwords file)
		infile = argv[1];
		outbasename = argv[2];
		//blocksize = atoi(argv[3])*1024;
		//ksample = atoi(argv[4]);
		
		/** prepares parameters for the index **/	
		char *params = NULL;

		if (argc >= 4) { 
			int i;	
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
			


	#ifdef LOGTIMES  
	/**/	char outFilename[256]="";	
	/**/	char *progname = argv[0];
	/**/	{int i ; for (i =strlen(argv[0])-2;i>0;i--) { if (argv[0][i] =='/') {progname = &(argv[0][i]);progname++; break; } } }
	/**/	sprintf(outFilename, "times_info_%s.txt", progname);
	/**/	timeFile = fopen(outFilename,"a");
	/**/	if (!timeFile) {printf("\n could not open log_times_info file (%s)... exitting\n",outFilename); exit(0);}
	/**/	fprintf(timeFile,"\n Building index qgrams: %s %s %s %s\n",argv[0],argv[1], argv[2],  params);
	#endif
				
		}	 	
					
	{
		//build_index (inputBuffer, finsize, stopwordsfile, &Index);  /** building the index */
		error = build_index_from_postings(infile, params,  &Index);
		IFERROR (error);

		error = setDefaultIntersectionOptions(Index, params);
		IFERRORIL (error);
		
		
		//fprintf(stderr,"\n basename = %s, outname = %s, blocksize = %d, sampleK = %d\n",infile, outbasename, blocksize,ksample);
		//build_index_baseFile((uchar *)infile, blocksize,  ksample,  &Index);

		/** saving the index to disk*/
		error = save_index (Index, outbasename);
		IFERROR (error);

		/** tells the mem used by the index */
		ulong indexsize;		
		error = index_size(Index, &indexsize);
		IFERROR (error);
		fprintf(stderr,"\nIndex occupied %lu bytes (in memory)",indexsize);

		/** shows the size of the original indexed text **/	
		ulong Text_length;
		error = get_length(Index, &Text_length);
		IFERROR (error);

		/** gets some "extra" data from the index **/
		char extendedinfo[16000];		
		error = index_info(Index, extendedinfo);
		IFERROR (error);	
	
		/** freeing the index */	
		fprintf(stderr,"\nbefore calling to buildII.c:: freeIndex\n\n"); fflush(stderr);		 			
			 			
		free_index(Index);	 			
		fprintf(stderr,"\n after free\n"); fflush(stderr);
		fprintf(stderr,"\n\n");

		end = getTime();	
			
		fflush(stderr);fflush(stdout);
		fprintf(stderr, "\nBuilding time: %.3f secs\n", end-start );
		fprintf(stdout,"Input: %lu bytes --> Output %lu bytes.\n", Text_length, indexsize);
		fprintf(stdout,"Overall compression --> %.2f%% (%.2f bits per char).\n\n",
     			(100.0*indexsize)/Text_length, (indexsize*8.0)/Text_length);
		fprintf(stdout,"\n\t Extended info \n %s\n\n",extendedinfo);     			


	#ifdef LOGTIMES 

	/**/	fprintf(timeFile, "\tBuilding time: %.3f secs\n", end-start );
	/**/	fprintf(timeFile, "\tInput: %lu bytes --> Output %lu bytes.\n", Text_length, indexsize);
	/**/	fprintf(timeFile,"\tOverall compression --> %.2f%% (%.2f bits per char).\n",
     											(100.0*indexsize)/Text_length, (indexsize*8.0)/Text_length);
	/**/	fprintf(timeFile,"\n\t Extended info \n %s\n=====================================\n",extendedinfo);
     											
	#endif 		

		


/**
		
	 //TEST OF USE
		// **loading the index2 
		void *Index2;
		load_index(outbasename, &Index2);
		
		error = setDefaultIntersectionOptions(Index2, params);
		IFERRORIL (error);

		

		// SEARCHING FOR A TEXT PATTERN (word/phrase).
		{unsigned char textPattern[MAX_TEXT_PATTERN_SIZE];
		 int occ;
		 int len;
		 uint *occs;
		 int i;
		 printf("\nSEARCH TEST for LOCATE\n");
			while(1) {	
				printf("Intro string: ");
				fgets((char*)textPattern, MAX_TEXT_PATTERN_SIZE, stdin);
				len = strlen((char*)textPattern);
				if (!strcmp((char*)textPattern,"\n") ) break;
				textPattern[len-1] = '\0';
				len --;
				
				locate(Index2, textPattern, len, (ulong **)&occs, (ulong *)&occ);
				
				printf("\n*** %s occurs %d times: In the source text in positions:\n\t",textPattern,occ);
				for (i=0;i<occ;i++) 
					printf("[%u]",occs[i]);
				fflush(stderr);	
				if (occ) free(occs);		
							
			if (!strcmp((char*)textPattern,"\n") ) break;
			}
		}	

		//printf("BCS.SIZE = %d",((twcsa*)Index2)->occs.bcsSize);
		
		// DISPLAYING THE OCCURRENCES OF A TEXT PATTERN (word/phrase).
		{unsigned char textPattern[MAX_TEXT_PATTERN_SIZE];
		 int occ;
		 printf("\nSEARCH TEST for DISPLAY (non pizzachili)\n");
			while(1) {	
				printf("Intro string: ");
				fgets((char*)textPattern, MAX_TEXT_PATTERN_SIZE, stdin);
				if (!strcmp((char*)textPattern,"\n") ) break;
				textPattern[strlen((char*)textPattern)-1] = '\0';
				occ = displayTextOcurrences(Index2,textPattern,strlen((char*)textPattern),8);
				printf("Ocurrences = %d\n", occ);
				if (!strcmp((char*)textPattern,"\n") ) break;
			}
		}		
		
		
			free_index(Index2);	
			
//	**/
	
	
	
	}	

	free(params);	
	#ifdef LOGTIMES  
	/**/	fclose(timeFile);
	#endif		
}

#endif
