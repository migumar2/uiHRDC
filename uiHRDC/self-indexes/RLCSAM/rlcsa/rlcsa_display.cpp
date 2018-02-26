#include <algorithm>
#include <cstdlib>
#include <ctime>
#include <fstream>
#include <iostream>

#include "rlcsa.h"
#include "misc/utils.h"

#ifdef MULTITHREAD_SUPPORT
#include <omp.h>
const int MAX_THREADS = 64;
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

int main(int argc, char** argv){
    unsigned int num_pos, numchars;
	double tot_time = 0, load_time = 0;
    unsigned char* extract_str;
    unsigned int from, to;
	unsigned char orig_file[257];

	int error = fscanf(stdin, "# number=%u length=%u file=%s\n", &num_pos, &numchars, orig_file);
	if(error != 3){
		fprintf(stderr, "Error: Intervals file header is not correct\n");
		exit(EXIT_FAILURE);
	}
	//fprintf(stderr, "# number=%lu length=%lu file=%s\n", num_pos, numchars, orig_file);
    unsigned int tot_ext = num_pos*numchars;
    /*Load Index*/
    double start = readTimer();
	RLCSA* rlcsa = new RLCSA(argv[1],false,argv[3]);
	load_time += readTimer() - start;

    if(!rlcsa->supportsDisplay()){
        std::cerr << "Error: Display is not supported!" << std::endl;
        return 2;
    }
    rlcsa->printInfo();
    rlcsa->reportSize(true);
    unsigned int r_length;
	while(num_pos){
		if(fscanf(stdin,"%u,%u\n", &from, &to) != 2) {
			fprintf(stderr, "Cannot read correctly intervals file\n");
			exit(EXIT_FAILURE);
		}

		/* Extract */
		start = readTimer();
		extract_str = rlcsa->display(from+0 , to-from+1, (unsigned int)0, r_length);
		tot_time += readTimer()-start;
		
		num_pos--;
		
		if(argc>4){//verbose
			std::cout<<"From: "<<from<<" to: "<<to<<std::endl;
            for(unsigned int i=0;i<to-from+1;i++){
                std::cout<<extract_str[i];
            }
            std::cout<<std::endl<<"********************************"<<std::endl;
		}
		delete extract_str;
	}
    delete rlcsa;
	fprintf (stdout, "mchars/s:%.4f\n", tot_ext / tot_time/1000000);
	fprintf (stdout, "Total num chars extracted = %u\n", tot_ext);
	fprintf (stdout, "Extract time = %.2f secs\n", tot_time);
	fprintf (stdout, "Extract_time/Num_chars_extracted = %.4f msecs/chars\n", (tot_time * 1000) / tot_ext);
	fprintf (stdout, "Num_chars_extracted/extract_time = %.4f chars/sec\n", tot_ext / tot_time);
	fprintf (stdout, "(Load_time+Extract_time)/Num_chars_extracted = %.4f msecs/chars\n", ((load_time+tot_time) * 1000) / tot_ext);
	fprintf (stdout, "Num_chars_extracted/(Load_time+Extract_time) = %.4f chars/sec\n", tot_ext/((load_time+tot_time)));

    return EXIT_SUCCESS;

}
