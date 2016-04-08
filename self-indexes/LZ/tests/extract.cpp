#include <cstdio>
#include <cstdlib>
#include <unistd.h>

#include "../utils.h"
#include "../static_selfindex.h"

#include <iostream>
#include <fstream>
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

int main(int argc, char** argv){
	unsigned int num_pos, numchars;
	double tot_time = 0, load_time = 0, aggregated_time = 0;
    	unsigned char* extract_str;
	unsigned char orig_file[257];

	unsigned int RUNS = 10;

	int error = fscanf(stdin, "# number=%u length=%u file=%s\n", &num_pos, &numchars, orig_file);
	if(error != 3){
		fprintf(stderr, "Error: Intervals file header is not correct\n");
		exit(EXIT_FAILURE);
	}
	//fprintf(stderr, "# number=%lu length=%lu file=%s\n", num_pos, numchars, orig_file);
   	unsigned int tot_ext = num_pos*numchars;

    	/*Load Index*/
    	lz77index::utils::startTime();
	lz77index::static_selfindex* idx = lz77index::static_selfindex::load(argv[1]);
	load_time += lz77index::utils::endTime();

	// PRINTF Index Size
	fprintf(stderr, "%u;;%u;;", idx->size(), tot_ext);
	fflush(stderr);

	vector<unsigned int> vfrom(num_pos);
	vector<unsigned int> vto(num_pos);
	unsigned int from, to;

	for (unsigned int i=0; i<num_pos; i++)
	{
		if(fscanf(stdin,"%u,%u\n", &from, &to) != 2) {
			fprintf(stderr, "Cannot read correctly intervals file\n");
			exit(EXIT_FAILURE);
		}
		
		vfrom[i] = from;
		vto[i] = to;
	}

	for (unsigned int i=0; i<RUNS; i++)
	{
		for (unsigned int j=0; j<num_pos; j++)
		{
			/* Extract */
			lz77index::utils::startTime();
			extract_str = idx->display(vfrom[j], vto[j]);
			tot_time += lz77index::utils::endTime();
		
			delete [] extract_str;
		}

		// PRINTF Run extract time
		fprintf(stderr, "%.2f;", tot_time);
		fflush(stderr);

		aggregated_time += tot_time;
		tot_time = 0;
		sleep(5);
	}

	// PRINTF Averaged extract time
	fprintf(stderr, ";%.2f \n", aggregated_time/RUNS);
	fflush(stderr);

	delete idx;
	
/*
	fprintf (stderr, "\n");
	fprintf (stderr, "mchars/s:%.4f\n", tot_ext / tot_time/1000000);
	fprintf (stderr, "Total num chars extracted = %u\n", tot_ext);
	fprintf (stderr, "Extract time = %.2f secs\n", tot_time);
	fprintf (stderr, "Extract_time/Num_chars_extracted = %.4f msecs/chars\n", (tot_time * 1000) / tot_ext);
	fprintf (stderr, "Num_chars_extracted/extract_time = %.4f chars/sec\n", tot_ext / tot_time);
	fprintf (stderr, "(Load_time+Extract_time)/Num_chars_extracted = %.4f msecs/chars\n", ((load_time+tot_time) * 1000) / tot_ext);
	fprintf (stderr, "Num_chars_extracted/(Load_time+Extract_time) = %.4f chars/sec\n", tot_ext/((load_time+tot_time)));
*/


    return EXIT_SUCCESS;
}
