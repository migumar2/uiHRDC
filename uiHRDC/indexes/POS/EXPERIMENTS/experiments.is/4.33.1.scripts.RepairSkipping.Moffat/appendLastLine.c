#include <stdlib.h>
#include <stdio.h>
       #include <sys/types.h>
       #include <sys/stat.h>
       #include <unistd.h>
#include <string.h>



int system(const char *string);


int main (int argc, char *argv[]) {
	if (argc < 3) {
		printf("\n syntax: %s <file> <msg> \n", argv[0],argv[1],argv[2]);exit(0);
	}
	
	FILE * f= fopen(argv[1],"r");
	char * msg = argv[2];
	
	struct stat b;
	stat(argv[1], &b);
	uint size = b.st_size;
	char * data = (char *) malloc (size + strlen(msg)+100);
	
	fread(data,sizeof(char),size,f);
	fclose(f);

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
