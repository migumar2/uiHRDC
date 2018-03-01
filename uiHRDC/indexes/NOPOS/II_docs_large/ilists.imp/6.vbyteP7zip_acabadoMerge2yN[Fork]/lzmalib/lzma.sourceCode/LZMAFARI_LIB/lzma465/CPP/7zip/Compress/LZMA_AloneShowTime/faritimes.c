#include <unistd.h>
#include <stdio.h>

/* only for getTime() */
#include <sys/time.h>
#include <sys/times.h>
#include <sys/resource.h>

#define GET_TIME_DIVIDER ((double) 1.0 )	 //getTime deals with "seconds" as time unit.
#define GET_TIME_UNIT "sec"
#define MSEC_TIME_DIVIDER  ((double) 1000.0) //1 sec = 1000 msec
#define MSEC_TIME_UNIT 	  "msec"
#define SEC_TIME_DIVIDER  ((double) 1.0	)	 //1 sec = 1 sec
#define SEC_TIME_UNIT 	  "sec"

//returns user-time in seconds
double iniTimeFari (void)
{
	double usertime, systime;
	struct rusage usage;
	double currtime;

	getrusage (RUSAGE_SELF, &usage);
	usertime = (double) usage.ru_utime.tv_sec * GET_TIME_DIVIDER +
		(double) usage.ru_utime.tv_usec / ( ((double) 1000000.0) / GET_TIME_DIVIDER);

	systime = (double) usage.ru_stime.tv_sec * GET_TIME_DIVIDER +
		(double) usage.ru_stime.tv_usec / ( ((double)1000000.0) / GET_TIME_DIVIDER);
	//currtime = usertime + systime;
	currtime = usertime;
	return (currtime );
}

//returns user-time in seconds
double endTimeFari(double *iniTime) {
	double usertime, systime;
	struct rusage usage;
	double currtime;

	getrusage (RUSAGE_SELF, &usage);
	usertime = (double) usage.ru_utime.tv_sec * GET_TIME_DIVIDER +
		(double) usage.ru_utime.tv_usec / ( ((double) 1000000.0) / GET_TIME_DIVIDER);

	systime = (double) usage.ru_stime.tv_sec * GET_TIME_DIVIDER +
		(double) usage.ru_stime.tv_usec / ( ((double)1000000.0) / GET_TIME_DIVIDER);
	//currtime = usertime + systime;
	currtime = usertime;	
	return (currtime - *iniTime);
}

//if the file "filename" exists (and it contains a double value at the beginning).
// - we read that value (it is the accumulated execution time from previous executions).
// - we add the current execution time to that value
// - we finally store the new "accumulated execution time" into the file again.
//else
// - we create that file and store the current execution time in it.
void saveTimesFile( double totaltime) {
	FILE *in, *out;
	double prevtimes;
	char filename[100] = "times.tmp_lzma_compressiontimes.fari.txt";
	
	if ( (in = fopen(filename,"rb")) ==NULL) { //non existing file ==> 0.0
		prevtimes= 0.0;
	}
	else { // read the previous total-time
		fscanf(in, "%lf",&prevtimes);
		fclose(in);
	}
		
		//fprintf(stderr,"\n prev time is %lf",prevtimes);
	// now we store the new "accumulated execution time"
	unlink(filename);
	if( (out = fopen(filename, "w")) ==NULL) {
		printf("Cannot open file %s for storing running times\n", filename);
		return;
	}	
	fprintf(out,"%lf seconds (acumulated compression user-time of lzma coder)\n",totaltime + prevtimes);
	fprintf(out,"%lf seconds (*last run* compression user-time of lzma coder)\n",totaltime);
	fclose (out);
	printf("[compression user-time = %3.10lf sec. (acum time = %3.10lfsec)]\n",totaltime, prevtimes+totaltime);			
}
