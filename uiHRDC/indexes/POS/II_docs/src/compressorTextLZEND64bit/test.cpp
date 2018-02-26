#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <math.h>
#include <sys/stat.h> 
#include <stdio.h>

#include "compressor.h"
using namespace std; 

int loadDocBeginngins(uint **offsets, uint *docs, char *basename);
int loadTextInMem (unsigned char **text, uint *size, char *basename);
int saveOutText(unsigned char *T, uint size, char *filename);


int main (int argc, char **argv)
{	
	if (argc == 4)
	{	

	  unsigned int i;
	  unsigned char *text;
	  uint textSize;
	  
	  /** 1 ** Loads the source text into memory. */
	  loadTextInMem(&text, &textSize, argv[1]);	
	  printf("\n\tLoaded Source Text into memory from %s %d", argv[1], textSize);	  
	  
	  /** 2 ** loads the array of document boundaries   */
	  uint ndocs;
	  uint *docboundaries;
	  loadDocBeginngins(&docboundaries, &ndocs,(char *)argv[2]);	
	  printf("\n\t Loaded document boundaries (as uint32) from file: %s...", argv[2]);		
	  printf("\n\t %u docs loaded \n", ndocs);	
	  
	  /** 3 ** compresses the source text, taking into account the doc boundaries */
	  /**   ** No symbols [i.e. phrases in repair] can cross the doc boundaries   */
	  void *ct;
	  build_representation (text, textSize, docboundaries, ndocs, NULL , &ct);
	  printf("\n\t The source text has been compressed ...");		
	  
	  /** 4 ** Obtains the size of the compressed represntation of the text (including doc boundaries) */
	  uint compsize;
	  size_representation(ct, &compsize);
	  printf("\n\t The compressed representation occupies (%u bytes)", compsize);		
	  printf("\n\t Ratio = %2.4f%% \n", (double)compsize*100/textSize);			  
  
	  /** 5 ** Saves the compressed representation to disk **/
	  save_representation(ct, argv[3]);	
	  printf("\n The compressed representation is saved to disk: %s\n",argv[3]);

	  /** 6 ** frees the ct structure that represents the source text in compressed form **/
	  free_representation(ct);	  

	  /** 7 ** Loads the compressed representation again from disk **/
	  load_representation(&ct,argv[3]);	
	  printf("\n The compressed representation is loaded from disk: %s\n",argv[3]);
	  
	  size_representation(ct, &compsize);
	  printf("\n\t The compressed representation occupies (%u bytes)", compsize);		
	  printf("\n\t Ratio = %2.4f%% \n", (double)compsize*100/textSize);			  
	  /** 8 ** extracts the first document */
	  unsigned char *document;
	  uint doclen;
	  extract_doc_representation (ct, 0, &document, &doclen);
	  printf("\n === Decompression of the first doc: == DOC 0 ======");
	  printf("\n%s doclen=%d",document,doclen);
	  printf("\n ====================== ***** ======================\n");
	  free(document);


	  /** 9 ** extracts the whole source text (doc by doc)*/
	  unsigned char *textDec= (unsigned char *) malloc (sizeof(char) * (textSize+1));
	  unsigned char *ptr= textDec;
	  for (i=0;i<ndocs;i++) {
		extract_doc_representation (ct, i, &document, &doclen);
		strncpy ((char *)ptr,(char*)document,doclen);
		free(document);
		ptr+=doclen;
	  }	

	  /** 10 ** compares the decompressed text with the source text.*/
	  for (i=0;i<textSize;i++) {
		if (text[i] != textDec[i]){
			printf("\n decompression failed in recovering the original text!!!(%d)",i);
			exit(-1);
		}
	  }
	  printf("\n decompression succeeded in recovering the original text!!!");
	  

  	  /** 11 ** Writes out the decompressed textDec.*/
	  saveOutText(textDec, textSize, argv[3]);

	  printf("\n Text has been output to file: %s",argv[3]);
	  
	  /** 12 ** Frees resources  **/
	  free(textDec);	  
	  free(docboundaries);
	  free(text);	

	  /** 13 ** frees the ct structure that represents the source text in compressed form **/
	  free_representation(ct);	  
	  printf("\n free_representation done!!");
	  printf("\n Compression/Decompression finished sucessfully\n\n");
	 
	}
	else
	{
		printf( "\n");
		printf( "Usage: %s <input file text> <input file docbeginings> <output file> \n", argv[0]);

		exit(-1);
	}
	
	return 0;
}	
	
	

/** ------------------------------------------------------------------
  * size of a file
  * ------------------------------------------------------------------*/
uint fileSize(char *filename) {
  
  struct stat file_status;		

  if(stat(filename, &file_status) != 0){
	  return(0);
  }
  return (uint) file_status.st_size;
}

/** ------------------------------------------------------------------
  * loads the source plain text into memory
  * ------------------------------------------------------------------*/
int loadTextInMem (unsigned char **text, uint *size, char *filename) {
	int file;
	uint len;
	unsigned char *buffer;

	len= fileSize(filename);
	buffer = (unsigned char *) malloc (sizeof(unsigned char) * len);

	if( (file = open(filename, O_RDONLY)) < 0) {
		printf("Cannot open file %s\n", filename);
		exit(0);
	}
/*	uint errr = read(file, buffer, len);
	if(errr != len) {
		printf("\n error in *read*!\n"); exit(-1);
	}*/
    uint total_read=0;
	uint errr;
    do{
    	errr = read(file, buffer+total_read, len);
        total_read+=errr;
    }while(errr>0);
	
	close(file);			
	*text = buffer;
	*size = len;
	
	return 0;
}



/** **********************************************************************/
/** Some functions used during index construction                        */
/** **********************************************************************/

	/* document beginnings within the source text are marked in offsets */
int loadDocBeginngins(uint **offsets, uint *docs, char *filename) {
	int file;
	if( (file = open(filename, O_RDONLY)) < 0) {
		printf("Cannot open file %s\n", filename);
		exit(0);
	}	

	uint ndocs = (uint) fileSize (filename)/sizeof(uint) -1;
	uint *docBeginnings = (uint *) malloc (sizeof(uint) * (ndocs +1));

	uint errr= read(file, docBeginnings, sizeof(uint) * (ndocs+1));	
	if(errr != (sizeof(uint) * (ndocs+1))) {
		printf("\n error in *read*!\n"); exit(-1);
	}
	close(file);	
	*docs = ndocs;
	*offsets = docBeginnings;
	
	return 0;		
}


/** **********************************************************************/
/** Saving the decompressed text                                         */
/** **********************************************************************/
int saveOutText(unsigned char *T, uint size, char *filename) {
	int file;
	unlink(filename);
	if( (file = open(filename, O_WRONLY|O_CREAT,S_IRWXG | S_IRWXU)) < 0) {
		printf("Cannot open file %s\n", filename);
		exit(0);
	}
	int errw = write(file, T, size);errw=0;
	close(file);				
	return 0;
}
