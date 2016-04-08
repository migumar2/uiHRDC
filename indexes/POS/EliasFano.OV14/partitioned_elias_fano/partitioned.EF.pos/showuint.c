#include <stdio.h>
       #include <unistd.h>


       #include <stdlib.h>

int main(int argc, char *argv[]) {
	if (argc <2) {
		printf("\n call %s as: %s filename",argv[0],argv[0]);
		exit(0);
	}
	
	unsigned int i;
	FILE *f= fopen(argv[1], "r");
	while (fread(&i,sizeof(unsigned int),1,f) >0) {
		printf("[%u]",i);
	}

	
}
