
// COMENT NEXT LINE TO AVOID USING LOGGING.
//#define DO_LOG


#ifdef DO_LOG


FILE *LOG_FILE;

void init_log(char *filename) {	
	LOG_FILE = fopen(filename,"w");
}


void out_results(uint *patts, uint n, uint *occs, uint noccs) {
static uint count_iters =1;

	uint i;
	
	fprintf(LOG_FILE,"\n** SEARCH %d**", count_iters++);
	
	fprintf(LOG_FILE,"\n\t PATTERNS:");
	for (i=0;i<n;i++) fprintf(LOG_FILE,"[%6d]", patts[i]);
	
	fprintf(LOG_FILE,"\n\t NOCCS = %d, OCCS =>",noccs);
	for (i=0;i<noccs;i++) {
		fprintf(LOG_FILE,"\n[%6d]", occs[i]);	
	}
		
}


void end_log() {
	fclose(LOG_FILE);
}

#endif
