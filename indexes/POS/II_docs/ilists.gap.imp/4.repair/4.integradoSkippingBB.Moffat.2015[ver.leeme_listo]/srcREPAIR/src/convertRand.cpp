#include <iostream>
#include <cassert>
#include <algorithm>
#include <stdio.h>
#include <string.h>

//#include "RepairGraph.h"

using namespace std;

/* Reads the file containing uints. */
int read_file(char *filename, uint **sourceInts, uint *length) {

  uint *source;
  uint t; 
  FILE *infile;
  
  infile = fopen(filename, "rb"); // b is for binary: required by DOS
  if(infile == NULL) return 30;
  
  /* store input file length */
  if(fseek(infile,0,SEEK_END) !=0 ) return 1;
  *length = ftell(infile)/(sizeof(uint));
  
  /* alloc memory for text (the overshoot is for suffix sorting) */
  source = (uint *) malloc((*length)*sizeof(uint)); 
  if(source == NULL) return 1;  
  
  /* read text in one sweep */
  rewind(infile);
  t = fread(source, sizeof(uint), (size_t) (*length), infile);
  if (t!=*length) return 1;
  *sourceInts = source;
  fclose(infile);
  
  return 0;
}


/* checks if a value appears in an array */
int inArray (int *V, int n, int val) {
	register int i;
	for (i=0;i<n;i++)
		if (V[i]==val) return 1;
	return 0;
}

int intCompare (const void *a,const void *b) {
	int *pa = (int *) a;
	int *pb = (int *) b;
	if (*pa < *pb) return -1;
	else if (*pa > *pb) return 1;
	return 0;
}

int main(int argc, char ** argv) {
  if(argc<3) {
    cout << "Usage: " << argv[0] << " <in_name> <out_File>" << endl;
    return 0;
  }

  char * fin = argv[1];
  uint * inbuffer;
  uint inlength;
  
  read_file (fin, &inbuffer, &inlength);
  
  char * fout = argv[2];
  FILE * fp = fopen64(fout,"w");
  
  
  assert(fp!=NULL);
    
  int *buff = (int *) inbuffer;
  int posts,terms;
  terms=buff[0];
  
  fwrite(&terms,sizeof(int),1,fp); //number of terms.
  posts = inlength - buff[0] -2;
  
  cout << "inlen " << inlength << " " ;
  
  fwrite(&posts,sizeof(int),1,fp); //number of posting values.
  
  
  cout << " MAX_POST_VALUE " << buff[1] << "\n" ;

  uint sourcepos =2;
  int value=0;
  int curTerm =0;
  int len;
  
  int maxLenPosting=0;
  
  uint *lens = (uint *) malloc (sizeof(uint) * terms);
  
  
/*	for (int i=0; i<terms; i++) {
		len = buff[sourcepos++];
  	   // fwrite(&len,sizeof(int),1,fplens); //number of terms.
		lens[i]= len;
		
		if (len > maxLenPosting) maxLenPosting =len;
		curTerm--; 
		fwrite(&curTerm,sizeof(int),1,fp);
		
		if (i==10000) printf("\nListando valores en LISTA[i=0..n-1] con i = %d\n (valores incrementados en 1 unidad) ==>",10000);
		for (int j=0; j<len;j++) {
			value = buff[sourcepos++];
			value +=1;  // para que el valor de posting más bajo sea "1" y no "0"
			fwrite(&value,sizeof(int),1,fp);
			if (i==10000) printf("[%d]",value);
		}
		
	}
*/	

int M = buff[1];	//maxPostValue.
M*=1.1;
int *post = (int *) malloc (M * sizeof(int));

/** loads lens array **/
	for (int i=0; i<terms; i++) {
		len = buff[sourcepos++];
		lens[i]= len;		
		if (len > maxLenPosting) maxLenPosting =len;
		sourcepos +=len;
	}

	srand(time(NULL));

	cout << "\n generating "<<terms << "random lists \n";

	for (int i=0; i<terms; i++) {
		len = lens[i];
		curTerm--; 
		fwrite(&curTerm,sizeof(int),1,fp);

		if ( (!(i%1000)) || ( i< 10)) {
			cout << "\n iter" << i << " of " << terms << " currlenlist = " << len << " for M = " << M ;
			fflush(stdout);
		}
		//generating a random posting-list of size "len"
		for (int j=0;j<len;j++) {
			int randVal =  (( rand() % M ) + 1 );	
			//cout << "\n randVal es " << randVal;		 
			//fflush(stdout);
			//avoid duplicates
			while (inArray (post, j, randVal)) {randVal =  (( rand() % M ) + 1 ); 
			}		
			post[j]= randVal;									
		}		
		//qsorting
		
		if (len > 1) qsort(post,len,sizeof(int),intCompare);


		//if (i==10000) printf("\nListando valores en LISTA[i=0..n-1] con i = %d\n (valores incrementados en 1 unidad) ==>",10000);
		for (int j=0; j<len;j++) {
			value = post[j];
			//value +=1;  //** para que el valor de posting más bajo sea "1" y no "0"
			fwrite(&value,sizeof(int),1,fp);
			//if (i==10000) printf("[%d]",value);
			//printf("[%d]",value);
		}
		//exit(0);
		
	}

	cout << "\n generation finished !!\n";
	fflush(stdout);
	free(post);

	char fnameLens[255]; fnameLens[0] ='\0';
	sprintf(fnameLens,"%s.lens",argv[2]);
	FILE * fplens = fopen64(fnameLens,"w");   //** to keep the len of the posting lists of all terms.

	fwrite(lens,sizeof(uint),terms,fplens);
	free(lens);
	fclose(fplens);
	

 //** saves some constantes: maxPostingLen and lenOfSourceFile.
 char foutConst[255]; foutConst[0]='\0';
 strcat(foutConst,argv[2]);
 strcat(foutConst,".il.const"); 
  FILE * fpConst = fopen64(foutConst,"w");
  assert(fpConst!=NULL);
  int valueIn = inlength *sizeof(int);   //**   <--------------- Len of source set of lists...
  fwrite(&valueIn     ,sizeof(int),1,fpConst);
  fwrite(&maxLenPosting,sizeof(int),1,fpConst);
  fclose(fpConst);
 cout << " words " << terms << " posts " << posts ;
 cout << " curTerm " << curTerm << " sourcepos " << sourcepos ;
 	
	fclose(fp);
	free( inbuffer );
	
	
	
  return 0;



}

