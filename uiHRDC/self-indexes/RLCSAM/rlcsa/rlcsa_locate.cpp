#include <cstdlib>
#include <ctime>
#include <iostream>
#include <fstream>
#include <vector>

#include "rlcsa.h"
#include "offsets.h"
#include "misc/utils.h"

#ifdef MULTITHREAD_SUPPORT
#include <omp.h>
#endif


using namespace CSA;

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
	unsigned char origfilename[257];

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

int main(int argc, char** argv)
{
	std::cout << "RLCSA test" << std::endl;
	if(argc < 4)
	{
		std::cout << "Usage: rlcsa_test basename sample [verbose] < patternfile" << std::endl;
		return 1;
	}

	std::cout << "Base name: " << argv[1] << std::endl;
	std::cout << "Sample: " << argv[3] << std::endl;

    	unsigned int len=1000,ordinal=0,numpatt,tot_numocc=0;
   	pfile_info(&numpatt);
    	unsigned int total_size=0;
    	unsigned int n=numpatt;
    	unsigned char* pattern = new unsigned char[len+1];
    	pattern[len] = '\0';
	if (pattern == NULL){
		std::cerr<<"Error: cannot allocate pattern"<<std::endl;
		exit(EXIT_FAILURE);
	}
    
	RLCSA* rlcsa = new RLCSA(argv[1], false,argv[3]);
   	rlcsa->printInfo();
    	rlcsa->reportSize(true);


	// LOADING DOCUMENT BOUNDARIES
	std::ifstream fdocs(argv[2]);
	fdocs.seekg(0,ios_base::end);
	usint ndocs = fdocs.tellg()/sizeof(int);
	fdocs.seekg(0,ios_base::beg);

	usint * doc_array = new usint[ndocs];
	fdocs.read((char*)doc_array,ndocs*sizeof(usint));
	fdocs.close();

    	pair_type result;
    	usint* matches;
    	double time=0;

    	while(numpatt){
		// if(fread(pattern, sizeof(unsigned char), len, stdin) != len){
		if (fscanf(stdin, "%u %u ", &ordinal, &len) != 2) {
			fprintf(stderr, "Error: cannot read patterns file\n");
			exit(EXIT_FAILURE);
		}

		if(fread(pattern, sizeof(unsigned char), len, stdin) != len){
			fprintf(stderr, "Error: cannot read patterns file\n");
			exit(EXIT_FAILURE);
		}

		pattern[len] = 0;

		total_size += len;

		/* Locate */
        	double start = readTimer();
        	result = rlcsa->count((char*)pattern);
        	tot_numocc += length(result);

        	matches = rlcsa->locate(result);

       		if(matches) 
		{ 
			usint *docs = doc_offset_exp(doc_array, ndocs, matches, length(result));
			
			if(argc>4)
			{
				std::cout << "[" << numpatt << "] -> " << length(result) << std::endl;
//				for (int i=0; i<length(result); i++)
//			 	std::cout << "[" << i << "] " << matches[i] << " -> [" << docs[2*i+1] << "," << docs[2*i+2] << "]" << std::endl;
			}

			delete[] matches; delete [] docs;
		}
        	time += readTimer() - start;
		
		numpatt--;
	}

	delete rlcsa; delete [] doc_array;

	fprintf(stderr, "time/occ:%.4f\n\n", (time * 1000) / tot_numocc);
	std::cout << "Patterns:    " << n << " (" << (n / time) << " / sec)" << std::endl;
	std::cout << "Total size:  " << total_size << " bytes (" << (total_size / time) << " / sec)" << std::endl;
	std::cout << "Matches:     " << tot_numocc << " (" << (tot_numocc / time) << " / sec)" << std::endl;
	std::cout << "Time:        " << time << " seconds" << std::endl;
	std::cout << std::endl;
	std::cout << std::endl;

  return 0;
}
