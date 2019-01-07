#include <stdlib.h>
#include <stdio.h>
       #include <sys/types.h>
       #include <sys/stat.h>
       #include <unistd.h>
#include <string.h>



int system(const char *string);


int main (int argc, char *argv[]) {
	if (argc < 3) {
		printf("\n syntax: %s <file> <msg> \n", argv[0]);exit(0);
	}
	
	FILE * f= fopen(argv[1],"r");
	if (f==NULL) {
		fprintf(stderr,"\n unable to open %s, %s program ends for parameter %s",argv[1],argv[0],argv[2]);
		exit(0);
	}
	char * msg = argv[2];
	
	struct stat b;
	stat(argv[1], &b);
	ulong size = b.st_size;
	if(size ==0) {printf("\n file %s is empty, aborting %s",argv[1],argv[0]);exit(0);}
	
	char * data = (char *) malloc (size + strlen(msg)+100);
	
	size_t nread= fread(data,sizeof(char),size,f);
	fclose(f);
	
	if (nread !=size)  {printf("\n could not read whole file %s, aborting %s",argv[1],argv[0]);exit(0);}

	uint i =size-1;
	while (i>0 && data[i] != '\n') i--;
	//while (i>0 && data[i] == '\n') i--;
	sprintf(&data[i],"\t#%s\n",msg);

	unlink(argv[1]);
	f= fopen(argv[1],"w");
	fwrite(data,sizeof(char),strlen(data),f);
	fclose(f);
	

exit(0);	
}
