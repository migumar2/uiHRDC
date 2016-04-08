#include <iostream>
#include <cassert>
#include <stdio.h>
#include <string.h>

#include <algorithm>

//#include "RepairGraph.h"

#define GAP 1
#include "constants_bitmap.h"

using namespace std;


/** spire 2009 bitmaps */
uint maxPostingValue(int *buff, int terms) {
  int i,j,len;
  ulong sourcepos =2;
  int maxPostValue=0;
  int value;

	
  for (i=0; i<terms; i++) {
    len = buff[sourcepos++];
		
    for (j=0; j<len;j++) {
      value = buff[sourcepos++];
      if (value > maxPostValue) maxPostValue = value;
    }
  }	
  printf("\n maxPostValue = %d, sourcePos_end = %lu\n",maxPostValue, sourcepos);fflush(stdout);
  return maxPostValue+GAP;
}

/* Reads the file containing uints. */
int read_file(char *filename, uint **sourceInts, size_t *length) {

  uint *source;
  size_t t; 
  FILE *infile;
  
  infile = fopen(filename, "rb"); // b is for binary: required by DOS
  if(infile == NULL) return 30;
  
  /* store input file length */
  if(fseek(infile,0,SEEK_END) !=0 ) return 1;
  *length = ((size_t) ftell(infile))/(sizeof(uint));
  
  uint BS = 1024*1024*16;
  /* alloc memory for text (the overshoot is for suffix sorting) */
  source = new uint[*length];//malloc(((size_t)(*length))*sizeof(uint)); 
  
  /* read text in one sweep */
  rewind(infile);
  uint count = 0;
  while(count < *length) {
    t = fread(source+count, sizeof(uint), min((uint)*length-count,BS), infile);
    count += t;
  }
  fflush(stdout);
  cout << " I have read " << (*length)*sizeof(uint) << " bytes" ;
  cout << " I have read " << (count)*sizeof(uint) << " bytes" ;
  
  if (count!=*length) return 1;
  *sourceInts = source;
  fclose(infile);
  
  return 0;
}



int main(int argc, char ** argv) {
  if(argc<4) {
    cout << "Usage: " << argv[0] << " <in_name> <out_File> <divBitmap>" << endl;
    return 0;
  }

  char * fin = argv[1];
  uint * inbuffer;
  size_t inlength;
  uint divBitmap = atoi(argv[3]);
  
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
  
  uint sourcepos =2;
  int value=0;
  int curTerm =0;
  int len;
  int i;
  
  int maxLenPosting=0;
  
  
  uint maxPostValue = maxPostingValue(buff, terms) ; /***************** spire **************/
  uint actualPosts = 0; //counts the number of values managed with re-pair. --> sequence for repair.
  
  uint *lens = (uint *) malloc (sizeof(uint) * terms);
  
  for ( i=0; i<terms; i++) {
    len = buff[sourcepos++];
    // fwrite(&len,sizeof(int),1,fplens); //number of terms.
    lens[i]= len;
    if (len > maxLenPosting) maxLenPosting =len;
    curTerm--; 
    fwrite(&curTerm,sizeof(int),1,fp);
		
    //if (i==10000) printf("\nListando valores en LISTA[i=0..n-1] con i = %d\n (valores incrementados en 1 unidad) ==>",10000);
    for (int j=0; j<len;j++) {
      value = buff[sourcepos++];
      value +=1;  //** para que el valor de posting más bajo sea "1" y no "0"
      if (!shouldUseBitmap(lens, i, maxPostValue, divBitmap) ){		
	fwrite(&value,sizeof(int),1,fp);
	actualPosts++;
      }
      //	if (i==10000) printf("[%d]",value);
    }
    //printf("\n list[%d].len = %d  USing bitmaps=(%d), ",i,lens[i],(shouldUseBitmap(lens, i, maxPostValue, divBitmap)));
  }

  cout << "sourcepos=" << sourcepos << endl;
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
  size_t valueIn = inlength *sizeof(int);   //**   <--------------- Len of source set of lists...
  
  fwrite(&valueIn     ,sizeof(size_t),1,fpConst);
  fwrite((&maxLenPosting),sizeof(int),1,fpConst);
  fwrite(&maxPostValue,sizeof(int),1,fpConst);  /******************* spire ***********/
  fwrite(&divBitmap,sizeof(int),1,fpConst);     /******************* spire ***********/

  printf("\n writting constants file: valueIN=%zu, MAXLENPOST=%d, maxPostVal=%d, divBitmap=%d",valueIn,maxLenPosting,maxPostValue,divBitmap);

  fclose(fpConst);
  
  cout << "\n words" << terms << ", posts: " << posts << ", repairPosts:" << actualPosts ;
  cout << " curTerm " << curTerm << " sourcepos " << sourcepos ;
  cout << "\n";

  fseek(fp, (long) (sizeof(uint)*1), SEEK_SET);
  fwrite(&actualPosts,sizeof(int),1,fp); //number of posting values.
  	
  fclose(fp);
  delete [] ( inbuffer );
  
  cout << "\n ================== convert program ends ==================" << endl << endl;
	
  return 0;
}

