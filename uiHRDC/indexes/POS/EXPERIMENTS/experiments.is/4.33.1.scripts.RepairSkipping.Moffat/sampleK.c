#include <stdlib.h>
       #include <stdio.h>


int system(const char *string);


int main (int argc, char *argv[]) {
	if (argc < 3) {
		printf("\n syntax: %s <index_base_name> <sampleK value> \n", argv[0]);exit(0);
	}
	printf("\n call to %s %s %s done \n",argv[0],argv[1],argv[2]);	

	char cmd[100];
	sprintf(cmd,"echo \"sampleK=%s\" > %s.const.samplek  ", argv[2],argv[1]);

	printf("\n\n sampleK parameter set with : %s \n\n ", cmd);				 
	fflush(stdout);fflush(stderr);
	int x = system(cmd);
	fflush(stdout);fflush(stderr);	
	exit(0);
	
}
