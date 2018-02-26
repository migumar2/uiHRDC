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
    double f_tot_time=0,n_tot_time=0;
    unsigned int f_tot=0,n_tot=0;
    
    while(numpatt){
		if(fread(pattern, sizeof(unsigned char), len, stdin) != len){
			fprintf(stderr, "Error: cannot read patterns file\n");
			exit(EXIT_FAILURE);
		}
		if(argc>3){//verbose
			std::cout<<"Pattern: "<<pattern<<std::endl;
        }
		/* Locate */
        double start = readTimer();
        result = rlcsa->count((char*)pattern);
        time = readTimer() - start;
        if(length(result)>0){
            f_tot_time += time;        
            f_tot ++;
        }else{
            n_tot_time += time;
            n_tot ++;
        }
		
		numpatt--;
		
		if(argc>3){//verbose
            std::cout<<length(result)<<std::endl<<"********************************"<<std::endl;
		}
	}
    delete rlcsa;
    fprintf(stderr, "found msec/pattern:%.5f\n",f_tot_time/f_tot*1000);
    fprintf(stderr, "Found Time: %.5f\n",f_tot_time);
    fprintf(stderr, "Found count: %u\n",f_tot);
    fprintf(stderr, "not msec/pattern:%.5f\n",n_tot_time/n_tot*1000);
    fprintf(stderr, "Not Time: %.5f\n",n_tot_time);
    fprintf(stderr, "Not count: %u\n",n_tot);
/*  std::cout << "Patterns:    " << n << " (" << (n / time) << " / sec)" << std::endl;
  std::cout << "Total size:  " << total_size << " bytes (" << (total_size / time) << " / sec)" << std::endl;
  std::cout << "Matches:     " << tot_numocc << " (" << (tot_numocc / time) << " / sec)" << std::endl;
  std::cout << "Time:        " << time << " seconds" << std::endl;
  std::cout << std::endl;
  std::cout << std::endl;*/

  return 0;
}
