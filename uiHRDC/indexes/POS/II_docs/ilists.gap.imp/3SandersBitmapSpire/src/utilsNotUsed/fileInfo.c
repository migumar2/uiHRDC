
#include "fileInfo.h"

unsigned long fileSize (char *filename){
	FILE *fpText;
	unsigned long fsize;
	fpText = fopen(filename,"rb");
	fsize=0;
	if (fpText) {
		fseek(fpText,0,2);
		fsize= ftell(fpText);
		fclose(fpText);
		////fprintf(stderr,"fileSize = %ld",fsize);
	}
	return fsize;
}

/*copies from infile to outfile */
void copyFile (char *infile, char *outfile){
	FILE *in, *out;
	unsigned long fsize;
	
	if ( (in = fopen(infile,"rb")) <0) {
		printf("Cannot open file %s\n", infile); exit(0);
	}	

	unlink(outfile);
	if( (out = fopen(outfile, "w")) < 0) {
		printf("Cannot open file %s\n", outfile);
		exit(0);
	}	

	fsize=fileSize(infile);
	if (fsize) {
		char *buff = (char *) malloc(sizeof(char)*fsize);
		if (fread(buff,sizeof(char),fsize,in)) {
			fwrite(buff,sizeof(char),fsize,out);			
		}		
		free(buff);
	}			
	fclose(in);
	fclose(out);	
}

