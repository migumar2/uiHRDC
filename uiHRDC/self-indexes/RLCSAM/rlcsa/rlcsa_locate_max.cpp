#include <cstdlib>
#include <ctime>
#include <iostream>
#include <vector>

#include "rlcsa.h"
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

int main(int argc, char** argv)
{
  std::cout << "RLCSA test" << std::endl;
  if(argc < 3)
  {
    std::cout << "Usage: rlcsa_test basename sample [verbose] < patternfile" << std::endl;
    return 1;
  }

  std::cout << "Base name: " << argv[1] << std::endl;
  std::cout << "Sample: " << argv[2] << std::endl;
unsigned int max_occs = atoi(argv[3]);
    unsigned int len,numpatt,tot_numocc=0;
    pfile_info(&len, &numpatt);
    unsigned int total_size= len*numpatt;
    unsigned int n=numpatt;
	unsigned char* pattern = new unsigned char[len+1];
    pattern[len] = '\0';
	if (pattern == NULL){
		std::cerr<<"Error: cannot allocate pattern"<<std::endl;
		exit(EXIT_FAILURE);
	}
    RLCSA* rlcsa = new RLCSA(argv[1], false,argv[2]);
    rlcsa->printInfo();
    rlcsa->reportSize(true);

    pair_type result;
    usint* matches;
    double time=0;
    while(numpatt){
		if(fread(pattern, sizeof(unsigned char), len, stdin) != len){
			fprintf(stderr, "Error: cannot read patterns file\n");
			exit(EXIT_FAILURE);
		}
		if(argc>4){//verbose
			std::cout<<"Pattern: "<<pattern<<std::endl;
        }
		/* Locate */
        unsigned int occs;
        double start = readTimer();
        result = rlcsa->count((char*)pattern);
        matches = rlcsa->locate(result,max_occs,&occs);
        tot_numocc += occs;
        if(matches) { delete[] matches; }
        time += readTimer() - start;
		
		numpatt--;
		
		if(argc>4){//verbose
            std::cout<<length(result)<<std::endl<<"********************************"<<std::endl;
		}
	}
    delete rlcsa;
	fprintf(stderr, "time/occ:%.4f\n\n", (time * 1000) / tot_numocc);
  std::cout << "Patterns:    " << n << " (" << (n / time) << " / sec)" << std::endl;
  std::cout << "Total size:  " << total_size << " bytes (" << (total_size / time) << " / sec)" << std::endl;
  std::cout << "Matches:     " << tot_numocc << " (" << (tot_numocc / time) << " / sec)" << std::endl;
  std::cout << "Time:        " << time << " seconds" << std::endl;
  std::cout << std::endl;
  std::cout << std::endl;

  return 0;
}
