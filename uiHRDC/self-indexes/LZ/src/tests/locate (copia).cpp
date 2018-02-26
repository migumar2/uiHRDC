#include <fstream> 
#include <cstdio>
#include <cstdlib>
#include <unistd.h>

#include "../utils.h"
#include "../static_selfindex.h"
#include "../offsets.h"

#include <iostream>
#include <vector>
using namespace std;

/* Open patterns file and read header */
void pfile_info(unsigned int* length, unsigned int* numpatt){
	int error;
	unsigned char c;
	unsigned char origfilename[257];

	error = fscanf(stdin, "# number=%u length=%u file=%s forbidden=", numpatt, length, origfilename);
	if (error != 3)	{
		fprintf (stderr, "Error: Patterns file header not correct\n");
		perror ("run_queries");
		exit (1);
	}
	//fprintf (stderr, "# patterns = %lu length = %lu file = %s forbidden chars = ", *numpatt, *length, origfilename);
	while ( (c = fgetc(stdin)) != 0) {
		if (c == '\n') break;
		//fprintf (stderr, "%d",c);
	}
	//fprintf(stderr, "\n");
}

void pfile_info(unsigned int* numpatt){
	int error;
	unsigned char c;

	error = fscanf(stdin, "#numberOfFrases::%u", numpatt);

	if (error != 1)	{
		fprintf (stderr, "Error: Patterns file header not correct\n");
		perror ("run_queries");
		exit (1);
	}
	//fprintf (stderr, "# patterns = %lu length = %lu file = %s forbidden chars = ", *numpatt, *length, origfilename);
	while ( (c = fgetc(stdin)) != 0) {
		if (c == '\n') break;
		//fprintf (stderr, "%d",c);
	}
	//fprintf(stderr, "\n");
}

int main(int argc, char** argv){
	unsigned int numocc, length=1000, ordinal=0, tot_numocc = 0;
	double tot_time = 0, load_time = 0, aggregated_time = 0;
    	std::vector<unsigned int>* occ_pos;

	unsigned int RUNS = 10;

    	/*Load Index*/
    	lz77index::utils::startTime();
	lz77index::static_selfindex* idx = lz77index::static_selfindex::load(argv[1]);
	load_time += lz77index::utils::endTime();

	if (argc == 3) {
		// Positions will mapped to (document,offset).... the
		// corresponding document boundaries must be loaded

		// LOADING DOCUMENT BOUNDARIES
		ifstream fdocs(argv[2]);
		fdocs.seekg(0,ios_base::end);
		uint ndocs = fdocs.tellg()/sizeof(int);
		fdocs.seekg(0,ios_base::beg);

		uint * doc_array = new uint[ndocs];
		fdocs.read((char*)doc_array,ndocs*sizeof(uint));
		fdocs.close();

		// PRINTF Index Size
		fprintf(stderr, "%u;%u;;", idx->size(), (uint)(ndocs*sizeof(unsigned int)));
		fflush(stderr);
	}
	else {
		// PRINTF Index Size
		fprintf(stderr, "%u;;", idx->size());
	}
	







	// LOADING PATTERNS
	unsigned int numpatt;
	pfile_info(&numpatt);

	unsigned char *pattern = new unsigned char[length+1];
    	pattern[length] = '\0';

	if (pattern == NULL){
		std::cerr<<"Error: cannot allocate pattern"<<std::endl;
		exit(EXIT_FAILURE);
	}

	vector<unsigned int> vlengths(numpatt);
	vector<unsigned char*> vpatterns(numpatt);

	for (unsigned int i=0; i<numpatt; i++)
	{
		if (fscanf(stdin, "%u %u ", &ordinal, &length) != 2) {
			fprintf(stderr, "Error: cannot read patterns file\n");
			exit(EXIT_FAILURE);
		}

		if(fread(pattern, sizeof(unsigned char), length, stdin) != length){
			fprintf(stderr, "Error: cannot read patterns file\n");
			exit(EXIT_FAILURE);
		}

		pattern[length] = 0;

		vlengths[i] = length;
		vpatterns[i] = new unsigned char[length+1];
		strcpy((char*)vpatterns[i], (char*)pattern);
	}
    
	for (unsigned int i=0; i<RUNS; i++)
	{
		for (unsigned int j=0; j<numpatt; j++)
		{
			/* Locate */
			lz77index::utils::startTime();

			strcpy((char*)pattern, (char*)vpatterns[j]);

			occ_pos = idx->locate(pattern, vlengths[j], &numocc);			
			uint *docs = doc_offset_exp(doc_array, ndocs, occ_pos->data(), numocc);

			delete occ_pos; delete [] docs;
			tot_time += lz77index::utils::endTime();

			tot_numocc += numocc;	
		}

		// PRINTF Occurrences
		if (i==0) fprintf(stderr, "%u;;", tot_numocc);

		// PRINTF Run locate time
		fprintf(stderr, "%.2f;", tot_time);
		fflush(stderr);

		aggregated_time += tot_time;
		tot_time = 0;
		sleep(5);
	}

	// PRINTF Averaged locate time
	fprintf(stderr, ";%.2f \n", aggregated_time/RUNS);
	fflush(stderr);

        delete idx;
/*
	fprintf(stderr, "Total Num occs found = %u\n", tot_numocc);
	fprintf(stderr, "Locate time = %.2f secs\n", tot_time);
	fprintf(stderr, "time/occ:%.4f\n\n", (tot_time * 1000) / tot_numocc);
	fprintf(stderr, "(Load_time+Locate_time)/Num_occs = %.4f msecs/occs\n\n", ((tot_time+load_time) * 1000) / tot_numocc);
*/

	delete [] pattern;
	for (unsigned int i=0; i<numpatt; i++) delete [] vpatterns[i];
	delete [] doc_array;

    	return EXIT_SUCCESS;
}
