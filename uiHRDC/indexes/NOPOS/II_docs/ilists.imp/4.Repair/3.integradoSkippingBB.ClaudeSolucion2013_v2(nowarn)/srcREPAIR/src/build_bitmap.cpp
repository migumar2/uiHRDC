#include <iostream>
#include <cassert>
#include <algorithm>
#include "basic.h"

#define GAP 1

using namespace std;


	typedef struct {
		uint *lenList;	
		uint nlists;		    //number of posting lists (equals to maxID+1)
		uint maxPostingValue;   //maximum value in any posting list (given as a source parameter)

		uint *zoneBitmaps;	//bitMap representing a large posting list (with maxPostingValue elements).
		uint bits_bitmaps;  //bits used by each bitmap
		uint ints_bitmaps_aligned ;  //uints needed by each bitmap, for them to be W-aligned into zoneOccs.
		uint numBitmaps;	//in number of ids treated as bitmaps.
		uint lenBitmapThreshold;   // such that list i receives bitmap if lenlist[i]> threshold.				
	}t_il;

t_il ail;  /* global variable */

/* Reads the file containing uints. */
int read_file(char *filename, uint **sourceInts, ulong *length) {

  uint *source;
  uint t; 
  FILE *infile;
  
  infile = fopen(filename, "rb"); // b is for binary: required by DOS
  if(infile == NULL) {
	cout << "seriously???" << endl;
	cout << filename << endl;
	return 30;
  }
  
  /* store input file length */
  if(fseek(infile,0,SEEK_END) !=0 ) return 1;
  *length = ftell(infile)/(sizeof(uint));
	cout << "length=" << *length << endl;
  
  /* alloc memory for text (the overshoot is for suffix sorting) */
  source = new uint[*length]; //(uint *) malloc((*length)*sizeof(uint)); 
  if(source == NULL) return 1;  
  
  /* read text in one sweep */
  rewind(infile);
	ulong BS = 1024*1024*16;
	ulong count = 0;
	while(count<*length) {
		t = fread(source+count, sizeof(uint), min(*length-count,BS), infile);
		count += t;
	}
  if (count!=*length) {
	cerr << "WTF?" << endl;
	return 1;
  }
  *sourceInts = source;
  fclose(infile);
  
  return 0;
}
/** *******************************************************************************
    * function used to parse the source posting lists that are loaded in build_il.*
    * Refer to "build_il() comments below" for a more detailed description        *
    * Basically, it reads the len of each posting list and its values.
		// *[Nwords=N]
		// *[maxPostvalue]
		// *[lenPost1][post11][post12]...[post1k�]
		// *[lenPost2][post21][post22]...[post2k��]
		// * ... 
		// *[lenPostN][postN1][postN2]...[postNk���]		
		//
	* It recreates array lenList, and uint ** occList and variables maxPostValue and nlists. */
int	parseAllPostingList(uint *nlists, uint **lenList, uint ***occList, 
                        uint *maxPostValue, uint *source, ulong length){

	uint **occs;
	uint *lens;
	uint n;
	
	register uint i,j,sourcepos,len;
	
	if (length<2) {
		cout << ":-(" << endl;
		return 1;
	}
	
	n=source[0];
	*maxPostValue = source[1];
	
	//allocating memory.
	lens = new uint[n]; //(uint *) malloc (sizeof(uint) * n);
	occs = new uint*[n]; //(uint **) malloc (sizeof(uint *) * n);
	
	sourcepos=2;
	
	for (i=0; i<n; i++) {
		lens[i]= len = source[sourcepos++];	
		occs[i]= new uint[len]; //(uint *) malloc (sizeof(uint) * len);
		for (j=0; j<len;j++) occs[i][j] = source[sourcepos++];
	}
	
	if (length !=sourcepos) {
		cout << ":-( 2" << endl;
		return 31;
	}
	
	*nlists =n;
	*lenList = lens;
	*occList = occs;	
	return 0;
}


/* Reads the file containing uints. */
int read_constants(char *filename, ulong *sourcelen, uint *maxLenPosting, uint *maxPostingValue, uint *divBitmap) {

  FILE *infile;
printf("\n\n opening file %s",filename);
  infile = fopen64(filename, "rb"); // b is for binary: required by DOS
  if(infile == NULL) return 30;
  
  //int t= fread(valConstants, sizeof(uint), 4, infile);
  //printf("\n bytes read constants = %d",t);

size_t rf;
  rf= fread(sourcelen,sizeof(ulong),1,infile);
  rf= fread(maxLenPosting,sizeof(int),1,infile);
  rf= fread(maxPostingValue,sizeof(int),1,infile);  /******************* spire ***********/
  rf= fread(divBitmap,sizeof(int),1,infile);     /******************* spire ***********/
  
  printf("\n ## Constants:##: sourcelen=%lu, maxLenPost = %d, MaxPostValue = %d, divBitmap = %d\n",
  					*sourcelen, *maxLenPosting, *maxPostingValue, *divBitmap);
  
  fclose(infile);
  return 0;
}



int save_bitmap(t_il *il, char *basename) {
	int file;
	char *filename;	
	filename = (char *)malloc(sizeof(char)*(strlen(basename)+20));
	strcpy(filename, basename); strcat(filename, ".");	strcat(filename, "bitmappost");
	fprintf(stderr,"\tSaving  bitmap-type postings: %s\n", filename);
	unlink(filename);
	if( (file = open(filename, O_WRONLY|O_CREAT,S_IRWXG | S_IRWXU|0666)) < 0) {
		printf("Cannot open file %s\n", filename);
		exit(0);
	}
	
	/** Saving the postings lists using bitmaps.*/
	{ ssize_t rf;
		rf= write(file, &(il->lenBitmapThreshold), sizeof(uint));		
		rf= write(file, &(il->bits_bitmaps), sizeof(uint));		
		rf= write(file, &(il->ints_bitmaps_aligned), sizeof(uint));	
		rf= write(file, &(il->numBitmaps), sizeof(uint));		
		rf= write(file, il->zoneBitmaps, (il->numBitmaps * il->ints_bitmaps_aligned) *  sizeof(uint));
	}	
	close(file);
	free(filename);	
	return 0;	
 }
 
 
int load_bitmap(t_il *il, char *basename) {
	int file;
	char *filename;	
	filename = (char *)malloc(sizeof(char)*(strlen(basename)+20));
	strcpy(filename, basename); strcat(filename, ".");	strcat(filename, "bitmappost");
	fprintf(stderr,"\nLoading  bitmap-type postings: %s\n", filename);
	if( (file = open(filename, O_RDONLY)) < 0) {
		printf("Cannot open file %s\n", filename);
		exit(0);
	}
	
	/** Loading the posting lists using bitmaps.*/
	{ ssize_t rf;
		rf= read(file, &(il->lenBitmapThreshold), sizeof(uint));		
		rf= read(file, &(il->bits_bitmaps), sizeof(uint));		
		rf= read(file, &(il->ints_bitmaps_aligned), sizeof(uint));	
		
		rf= read(file, &(il->numBitmaps), sizeof(uint));		

		il->zoneBitmaps = (uint *) malloc (sizeof(uint) * ( il->numBitmaps * il->ints_bitmaps_aligned));
		il->zoneBitmaps[( il->numBitmaps * il->ints_bitmaps_aligned) -1] =0000; //so valgrind is shut up		
		rf= read(file, il->zoneBitmaps, ( il->numBitmaps * il->ints_bitmaps_aligned) *  sizeof(uint));
	}
	
	close(file);	
	free(filename);
	return 0;
 }
 
 int free_il (t_il *il) {

	if (il->lenList) 
		delete [] il->lenList;
	free(il->zoneBitmaps);
	//free(il);
	return 0;
}
 
	/* checks if an id should use either bitmap or bytecodes: valid during build_il() */
int shouldUseBitmap(t_il *il, uint id) {  /* 0 <= id < il->nlists */
	if (il->lenList[id] > il->lenBitmapThreshold)
		return 1;
	return 0;
}


int extractList_il_check (t_il *il, uint id, uint ith, uint **list, uint *len) {
	register uint lenlist  = il->lenList[id];
	uint *alist = new uint[lenlist]; //(uint *) malloc (sizeof(uint) * lenlist);	

	//a list encoded with a bitmap.
	if ( shouldUseBitmap(il,id) ) {
		
		uint inipos = ith*il->ints_bitmaps_aligned*WW32; //pos in zoneBitmaps.
		uint endpos = inipos + il->bits_bitmaps;
		uint *zoneBitmaps = il->zoneBitmaps;
		uint i;
		
		lenlist=0;		
		for (i=inipos; i<endpos; i++) {
			if (bitget(zoneBitmaps,i)) 
				alist[lenlist++] = i-inipos;
		}
	}
	*list =alist;
	*len =lenlist;		
	return 0;				

}



int main(int argc, char ** argv) {
  if(argc<3) {
    cout << "Usage: " << argv[0] << " <in_name> <out_File_basename> " << endl;
    return 0;
  }

	t_il *il = &ail;
	
	int error;
	char * infile = argv[1];
	char * outfile = argv[2];

  	uint *source; ulong source_len;  //source file containing original posting lists.
	error = read_file(infile, &source, &source_len);
	  
	uint *lenList=NULL; uint **occList=NULL; uint maxPostingValue=0; uint nlists=0;
	
	fprintf(stderr,"\n paso 1 ");fflush(stderr);		

	cout << "source_len=" << source_len << endl;
	error = parseAllPostingList(&nlists, &lenList, &occList, &maxPostingValue, source, source_len);	
	if (error) {
		printf("\n parse of posting lists failed!");
	}
	maxPostingValue +=GAP;  // ids in [1..]

	//free(source);
	
	uint divBitmap;
	{ //loads the file with the constants.
		char constFname[256];
		sprintf(constFname,"%s.const",outfile); 
		ulong sourcelen;
		uint  maxlenPosting ,maxPostingValue1;

		divBitmap = sourcelen = maxlenPosting = maxPostingValue1 = 0; //just initilization
		read_constants(constFname,&sourcelen, &maxlenPosting, &maxPostingValue1, &divBitmap);

		printf("\n constantsRead: nlist = %d, sourcelen = %lu",nlists,sourcelen);
		printf("\n constantsRead: maxPostValue = %d, maxPostValue1 = %d",maxPostingValue,maxPostingValue1);
		printf("\n constantsRead: maxLenPosting = %d, divBitmap = %d",maxlenPosting,divBitmap);
	}		
	
	il->nlists = nlists;
	il->maxPostingValue = maxPostingValue;
	il->lenBitmapThreshold = maxPostingValue / divBitmap;
	il->lenList = lenList;

	printf("\n constantsRead: divBitmap %u--> lenBitmapThreshold = %u",divBitmap, il->lenBitmapThreshold);
	
	
	/*********/
	/** 5.0 ** Sets zoneBitmaps[] and numBitmaps; sets also occs for the lists using bitmap. **/
			

		{  //counts the number of terms indexed with bitmaps and reserves memory for them.
			
			uint i;
			uint numBitmaps=0;
			uint nwords = il->nlists;  
							 
			for (i=0;i<nwords; i++){
				uint option= shouldUseBitmap(il, i);	
				if (option) numBitmaps++;
				
				//if (i<2000) //printfs only
				//	fprintf(stderr,"\n id=%6d, with lenlist=%6d, shouldUseBitmap=%d, [thresholdLen = %7d]",i,lenList[i],option, il->lenBitmapThreshold);
			}  	
			fprintf(stderr,"\n Number of lists = %d, %d using bitmaps, %d using bcs",il->nlists, numBitmaps, il->nlists - numBitmaps);
							
			il->numBitmaps = numBitmaps;
			il->bits_bitmaps = il->maxPostingValue+1;  //also docId = (0.. maxPostingValue]
			il->ints_bitmaps_aligned =  (il->bits_bitmaps +WW32-1) /WW32;
			il->zoneBitmaps = (uint *) malloc (sizeof(uint) *  (il->numBitmaps * il->ints_bitmaps_aligned));
			for (i=0;i< ( il->numBitmaps * il->ints_bitmaps_aligned); i++)
				il->zoneBitmaps[i] = 0000;   //so no further "set to zero" is needed for each bitmap.					
		}
	
		/** 5.b **
			Sets also bitmaps for those terms indexed with a bitmap.
			Computes and sets the values for "occs[]". <-- NOT DONE YET.
		*/
		{
			uint nwords = il->nlists;
			uint id,j,len;
			
			uint counterBitmaps=0;
			for (id=0; id<nwords; id++) {
				len= il->lenList[id];
				
				if ( shouldUseBitmap(il,id) ) { //** spire **/					
					 //using a bitmap...
					uint iniPosBitmap = counterBitmaps * il->ints_bitmaps_aligned * WW32;
					//il->occs[id] = counterBitmaps;
					for (j=0;j<len;j++)  
						bitset(il->zoneBitmaps, (iniPosBitmap + occList[id][j] + GAP));							
					counterBitmaps++;
				}
			}
			fprintf(stderr,"\n\n Active bits for %d bitmaps where already set.",counterBitmaps);						
		}		
		
		
		/*********/
		fprintf(stderr,"\n ! CHECKING that the decoded lists are identical to the original ones. ");
		{ //checking:: decoding a list
			uint i, id ,len;
			uint *list;
			uint ith=0;
			
			for (id=0;id < il->nlists;id++) {
				if ( !shouldUseBitmap(il,id) ) continue;
				extractList_il_check (il, id, ith, &list, &len);	
				
				//printf("\n %d of %d",ith,id);
				for (i=0;i<len;i++){
					if (list[i] != (occList[id][i]+GAP)) {
						fprintf(stderr,"\n decoding of lists failed for id = %d: DIFFERENT!!: (%d,%d)",id, list[i],occList[id][i]);
						exit(0);
					}
				} 
				ith++;
				
				//free(list);				
				delete [] list;
			}	
		fprintf(stderr,"\n ! Decoding the list of occurrences worked fine (the same as previous values) ");
		
		}
		/***/


		
		{ 
			uint i;
			//for (i=0;i<il->nlists;i++) free(occList[i]);
			//free(occList);
			for (i=0;i<il->nlists;i++) delete [] occList[i];
			delete [] occList;


			save_bitmap(il, outfile) ;			 
			free_il(il);
			delete [] source;
		}
	
  cout << "\n ================== build_bitmap program ends ==================" << endl << endl;		
}

