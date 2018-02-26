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
	unsigned int length, numpatt;
	double f_tot_time = 0, n_tot_time = 0, load_time = 0, time=0;
	unsigned char *pattern;
    unsigned int f_tot = 0, n_tot = 0;

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

		if(argc>2){//verbose
        	std::cout<<"Pattern: "<<pattern<<std::endl;
        }
		/* Locate */
        bool found;
		lz77index::utils::startTime();
		found = idx->exist(pattern, length);
		time = lz77index::utils::endTime();
        if(found){
            f_tot_time += time;        
            f_tot ++;
        }else{
            n_tot_time += time;
            n_tot ++;
        }
		
		numpatt--;
		
		if(argc>2){//verbose
		}
	}
    delete idx;
    fprintf(stderr, "found msec/pattern:%.5f\n",f_tot_time/f_tot*1000);
    fprintf(stderr, "Found Time: %.5f\n",f_tot_time);
    fprintf(stderr, "Found count: %u\n",f_tot);
    fprintf(stderr, "not msec/pattern:%.5f\n",n_tot_time/n_tot*1000);
    fprintf(stderr, "Not Time: %.5f\n",n_tot_time);
    fprintf(stderr, "Not count: %u\n",n_tot);
    
//	fprintf(stderr, "Total Num occs found = %u\n", tot_numocc);
//	fprintf(stderr, "Locate time = %.2f secs\n", tot_time);
//	fprintf(stderr, "Locate_time/Num_occs = %.4f msec/occss\n\n", (tot_time * 1000) / tot_numocc);
//	fprintf(stderr, "(Load_time+Locate_time)/Num_occs = %.4f msecs/occs\n\n", ((tot_time+load_time) * 1000) / tot_numocc);

	delete [] pattern;
    return EXIT_SUCCESS;
}
