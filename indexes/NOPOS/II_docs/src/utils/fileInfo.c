
#include "fileInfo.h"

size_t fileSize (char *filename){
	FILE *fpText;
	size_t fsize;
	fpText = fopen(filename,"rb");
	fsize=0;
	if (fpText) {
		fseek(fpText,0,2);
		fsize= (size_t) ftell(fpText);
		fclose(fpText);
		////fprintf(stderr,"fileSize = %ld",fsize);
	}
	return fsize;
}

/*copies from infile to outfile */
void copyFile (char *infile, char *outfile){
	FILE *in, *out;
	size_t fsize;
	
	if ( (in = fopen(infile,"rb")) == NULL) {
		printf("Cannot open file %s\n", infile); exit(0);
	}	

	unlink(outfile);
	if( (out = fopen(outfile, "w")) == NULL) {
		printf("Cannot create file %s\n", outfile);
		exit(0);
	}	

	fsize=fileSize(infile);
	if (fsize) {
		char *buff = (char *) malloc(sizeof(char)*fsize);
		if (!buff) {
			printf("\n could not allocate %lu bytes in 'copyFile'... exiting",fsize);exit(0);
		}
		if (fread(buff,sizeof(char),fsize,in)) {
			fwrite(buff,sizeof(char),fsize,out);			
		}		
		free(buff);
	}			
	fclose(in);
	fclose(out);	
}



