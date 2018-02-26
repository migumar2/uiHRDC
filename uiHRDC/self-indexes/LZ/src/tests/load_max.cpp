#include <cstdio>
#include <cstdlib>
#include "../utils.h"
#include "../static_selfindex.h"
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
	unsigned int numocc, length, tot_numocc = 0, numpatt;
	double tot_time = 0, load_time = 0;
	unsigned char *pattern;
    std::vector<unsigned int>* occ_pos;
    unsigned int max_occs = atoi(argv[2]);
	pfile_info(&length, &numpatt);

	pattern = new unsigned char[length+1];
    pattern[length] = '\0';
	if (pattern == NULL){
		std::cerr<<"Error: cannot allocate pattern"<<std::endl;
		exit(EXIT_FAILURE);
	}

    /*Load Index*/
    lz77index::utils::startTime();
	lz77index::static_selfindex* idx = lz77index::static_selfindex::load(argv[1]);
	load_time += lz77index::utils::endTime();
    
	while(numpatt){
		if(fread(pattern, sizeof(unsigned char), length, stdin) != length){
			fprintf(stderr, "Error: cannot read patterns file\n");
			exit(EXIT_FAILURE);
		}

		if(argc>3){//verbose
        	std::cout<<"Pattern: "<<pattern<<std::endl;
        }
		/* Locate */
		lz77index::utils::startTime();
		occ_pos = idx->locate(pattern, length, &numocc, max_occs);
		tot_time += lz77index::utils::endTime();
		
		tot_numocc += numocc;
		numpatt--;
		
		if(argc>3){//verbose
            std::cout<<"Occs: "<<numocc<<std::endl;
            for(unsigned int i=0;i<numocc;i++){
                std::cout<<occ_pos->at(i)<<" ";
            }
            std::cout<<std::endl<<"********************************"<<std::endl;
		}
		delete occ_pos;
	}
    delete idx;
	fprintf(stderr, "Total Num occs found = %u\n", tot_numocc);
	fprintf(stderr, "Locate time = %.2f secs\n", tot_time);
	fprintf(stderr, "Locate_time/Num_occs = %.4f msec/occss\n\n", (tot_time * 1000) / tot_numocc);
	fprintf(stderr, "(Load_time+Locate_time)/Num_occs = %.4f msecs/occs\n\n", ((tot_time+load_time) * 1000) / tot_numocc);

	delete [] pattern;
    return EXIT_SUCCESS;
}
